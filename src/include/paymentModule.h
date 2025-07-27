#pragma once
#include <string>
#include <vector>

namespace payment {

    // Enum for payment status
    enum class PaymentStatus {
        Pending,
        Completed,
        Failed,
        Cancelled
    };

    // Forward declaration of Payment struct (definition elsewhere)
    struct Payment;

    // Process a payment and return transaction ID
    std::string processPayment(Payment& payment);

    // Handle a transaction (e.g., update status, log, etc.)
    bool handleTransaction(const std::string& transaction_id);

    // Update the payment status
    bool updatePaymentStatus(int payment_id, PaymentStatus status);

    // Retrieve payment status
    PaymentStatus getPaymentStatus(int payment_id);

    // Get all payments for reporting or management
    std::vector<Payment> getAllPayments();

} // namespace payment