#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include <random>
#include "../include/models.hpp"
#include "../include/ticketModule.hpp"
#include "../include/concertModule.hpp"
#include "../include/attendeeModule.hpp"
#include "../include/venueModule.hpp"

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
        case Model::TicketStatus::AVAILABLE:   return "AVAILABLE";
        case Model::TicketStatus::RESERVED:    return "RESERVED";
        case Model::TicketStatus::SOLD:        return "SOLD";
        case Model::TicketStatus::CHECKED_IN:  return "CHECKED_IN";
        case Model::TicketStatus::CANCELLED:   return "CANCELLED";
        case Model::TicketStatus::EXPIRED:     return "EXPIRED";
        default:                              return "UNKNOWN";
    }
}

// Generate a random string (for testing QR codes)
std::string generateRandomString(size_t length) {
    static const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, charset.size() - 1);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[distribution(generator)];
    }
    
    return result;
}

// Detailed display of a single ticket
void displayTicket(const std::shared_ptr<Model::Ticket>& ticket, bool detailed = true) {
    if (!ticket) {
        std::cout << "[NULL TICKET REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Ticket ID: " << ticket->ticket_id << std::endl;
    std::cout << "Concert ID: " << ticket->concert_id << std::endl;
    
    if (detailed) {
        std::cout << "Price: $" << std::fixed << std::setprecision(2) << ticket->price << std::endl;
        std::cout << "Status: " << getTicketStatusString(ticket->status) << std::endl;
        std::cout << "Seat ID: " << ticket->seat_id << std::endl;
        std::cout << "Ticket Type: " << ticket->ticket_type << std::endl;
        std::cout << "Attendee ID: " << (ticket->attendee_id > 0 ? std::to_string(ticket->attendee_id) : "Not assigned") << std::endl;
        std::cout << "Purchase Date: " << (ticket->purchase_date.year > 0 ? ticket->purchase_date.iso8601String : "N/A") << std::endl;
        std::cout << "QR Code: " << (!ticket->qr_code.empty() ? ticket->qr_code : "[Not generated]") << std::endl;
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
    std::cout << std::setw(6) << "ID"
              << std::setw(10) << "Concert"
              << std::setw(8) << "Seat"
              << std::setw(10) << "Type"
              << std::setw(15) << "Status"
              << std::setw(12) << "Price"
              << std::setw(12) << "Attendee" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& ticket : tickets) {
        std::cout << std::setw(6) << ticket->ticket_id
                  << std::setw(10) << ticket->concert_id
                  << std::setw(8) << ticket->seat_id
                  << std::setw(10) << ticket->ticket_type
                  << std::setw(15) << getTicketStatusString(ticket->status)
                  << std::setw(12) << "$" << std::fixed << std::setprecision(2) << ticket->price
                  << std::setw(12) << (ticket->attendee_id > 0 ? std::to_string(ticket->attendee_id) : "N/A") << std::endl;
    }
    displaySeparator();
}

// Create test data for ticket testing
void createTestData(
    ConcertModule& concertModule,
    VenueModule& venueModule,
    AttendeeModule& attendeeModule,
    std::shared_ptr<Model::Concert>& testConcert,
    std::shared_ptr<Model::Venue>& testVenue,
    std::vector<std::shared_ptr<Model::Attendee>>& testAttendees
) {
    // Create a test venue
    testVenue = venueModule.createVenue(
        "Test Concert Hall",
        "123 Test St",
        "Test City",
        "TC",
        "12345",
        "Test Country",
        100,
        "Test venue for ticket module testing",
        "test@venue.com"
    );
    
    // Add some seats to the venue
    for (int i = 1; i <= 5; i++) {
        venueModule.addSeat(testVenue->id, "VIP", "A", std::to_string(i));
    }
    for (int i = 1; i <= 10; i++) {
        venueModule.addSeat(testVenue->id, "Regular", "B", std::to_string(i));
    }
    for (int i = 1; i <= 10; i++) {
        venueModule.addSeat(testVenue->id, "Budget", "C", std::to_string(i));
    }
    
    // Create a test concert
    Model::DateTime startDate;
    startDate.fromIso8601String("2024-09-15T19:00:00");
    
    Model::DateTime endDate;
    endDate.fromIso8601String("2024-09-15T22:00:00");
    
    testConcert = concertModule.createConcert(
        "Test Concert",
        testVenue->id,
        "A test concert for ticket module",
        startDate,
        endDate,
        Model::EventStatus::SCHEDULED
    );
    
    // Create some test attendees
    testAttendees.push_back(
        attendeeModule.createAttendee(
            "John",
            "Doe",
            "john.doe@example.com",
            "123-456-7890",
            "123 Main St",
            "Any City",
            "AC",
            "12345",
            "Test Country"
        )
    );
    
    testAttendees.push_back(
        attendeeModule.createAttendee(
            "Jane",
            "Smith",
            "jane.smith@example.com",
            "987-654-3210",
            "456 Oak St",
            "Other City",
            "OC",
            "54321",
            "Test Country"
        )
    );
    
    testAttendees.push_back(
        attendeeModule.createAttendee(
            "Bob",
            "Johnson",
            "bob.johnson@example.com",
            "555-555-5555",
            "789 Pine St",
            "Third City",
            "TC",
            "67890",
            "Test Country"
        )
    );
}

// Test ticket generation
std::vector<std::shared_ptr<Model::Ticket>> testTicketGeneration(
    TicketModule& ticketModule,
    const std::shared_ptr<Model::Concert>& concert,
    const std::shared_ptr<Model::Venue>& venue
) {
    displayHeader("TICKET GENERATION TEST");
    
    std::cout << "Generating tickets for concert ID " << concert->id 
              << " (" << concert->name << ") at venue ID " 
              << venue->id << " (" << venue->name << ")" << std::endl;
    
    // Get all seats for the venue
    VenueModule venueModule;
    auto allSeats = venueModule.getSeatsForVenue(venue->id);
    
    // Define pricing for different seat types
    double vipPrice = 199.99;
    double regularPrice = 99.99;
    double budgetPrice = 49.99;
    
    // Generate tickets
    std::vector<std::shared_ptr<Model::Ticket>> allTickets;
    
    // For VIP seats
    auto vipSeats = std::vector<std::shared_ptr<Model::Seat>>();
    for (const auto& seat : allSeats) {
        if (seat->seat_type == "VIP") {
            vipSeats.push_back(seat);
        }
    }
    
    if (!vipSeats.empty()) {
        std::cout << "Generating VIP tickets..." << std::endl;
        auto vipTickets = ticketModule.generateTicketsForConcert(
            concert->id,
            vipSeats,
            "VIP",
            vipPrice
        );
        allTickets.insert(allTickets.end(), vipTickets.begin(), vipTickets.end());
        std::cout << "Generated " << vipTickets.size() << " VIP tickets at $" << vipPrice << " each." << std::endl;
    }
    
    // For Regular seats
    auto regularSeats = std::vector<std::shared_ptr<Model::Seat>>();
    for (const auto& seat : allSeats) {
        if (seat->seat_type == "Regular") {
            regularSeats.push_back(seat);
        }
    }
    
    if (!regularSeats.empty()) {
        std::cout << "Generating Regular tickets..." << std::endl;
        auto regularTickets = ticketModule.generateTicketsForConcert(
            concert->id,
            regularSeats,
            "Regular",
            regularPrice
        );
        allTickets.insert(allTickets.end(), regularTickets.begin(), regularTickets.end());
        std::cout << "Generated " << regularTickets.size() << " Regular tickets at $" << regularPrice << " each." << std::endl;
    }
    
    // For Budget seats
    auto budgetSeats = std::vector<std::shared_ptr<Model::Seat>>();
    for (const auto& seat : allSeats) {
        if (seat->seat_type == "Budget") {
            budgetSeats.push_back(seat);
        }
    }
    
    if (!budgetSeats.empty()) {
        std::cout << "Generating Budget tickets..." << std::endl;
        auto budgetTickets = ticketModule.generateTicketsForConcert(
            concert->id,
            budgetSeats,
            "Budget",
            budgetPrice
        );
        allTickets.insert(allTickets.end(), budgetTickets.begin(), budgetTickets.end());
        std::cout << "Generated " << budgetTickets.size() << " Budget tickets at $" << budgetPrice << " each." << std::endl;
    }
    
    std::cout << "\nTotal tickets generated: " << allTickets.size() << std::endl;
    
    // Display a few sample tickets
    std::cout << "\nSample tickets:" << std::endl;
    int samplesToShow = std::min(3, static_cast<int>(allTickets.size()));
    for (int i = 0; i < samplesToShow; i++) {
        displaySeparator();
        displayTicket(allTickets[i]);
    }
    
    return allTickets;
}

// Test READ operations
void testReadOperations(
    TicketModule& ticketModule,
    const std::shared_ptr<Model::Concert>& concert,
    const std::vector<std::shared_ptr<Model::Ticket>>& tickets
) {
    displayHeader("READ OPERATIONS TEST");
    
    // 1. Get all tickets
    std::cout << "1. Getting all tickets:" << std::endl;
    auto allTickets = ticketModule.getAllTickets();
    std::cout << "Total tickets in system: " << allTickets.size() << std::endl;
    
    // 2. Get tickets by ID
    if (!tickets.empty()) {
        int idToFind = tickets[0]->ticket_id;
        std::cout << "\n2. Getting ticket by ID " << idToFind << ":" << std::endl;
        auto foundTicket = ticketModule.getTicketById(idToFind);
        if (foundTicket) {
            displayTicket(foundTicket);
        } else {
            std::cout << "Ticket not found!" << std::endl;
        }
    }
    
    // 3. Find tickets by concert ID
    std::cout << "\n3. Finding tickets for concert ID " << concert->id << ":" << std::endl;
    auto concertTickets = ticketModule.getTicketsByConcert(concert->id);
    std::cout << "Found " << concertTickets.size() << " tickets for this concert." << std::endl;
    
    // Display a sample of tickets by type
    std::cout << "\n4. Sample of tickets by type:" << std::endl;
    
    // For each ticket type, find and display one
    std::map<std::string, bool> displayedTypes;
    for (const auto& ticket : tickets) {
        if (!displayedTypes[ticket->ticket_type]) {
            std::cout << "\nTicket type: " << ticket->ticket_type << std::endl;
            displayTicket(ticket);
            displayedTypes[ticket->ticket_type] = true;
        }
    }
    
    // 5. Find available tickets
    std::cout << "\n5. Finding available tickets for concert ID " << concert->id << ":" << std::endl;
    auto availableTickets = ticketModule.getAvailableTickets(concert->id);
    std::cout << "Found " << availableTickets.size() << " available tickets." << std::endl;
    
    if (availableTickets.size() > 0) {
        std::cout << "Sample of available tickets:" << std::endl;
        displayTicketList({availableTickets[0]});
    }
}

// Test ticket assignment and status update
void testTicketAssignment(
    TicketModule& ticketModule,
    const std::vector<std::shared_ptr<Model::Ticket>>& tickets,
    const std::vector<std::shared_ptr<Model::Attendee>>& attendees
) {
    displayHeader("TICKET ASSIGNMENT TEST");
    
    if (tickets.empty() || attendees.empty()) {
        std::cout << "Not enough test data available for ticket assignment testing." << std::endl;
        return;
    }
    
    // 1. Assign a ticket to an attendee
    auto ticket1 = tickets[0];
    auto attendee1 = attendees[0];
    
    std::cout << "1. Assigning ticket ID " << ticket1->ticket_id 
              << " to attendee " << attendee1->first_name << " " << attendee1->last_name 
              << " (ID: " << attendee1->id << ")" << std::endl;
    
    std::cout << "Before assignment:" << std::endl;
    displayTicket(ticket1);
    
    bool assignmentResult = ticketModule.assignTicketToAttendee(
        ticket1->ticket_id, 
        attendee1->id
    );
    
    std::cout << "\nAssignment result: " << (assignmentResult ? "Success" : "Failed") << std::endl;
    
    if (assignmentResult) {
        std::cout << "\nAfter assignment:" << std::endl;
        auto updatedTicket = ticketModule.getTicketById(ticket1->ticket_id);
        displayTicket(updatedTicket);
    }
    displaySeparator();
    
    // 2. Assign another ticket to another attendee
    if (tickets.size() > 1 && attendees.size() > 1) {
        auto ticket2 = tickets[1];
        auto attendee2 = attendees[1];
        
        std::cout << "2. Assigning ticket ID " << ticket2->ticket_id 
                << " to attendee " << attendee2->first_name << " " << attendee2->last_name 
                << " (ID: " << attendee2->id << ")" << std::endl;
        
        bool assignmentResult2 = ticketModule.assignTicketToAttendee(
            ticket2->ticket_id, 
            attendee2->id
        );
        
        std::cout << "Assignment result: " << (assignmentResult2 ? "Success" : "Failed") << std::endl;
        
        if (assignmentResult2) {
            auto updatedTicket2 = ticketModule.getTicketById(ticket2->ticket_id);
            displayTicket(updatedTicket2);
        }
        displaySeparator();
    }
    
    // 3. Get tickets by attendee
    std::cout << "3. Getting tickets for attendee ID " << attendee1->id << ":" << std::endl;
    auto attendeeTickets = ticketModule.getTicketsByAttendee(attendee1->id);
    displayTicketList(attendeeTickets);
    displaySeparator();
    
    // 4. Generate QR codes for assigned tickets
    std::cout << "4. Generating QR codes for assigned tickets:" << std::endl;
    
    for (const auto& ticket : attendeeTickets) {
        std::string qrCode = generateRandomString(20); // Simulate QR code generation
        std::cout << "Generating QR code for ticket ID " << ticket->ticket_id << ": " << qrCode << std::endl;
        
        bool qrResult = ticketModule.updateTicketQRCode(ticket->ticket_id, qrCode);
        std::cout << "QR code update result: " << (qrResult ? "Success" : "Failed") << std::endl;
        
        if (qrResult) {
            auto updatedTicket = ticketModule.getTicketById(ticket->ticket_id);
            std::cout << "Updated QR code: " << updatedTicket->qr_code << std::endl;
        }
        displaySeparator('-', 30);
    }
}

// Test ticket validation
void testTicketValidation(
    TicketModule& ticketModule,
    const std::vector<std::shared_ptr<Model::Ticket>>& tickets
) {
    displayHeader("TICKET VALIDATION TEST");
    
    // Find a ticket with QR code
    std::shared_ptr<Model::Ticket> ticketWithQR = nullptr;
    for (const auto& ticket : tickets) {
        if (!ticket->qr_code.empty()) {
            ticketWithQR = ticket;
            break;
        }
    }
    
    if (!ticketWithQR) {
        std::cout << "No tickets with QR codes found for validation testing." << std::endl;
        return;
    }
    
    // 1. Validate a valid ticket
    std::cout << "1. Validating ticket with QR code: " << ticketWithQR->qr_code << std::endl;
    
    auto validationResult = ticketModule.validateTicketByQRCode(ticketWithQR->qr_code);
    std::cout << "Validation result: " << (validationResult ? "Valid" : "Invalid") << std::endl;
    
    if (validationResult) {
        std::cout << "Validated ticket details:" << std::endl;
        displayTicket(validationResult);
    }
    displaySeparator();
    
    // 2. Check-in the ticket
    std::cout << "2. Checking in ticket ID " << ticketWithQR->ticket_id << std::endl;
    
    bool checkInResult = ticketModule.checkInTicket(ticketWithQR->ticket_id);
    std::cout << "Check-in result: " << (checkInResult ? "Success" : "Failed") << std::endl;
    
    if (checkInResult) {
        std::cout << "Ticket status after check-in:" << std::endl;
        auto updatedTicket = ticketModule.getTicketById(ticketWithQR->ticket_id);
        displayTicket(updatedTicket);
    }
    displaySeparator();
    
    // 3. Try to validate an invalid QR code
    std::cout << "3. Validating invalid QR code: INVALID_QR_CODE_12345" << std::endl;
    
    auto invalidValidation = ticketModule.validateTicketByQRCode("INVALID_QR_CODE_12345");
    std::cout << "Validation result: " << (invalidValidation ? "Valid (unexpected)" : "Invalid (expected)") << std::endl;
    displaySeparator();
    
    // 4. Try to check-in an already checked-in ticket
    if (checkInResult) {
        std::cout << "4. Attempting to check-in an already checked-in ticket (ID: " << ticketWithQR->ticket_id << ")" << std::endl;
        
        bool secondCheckIn = ticketModule.checkInTicket(ticketWithQR->ticket_id);
        std::cout << "Second check-in result: " << (secondCheckIn ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
    }
}

// Test ticket cancellation and refund
void testTicketCancellation(
    TicketModule& ticketModule,
    const std::vector<std::shared_ptr<Model::Ticket>>& tickets
) {
    displayHeader("TICKET CANCELLATION TEST");
    
    // Find a ticket that is assigned to an attendee but not checked in
    std::shared_ptr<Model::Ticket> ticketToCancel = nullptr;
    for (const auto& ticket : tickets) {
        if (ticket->attendee_id > 0 && ticket->status != Model::TicketStatus::CHECKED_IN) {
            ticketToCancel = ticket;
            break;
        }
    }
    
    if (!ticketToCancel) {
        std::cout << "No suitable tickets found for cancellation testing." << std::endl;
        return;
    }
    
    // 1. Cancel the ticket
    std::cout << "1. Cancelling ticket ID " << ticketToCancel->ticket_id << std::endl;
    std::cout << "Before cancellation:" << std::endl;
    displayTicket(ticketToCancel);
    
    bool cancellationResult = ticketModule.cancelTicket(ticketToCancel->ticket_id);
    std::cout << "\nCancellation result: " << (cancellationResult ? "Success" : "Failed") << std::endl;
    
    if (cancellationResult) {
        std::cout << "\nAfter cancellation:" << std::endl;
        auto updatedTicket = ticketModule.getTicketById(ticketToCancel->ticket_id);
        displayTicket(updatedTicket);
    }
    displaySeparator();
    
    // 2. Verify the ticket is now available for re-sale
    std::cout << "2. Checking if ticket can be reassigned after cancellation:" << std::endl;
    
    auto availableTickets = ticketModule.getAvailableTickets(ticketToCancel->concert_id);
    bool isAvailable = false;
    
    for (const auto& ticket : availableTickets) {
        if (ticket->ticket_id == ticketToCancel->ticket_id) {
            isAvailable = true;
            break;
        }
    }
    
    std::cout << "Ticket is " << (isAvailable ? "available" : "not available") << " for reassignment." << std::endl;
}

// Test bulk operations and ticket statistics
void testBulkOperations(
    TicketModule& ticketModule,
    const std::shared_ptr<Model::Concert>& concert
) {
    displayHeader("BULK OPERATIONS AND STATISTICS TEST");
    
    // 1. Get ticket statistics for a concert
    std::cout << "1. Ticket statistics for concert ID " << concert->id << ":" << std::endl;
    
    auto allTicketsForConcert = ticketModule.getTicketsByConcert(concert->id);
    int totalTickets = allTicketsForConcert.size();
    
    int availableCount = 0;
    int soldCount = 0;
    int checkedInCount = 0;
    int cancelledCount = 0;
    double totalRevenue = 0.0;
    
    std::map<std::string, int> ticketsByType;
    
    for (const auto& ticket : allTicketsForConcert) {
        // Count by status
        switch (ticket->status) {
            case Model::TicketStatus::AVAILABLE:
                availableCount++;
                break;
            case Model::TicketStatus::SOLD:
                soldCount++;
                if (ticket->price > 0) {
                    totalRevenue += ticket->price;
                }
                break;
            case Model::TicketStatus::CHECKED_IN:
                checkedInCount++;
                if (ticket->price > 0) {
                    totalRevenue += ticket->price;
                }
                break;
            case Model::TicketStatus::CANCELLED:
                cancelledCount++;
                break;
            default:
                break;
        }
        
        // Count by type
        ticketsByType[ticket->ticket_type]++;
    }
    
    // Display statistics
    std::cout << "Total tickets: " << totalTickets << std::endl;
    std::cout << "Available tickets: " << availableCount << " (" 
              << (totalTickets > 0 ? (availableCount * 100.0 / totalTickets) : 0) << "%)" << std::endl;
    std::cout << "Sold tickets: " << soldCount << " (" 
              << (totalTickets > 0 ? (soldCount * 100.0 / totalTickets) : 0) << "%)" << std::endl;
    std::cout << "Checked-in tickets: " << checkedInCount << " (" 
              << (totalTickets > 0 ? (checkedInCount * 100.0 / totalTickets) : 0) << "%)" << std::endl;
    std::cout << "Cancelled tickets: " << cancelledCount << " (" 
              << (totalTickets > 0 ? (cancelledCount * 100.0 / totalTickets) : 0) << "%)" << std::endl;
    std::cout << "Total revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
    
    std::cout << "\nTickets by type:" << std::endl;
    for (const auto& pair : ticketsByType) {
        std::cout << "  " << pair.first << ": " << pair.second << " tickets" << std::endl;
    }
    displaySeparator();
    
    // 2. Bulk status update (if there are available tickets)
    if (availableCount > 0) {
        std::cout << "2. Performing bulk status update (marking some available tickets as reserved):" << std::endl;
        
        int updateCount = std::min(3, availableCount);
        int successCount = 0;
        
        for (const auto& ticket : allTicketsForConcert) {
            if (ticket->status == Model::TicketStatus::AVAILABLE && successCount < updateCount) {
                bool updateResult = ticketModule.updateTicketStatus(ticket->ticket_id, Model::TicketStatus::RESERVED);
                
                if (updateResult) {
                    successCount++;
                    std::cout << "Reserved ticket ID " << ticket->ticket_id << std::endl;
                }
            }
            
            if (successCount >= updateCount) {
                break;
            }
        }
        
        std::cout << "Successfully reserved " << successCount << " tickets." << std::endl;
    }
}

// Test DELETE operations
void testDeleteOperations(
    TicketModule& ticketModule,
    const std::vector<std::shared_ptr<Model::Ticket>>& tickets
) {
    displayHeader("DELETE OPERATIONS TEST");
    
    if (tickets.empty()) {
        std::cout << "No tickets available for delete testing." << std::endl;
        return;
    }
    
    // Find a ticket to delete (preferably one that's available)
    std::shared_ptr<Model::Ticket> ticketToDelete = nullptr;
    for (const auto& ticket : tickets) {
        if (ticket->status == Model::TicketStatus::AVAILABLE) {
            ticketToDelete = ticket;
            break;
        }
    }
    
    // If no available ticket found, use the last ticket
    if (!ticketToDelete && !tickets.empty()) {
        ticketToDelete = tickets.back();
    }
    
    if (!ticketToDelete) {
        std::cout << "No tickets available for deletion." << std::endl;
        return;
    }
    
    // 1. Delete a ticket
    std::cout << "1. Deleting ticket with ID " << ticketToDelete->ticket_id << ":" << std::endl;
    displayTicket(ticketToDelete);
    
    bool deleteResult = ticketModule.deleteTicket(ticketToDelete->ticket_id);
    std::cout << "\nDeletion result: " << (deleteResult ? "Success" : "Failed") << std::endl;
    
    // 2. Verify the ticket is deleted
    if (deleteResult) {
        std::cout << "\n2. Verifying ticket deletion:" << std::endl;
        auto verifyTicket = ticketModule.getTicketById(ticketToDelete->ticket_id);
        
        if (!verifyTicket) {
            std::cout << "Verification successful: Ticket no longer exists." << std::endl;
        } else {
            std::cout << "Verification failed: Ticket still exists!" << std::endl;
        }
    }
    
    // 3. Try to delete a non-existent ticket
    std::cout << "\n3. Attempting to delete non-existent ticket (ID 9999):" << std::endl;
    bool nonExistentDelete = ticketModule.deleteTicket(9999);
    std::cout << "Delete result: " << (nonExistentDelete ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
}

// Main testing function
int main() {
    std::cout << "\n\n";
    displayHeader("TICKET MODULE COMPREHENSIVE TEST");
    std::cout << "Date: " << Model::DateTime::now().iso8601String << "\n\n";
    
    // Create module instances
    TicketModule ticketModule;
    ConcertModule concertModule;
    VenueModule venueModule;
    AttendeeModule attendeeModule;
    
    // Test data
    std::shared_ptr<Model::Concert> testConcert;
    std::shared_ptr<Model::Venue> testVenue;
    std::vector<std::shared_ptr<Model::Attendee>> testAttendees;
    std::vector<std::shared_ptr<Model::Ticket>> testTickets;
    
    try {
        // Set up test data
        std::cout << "Setting up test data..." << std::endl;
        createTestData(concertModule, venueModule, attendeeModule, testConcert, testVenue, testAttendees);
        std::cout << "Test data setup complete.\n\n";
        
        // Test ticket generation
        testTickets = testTicketGeneration(ticketModule, testConcert, testVenue);
        std::cout << "\n\n";
        
        // Test READ operations
        testReadOperations(ticketModule, testConcert, testTickets);
        std::cout << "\n\n";
        
        // Test ticket assignment
        testTicketAssignment(ticketModule, testTickets, testAttendees);
        std::cout << "\n\n";
        
        // Test ticket validation
        testTicketValidation(ticketModule, testTickets);
        std::cout << "\n\n";
        
        // Test ticket cancellation
        testTicketCancellation(ticketModule, testTickets);
        std::cout << "\n\n";
        
        // Test bulk operations and statistics
        testBulkOperations(ticketModule, testConcert);
        std::cout << "\n\n";
        
        // Test DELETE operations
        testDeleteOperations(ticketModule, testTickets);
        
        displayHeader("TEST COMPLETED SUCCESSFULLY");
    }
    catch (const std::exception& e) {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
