#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include "models.hpp"
#include "baseModule.hpp"

namespace PaymentManager {

    /**
     * @brief Payment Module for handling all payment-related operations
     * 
     * This module manages payment pro        // BaseModule implementation
        int getEntityId(const std::shared_ptr<Model::Payment>& entity) const override {
            return entity->payment_id;
        }ssing, transaction handling, refunds,
     * and payment status tracking for the MuseIO Concert Management System.
     */
    class PaymentModule : public BaseModule<Model::Payment, int> {
    public:
        /**
         * @brief Constructor
         * @param filePath Path to the payment data file
         */
        PaymentModule(const std::string& filePath) : BaseModule<Model::Payment, int>(filePath) {
            loadEntities();
        }

        /**
         * @brief Destructor
         */
        ~PaymentModule() {
            saveEntities();
        }

        // Core Payment Operations
        
        /**
         * @brief Process a new payment transaction
         * @param attendee_id ID of the attendee making the payment
         * @param amount Payment amount
         * @param currency Currency code (e.g., "USD", "EUR")
         * @param payment_method Payment method (e.g., "Credit Card", "PayPal")
         * @return Transaction ID if successful, empty string if failed
         */
        std::string processPayment(int attendee_id, double amount, 
                                 const std::string& currency, 
                                 const std::string& payment_method) {
            // Validate payment data
            if (!validatePaymentData(amount, currency)) {
                return "";
            }

            // Generate transaction ID
            std::string transaction_id = generateTransactionId();
            
            // Create payment
            int payment_id = createPayment(attendee_id, amount, currency, payment_method, transaction_id);
            if (payment_id == -1) {
                return "";
            }

            // For simulation, we'll mark the payment as completed
            updatePaymentStatus(payment_id, Model::PaymentStatus::COMPLETED);
            
            return transaction_id;
        }

        /**
         * @brief Create a new payment record
         * @param attendee_id ID of the attendee making the payment
         * @param amount Payment amount
         * @param currency Currency code
         * @param payment_method Payment method
         * @param transaction_id External transaction ID
         * @return Payment ID if successful, -1 if failed
         */
        int createPayment(int attendee_id, double amount, const std::string& currency,
                        const std::string& payment_method, const std::string& transaction_id) {
            if (!validatePaymentData(amount, currency)) {
                return -1;
            }

            auto payment = std::make_shared<Model::Payment>();
            payment->payment_id = generateNewId();
            payment->amount = amount;
            payment->currency = currency;
            payment->payment_method = payment_method;
            payment->transaction_id = transaction_id;
            payment->status = Model::PaymentStatus::PENDING;
            payment->payment_date_time = Model::DateTime::now();
            // Note: attendee reference will be set by calling code if needed

            entities.push_back(payment);
            saveEntities();
            
            logPaymentTransaction(*payment, "CREATED");
            return payment->payment_id;
        }

        /**
         * @brief Create a payment record
         * @param payment Payment object to create
         * @return Payment ID if successful, -1 if failed
         */
        int createPayment(const Model::Payment& payment) {
            if (!validatePaymentData(payment.amount, payment.currency)) {
                return -1;
            }

            auto newPayment = std::make_shared<Model::Payment>(payment);
            newPayment->payment_id = generateNewId();
            newPayment->payment_date_time = Model::DateTime::now();

            entities.push_back(newPayment);
            saveEntities();
            
            logPaymentTransaction(*newPayment, "CREATED");
            return newPayment->payment_id;
        }

        /**
         * @brief Update payment status
         * @param payment_id Payment ID to update
         * @param status New payment status
         * @return true if successful, false otherwise
         */
        bool updatePaymentStatus(int payment_id, Model::PaymentStatus status) {
            auto payment = getPaymentById(payment_id);
            if (!payment) {
                return false;
            }

            payment->status = status;
            payment->payment_date_time = Model::DateTime::now();
            saveEntities();
            
            logPaymentTransaction(*payment, "STATUS_UPDATED");
            return true;
        }

        /**
         * @brief Process a refund for a payment
         * @param payment_id Original payment ID
         * @param refund_amount Amount to refund (0 for full refund)
         * @param reason Reason for refund
         * @return Refund transaction ID if successful, empty string if failed
         */
        std::string processRefund(int payment_id, double refund_amount = 0.0, 
                                const std::string& reason = "") {
            auto originalPayment = getPaymentById(payment_id);
            if (!originalPayment || originalPayment->status != Model::PaymentStatus::COMPLETED) {
                return "";
            }

            double refund = (refund_amount > 0) ? refund_amount : originalPayment->amount;
            if (refund > originalPayment->amount) {
                return "";
            }

            // Create refund payment record
            auto refundPayment = std::make_shared<Model::Payment>();
            refundPayment->payment_id = generateNewId();
            refundPayment->amount = -refund; // Negative amount for refund
            refundPayment->currency = originalPayment->currency;
            refundPayment->payment_method = originalPayment->payment_method;
            refundPayment->transaction_id = generateTransactionId();
            refundPayment->status = Model::PaymentStatus::REFUNDED;
            refundPayment->payment_date_time = Model::DateTime::now();

            entities.push_back(refundPayment);
            
            // Update original payment status if full refund
            if (refund == originalPayment->amount) {
                originalPayment->status = Model::PaymentStatus::REFUNDED;
                originalPayment->payment_date_time = Model::DateTime::now();
            }

            saveEntities();
            logPaymentTransaction(*refundPayment, "REFUNDED");
            return refundPayment->transaction_id;
        }

        // Query Operations

        /**
         * @brief Get payment by ID
         * @param payment_id Payment ID to retrieve
         * @return Shared pointer to payment, nullptr if not found
         */
        std::shared_ptr<Model::Payment> getPaymentById(int payment_id) {
            auto it = std::find_if(entities.begin(), entities.end(),
                [payment_id](const std::shared_ptr<Model::Payment>& payment) {
                    return payment->payment_id == payment_id;
                });
            return (it != entities.end()) ? *it : nullptr;
        }

        /**
         * @brief Get all payments for a specific attendee
         * @param attendee_id Attendee ID
         * @return Vector of payments for the attendee
         */
        std::vector<std::shared_ptr<Model::Payment>> getPaymentsByAttendee(int attendee_id) {
            std::vector<std::shared_ptr<Model::Payment>> result;
            std::copy_if(entities.begin(), entities.end(), std::back_inserter(result),
                [attendee_id](const std::shared_ptr<Model::Payment>& payment) {
                    auto attendee = payment->attendee.lock();
                    return attendee && attendee->id == attendee_id;
                });
            return result;
        }

        /**
         * @brief Get payments by status
         * @param status Payment status to filter by
         * @return Vector of payments with the specified status
         */
        std::vector<std::shared_ptr<Model::Payment>> getPaymentsByStatus(Model::PaymentStatus status) {
            std::vector<std::shared_ptr<Model::Payment>> result;
            std::copy_if(entities.begin(), entities.end(), std::back_inserter(result),
                [status](const std::shared_ptr<Model::Payment>& payment) {
                    return payment->status == status;
                });
            return result;
        }

        /**
         * @brief Get payments within a date range
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Vector of payments within the date range
         */
        std::vector<std::shared_ptr<Model::Payment>> getPaymentsByDateRange(
            const std::string& start_date, const std::string& end_date) {
            std::vector<std::shared_ptr<Model::Payment>> result;
            std::copy_if(entities.begin(), entities.end(), std::back_inserter(result),
                [&start_date, &end_date](const std::shared_ptr<Model::Payment>& payment) {
                    return payment->payment_date_time.iso8601String >= start_date && 
                           payment->payment_date_time.iso8601String <= end_date;
                });
            return result;
        }

        /**
         * @brief Get recent payments (last N payments)
         * @param limit Number of recent payments to retrieve (default: 30)
         * @return Vector of recent payments
         */
        std::vector<std::shared_ptr<Model::Payment>> getRecentPayments(int limit = 30) {
            std::vector<std::shared_ptr<Model::Payment>> sorted_payments = entities;
            std::sort(sorted_payments.begin(), sorted_payments.end(),
                [](const std::shared_ptr<Model::Payment>& a, const std::shared_ptr<Model::Payment>& b) {
                    return a->payment_date_time.iso8601String > b->payment_date_time.iso8601String;
                });
            
            if (sorted_payments.size() > static_cast<size_t>(limit)) {
                sorted_payments.resize(limit);
            }
            return sorted_payments;
        }

        // Transaction Management

        /**
         * @brief Validate a transaction ID
         * @param transaction_id Transaction ID to validate
         * @return true if valid, false otherwise
         */
        bool validateTransaction(const std::string& transaction_id) {
            return !transaction_id.empty() && transaction_id.length() >= 8;
        }

        /**
         * @brief Handle transaction callback from payment gateway
         * @param transaction_id External transaction ID
         * @param status Payment status from gateway
         * @param gateway_response Response data from payment gateway
         * @return true if handled successfully, false otherwise
         */
        bool handleTransactionCallback(const std::string& transaction_id,
                                     const std::string& status,
                                     const std::string& gateway_response) {
            auto it = std::find_if(entities.begin(), entities.end(),
                [&transaction_id](const std::shared_ptr<Model::Payment>& payment) {
                    return payment->transaction_id == transaction_id;
                });
            
            if (it == entities.end()) {
                return false;
            }

            auto payment = *it;
            payment->payment_date_time = Model::DateTime::now();

            if (status == "completed") {
                payment->status = Model::PaymentStatus::COMPLETED;
            } else if (status == "failed") {
                payment->status = Model::PaymentStatus::FAILED;
            }

            saveEntities();
            logPaymentTransaction(*payment, "CALLBACK_PROCESSED");
            return true;
        }

        // Analytics and Reporting

        /**
         * @brief Calculate total revenue for a date range
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @param currency Currency to filter by (empty for all)
         * @return Total revenue amount
         */
        double calculateRevenue(const std::string& start_date, 
                              const std::string& end_date,
                              const std::string& currency = "") {
            double total = 0.0;
            for (const auto& payment : entities) {
                if (payment->status == Model::PaymentStatus::COMPLETED &&
                    payment->amount > 0 && // Exclude refunds
                    payment->payment_date_time.iso8601String >= start_date &&
                    payment->payment_date_time.iso8601String <= end_date &&
                    (currency.empty() || payment->currency == currency)) {
                    total += payment->amount;
                }
            }
            return total;
        }

        /**
         * @brief Get payment statistics
         * @return Payment statistics struct
         */
        struct PaymentStats {
            int total_payments;
            int completed_payments;
            int pending_payments;
            int failed_payments;
            int refunded_payments;
            double total_revenue;
            double average_payment_amount;
            std::string most_used_payment_method;
        };
        PaymentStats getPaymentStatistics() {
            PaymentStats stats = {};
            std::map<std::string, int> methodCount;
            double totalAmount = 0.0;
            int validPayments = 0;

            for (const auto& payment : entities) {
                stats.total_payments++;
                
                switch (payment->status) {
                    case Model::PaymentStatus::COMPLETED:
                        stats.completed_payments++;
                        if (payment->amount > 0) {
                            stats.total_revenue += payment->amount;
                        }
                        break;
                    case Model::PaymentStatus::PENDING:
                        stats.pending_payments++;
                        break;
                    case Model::PaymentStatus::FAILED:
                        stats.failed_payments++;
                        break;
                    case Model::PaymentStatus::REFUNDED:
                        stats.refunded_payments++;
                        break;
                }

                if (payment->amount > 0) {
                    totalAmount += payment->amount;
                    validPayments++;
                    methodCount[payment->payment_method]++;
                }
            }

            stats.average_payment_amount = validPayments > 0 ? totalAmount / validPayments : 0.0;
            
            // Find most used payment method
            int maxCount = 0;
            for (const auto& pair : methodCount) {
                if (pair.second > maxCount) {
                    maxCount = pair.second;
                    stats.most_used_payment_method = pair.first;
                }
            }

            return stats;
        }

        /**
         * @brief Generate payment report for a specific period
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Payment report data
         */
        std::string generatePaymentReport(const std::string& start_date, 
                                        const std::string& end_date) {
            auto payments = getPaymentsByDateRange(start_date, end_date);
            std::ostringstream report;
            
            report << "Payment Report (" << start_date << " to " << end_date << ")\n";
            report << "=====================================\n\n";
            
            double totalRevenue = calculateRevenue(start_date, end_date);
            report << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << "\n";
            report << "Total Transactions: " << payments.size() << "\n\n";
            
            std::map<std::string, double> currencyTotals;
            std::map<std::string, int> methodCounts;
            
            for (const auto& payment : payments) {
                if (payment->status == Model::PaymentStatus::COMPLETED && payment->amount > 0) {
                    currencyTotals[payment->currency] += payment->amount;
                    methodCounts[payment->payment_method]++;
                }
            }
            
            report << "Revenue by Currency:\n";
            for (const auto& pair : currencyTotals) {
                report << "  " << pair.first << ": " << std::fixed << std::setprecision(2) << pair.second << "\n";
            }
            
            report << "\nPayment Methods:\n";
            for (const auto& pair : methodCounts) {
                report << "  " << pair.first << ": " << pair.second << " transactions\n";
            }
            
            return report.str();
        }

    protected:
        // BaseModule implementation
        int getEntityId(const std::shared_ptr<Model::Payment>& entity) const override {
            return entity->payment_id;
        }
        
        void loadEntities() override {
            entities.clear();
            std::ifstream file(dataFilePath, std::ios::binary);
            if (!file.is_open()) {
                return; // File doesn't exist yet, start with empty collection
            }

            size_t count;
            file.read(reinterpret_cast<char*>(&count), sizeof(count));
            
            for (size_t i = 0; i < count; ++i) {
                auto payment = std::make_shared<Model::Payment>();
                
                // Read payment data
                file.read(reinterpret_cast<char*>(&payment->payment_id), sizeof(payment->payment_id));
                file.read(reinterpret_cast<char*>(&payment->amount), sizeof(payment->amount));
                file.read(reinterpret_cast<char*>(&payment->status), sizeof(payment->status));
                
                // Read strings
                size_t len;
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                payment->currency.resize(len);
                file.read(&payment->currency[0], len);
                
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                payment->payment_method.resize(len);
                file.read(&payment->payment_method[0], len);
                
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                payment->transaction_id.resize(len);
                file.read(&payment->transaction_id[0], len);
                
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                payment->payment_date_time.iso8601String.resize(len);
                file.read(&payment->payment_date_time.iso8601String[0], len);
                
                entities.push_back(payment);
            }
            file.close();
        }
        
        bool saveEntities() override {
            std::ofstream file(dataFilePath, std::ios::binary);
            if (!file.is_open()) {
                return false;
            }

            size_t count = entities.size();
            file.write(reinterpret_cast<const char*>(&count), sizeof(count));
            
            for (const auto& payment : entities) {
                file.write(reinterpret_cast<const char*>(&payment->payment_id), sizeof(payment->payment_id));
                file.write(reinterpret_cast<const char*>(&payment->amount), sizeof(payment->amount));
                file.write(reinterpret_cast<const char*>(&payment->status), sizeof(payment->status));
                
                // Write strings
                size_t len = payment->currency.size();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(payment->currency.c_str(), len);
                
                len = payment->payment_method.size();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(payment->payment_method.c_str(), len);
                
                len = payment->transaction_id.size();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(payment->transaction_id.c_str(), len);
                
                len = payment->payment_date_time.iso8601String.size();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(payment->payment_date_time.iso8601String.c_str(), len);
            }
            
            file.close();
            return true;
        }

    private:
        /**
         * @brief Generate a unique transaction ID
         * @return Unique transaction ID
         */
        std::string generateTransactionId() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<> dis(100000, 999999);
            
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            
            return "TXN" + std::to_string(timestamp) + std::to_string(dis(gen));
        }

        /**
         * @brief Validate payment amount and currency
         * @param amount Payment amount
         * @param currency Currency code
         * @return true if valid, false otherwise
         */
        bool validatePaymentData(double amount, const std::string& currency) {
            if (amount <= 0.0) return false;
            if (currency.empty() || currency.length() != 3) return false;
            
            // Basic currency validation
            std::vector<std::string> validCurrencies = {"USD", "EUR", "GBP", "CAD", "AUD"};
            return std::find(validCurrencies.begin(), validCurrencies.end(), currency) != validCurrencies.end();
        }

        /**
         * @brief Log payment transaction for audit trail
         * @param payment Payment to log
         * @param action Action performed (e.g., "CREATED", "UPDATED", "REFUNDED")
         */
        void logPaymentTransaction(const Model::Payment& payment, const std::string& action) {
            // In a real implementation, this would write to an audit log file
            // For now, we'll just output to console in debug builds
            #ifdef DEBUG
            std::cout << "[PAYMENT AUDIT] " << action << " - Payment ID: " << payment.payment_id 
                      << ", Amount: " << payment.amount << " " << payment.currency 
                      << ", Status: " << static_cast<int>(payment.status) << std::endl;
            #endif
        }
    };

}