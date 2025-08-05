#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "models.hpp"
#include "baseModule.hpp"

namespace PaymentManager {

    /**
     * @brief Payment Module for handling all payment-related operations
     * 
     * This module manages payment processing, transaction handling, refunds,
     * and payment status tracking for the MuseIO Concert Management System.
     */
    class PaymentModule : public BaseModule<Model::Payment, int> {
    public:
        /**
         * @brief Constructor
         * @param filePath Path to the payment data file
         */
        PaymentModule(const std::string& filePath = "data/payments.dat");

        /**
         * @brief Destructor
         */
        ~PaymentModule() = default;

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
                                 const std::string& payment_method);

        /**
         * @brief Create a payment record
         * @param payment Payment object to create
         * @return Payment ID if successful, -1 if failed
         */
        int createPayment(const Model::Payment& payment);

        /**
         * @brief Update payment status
         * @param payment_id Payment ID to update
         * @param status New payment status
         * @return true if successful, false otherwise
         */
        bool updatePaymentStatus(int payment_id, Model::PaymentStatus status);

        /**
         * @brief Process a refund for a payment
         * @param payment_id Original payment ID
         * @param refund_amount Amount to refund (0 for full refund)
         * @param reason Reason for refund
         * @return Refund transaction ID if successful, empty string if failed
         */
        std::string processRefund(int payment_id, double refund_amount = 0.0, 
                                const std::string& reason = "");

        // Query Operations

        /**
         * @brief Get payment by ID
         * @param payment_id Payment ID to retrieve
         * @return Shared pointer to payment, nullptr if not found
         */
        std::shared_ptr<Model::Payment> getPaymentById(int payment_id);

        /**
         * @brief Get all payments for a specific attendee
         * @param attendee_id Attendee ID
         * @return Vector of payments for the attendee
         */
        std::vector<std::shared_ptr<Model::Payment>> getPaymentsByAttendee(int attendee_id);

        /**
         * @brief Get payments by status
         * @param status Payment status to filter by
         * @return Vector of payments with the specified status
         */
        std::vector<std::shared_ptr<Model::Payment>> getPaymentsByStatus(Model::PaymentStatus status);

        /**
         * @brief Get payments within a date range
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Vector of payments within the date range
         */
        std::vector<std::shared_ptr<Model::Payment>> getPaymentsByDateRange(
            const std::string& start_date, const std::string& end_date);

        /**
         * @brief Get recent payments (last N payments)
         * @param limit Number of recent payments to retrieve (default: 30)
         * @return Vector of recent payments
         */
        std::vector<std::shared_ptr<Model::Payment>> getRecentPayments(int limit = 30);

        // Transaction Management

        /**
         * @brief Validate a transaction ID
         * @param transaction_id Transaction ID to validate
         * @return true if valid, false otherwise
         */
        bool validateTransaction(const std::string& transaction_id);

        /**
         * @brief Handle transaction callback from payment gateway
         * @param transaction_id External transaction ID
         * @param status Payment status from gateway
         * @param gateway_response Response data from payment gateway
         * @return true if handled successfully, false otherwise
         */
        bool handleTransactionCallback(const std::string& transaction_id,
                                     const std::string& status,
                                     const std::string& gateway_response);

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
                              const std::string& currency = "");

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
        PaymentStats getPaymentStatistics();

        /**
         * @brief Generate payment report for a specific period
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Payment report data
         */
        std::string generatePaymentReport(const std::string& start_date, 
                                        const std::string& end_date);

    protected:
        // BaseModule implementation
        int getEntityId(const std::shared_ptr<Model::Payment>& entity) const override;
        void loadEntities() override;
        bool saveEntities() override;

    private:
        /**
         * @brief Generate a unique transaction ID
         * @return Unique transaction ID
         */
        std::string generateTransactionId();

        /**
         * @brief Validate payment amount and currency
         * @param amount Payment amount
         * @param currency Currency code
         * @return true if valid, false otherwise
         */
        bool validatePaymentData(double amount, const std::string& currency);

        /**
         * @brief Log payment transaction for audit trail
         * @param payment Payment to log
         * @param action Action performed (e.g., "CREATED", "UPDATED", "REFUNDED")
         */
        void logPaymentTransaction(const Model::Payment& payment, const std::string& action);
    };

}