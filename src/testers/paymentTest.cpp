#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include "../include/models.hpp"
#include "../include/paymentModule.hpp"

// Utility function to display a separator line
void displaySeparator(char symbol = '-', int length = 50) {
    std::cout << std::string(length, symbol) << std::endl;
}

// Utility function to display a section header
void displayHeader(const std::string& title) {
    displaySeparator('=');
    std::cout << title << std::endl;
    displaySeparator('=');
}

// Utility function for displaying payment status
std::string getPaymentStatusString(Model::PaymentStatus status) {
    switch (status) {
        case Model::PaymentStatus::PENDING:   return "PENDING";
        case Model::PaymentStatus::COMPLETED: return "COMPLETED";
        case Model::PaymentStatus::FAILED:    return "FAILED";
        case Model::PaymentStatus::REFUNDED:  return "REFUNDED";
        default:                              return "UNKNOWN";
    }
}

// Detailed display of a single payment
void displayPayment(const std::shared_ptr<Model::Payment>& payment, bool detailed = true) {
    if (!payment) {
        std::cout << "[NULL PAYMENT REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Payment ID: " << payment->payment_id << std::endl;
    std::cout << "Amount: " << std::fixed << std::setprecision(2) << payment->amount 
              << " " << payment->currency << std::endl;
    
    if (detailed) {
        std::cout << "Payment Method: " << payment->payment_method << std::endl;
        std::cout << "Transaction ID: " << payment->transaction_id << std::endl;
        std::cout << "Status: " << getPaymentStatusString(payment->status) << std::endl;
        std::cout << "Date/Time: " << payment->payment_date_time.iso8601String << std::endl;
        
        auto attendee = payment->attendee.lock();
        if (attendee) {
            std::cout << "Attendee: " << attendee->name << " (ID: " << attendee->getId() << ")" << std::endl;
        } else {
            std::cout << "Attendee: [Not linked]" << std::endl;
        }
    }
}

// Display a collection of payments
void displayPaymentList(const std::vector<std::shared_ptr<Model::Payment>>& payments) {
    if (payments.empty()) {
        std::cout << "No payments found." << std::endl;
        return;
    }
    
    std::cout << "Found " << payments.size() << " payment(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(8) << "ID"
              << std::setw(12) << "Amount"
              << std::setw(8) << "Currency"
              << std::setw(15) << "Method"
              << std::setw(12) << "Status"
              << std::setw(20) << "Transaction ID" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& payment : payments) {
        std::cout << std::setw(8) << payment->payment_id
                  << std::setw(12) << std::fixed << std::setprecision(2) << payment->amount
                  << std::setw(8) << payment->currency
                  << std::setw(15) << payment->payment_method
                  << std::setw(12) << getPaymentStatusString(payment->status)
                  << std::setw(20) << payment->transaction_id << std::endl;
    }
    displaySeparator();
}

// Test CREATE operation
std::vector<std::shared_ptr<Model::Payment>> testCreateOperation(PaymentManager::PaymentModule& module) {
    displayHeader("CREATE OPERATION TEST");
    
    std::vector<std::shared_ptr<Model::Payment>> createdPayments;
    
    // Create first payment
    std::cout << "Creating payment 1 (USD Credit Card)..." << std::endl;
    int paymentId1 = module.createPayment(1, 99.99, "USD", "Credit Card", "TXN001");
    if (paymentId1 != -1) {
        auto payment1 = module.getPaymentById(paymentId1);
        if (payment1) {
            createdPayments.push_back(payment1);
            displayPayment(payment1);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create payment 1" << std::endl;
    }
    
    // Create second payment
    std::cout << "Creating payment 2 (EUR PayPal)..." << std::endl;
    int paymentId2 = module.createPayment(2, 149.50, "EUR", "PayPal", "TXN002");
    if (paymentId2 != -1) {
        auto payment2 = module.getPaymentById(paymentId2);
        if (payment2) {
            createdPayments.push_back(payment2);
            displayPayment(payment2);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create payment 2" << std::endl;
    }
    
    // Create third payment
    std::cout << "Creating payment 3 (USD Debit Card)..." << std::endl;
    int paymentId3 = module.createPayment(3, 75.25, "USD", "Debit Card", "TXN003");
    if (paymentId3 != -1) {
        auto payment3 = module.getPaymentById(paymentId3);
        if (payment3) {
            createdPayments.push_back(payment3);
            displayPayment(payment3);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create payment 3" << std::endl;
    }
    
    // Test invalid payment creation (negative amount)
    std::cout << "Testing invalid payment (negative amount)..." << std::endl;
    int invalidPaymentId = module.createPayment(4, -50.0, "USD", "Credit Card", "TXN004");
    if (invalidPaymentId == -1) {
        std::cout << "Correctly rejected negative amount payment" << std::endl;
    } else {
        std::cout << "ERROR: Should have rejected negative amount payment" << std::endl;
    }
    
    // Test invalid payment creation (invalid currency)
    std::cout << "Testing invalid payment (invalid currency)..." << std::endl;
    int invalidPaymentId2 = module.createPayment(5, 50.0, "XYZ", "Credit Card", "TXN005");
    if (invalidPaymentId2 == -1) {
        std::cout << "Correctly rejected invalid currency payment" << std::endl;
    } else {
        std::cout << "ERROR: Should have rejected invalid currency payment" << std::endl;
    }
    
    return createdPayments;
}

// Test READ operations
void testReadOperations(PaymentManager::PaymentModule& module, const std::vector<std::shared_ptr<Model::Payment>>& testPayments) {
    displayHeader("READ OPERATIONS TEST");
    
    // Get payment by ID
    if (!testPayments.empty()) {
        int idToFind = testPayments[0]->payment_id;
        std::cout << "1. Getting payment by ID " << idToFind << ":" << std::endl;
        auto foundPayment = module.getPaymentById(idToFind);
        if (foundPayment) {
            displayPayment(foundPayment);
        } else {
            std::cout << "Payment not found!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Get payments by status
    std::cout << "2. Getting payments by PENDING status:" << std::endl;
    auto pendingPayments = module.getPaymentsByStatus(Model::PaymentStatus::PENDING);
    displayPaymentList(pendingPayments);
    std::cout << std::endl;
    
    // Get recent payments
    std::cout << "3. Getting recent payments (limit 5):" << std::endl;
    auto recentPayments = module.getRecentPayments(5);
    displayPaymentList(recentPayments);
    std::cout << std::endl;
    
    // Get payments by date range
    std::cout << "4. Getting payments by date range (wide range):" << std::endl;
    auto dateRangePayments = module.getPaymentsByDateRange("2020-01-01T00:00:00Z", "2030-12-31T23:59:59Z");
    displayPaymentList(dateRangePayments);
    std::cout << std::endl;
    
    // Test not finding a payment
    std::cout << "5. Testing non-existent ID lookup:" << std::endl;
    auto nonExistentPayment = module.getPaymentById(9999);
    if (!nonExistentPayment) {
        std::cout << "Correctly returned nullptr for non-existent ID 9999" << std::endl;
    }
    std::cout << std::endl;
}

// Test UPDATE operations
void testUpdateOperations(PaymentManager::PaymentModule& module, const std::vector<std::shared_ptr<Model::Payment>>& testPayments) {
    displayHeader("UPDATE OPERATIONS TEST");
    
    if (testPayments.empty()) {
        std::cout << "No test payments available for update testing." << std::endl;
        return;
    }
    
    // Update payment status to COMPLETED
    int paymentIdToUpdate = testPayments[0]->payment_id;
    std::cout << "1. Updating payment " << paymentIdToUpdate << " to COMPLETED status:" << std::endl;
    bool updateSuccess = module.updatePaymentStatus(paymentIdToUpdate, Model::PaymentStatus::COMPLETED);
    if (updateSuccess) {
        auto updatedPayment = module.getPaymentById(paymentIdToUpdate);
        if (updatedPayment) {
            displayPayment(updatedPayment);
        }
    } else {
        std::cout << "Failed to update payment status" << std::endl;
    }
    std::cout << std::endl;
    
    // Update another payment to FAILED
    if (testPayments.size() > 1) {
        int paymentIdToFail = testPayments[1]->payment_id;
        std::cout << "2. Updating payment " << paymentIdToFail << " to FAILED status:" << std::endl;
        bool failSuccess = module.updatePaymentStatus(paymentIdToFail, Model::PaymentStatus::FAILED);
        if (failSuccess) {
            auto failedPayment = module.getPaymentById(paymentIdToFail);
            if (failedPayment) {
                displayPayment(failedPayment);
            }
        } else {
            std::cout << "Failed to update payment status" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test updating non-existent payment
    std::cout << "3. Testing update of non-existent payment:" << std::endl;
    bool invalidUpdate = module.updatePaymentStatus(9999, Model::PaymentStatus::COMPLETED);
    if (!invalidUpdate) {
        std::cout << "Correctly rejected update of non-existent payment" << std::endl;
    } else {
        std::cout << "ERROR: Should have rejected update of non-existent payment" << std::endl;
    }
    std::cout << std::endl;
}

// Test PAYMENT PROCESSING operations
void testPaymentProcessing(PaymentManager::PaymentModule& module) {
    displayHeader("PAYMENT PROCESSING TEST");
    
    // Test successful payment processing
    std::cout << "1. Processing a new payment..." << std::endl;
    std::string transactionId = module.processPayment(100, 250.0, "USD", "Credit Card");
    if (!transactionId.empty()) {
        std::cout << "Payment processed successfully with transaction ID: " << transactionId << std::endl;
        
        // Find the processed payment
        auto recentPayments = module.getRecentPayments(5);
        for (const auto& payment : recentPayments) {
            if (payment->transaction_id == transactionId) {
                displayPayment(payment);
                break;
            }
        }
    } else {
        std::cout << "Failed to process payment" << std::endl;
    }
    std::cout << std::endl;
    
    // Test payment processing with invalid data
    std::cout << "2. Testing payment processing with invalid data..." << std::endl;
    std::string invalidTransaction = module.processPayment(101, -100.0, "USD", "Credit Card");
    if (invalidTransaction.empty()) {
        std::cout << "Correctly rejected invalid payment processing" << std::endl;
    } else {
        std::cout << "ERROR: Should have rejected invalid payment processing" << std::endl;
    }
    std::cout << std::endl;
}

// Test REFUND operations
void testRefundOperations(PaymentManager::PaymentModule& module, const std::vector<std::shared_ptr<Model::Payment>>& testPayments) {
    displayHeader("REFUND OPERATIONS TEST");
    
    if (testPayments.empty()) {
        std::cout << "No test payments available for refund testing." << std::endl;
        return;
    }
    
    // First, ensure we have a completed payment to refund
    int refundPaymentId = testPayments[0]->payment_id;
    module.updatePaymentStatus(refundPaymentId, Model::PaymentStatus::COMPLETED);
    
    std::cout << "1. Processing full refund for payment " << refundPaymentId << ":" << std::endl;
    std::string refundTransactionId = module.processRefund(refundPaymentId, 0.0, "Customer request");
    if (!refundTransactionId.empty()) {
        std::cout << "Refund processed successfully with transaction ID: " << refundTransactionId << std::endl;
        
        // Check original payment status
        auto originalPayment = module.getPaymentById(refundPaymentId);
        if (originalPayment) {
            displayPayment(originalPayment);
        }
    } else {
        std::cout << "Failed to process refund" << std::endl;
    }
    std::cout << std::endl;
    
    // Test partial refund
    if (testPayments.size() > 2) {
        int partialRefundPaymentId = testPayments[2]->payment_id;
        module.updatePaymentStatus(partialRefundPaymentId, Model::PaymentStatus::COMPLETED);
        
        std::cout << "2. Processing partial refund for payment " << partialRefundPaymentId << ":" << std::endl;
        std::string partialRefundId = module.processRefund(partialRefundPaymentId, 25.0, "Partial refund");
        if (!partialRefundId.empty()) {
            std::cout << "Partial refund processed successfully with transaction ID: " << partialRefundId << std::endl;
            
            // Check original payment status (should still be COMPLETED for partial refunds)
            auto originalPayment = module.getPaymentById(partialRefundPaymentId);
            if (originalPayment) {
                displayPayment(originalPayment);
            }
        } else {
            std::cout << "Failed to process partial refund" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test refund of non-existent payment
    std::cout << "3. Testing refund of non-existent payment:" << std::endl;
    std::string invalidRefund = module.processRefund(9999, 50.0, "Invalid refund");
    if (invalidRefund.empty()) {
        std::cout << "Correctly rejected refund of non-existent payment" << std::endl;
    } else {
        std::cout << "ERROR: Should have rejected refund of non-existent payment" << std::endl;
    }
    std::cout << std::endl;
}

// Test ANALYTICS operations
void testAnalyticsOperations(PaymentManager::PaymentModule& module) {
    displayHeader("ANALYTICS OPERATIONS TEST");
    
    // Test revenue calculation
    std::cout << "1. Calculating total revenue:" << std::endl;
    double totalRevenue = module.calculateRevenue("2020-01-01T00:00:00Z", "2030-12-31T23:59:59Z");
    std::cout << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
    std::cout << std::endl;
    
    // Test revenue by currency
    std::cout << "2. Calculating USD revenue:" << std::endl;
    double usdRevenue = module.calculateRevenue("2020-01-01T00:00:00Z", "2030-12-31T23:59:59Z", "USD");
    std::cout << "USD Revenue: $" << std::fixed << std::setprecision(2) << usdRevenue << std::endl;
    std::cout << std::endl;
    
    // Test payment statistics
    std::cout << "3. Getting payment statistics:" << std::endl;
    auto stats = module.getPaymentStatistics();
    std::cout << "Total Payments: " << stats.total_payments << std::endl;
    std::cout << "Completed Payments: " << stats.completed_payments << std::endl;
    std::cout << "Pending Payments: " << stats.pending_payments << std::endl;
    std::cout << "Failed Payments: " << stats.failed_payments << std::endl;
    std::cout << "Refunded Payments: " << stats.refunded_payments << std::endl;
    std::cout << "Total Revenue: $" << std::fixed << std::setprecision(2) << stats.total_revenue << std::endl;
    std::cout << "Average Payment: $" << std::fixed << std::setprecision(2) << stats.average_payment_amount << std::endl;
    std::cout << "Most Used Method: " << stats.most_used_payment_method << std::endl;
    std::cout << std::endl;
    
    // Test payment report generation
    std::cout << "4. Generating payment report:" << std::endl;
    std::string report = module.generatePaymentReport("2020-01-01T00:00:00Z", "2030-12-31T23:59:59Z");
    std::cout << report << std::endl;
}

// Test TRANSACTION MANAGEMENT operations
void testTransactionManagement(PaymentManager::PaymentModule& module) {
    displayHeader("TRANSACTION MANAGEMENT TEST");
    
    // Test transaction validation
    std::cout << "1. Testing transaction validation:" << std::endl;
    bool validTransaction = module.validateTransaction("TXN12345678");
    std::cout << "Valid transaction ID (TXN12345678): " << (validTransaction ? "PASS" : "FAIL") << std::endl;
    
    bool invalidTransaction = module.validateTransaction("SHORT");
    std::cout << "Invalid transaction ID (SHORT): " << (!invalidTransaction ? "PASS" : "FAIL") << std::endl;
    
    bool emptyTransaction = module.validateTransaction("");
    std::cout << "Empty transaction ID: " << (!emptyTransaction ? "PASS" : "FAIL") << std::endl;
    std::cout << std::endl;
    
    // Test transaction callback handling
    std::cout << "2. Testing transaction callback handling:" << std::endl;
    int callbackPaymentId = module.createPayment(200, 125.75, "USD", "Credit Card", "TXN_CALLBACK_TEST");
    if (callbackPaymentId != -1) {
        auto payment = module.getPaymentById(callbackPaymentId);
        if (payment) {
            // Test successful callback
            bool callbackSuccess = module.handleTransactionCallback(payment->transaction_id, "completed", "Gateway response");
            std::cout << "Successful callback handling: " << (callbackSuccess ? "PASS" : "FAIL") << std::endl;
            
            // Verify status was updated
            payment = module.getPaymentById(callbackPaymentId);
            if (payment && payment->status == Model::PaymentStatus::COMPLETED) {
                std::cout << "Payment status updated by callback: PASS" << std::endl;
            }
            
            // Test failed callback
            bool failedCallback = module.handleTransactionCallback(payment->transaction_id, "failed", "Gateway error");
            std::cout << "Failed callback handling: " << (failedCallback ? "PASS" : "FAIL") << std::endl;
        }
    }
    
    // Test callback for non-existent transaction
    bool invalidCallback = module.handleTransactionCallback("NON_EXISTENT_TXN", "completed", "Response");
    std::cout << "Invalid callback rejection: " << (!invalidCallback ? "PASS" : "FAIL") << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "\n\n";
    displayHeader("PAYMENT MODULE COMPREHENSIVE TEST");
    std::cout << "Date: " << Model::DateTime::now().iso8601String << "\n\n";
    
    // Create module instance
    PaymentManager::PaymentModule module("test_payments.dat");
    
    // Vector to keep track of created payments for testing
    std::vector<std::shared_ptr<Model::Payment>> testPayments;
    
    try {
        // Test CREATE operations
        testPayments = testCreateOperation(module);
        std::cout << "\n\n";
        
        // Test READ operations
        testReadOperations(module, testPayments);
        std::cout << "\n\n";
        
        // Test UPDATE operations
        testUpdateOperations(module, testPayments);
        std::cout << "\n\n";
        
        // Test PAYMENT PROCESSING operations
        testPaymentProcessing(module);
        std::cout << "\n\n";
        
        // Test REFUND operations
        testRefundOperations(module, testPayments);
        std::cout << "\n\n";
        
        // Test ANALYTICS operations
        testAnalyticsOperations(module);
        std::cout << "\n\n";
        
        // Test TRANSACTION MANAGEMENT operations
        testTransactionManagement(module);
        
        displayHeader("TEST COMPLETED SUCCESSFULLY");
    }
    catch (const std::exception& e) {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
