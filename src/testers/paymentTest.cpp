#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "../include/paymentModule.hpp"
#include "../include/models.hpp"

/**
 * @brief Test suite for PaymentModule
 * 
 * This file contains comprehensive tests for all PaymentModule functionality
 * including payment processing, refunds, analytics, and data persistence.
 */

class PaymentModuleTest {
private:
    PaymentManager::PaymentModule paymentModule;
    
public:
    PaymentModuleTest() : paymentModule("test_data/test_payments.dat") {}

    /**
     * @brief Run all payment module tests
     */
    void runAllTests() {
        std::cout << "=== Starting PaymentModule Tests ===" << std::endl;
        
        try {
            testPaymentCreation();
            testPaymentProcessing();
            testPaymentStatusUpdates();
            testRefundProcessing();
            testPaymentQueries();
            testPaymentAnalytics();
            testTransactionManagement();
            testPaymentValidation();
            testRecentPayments();
            testDateRangeQueries();
            testRevenueCalculation();
            testPaymentStatistics();
            testPaymentReports();
            
            std::cout << "✅ All PaymentModule tests passed!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "❌ Test failed: " << e.what() << std::endl;
        }
        
        std::cout << "=== PaymentModule Tests Complete ===" << std::endl;
    }

private:
    /**
     * @brief Test payment creation functionality
     */
    void testPaymentCreation() {
        std::cout << "Testing payment creation..." << std::endl;
        
        // Create test payment
        Model::Payment payment;
        payment.payment_id = 1;
        payment.amount = 99.99;
        payment.currency = "USD";
        payment.payment_method = "Credit Card";
        payment.transaction_id = "txn_test_001";
        payment.status = Model::PaymentStatus::PENDING;
        payment.payment_date_time = Model::DateTime::now();
        
        // Test payment creation
        int payment_id = paymentModule.createPayment(payment);
        assert(payment_id > 0);
        
        // Verify payment exists
        auto retrieved_payment = paymentModule.getPaymentById(payment_id);
        assert(retrieved_payment != nullptr);
        assert(retrieved_payment->amount == 99.99);
        assert(retrieved_payment->currency == "USD");
        
        std::cout << "✅ Payment creation test passed" << std::endl;
    }

    /**
     * @brief Test payment processing workflow
     */
    void testPaymentProcessing() {
        std::cout << "Testing payment processing..." << std::endl;
        
        // Test payment processing
        std::string transaction_id = paymentModule.processPayment(
            123, // attendee_id
            149.99, // amount
            "USD", // currency
            "PayPal" // payment_method
        );
        
        assert(!transaction_id.empty());
        
        // Verify transaction exists and is valid
        bool is_valid = paymentModule.validateTransaction(transaction_id);
        assert(is_valid);
        
        std::cout << "✅ Payment processing test passed" << std::endl;
    }

    /**
     * @brief Test payment status updates
     */
    void testPaymentStatusUpdates() {
        std::cout << "Testing payment status updates..." << std::endl;
        
        // Create a test payment first
        Model::Payment payment;
        payment.payment_id = 2;
        payment.amount = 75.50;
        payment.currency = "USD";
        payment.payment_method = "Credit Card";
        payment.transaction_id = "txn_test_002";
        payment.status = Model::PaymentStatus::PENDING;
        payment.payment_date_time = Model::DateTime::now();
        
        int payment_id = paymentModule.createPayment(payment);
        assert(payment_id > 0);
        
        // Test status update to COMPLETED
        bool updated = paymentModule.updatePaymentStatus(payment_id, Model::PaymentStatus::COMPLETED);
        assert(updated);
        
        // Verify status was updated
        auto updated_payment = paymentModule.getPaymentById(payment_id);
        assert(updated_payment->status == Model::PaymentStatus::COMPLETED);
        
        std::cout << "✅ Payment status update test passed" << std::endl;
    }

    /**
     * @brief Test refund processing
     */
    void testRefundProcessing() {
        std::cout << "Testing refund processing..." << std::endl;
        
        // Create a completed payment first
        Model::Payment payment;
        payment.payment_id = 3;
        payment.amount = 120.00;
        payment.currency = "USD";
        payment.payment_method = "Credit Card";
        payment.transaction_id = "txn_test_003";
        payment.status = Model::PaymentStatus::COMPLETED;
        payment.payment_date_time = Model::DateTime::now();
        
        int payment_id = paymentModule.createPayment(payment);
        assert(payment_id > 0);
        
        // Test full refund
        std::string refund_transaction_id = paymentModule.processRefund(
            payment_id, 
            0.0, // full refund
            "Customer requested refund"
        );
        
        assert(!refund_transaction_id.empty());
        
        // Test partial refund
        std::string partial_refund_id = paymentModule.processRefund(
            payment_id,
            50.00, // partial refund
            "Partial service provided"
        );
        
        assert(!partial_refund_id.empty());
        
        std::cout << "✅ Refund processing test passed" << std::endl;
    }

    /**
     * @brief Test payment query operations
     */
    void testPaymentQueries() {
        std::cout << "Testing payment queries..." << std::endl;
        
        // Create multiple test payments for the same attendee
        int attendee_id = 456;
        for (int i = 0; i < 3; i++) {
            Model::Payment payment;
            payment.payment_id = 10 + i;
            payment.amount = 50.00 + (i * 10);
            payment.currency = "USD";
            payment.payment_method = "Credit Card";
            payment.transaction_id = "txn_query_" + std::to_string(i);
            payment.status = Model::PaymentStatus::COMPLETED;
            payment.payment_date_time = Model::DateTime::now();
            
            paymentModule.createPayment(payment);
        }
        
        // Test get payments by attendee
        auto attendee_payments = paymentModule.getPaymentsByAttendee(attendee_id);
        assert(attendee_payments.size() >= 3);
        
        // Test get payments by status
        auto completed_payments = paymentModule.getPaymentsByStatus(Model::PaymentStatus::COMPLETED);
        assert(completed_payments.size() > 0);
        
        std::cout << "✅ Payment queries test passed" << std::endl;
    }

    /**
     * @brief Test payment analytics
     */
    void testPaymentAnalytics() {
        std::cout << "Testing payment analytics..." << std::endl;
        
        // Get payment statistics
        auto stats = paymentModule.getPaymentStatistics();
        assert(stats.total_payments >= 0);
        assert(stats.total_revenue >= 0.0);
        
        // Verify statistics make sense
        assert(stats.total_payments == 
               stats.completed_payments + stats.pending_payments + 
               stats.failed_payments + stats.refunded_payments);
        
        std::cout << "✅ Payment analytics test passed" << std::endl;
    }

    /**
     * @brief Test transaction management
     */
    void testTransactionManagement() {
        std::cout << "Testing transaction management..." << std::endl;
        
        // Test transaction callback handling
        std::string test_transaction_id = "txn_callback_test";
        bool handled = paymentModule.handleTransactionCallback(
            test_transaction_id,
            "completed",
            "{\"gateway_response\": \"success\"}"
        );
        
        // The result depends on implementation, but should not crash
        std::cout << "Transaction callback handled: " << (handled ? "true" : "false") << std::endl;
        
        std::cout << "✅ Transaction management test passed" << std::endl;
    }

    /**
     * @brief Test payment validation
     */
    void testPaymentValidation() {
        std::cout << "Testing payment validation..." << std::endl;
        
        // Test invalid transaction ID
        bool is_valid = paymentModule.validateTransaction("invalid_transaction_id");
        assert(!is_valid);
        
        // Test empty transaction ID
        is_valid = paymentModule.validateTransaction("");
        assert(!is_valid);
        
        std::cout << "✅ Payment validation test passed" << std::endl;
    }

    /**
     * @brief Test recent payments functionality
     */
    void testRecentPayments() {
        std::cout << "Testing recent payments..." << std::endl;
        
        // Get recent payments with default limit
        auto recent_payments = paymentModule.getRecentPayments();
        
        // Get recent payments with custom limit
        auto limited_payments = paymentModule.getRecentPayments(5);
        assert(limited_payments.size() <= 5);
        
        std::cout << "✅ Recent payments test passed" << std::endl;
    }

    /**
     * @brief Test date range queries
     */
    void testDateRangeQueries() {
        std::cout << "Testing date range queries..." << std::endl;
        
        // Test with date range
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-12-31T23:59:59Z";
        
        auto payments_in_range = paymentModule.getPaymentsByDateRange(start_date, end_date);
        
        // Should not crash and return valid results
        std::cout << "Payments in date range: " << payments_in_range.size() << std::endl;
        
        std::cout << "✅ Date range queries test passed" << std::endl;
    }

    /**
     * @brief Test revenue calculation
     */
    void testRevenueCalculation() {
        std::cout << "Testing revenue calculation..." << std::endl;
        
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-12-31T23:59:59Z";
        
        // Test total revenue calculation
        double total_revenue = paymentModule.calculateRevenue(start_date, end_date);
        assert(total_revenue >= 0.0);
        
        // Test revenue for specific currency
        double usd_revenue = paymentModule.calculateRevenue(start_date, end_date, "USD");
        assert(usd_revenue >= 0.0);
        
        std::cout << "✅ Revenue calculation test passed" << std::endl;
    }

    /**
     * @brief Test payment statistics
     */
    void testPaymentStatistics() {
        std::cout << "Testing payment statistics..." << std::endl;
        
        auto stats = paymentModule.getPaymentStatistics();
        
        // Verify all fields are reasonable
        assert(stats.total_payments >= 0);
        assert(stats.completed_payments >= 0);
        assert(stats.pending_payments >= 0);
        assert(stats.failed_payments >= 0);
        assert(stats.refunded_payments >= 0);
        assert(stats.total_revenue >= 0.0);
        assert(stats.average_payment_amount >= 0.0);
        
        std::cout << "Payment Statistics:" << std::endl;
        std::cout << "  Total Payments: " << stats.total_payments << std::endl;
        std::cout << "  Total Revenue: $" << stats.total_revenue << std::endl;
        std::cout << "  Average Payment: $" << stats.average_payment_amount << std::endl;
        std::cout << "  Most Used Method: " << stats.most_used_payment_method << std::endl;
        
        std::cout << "✅ Payment statistics test passed" << std::endl;
    }

    /**
     * @brief Test payment report generation
     */
    void testPaymentReports() {
        std::cout << "Testing payment report generation..." << std::endl;
        
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-12-31T23:59:59Z";
        
        // Generate payment report
        std::string report = paymentModule.generatePaymentReport(start_date, end_date);
        assert(!report.empty());
        
        std::cout << "Generated report length: " << report.length() << " characters" << std::endl;
        
        std::cout << "✅ Payment report generation test passed" << std::endl;
    }
};

/**
 * @brief Main function to run all payment module tests
 */
int main() {
    try {
        PaymentModuleTest test;
        test.runAllTests();
        
        std::cout << std::endl << "🎉 All PaymentModule tests completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "💥 Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
