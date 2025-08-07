#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include <cassert>
#include "../include/models.hpp"
#include "../include/venueModule.hpp"

// Test result counters
int testsPassed = 0;
int testsFailed = 0;

// Utility function to display a separator line
void displaySeparator(char symbol = '-', int length = 60) {
    std::cout << std::string(length, symbol) << std::endl;
}

// Utility function to display a section header
void displayHeader(const std::string& title) {
    displaySeparator('=');
    std::cout << "  " << title << std::endl;
    displaySeparator('=');
}

// Utility function to display test results
void displayTestResult(const std::string& testName, bool passed, const std::string& details = "") {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName;
    if (!details.empty()) {
        std::cout << " - " << details;
    }
    std::cout << std::endl;
    
    if (passed) {
        testsPassed++;
    } else {
        testsFailed++;
    }
}

// Utility function for displaying ticket status
std::string getTicketStatusString(Model::TicketStatus status) {
    switch (status) {
        case Model::TicketStatus::AVAILABLE:    return "AVAILABLE";
        case Model::TicketStatus::SOLD:         return "SOLD";
        case Model::TicketStatus::CHECKED_IN:   return "CHECKED_IN";
        case Model::TicketStatus::CANCELLED:    return "CANCELLED";
        case Model::TicketStatus::EXPIRED:      return "EXPIRED";
        default:                                return "UNKNOWN";
    }
}

// Detailed display of a single venue
void displayVenue(const std::shared_ptr<Model::Venue>& venue, bool detailed = true) {
    if (!venue) {
        std::cout << "[NULL VENUE REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Venue ID: " << venue->id << std::endl;
    std::cout << "Name: " << venue->name << std::endl;
    
    if (detailed) {
        std::cout << "Address: " << venue->address << std::endl;
        std::cout << "City: " << venue->city << ", " << venue->state << " " << venue->zip_code << std::endl;
        std::cout << "Country: " << venue->country << std::endl;
        std::cout << "Capacity: " << venue->capacity << std::endl;
        std::cout << "Description: " << (venue->description.empty() ? "[None]" : venue->description) << std::endl;
        std::cout << "Contact: " << (venue->contact_info.empty() ? "[None]" : venue->contact_info) << std::endl;
        std::cout << "Seatmap: " << (venue->seatmap.empty() ? "[None]" : venue->seatmap) << std::endl;
        std::cout << "Seat Count: " << venue->seats.size() << std::endl;
        
        if (venue->rows > 0 && venue->columns > 0) {
            std::cout << "2D Layout: " << venue->rows << " rows × " << venue->columns << " columns" << std::endl;
        } else {
            std::cout << "2D Layout: Not initialized" << std::endl;
        }
    }
}

// Display a collection of venues
void displayVenues(const std::vector<std::shared_ptr<Model::Venue>>& venues, const std::string& title = "Venues") {
    std::cout << "\n" << title << " (" << venues.size() << " found):" << std::endl;
    displaySeparator('-', 40);
    
    if (venues.empty()) {
        std::cout << "No venues found." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < venues.size(); i++) {
        std::cout << "[" << (i + 1) << "] ";
        displayVenue(venues[i], false);
        std::cout << std::endl;
    }
}

// Test basic venue CRUD operations
void testBasicVenueCRUD() {
    displayHeader("Testing Basic Venue CRUD Operations");
    
    // Test 1: Create a venue
    auto venue1 = VenueManager::createVenue(
        "Madison Square Garden",
        "4 Pennsylvania Plaza",
        "New York",
        "NY",
        "10001",
        "USA",
        20000,
        "Iconic entertainment venue in the heart of Manhattan",
        "info@msg.com",
        "https://msg.com/seatmap"
    );
    
    displayTestResult("Create venue", venue1 != nullptr, "Madison Square Garden");
    if (venue1) {
        std::cout << "Created venue details:" << std::endl;
        displayVenue(venue1);
        std::cout << std::endl;
    }
    
    // Test 2: Get venue by ID
    if (venue1) {
        auto retrievedVenue = VenueManager::getVenueById(venue1->id);
        displayTestResult("Get venue by ID", 
            retrievedVenue != nullptr && retrievedVenue->id == venue1->id,
            "ID: " + std::to_string(venue1->id));
    }
    
    // Test 3: Create another venue for testing
    auto venue2 = VenueManager::createVenue(
        "Apollo Theater",
        "253 W 125th St",
        "New York",
        "NY",
        "10027",
        "USA",
        1506,
        "Historic theater in Harlem",
        "contact@apollotheater.org"
    );
    
    displayTestResult("Create second venue", venue2 != nullptr, "Apollo Theater");
    
    // Test 4: Find venues by name
    auto foundByName = VenueManager::findVenuesByName("madison");
    displayTestResult("Find venues by name (case insensitive)", 
        foundByName.size() >= 1, "Found " + std::to_string(foundByName.size()) + " venue(s)");
    
    // Test 5: Find venues by city
    auto foundByCity = VenueManager::findVenuesByCity("New York");
    displayTestResult("Find venues by city", 
        foundByCity.size() >= 2, "Found " + std::to_string(foundByCity.size()) + " venue(s) in NYC");
    
    // Test 6: Find venues by capacity
    auto foundByCapacity = VenueManager::findVenuesByCapacity(10000);
    displayTestResult("Find venues by minimum capacity", 
        foundByCapacity.size() >= 1, "Found " + std::to_string(foundByCapacity.size()) + " large venue(s)");
    
    // Test 7: Update venue
    bool updateSuccess = false;
    if (venue1) {
        updateSuccess = VenueManager::updateVenue(
            venue1->id,
            "", // Don't change name
            "", // Don't change address
            "", // Don't change city
            "", // Don't change state
            "", // Don't change zip
            "", // Don't change country
            20100, // Update capacity
            "The world's most famous arena - updated!", // Update description
            "newinfo@msg.com" // Update contact
        );
    }
    displayTestResult("Update venue", updateSuccess, "Updated capacity and description");
    
    if (updateSuccess && venue1) {
        auto updatedVenue = VenueManager::getVenueById(venue1->id);
        std::cout << "Updated venue details:" << std::endl;
        displayVenue(updatedVenue);
        std::cout << std::endl;
    }
}

// Test 2D seating plan functionality
void test2DSeatingPlan() {
    displayHeader("Testing 2D Seating Plan Functionality");
    
    // Create a test venue for 2D operations
    auto testVenue = VenueManager::createVenue(
        "Test Theater",
        "123 Test Street",
        "Test City",
        "TS",
        "12345",
        "USA",
        100,
        "Theater for testing 2D seating"
    );
    
    displayTestResult("Create test venue for 2D operations", testVenue != nullptr);
    
    if (!testVenue) {
        std::cout << "Cannot proceed with 2D tests - venue creation failed" << std::endl;
        return;
    }
    
    int venueId = testVenue->id;
    
    // Test 1: Initialize 2D seating plan
    bool initSuccess = VenueManager::initializeVenueSeatingPlan(venueId, 5, 10);
    displayTestResult("Initialize 2D seating plan (5x10)", initSuccess);
    
    // Test 2: Create standard seating plan
    bool createSuccess = VenueManager::createStandardSeatingPlan(venueId, 5, 10, "Regular");
    displayTestResult("Create standard seating plan", createSuccess, "50 seats (5 rows × 10 columns)");
    
    // Test 3: Get venue statistics
    std::string stats = VenueManager::getVenueSeatingStats(venueId);
    displayTestResult("Get venue seating statistics", !stats.empty());
    std::cout << "\nVenue Statistics:\n" << stats << std::endl;
    
    // Test 4: Get seat at specific coordinates
    auto seatA5 = VenueManager::getSeatAt(venueId, 0, 4); // Row A, Column 5
    displayTestResult("Get seat at specific coordinates (A5)", 
        seatA5 != nullptr && seatA5->row_number == "A" && seatA5->col_number == "5");
    
    if (seatA5) {
        std::cout << "Seat A5 details: " << seatA5->row_number << seatA5->col_number 
                  << " (ID: " << seatA5->seat_id << ", Type: " << seatA5->seat_type 
                  << ", Status: " << getTicketStatusString(seatA5->status) << ")" << std::endl;
    }
    
    // Test 5: Get seats in a specific row
    auto rowBSeats = VenueManager::getSeatsInRow(venueId, 1); // Row B (index 1)
    displayTestResult("Get seats in row B", rowBSeats.size() == 10, 
        "Retrieved " + std::to_string(rowBSeats.size()) + " seats");
    
    // Test 6: Get all seats for venue
    auto allSeats = VenueManager::getSeatsForVenue(venueId);
    displayTestResult("Get all seats for venue", allSeats.size() == 50, 
        "Retrieved " + std::to_string(allSeats.size()) + " total seats");
    
    // Test 7: Get available seats
    auto availableSeats = VenueManager::getAvailableSeats(venueId);
    displayTestResult("Get available seats", availableSeats.size() == 50, 
        "All " + std::to_string(availableSeats.size()) + " seats available");
}

// Test advanced 2D operations
void testAdvanced2DOperations() {
    displayHeader("Testing Advanced 2D Operations");
    
    // Create a venue for advanced testing
    auto advancedVenue = VenueManager::createVenue(
        "Advanced Test Arena",
        "456 Advanced Ave",
        "Test City",
        "TS",
        "12346",
        "USA",
        200
    );
    
    if (!advancedVenue) {
        std::cout << "Cannot proceed with advanced tests - venue creation failed" << std::endl;
        return;
    }
    
    int venueId = advancedVenue->id;
    
    // Initialize and create seating
    VenueManager::initializeVenueSeatingPlan(venueId, 8, 12);
    VenueManager::createStandardSeatingPlan(venueId, 8, 12, "Regular");
    
    // Test 1: Find adjacent seats for group booking
    auto adjacentGroups = VenueManager::findAdjacentSeats(venueId, 4);
    displayTestResult("Find adjacent seats (groups of 4)", !adjacentGroups.empty(), 
        "Found " + std::to_string(adjacentGroups.size()) + " possible groups");
    
    // Test 2: Reserve a block of seats
    bool reserveSuccess = false;
    if (!adjacentGroups.empty()) {
        reserveSuccess = VenueManager::reserveSeatBlock(venueId, adjacentGroups[0]);
        displayTestResult("Reserve seat block", reserveSuccess, "Reserved 4 adjacent seats");
        
        if (reserveSuccess) {
            std::cout << "Reserved seats: ";
            for (const auto& seat : adjacentGroups[0]) {
                std::cout << seat->row_number << seat->col_number << " ";
            }
            std::cout << std::endl;
        }
    }
    
    // Test 3: Update seat status manually
    auto testSeat = VenueManager::getSeatAt(venueId, 2, 5); // Row C, Seat 6
    bool statusUpdateSuccess = false;
    if (testSeat) {
        statusUpdateSuccess = VenueManager::updateSeatStatus(venueId, testSeat->seat_id, Model::TicketStatus::CHECKED_IN);
        displayTestResult("Update individual seat status", statusUpdateSuccess, 
            "Set " + testSeat->row_number + testSeat->col_number + " to CHECKED_IN");
    }
    
    // Test 4: Add individual seat
    auto newSeat = VenueManager::addSeat(venueId, "VIP", "Z", "99");
    displayTestResult("Add individual seat", newSeat != nullptr, "Added VIP seat Z99");
    
    // Test 5: Remove a seat
    bool removeSuccess = false;
    if (newSeat) {
        removeSuccess = VenueManager::removeSeat(venueId, newSeat->seat_id);
        displayTestResult("Remove seat", removeSuccess, "Removed seat Z99");
    }
    
    // Test 6: Find adjacent seats after reservations
    auto adjacentAfterReservation = VenueManager::findAdjacentSeats(venueId, 4);
    displayTestResult("Find adjacent seats after reservations", 
        adjacentAfterReservation.size() < adjacentGroups.size(),
        "Found " + std::to_string(adjacentAfterReservation.size()) + " groups (reduced from " + 
        std::to_string(adjacentGroups.size()) + ")");
    
    // Display updated statistics
    std::string updatedStats = VenueManager::getVenueSeatingStats(venueId);
    std::cout << "\nUpdated Venue Statistics:\n" << updatedStats << std::endl;
}

// Test seating plan visualization
void testSeatingPlanVisualization() {
    displayHeader("Testing Seating Plan Visualization");
    
    // Create a smaller venue for easier visualization testing
    auto vizVenue = VenueManager::createVenue(
        "Visualization Test Theater",
        "789 Viz Street",
        "Test City",
        "TS",
        "12347",
        "USA",
        35,
        "Small theater for visualization testing"
    );
    
    if (!vizVenue) {
        std::cout << "Cannot proceed with visualization tests - venue creation failed" << std::endl;
        return;
    }
    
    int venueId = vizVenue->id;
    
    // Test 1: Visualization without initialization
    std::string emptyViz = VenueManager::getSeatingPlanVisualization(venueId);
    displayTestResult("Visualization without 2D initialization", 
        emptyViz.find("not initialized") != std::string::npos);
    
    // Test 2: Initialize and create small seating plan
    VenueManager::initializeVenueSeatingPlan(venueId, 5, 7);
    VenueManager::createStandardSeatingPlan(venueId, 5, 7, "Regular");
    
    std::string initialViz = VenueManager::getSeatingPlanVisualization(venueId);
    displayTestResult("Initial seating plan visualization", !initialViz.empty());
    
    std::cout << "\nInitial Seating Plan:\n" << initialViz << std::endl;
    
    // Test 3: Make some reservations and show updated visualization
    auto groups = VenueManager::findAdjacentSeats(venueId, 3);
    if (!groups.empty()) {
        VenueManager::reserveSeatBlock(venueId, groups[0]);
    }
    
    // Check in some seats
    auto seatB2 = VenueManager::getSeatAt(venueId, 1, 1);
    if (seatB2) {
        VenueManager::updateSeatStatus(venueId, seatB2->seat_id, Model::TicketStatus::CHECKED_IN);
    }
    
    auto seatC4 = VenueManager::getSeatAt(venueId, 2, 3);
    if (seatC4) {
        VenueManager::updateSeatStatus(venueId, seatC4->seat_id, Model::TicketStatus::CANCELLED);
    }
    
    std::string updatedViz = VenueManager::getSeatingPlanVisualization(venueId);
    displayTestResult("Updated seating plan visualization", !updatedViz.empty());
    
    std::cout << "\nUpdated Seating Plan (after reservations):\n" << updatedViz << std::endl;
    
    // Test 4: Final statistics
    std::string finalStats = VenueManager::getVenueSeatingStats(venueId);
    std::cout << "Final Statistics:\n" << finalStats << std::endl;
}

// Test edge cases and error handling
void testEdgeCases() {
    displayHeader("Testing Edge Cases and Error Handling");
    
    // Test 1: Operations on non-existent venue
    auto nonExistentSeat = VenueManager::getSeatAt(99999, 0, 0);
    displayTestResult("Get seat from non-existent venue", nonExistentSeat == nullptr);
    
    bool nonExistentInit = VenueManager::initializeVenueSeatingPlan(99999, 5, 5);
    displayTestResult("Initialize non-existent venue", !nonExistentInit);
    
    std::string nonExistentStats = VenueManager::getVenueSeatingStats(99999);
    displayTestResult("Get stats for non-existent venue", 
        nonExistentStats.find("not found") != std::string::npos);
    
    // Test 2: Invalid coordinates
    auto testVenue = VenueManager::createVenue("Edge Test Venue", "123 Edge St", "Test", "TS", "12348", "USA", 25);
    if (testVenue) {
        VenueManager::initializeVenueSeatingPlan(testVenue->id, 3, 5);
        VenueManager::createStandardSeatingPlan(testVenue->id, 3, 5);
        
        auto invalidSeat = VenueManager::getSeatAt(testVenue->id, 10, 10);
        displayTestResult("Get seat with invalid coordinates", invalidSeat == nullptr);
        
        auto invalidRow = VenueManager::getSeatsInRow(testVenue->id, 10);
        displayTestResult("Get seats from invalid row", invalidRow.empty());
    }
    
    // Test 3: Adjacent seats with invalid parameters
    if (testVenue) {
        auto invalidAdjacent = VenueManager::findAdjacentSeats(testVenue->id, 0);
        displayTestResult("Find adjacent seats with invalid count", invalidAdjacent.empty());
        
        auto tooManyAdjacent = VenueManager::findAdjacentSeats(testVenue->id, 20);
        displayTestResult("Find more adjacent seats than available", tooManyAdjacent.empty());
    }
    
    // Test 4: Reserve empty seat block
    std::vector<std::shared_ptr<Model::Seat>> emptyBlock;
    bool emptyReserve = VenueManager::reserveSeatBlock(testVenue ? testVenue->id : 1, emptyBlock);
    displayTestResult("Reserve empty seat block", !emptyReserve);
}

// Test cleanup and deletion
void testCleanup() {
    displayHeader("Testing Cleanup and Deletion");
    
    // Create a venue for deletion testing
    auto deleteVenue = VenueManager::createVenue(
        "Venue To Delete",
        "Delete Street",
        "Delete City",
        "DL",
        "00000",
        "USA",
        50
    );
    
    bool venueCreated = deleteVenue != nullptr;
    displayTestResult("Create venue for deletion", venueCreated);
    
    if (deleteVenue) {
        int venueId = deleteVenue->id;
        
        // Add some seats
        VenueManager::initializeVenueSeatingPlan(venueId, 2, 5);
        VenueManager::createStandardSeatingPlan(venueId, 2, 5);
        
        // Verify venue exists and has seats
        auto beforeDelete = VenueManager::getVenueById(venueId);
        bool hasSeatsBeforeDelete = beforeDelete && !beforeDelete->seats.empty();
        displayTestResult("Venue has seats before deletion", hasSeatsBeforeDelete);
        
        // Delete the venue
        bool deleteSuccess = VenueManager::deleteVenue(venueId);
        displayTestResult("Delete venue", deleteSuccess);
        
        // Verify venue is gone
        auto afterDelete = VenueManager::getVenueById(venueId);
        displayTestResult("Venue no longer exists after deletion", afterDelete == nullptr);
    }
}

// Main test function
int main() {
    std::cout << "==================================================================" << std::endl;
    std::cout << "           COMPREHENSIVE VENUE MODULE TEST SUITE" << std::endl;
    std::cout << "                 Testing 2D Array Functionality" << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << std::endl;
    
    try {
        // Run all test suites
        testBasicVenueCRUD();
        test2DSeatingPlan();
        testAdvanced2DOperations();
        testSeatingPlanVisualization();
        testEdgeCases();
        testCleanup();
        
        // Display final results
        displayHeader("Test Results Summary");
        std::cout << "Tests Passed: " << testsPassed << std::endl;
        std::cout << "Tests Failed: " << testsFailed << std::endl;
        std::cout << "Total Tests:  " << (testsPassed + testsFailed) << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
                  << (testsPassed * 100.0 / (testsPassed + testsFailed)) << "%" << std::endl;
        
        if (testsFailed == 0) {
            std::cout << "\n🎉 ALL TESTS PASSED! The VenueModule 2D functionality is working correctly." << std::endl;
        } else {
            std::cout << "\n⚠️  SOME TESTS FAILED. Please review the failed tests above." << std::endl;
        }
        
        displaySeparator('=');
        
    } catch (const std::exception& e) {
        std::cerr << "\nFATAL ERROR during testing: " << e.what() << std::endl;
        return 1;
    }
    
    return (testsFailed == 0) ? 0 : 1;
}
