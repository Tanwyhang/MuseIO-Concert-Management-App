#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include "../include/models.hpp"
#include "../include/ticketModule.hpp"

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

// Utility function for displaying ticket status
std::string getTicketStatusString(Model::TicketStatus status) {
    switch (status) {
        case Model::TicketStatus::AVAILABLE: return "AVAILABLE";
        case Model::TicketStatus::SOLD:      return "SOLD";
        case Model::TicketStatus::CHECKED_IN: return "CHECKED_IN";
        case Model::TicketStatus::CANCELLED: return "CANCELLED";
        case Model::TicketStatus::EXPIRED:   return "EXPIRED";
        default:                             return "UNKNOWN";
    }
}

// Detailed display of a single ticket
void displayTicket(const std::shared_ptr<Model::Ticket>& ticket, bool detailed = true) {
    if (!ticket) {
        std::cout << "[NULL TICKET REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Ticket ID: " << ticket->ticket_id << std::endl;
    std::cout << "Status: " << getTicketStatusString(ticket->status) << std::endl;
    
    if (detailed) {
        std::cout << "QR Code: " << ticket->qr_code << std::endl;
        std::cout << "Created At: " << ticket->created_at.iso8601String << std::endl;
        std::cout << "Updated At: " << ticket->updated_at.iso8601String << std::endl;
        
        auto attendee = ticket->attendee.lock();
        if (attendee) {
            std::cout << "Attendee: " << attendee->name << " (ID: " << attendee->id << ")" << std::endl;
        } else {
            std::cout << "Attendee: [Not linked]" << std::endl;
        }
        
        auto payment = ticket->payment.lock();
        if (payment) {
            std::cout << "Payment: ID " << payment->payment_id << " ($" << std::fixed << std::setprecision(2) << payment->amount << ")" << std::endl;
        } else {
            std::cout << "Payment: [Not linked]" << std::endl;
        }
        
        auto concertTicket = ticket->concert_ticket.lock();
        if (concertTicket) {
            std::cout << "Concert Ticket: $" << std::fixed << std::setprecision(2) << concertTicket->base_price << std::endl;
        } else {
            std::cout << "Concert Ticket: [Not linked]" << std::endl;
        }
    }
}

// Display a collection of tickets
void displayTicketList(const std::vector<std::shared_ptr<Model::Ticket>>& tickets) {
    if (tickets.empty()) {
        std::cout << "No tickets found." << std::endl;
        return;
    }
    
    std::cout << "Found " << tickets.size() << " ticket(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(10) << "Ticket ID"
              << std::setw(12) << "Status"
              << std::setw(25) << "QR Code"
              << std::setw(20) << "Created At" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& ticket : tickets) {
        std::string shortQR = ticket->qr_code.length() > 20 ? 
                             ticket->qr_code.substr(0, 17) + "..." : 
                             ticket->qr_code;
        std::string shortDate = ticket->created_at.iso8601String.length() > 16 ?
                               ticket->created_at.iso8601String.substr(0, 16) :
                               ticket->created_at.iso8601String;
        
        std::cout << std::setw(10) << ticket->ticket_id
                  << std::setw(12) << getTicketStatusString(ticket->status)
                  << std::setw(25) << shortQR
                  << std::setw(20) << shortDate << std::endl;
    }
    displaySeparator();
}

// Test CREATE operation
std::vector<std::shared_ptr<Model::Ticket>> testCreateOperation(TicketManager::TicketModule& module) {
    displayHeader("CREATE OPERATION TEST");
    
    std::vector<std::shared_ptr<Model::Ticket>> createdTickets;
    
    // Create ticket 1
    std::cout << "Creating ticket 1 (Regular ticket for attendee 1, concert 1)..." << std::endl;
    int ticketId1 = module.createTicket(1, 1, "Regular");
    if (ticketId1 != -1) {
        auto ticket1 = module.getTicketById(ticketId1);
        if (ticket1) {
            createdTickets.push_back(ticket1);
            displayTicket(ticket1);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create ticket 1" << std::endl;
    }
    
    // Create ticket 2
    std::cout << "Creating ticket 2 (VIP ticket for attendee 2, concert 1)..." << std::endl;
    int ticketId2 = module.createTicket(2, 1, "VIP");
    if (ticketId2 != -1) {
        auto ticket2 = module.getTicketById(ticketId2);
        if (ticket2) {
            createdTickets.push_back(ticket2);
            displayTicket(ticket2);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create ticket 2" << std::endl;
    }
    
    // Create ticket 3
    std::cout << "Creating ticket 3 (Regular ticket for attendee 3, concert 2)..." << std::endl;
    int ticketId3 = module.createTicket(3, 2, "Regular");
    if (ticketId3 != -1) {
        auto ticket3 = module.getTicketById(ticketId3);
        if (ticket3) {
            createdTickets.push_back(ticket3);
            displayTicket(ticket3);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create ticket 3" << std::endl;
    }
    
    // Test multiple ticket creation
    std::cout << "Testing multiple ticket creation (5 tickets for concert 1)..." << std::endl;
    auto bulkTicketIds = module.createMultipleTickets(1, 1, 5, "Regular");
    std::cout << "Multiple ticket creation result: " << bulkTicketIds.size() << " tickets created" << std::endl;
    
    // Get the actual ticket objects for bulk tickets
    std::vector<std::shared_ptr<Model::Ticket>> bulkTickets;
    for (int ticketId : bulkTicketIds) {
        auto ticket = module.getTicketById(ticketId);
        if (ticket) {
            bulkTickets.push_back(ticket);
        }
    }
    
    // Add bulk tickets to our test collection
    for (const auto& ticket : bulkTickets) {
        createdTickets.push_back(ticket);
    }
    
    return createdTickets;
}

// Test READ operations
void testReadOperations(TicketManager::TicketModule& module, const std::vector<std::shared_ptr<Model::Ticket>>& testTickets) {
    displayHeader("READ OPERATIONS TEST");
    
    // Get ticket by ID
    if (!testTickets.empty()) {
        int idToFind = testTickets[0]->ticket_id;
        std::cout << "1. Getting ticket by ID " << idToFind << ":" << std::endl;
        auto foundTicket = module.getTicketById(idToFind);
        if (foundTicket) {
            displayTicket(foundTicket);
        } else {
            std::cout << "Ticket not found!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Get tickets by status
    std::cout << "2. Getting tickets by SOLD status:" << std::endl;
    auto soldTickets = module.getTicketsByStatus(Model::TicketStatus::SOLD);
    displayTicketList(soldTickets);
    std::cout << std::endl;
    
    // Get tickets for a specific concert
    std::cout << "3. Getting tickets for concert 1:" << std::endl;
    auto concertTickets = module.getTicketsByConcert(1);
    displayTicketList(concertTickets);
    std::cout << std::endl;
    
    // Get tickets for a specific attendee
    std::cout << "4. Getting tickets for attendee 1:" << std::endl;
    auto attendeeTickets = module.getTicketsByAttendee(1);
    displayTicketList(attendeeTickets);
    std::cout << std::endl;
    
    // Test not finding a ticket
    std::cout << "5. Testing retrieval of non-existent ticket (ID 9999):" << std::endl;
    auto notFound = module.getTicketById(9999);
    if (notFound) {
        std::cout << "Unexpected: Found ticket with ID 9999" << std::endl;
    } else {
        std::cout << "Correctly returned nullptr for non-existent ticket" << std::endl;
    }
    std::cout << std::endl;
}

// Test UPDATE operations
void testUpdateOperations(TicketManager::TicketModule& module, const std::vector<std::shared_ptr<Model::Ticket>>& testTickets) {
    displayHeader("UPDATE OPERATIONS TEST");
    
    if (testTickets.empty()) {
        std::cout << "No tickets available for update testing." << std::endl;
        return;
    }
    
    // Update ticket status
    auto ticketToUpdate = testTickets[0];
    std::cout << "1. Updating ticket ID " << ticketToUpdate->ticket_id << " status:" << std::endl;
    std::cout << "Before update:" << std::endl;
    displayTicket(ticketToUpdate, false);
    
    bool updateResult = module.updateTicketStatus(ticketToUpdate->ticket_id, Model::TicketStatus::CHECKED_IN);
    if (updateResult) {
        std::cout << "\nAfter update:" << std::endl;
        auto updatedTicket = module.getTicketById(ticketToUpdate->ticket_id);
        if (updatedTicket) {
            displayTicket(updatedTicket, false);
        }
    } else {
        std::cout << "Update failed!" << std::endl;
    }
    std::cout << std::endl;
    
    // Test updating non-existent ticket
    std::cout << "2. Testing update of non-existent ticket (ID 8888):" << std::endl;
    bool failedUpdate = module.updateTicketStatus(8888, Model::TicketStatus::CANCELLED);
    if (!failedUpdate) {
        std::cout << "Correctly failed to update non-existent ticket" << std::endl;
    } else {
        std::cout << "Unexpected: Successfully updated non-existent ticket" << std::endl;
    }
    std::cout << std::endl;
}

// Test QR Code operations
void testQRCodeOperations(TicketManager::TicketModule& module, const std::vector<std::shared_ptr<Model::Ticket>>& testTickets) {
    displayHeader("QR CODE OPERATIONS TEST");
    
    if (testTickets.empty()) {
        std::cout << "No tickets available for QR code testing." << std::endl;
        return;
    }
    
    // Generate QR code for a ticket
    auto testTicket = testTickets[0];
    std::cout << "1. Generating QR code for ticket ID " << testTicket->ticket_id << ":" << std::endl;
    std::string qrCode = module.generateQRCode(testTicket->ticket_id);
    if (!qrCode.empty()) {
        std::cout << "Generated QR code: " << qrCode << std::endl;
    } else {
        std::cout << "Failed to generate QR code" << std::endl;
    }
    std::cout << std::endl;
    
    // Validate QR code
    std::cout << "2. Validating QR code:" << std::endl;
    if (!testTicket->qr_code.empty()) {
        int validatedTicketId = module.validateQRCode(testTicket->qr_code);
        if (validatedTicketId == testTicket->ticket_id) {
            std::cout << "QR code validation successful - Ticket ID: " << validatedTicketId << std::endl;
        } else {
            std::cout << "QR code validation failed" << std::endl;
        }
    } else {
        std::cout << "No QR code available to validate" << std::endl;
    }
    std::cout << std::endl;
    
    // Test check-in with QR code
    std::cout << "3. Testing check-in with QR code:" << std::endl;
    if (!testTicket->qr_code.empty()) {
        bool checkedIn = module.checkInWithQRCode(testTicket->qr_code);
        if (checkedIn) {
            std::cout << "Check-in successful!" << std::endl;
            auto updatedTicket = module.getTicketById(testTicket->ticket_id);
            if (updatedTicket) {
                std::cout << "Updated ticket status: " << getTicketStatusString(updatedTicket->status) << std::endl;
            }
        } else {
            std::cout << "Check-in failed" << std::endl;
        }
    } else {
        std::cout << "No QR code available for check-in" << std::endl;
    }
    std::cout << std::endl;
}

// Test Ticket Reservation operations
void testReservationOperations(TicketManager::TicketModule& module) {
    displayHeader("TICKET RESERVATION OPERATIONS TEST");
    
    // Reserve tickets
    std::cout << "1. Reserving 3 tickets for concert 1:" << std::endl;
    std::string reservationId = module.reserveTickets(1, 3, 15);
    if (!reservationId.empty()) {
        std::cout << "Reservation created successfully - ID: " << reservationId << std::endl;
        
        // Confirm reservation
        std::cout << "\n2. Confirming reservation for attendee 4:" << std::endl;
        std::vector<int> confirmedTickets = module.confirmReservation(reservationId, 4);
        if (!confirmedTickets.empty()) {
            std::cout << "Reservation confirmed - " << confirmedTickets.size() << " tickets created:" << std::endl;
            for (int ticketId : confirmedTickets) {
                auto ticket = module.getTicketById(ticketId);
                if (ticket) {
                    displayTicket(ticket, false);
                    displaySeparator();
                }
            }
        } else {
            std::cout << "Failed to confirm reservation" << std::endl;
        }
    } else {
        std::cout << "Failed to create reservation" << std::endl;
    }
    std::cout << std::endl;
    
    // Test releasing a reservation
    std::cout << "3. Testing reservation release:" << std::endl;
    std::string testReservationId = module.reserveTickets(2, 2, 10);
    if (!testReservationId.empty()) {
        std::cout << "Created test reservation: " << testReservationId << std::endl;
        bool released = module.releaseReservation(testReservationId);
        if (released) {
            std::cout << "Reservation released successfully" << std::endl;
        } else {
            std::cout << "Failed to release reservation" << std::endl;
        }
    }
    std::cout << std::endl;
}

// Test Availability operations
void testAvailabilityOperations(TicketManager::TicketModule& module) {
    displayHeader("TICKET AVAILABILITY OPERATIONS TEST");
    
    // Check available ticket count
    std::cout << "1. Checking available tickets for concert 1:" << std::endl;
    int available = module.getAvailableTicketCount(1);
    std::cout << "Available tickets: " << available << std::endl;
    std::cout << std::endl;
    
    // Check if tickets are on sale
    std::cout << "2. Checking if tickets are on sale for concert 1:" << std::endl;
    bool onSale = module.areTicketsOnSale(1);
    std::cout << "Tickets on sale: " << (onSale ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Check available tickets for a different concert
    std::cout << "3. Checking available tickets for concert 2:" << std::endl;
    int available2 = module.getAvailableTicketCount(2);
    std::cout << "Available tickets: " << available2 << std::endl;
    std::cout << std::endl;
}

// Test DELETE operations
void testDeleteOperations(TicketManager::TicketModule& module, const std::vector<std::shared_ptr<Model::Ticket>>& testTickets) {
    displayHeader("DELETE OPERATIONS TEST");
    
    if (testTickets.size() < 2) {
        std::cout << "Not enough tickets for delete testing." << std::endl;
        return;
    }
    
    // Cancel a ticket
    int ticketIdToCancel = testTickets[1]->ticket_id;
    std::cout << "1. Cancelling ticket ID " << ticketIdToCancel << ":" << std::endl;
    
    // Verify it exists first
    auto ticketBeforeCancel = module.getTicketById(ticketIdToCancel);
    if (ticketBeforeCancel) {
        std::cout << "Ticket exists before cancellation:" << std::endl;
        displayTicket(ticketBeforeCancel, false);
        
        bool cancelResult = module.cancelTicket(ticketIdToCancel, "Test cancellation");
        if (cancelResult) {
            std::cout << "Ticket cancelled successfully!" << std::endl;
            
            // Verify status changed
            auto ticketAfterCancel = module.getTicketById(ticketIdToCancel);
            if (ticketAfterCancel && ticketAfterCancel->status == Model::TicketStatus::CANCELLED) {
                std::cout << "Verified: Ticket status changed to CANCELLED" << std::endl;
            } else {
                std::cout << "Error: Ticket status not updated correctly!" << std::endl;
            }
        } else {
            std::cout << "Cancel operation failed!" << std::endl;
        }
    } else {
        std::cout << "Error: Ticket doesn't exist before cancellation!" << std::endl;
    }
    std::cout << std::endl;
    
    // Test cancelling non-existent ticket
    std::cout << "2. Testing cancellation of non-existent ticket (ID 7777):" << std::endl;
    bool failedCancel = module.cancelTicket(7777, "Test");
    if (!failedCancel) {
        std::cout << "Correctly failed to cancel non-existent ticket" << std::endl;
    } else {
        std::cout << "Unexpected: Successfully cancelled non-existent ticket" << std::endl;
    }
    std::cout << std::endl;
}

// Test edge cases and error conditions
void testEdgeCases(TicketManager::TicketModule& module) {
    displayHeader("EDGE CASES AND ERROR CONDITIONS TEST");
    
    // Test with invalid attendee/concert IDs
    std::cout << "1. Testing ticket creation with invalid IDs:" << std::endl;
    int invalidTicket = module.createTicket(-1, -1, "Invalid");
    if (invalidTicket == -1) {
        std::cout << "Correctly rejected invalid ticket creation" << std::endl;
    } else {
        std::cout << "Unexpected: Created ticket with invalid IDs" << std::endl;
    }
    std::cout << std::endl;
    
    // Test querying with non-existent IDs
    std::cout << "2. Testing queries with non-existent IDs:" << std::endl;
    auto noTickets = module.getTicketsByAttendee(99999);
    std::cout << "Tickets for non-existent attendee: " << noTickets.size() << std::endl;
    
    auto noConcertTickets = module.getTicketsByConcert(99999);
    std::cout << "Tickets for non-existent concert: " << noConcertTickets.size() << std::endl;
    std::cout << std::endl;
    
    // Test invalid QR code validation
    std::cout << "3. Testing invalid QR code validation:" << std::endl;
    int invalidQRResult = module.validateQRCode("invalid_qr_code");
    if (invalidQRResult == -1) {
        std::cout << "Correctly rejected invalid QR code" << std::endl;
    } else {
        std::cout << "Unexpected: Validated invalid QR code" << std::endl;
    }
    std::cout << std::endl;
}

// Main test function
int main() {
    displayHeader("TICKET MODULE COMPREHENSIVE TEST");
    
    try {
        // Initialize the module
        TicketManager::TicketModule ticketModule("test_tickets.dat");
        std::cout << "TicketModule initialized successfully!" << std::endl;
        displaySeparator();
        
        // Test CREATE operations
        auto testTickets = testCreateOperation(ticketModule);
        
        // Test READ operations
        testReadOperations(ticketModule, testTickets);
        
        // Test UPDATE operations
        testUpdateOperations(ticketModule, testTickets);
        
        // Test QR Code operations
        testQRCodeOperations(ticketModule, testTickets);
        
        // Test Reservation operations
        testReservationOperations(ticketModule);
        
        // Test Availability operations
        testAvailabilityOperations(ticketModule);
        
        // Test Edge Cases
        testEdgeCases(ticketModule);
        
        // Test DELETE operations (last to preserve test data)
        testDeleteOperations(ticketModule, testTickets);
        
        displayHeader("ALL TESTS COMPLETED SUCCESSFULLY");
        std::cout << "Ticket Module testing completed without critical errors." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during testing: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught during testing." << std::endl;
        return 1;
    }
    
    return 0;
}
