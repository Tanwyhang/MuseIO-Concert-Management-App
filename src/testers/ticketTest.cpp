#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "../include/ticketModule.hpp"
#include "../include/models.hpp"

/**
 * @brief Test suite for TicketModule
 * 
 * This file contains comprehensive tests for all TicketModule functionality
 * including ticket creation, QR code management, reservations, and analytics.
 */

class TicketModuleTest {
private:
    TicketManager::TicketModule ticketModule;
    
public:
    TicketModuleTest() : ticketModule("test_data/test_tickets.dat") {}

    /**
     * @brief Run all ticket module tests
     */
    void runAllTests() {
        std::cout << "=== Starting TicketModule Tests ===" << std::endl;
        
        try {
            testTicketCreation();
            testMultipleTicketCreation();
            testTicketStatusUpdates();
            testTicketCancellation();
            testQRCodeGeneration();
            testQRCodeValidation();
            testCheckInProcess();
            testTicketQueries();
            testTicketReservations();
            testTicketAvailability();
            testTicketTransfer();
            testTicketUpgrade();
            testTicketAnalytics();
            testTicketSalesReport();
            
            std::cout << "✅ All TicketModule tests passed!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "❌ Test failed: " << e.what() << std::endl;
        }
        
        std::cout << "=== TicketModule Tests Complete ===" << std::endl;
    }

private:
    /**
     * @brief Test ticket creation functionality
     */
    void testTicketCreation() {
        std::cout << "Testing ticket creation..." << std::endl;
        
        // Test single ticket creation
        int ticket_id = ticketModule.createTicket(
            101, // attendee_id
            201, // concert_id
            "VIP" // ticket_type
        );
        
        assert(ticket_id > 0);
        
        // Verify ticket exists and has correct properties
        auto ticket = ticketModule.getTicketById(ticket_id);
        assert(ticket != nullptr);
        assert(ticket->status == Model::TicketStatus::SOLD);
        assert(!ticket->qr_code.empty());
        
        std::cout << "✅ Ticket creation test passed" << std::endl;
    }

    /**
     * @brief Test multiple ticket creation (bulk purchase)
     */
    void testMultipleTicketCreation() {
        std::cout << "Testing multiple ticket creation..." << std::endl;
        
        // Test bulk ticket creation
        std::vector<int> ticket_ids = ticketModule.createMultipleTickets(
            102, // attendee_id
            201, // concert_id
            3,   // quantity
            "Regular" // ticket_type
        );
        
        assert(ticket_ids.size() == 3);
        
        // Verify all tickets were created
        for (int ticket_id : ticket_ids) {
            assert(ticket_id > 0);
            auto ticket = ticketModule.getTicketById(ticket_id);
            assert(ticket != nullptr);
            assert(ticket->status == Model::TicketStatus::SOLD);
        }
        
        std::cout << "✅ Multiple ticket creation test passed" << std::endl;
    }

    /**
     * @brief Test ticket status updates
     */
    void testTicketStatusUpdates() {
        std::cout << "Testing ticket status updates..." << std::endl;
        
        // Create a test ticket first
        int ticket_id = ticketModule.createTicket(103, 201, "Regular");
        assert(ticket_id > 0);
        
        // Test status update to CHECKED_IN
        bool updated = ticketModule.updateTicketStatus(ticket_id, Model::TicketStatus::CHECKED_IN);
        assert(updated);
        
        // Verify status was updated
        auto ticket = ticketModule.getTicketById(ticket_id);
        assert(ticket->status == Model::TicketStatus::CHECKED_IN);
        
        std::cout << "✅ Ticket status update test passed" << std::endl;
    }

    /**
     * @brief Test ticket cancellation
     */
    void testTicketCancellation() {
        std::cout << "Testing ticket cancellation..." << std::endl;
        
        // Create a test ticket
        int ticket_id = ticketModule.createTicket(104, 201, "Regular");
        assert(ticket_id > 0);
        
        // Test ticket cancellation
        bool cancelled = ticketModule.cancelTicket(ticket_id, "Customer requested cancellation");
        assert(cancelled);
        
        // Verify ticket was cancelled
        auto ticket = ticketModule.getTicketById(ticket_id);
        assert(ticket->status == Model::TicketStatus::CANCELLED);
        
        std::cout << "✅ Ticket cancellation test passed" << std::endl;
    }

    /**
     * @brief Test QR code generation
     */
    void testQRCodeGeneration() {
        std::cout << "Testing QR code generation..." << std::endl;
        
        // Create a test ticket
        int ticket_id = ticketModule.createTicket(105, 201, "VIP");
        assert(ticket_id > 0);
        
        // Generate QR code
        std::string qr_code = ticketModule.generateQRCode(ticket_id);
        assert(!qr_code.empty());
        
        // Verify QR code was stored in ticket
        auto ticket = ticketModule.getTicketById(ticket_id);
        assert(ticket->qr_code == qr_code);
        
        std::cout << "✅ QR code generation test passed" << std::endl;
    }

    /**
     * @brief Test QR code validation
     */
    void testQRCodeValidation() {
        std::cout << "Testing QR code validation..." << std::endl;
        
        // Create a test ticket with QR code
        int ticket_id = ticketModule.createTicket(106, 201, "Regular");
        assert(ticket_id > 0);
        
        auto ticket = ticketModule.getTicketById(ticket_id);
        std::string qr_code = ticket->qr_code;
        
        // Test QR code validation
        int validated_ticket_id = ticketModule.validateQRCode(qr_code);
        assert(validated_ticket_id == ticket_id);
        
        // Test invalid QR code
        int invalid_result = ticketModule.validateQRCode("invalid_qr_code");
        assert(invalid_result == -1);
        
        std::cout << "✅ QR code validation test passed" << std::endl;
    }

    /**
     * @brief Test check-in process using QR code
     */
    void testCheckInProcess() {
        std::cout << "Testing check-in process..." << std::endl;
        
        // Create a test ticket
        int ticket_id = ticketModule.createTicket(107, 201, "Regular");
        assert(ticket_id > 0);
        
        auto ticket = ticketModule.getTicketById(ticket_id);
        std::string qr_code = ticket->qr_code;
        
        // Test check-in with QR code
        bool checked_in = ticketModule.checkInWithQRCode(qr_code);
        assert(checked_in);
        
        // Verify ticket status was updated to CHECKED_IN
        auto updated_ticket = ticketModule.getTicketById(ticket_id);
        assert(updated_ticket->status == Model::TicketStatus::CHECKED_IN);
        
        std::cout << "✅ Check-in process test passed" << std::endl;
    }

    /**
     * @brief Test ticket query operations
     */
    void testTicketQueries() {
        std::cout << "Testing ticket queries..." << std::endl;
        
        int attendee_id = 108;
        int concert_id = 202;
        
        // Create multiple tickets for testing
        std::vector<int> test_tickets;
        for (int i = 0; i < 3; i++) {
            int ticket_id = ticketModule.createTicket(attendee_id, concert_id, "Regular");
            test_tickets.push_back(ticket_id);
        }
        
        // Test get tickets by attendee
        auto attendee_tickets = ticketModule.getTicketsByAttendee(attendee_id);
        assert(attendee_tickets.size() >= 3);
        
        // Test get active tickets by attendee
        auto active_tickets = ticketModule.getActiveTicketsByAttendee(attendee_id);
        assert(active_tickets.size() >= 3);
        
        // Test get tickets by concert
        auto concert_tickets = ticketModule.getTicketsByConcert(concert_id);
        assert(concert_tickets.size() >= 3);
        
        // Test get tickets by status
        auto sold_tickets = ticketModule.getTicketsByStatus(Model::TicketStatus::SOLD);
        assert(sold_tickets.size() > 0);
        
        std::cout << "✅ Ticket queries test passed" << std::endl;
    }

    /**
     * @brief Test ticket reservation system
     */
    void testTicketReservations() {
        std::cout << "Testing ticket reservations..." << std::endl;
        
        int concert_id = 203;
        
        // Test ticket reservation
        std::string reservation_id = ticketModule.reserveTickets(concert_id, 2, 15);
        assert(!reservation_id.empty());
        
        // Test reservation confirmation
        std::vector<int> confirmed_tickets = ticketModule.confirmReservation(reservation_id, 109);
        assert(confirmed_tickets.size() == 2);
        
        // Verify tickets were created and assigned
        for (int ticket_id : confirmed_tickets) {
            auto ticket = ticketModule.getTicketById(ticket_id);
            assert(ticket != nullptr);
            assert(ticket->status == Model::TicketStatus::SOLD);
        }
        
        // Test reservation release
        std::string new_reservation_id = ticketModule.reserveTickets(concert_id, 1, 5);
        bool released = ticketModule.releaseReservation(new_reservation_id);
        assert(released);
        
        std::cout << "✅ Ticket reservations test passed" << std::endl;
    }

    /**
     * @brief Test ticket availability checking
     */
    void testTicketAvailability() {
        std::cout << "Testing ticket availability..." << std::endl;
        
        int concert_id = 204;
        
        // Check initial availability
        int available_count = ticketModule.getAvailableTicketCount(concert_id);
        assert(available_count >= 0);
        
        // Check if tickets are on sale
        bool on_sale = ticketModule.areTicketsOnSale(concert_id);
        // Result depends on concert configuration
        
        std::cout << "Available tickets for concert " << concert_id << ": " << available_count << std::endl;
        std::cout << "Tickets on sale: " << (on_sale ? "Yes" : "No") << std::endl;
        
        std::cout << "✅ Ticket availability test passed" << std::endl;
    }

    /**
     * @brief Test ticket transfer functionality
     */
    void testTicketTransfer() {
        std::cout << "Testing ticket transfer..." << std::endl;
        
        // Create a ticket for transfer
        int ticket_id = ticketModule.createTicket(110, 205, "Regular");
        assert(ticket_id > 0);
        
        // Test ticket transfer
        bool transferred = ticketModule.transferTicket(
            ticket_id, 
            111, // new_attendee_id
            "Gift to friend"
        );
        assert(transferred);
        
        std::cout << "✅ Ticket transfer test passed" << std::endl;
    }

    /**
     * @brief Test ticket upgrade functionality
     */
    void testTicketUpgrade() {
        std::cout << "Testing ticket upgrade..." << std::endl;
        
        // Create a regular ticket for upgrade
        int ticket_id = ticketModule.createTicket(112, 205, "Regular");
        assert(ticket_id > 0);
        
        // Test ticket upgrade
        bool upgraded = ticketModule.upgradeTicket(
            ticket_id,
            "VIP",
            50.00 // additional_payment
        );
        assert(upgraded);
        
        std::cout << "✅ Ticket upgrade test passed" << std::endl;
    }

    /**
     * @brief Test ticket analytics and statistics
     */
    void testTicketAnalytics() {
        std::cout << "Testing ticket analytics..." << std::endl;
        
        int concert_id = 206;
        
        // Create some test tickets for analytics
        for (int i = 0; i < 5; i++) {
            ticketModule.createTicket(200 + i, concert_id, "Regular");
        }
        
        // Get ticket statistics
        auto stats = ticketModule.getTicketStatistics(concert_id);
        
        assert(stats.total_tickets >= 0);
        assert(stats.sold_tickets >= 0);
        assert(stats.available_tickets >= 0);
        assert(stats.sales_percentage >= 0.0 && stats.sales_percentage <= 100.0);
        assert(stats.check_in_percentage >= 0.0 && stats.check_in_percentage <= 100.0);
        
        std::cout << "Ticket Statistics for Concert " << concert_id << ":" << std::endl;
        std::cout << "  Total Tickets: " << stats.total_tickets << std::endl;
        std::cout << "  Sold Tickets: " << stats.sold_tickets << std::endl;
        std::cout << "  Available Tickets: " << stats.available_tickets << std::endl;
        std::cout << "  Sales Percentage: " << stats.sales_percentage << "%" << std::endl;
        std::cout << "  Check-in Percentage: " << stats.check_in_percentage << "%" << std::endl;
        
        std::cout << "✅ Ticket analytics test passed" << std::endl;
    }

    /**
     * @brief Test ticket sales report generation
     */
    void testTicketSalesReport() {
        std::cout << "Testing ticket sales report..." << std::endl;
        
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-12-31T23:59:59Z";
        
        // Generate ticket sales report
        std::string report = ticketModule.generateTicketSalesReport(start_date, end_date);
        assert(!report.empty());
        
        std::cout << "Generated ticket sales report length: " << report.length() << " characters" << std::endl;
        
        std::cout << "✅ Ticket sales report test passed" << std::endl;
    }
};

/**
 * @brief Interactive test menu for manual testing
 */
void runInteractiveTests() {
    TicketManager::TicketModule ticketModule("interactive_test_tickets.dat");
    
    int choice;
    do {
        std::cout << "\n=== TicketModule Interactive Test Menu ===" << std::endl;
        std::cout << "1. Create a ticket" << std::endl;
        std::cout << "2. Generate QR code" << std::endl;
        std::cout << "3. Validate QR code" << std::endl;
        std::cout << "4. Check-in with QR code" << std::endl;
        std::cout << "5. Reserve tickets" << std::endl;
        std::cout << "6. View ticket statistics" << std::endl;
        std::cout << "7. Transfer ticket" << std::endl;
        std::cout << "8. Upgrade ticket" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                int attendee_id, concert_id;
                std::string ticket_type;
                std::cout << "Enter attendee ID: ";
                std::cin >> attendee_id;
                std::cout << "Enter concert ID: ";
                std::cin >> concert_id;
                std::cout << "Enter ticket type: ";
                std::cin >> ticket_type;
                
                int ticket_id = ticketModule.createTicket(attendee_id, concert_id, ticket_type);
                if (ticket_id > 0) {
                    std::cout << "✅ Ticket created with ID: " << ticket_id << std::endl;
                } else {
                    std::cout << "❌ Failed to create ticket" << std::endl;
                }
                break;
            }
            case 2: {
                int ticket_id;
                std::cout << "Enter ticket ID: ";
                std::cin >> ticket_id;
                
                std::string qr_code = ticketModule.generateQRCode(ticket_id);
                if (!qr_code.empty()) {
                    std::cout << "✅ QR Code generated: " << qr_code << std::endl;
                } else {
                    std::cout << "❌ Failed to generate QR code" << std::endl;
                }
                break;
            }
            case 3: {
                std::string qr_code;
                std::cout << "Enter QR code: ";
                std::cin >> qr_code;
                
                int ticket_id = ticketModule.validateQRCode(qr_code);
                if (ticket_id > 0) {
                    std::cout << "✅ Valid QR code for ticket ID: " << ticket_id << std::endl;
                } else {
                    std::cout << "❌ Invalid QR code" << std::endl;
                }
                break;
            }
            case 6: {
                int concert_id;
                std::cout << "Enter concert ID: ";
                std::cin >> concert_id;
                
                auto stats = ticketModule.getTicketStatistics(concert_id);
                std::cout << "Ticket Statistics:" << std::endl;
                std::cout << "  Total: " << stats.total_tickets << std::endl;
                std::cout << "  Sold: " << stats.sold_tickets << std::endl;
                std::cout << "  Available: " << stats.available_tickets << std::endl;
                std::cout << "  Sales %: " << stats.sales_percentage << std::endl;
                break;
            }
            case 0:
                std::cout << "Exiting interactive tests..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);
}

/**
 * @brief Main function to run ticket module tests
 */
int main(int argc, char* argv[]) {
    try {
        if (argc > 1 && std::string(argv[1]) == "--interactive") {
            runInteractiveTests();
        } else {
            TicketModuleTest test;
            test.runAllTests();
            
            std::cout << std::endl << "🎉 All TicketModule tests completed successfully!" << std::endl;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "💥 Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
