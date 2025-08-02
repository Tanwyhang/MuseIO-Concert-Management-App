#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include "../include/models.hpp"
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

// Utility function for displaying seat status
std::string getSeatStatusString(Model::TicketStatus status) {
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
        std::cout << "City: " << venue->city << std::endl;
        std::cout << "State: " << venue->state << std::endl;
        std::cout << "ZIP Code: " << venue->zip_code << std::endl;
        std::cout << "Country: " << venue->country << std::endl;
        std::cout << "Capacity: " << venue->capacity << std::endl;
        std::cout << "Description: " << venue->description << std::endl;
        std::cout << "Contact: " << venue->contact_info << std::endl;
        std::cout << "Seatmap: " << (venue->seatmap.empty() ? "[None]" : venue->seatmap) << std::endl;
        std::cout << "Number of Seats: " << venue->seats.size() << std::endl;
    }
}

// Display a collection of venues
void displayVenueList(const std::vector<std::shared_ptr<Model::Venue>>& venues) {
    if (venues.empty()) {
        std::cout << "No venues found." << std::endl;
        return;
    }
    
    std::cout << "Found " << venues.size() << " venue(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(5) << "ID"
              << std::setw(25) << "Name"
              << std::setw(20) << "City"
              << std::setw(15) << "Capacity"
              << std::setw(10) << "Seats" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& venue : venues) {
        std::cout << std::setw(5) << venue->id
                  << std::setw(25) << venue->name
                  << std::setw(20) << venue->city
                  << std::setw(15) << venue->capacity
                  << std::setw(10) << venue->seats.size() << std::endl;
    }
    displaySeparator();
}

// Display a collection of seats
void displaySeatList(const std::vector<std::shared_ptr<Model::Seat>>& seats) {
    if (seats.empty()) {
        std::cout << "No seats found." << std::endl;
        return;
    }
    
    std::cout << "Found " << seats.size() << " seat(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(5) << "ID"
              << std::setw(15) << "Type"
              << std::setw(10) << "Row"
              << std::setw(10) << "Column"
              << std::setw(15) << "Status" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& seat : seats) {
        std::cout << std::setw(5) << seat->seat_id
                  << std::setw(15) << seat->seat_type
                  << std::setw(10) << seat->row_number
                  << std::setw(10) << seat->col_number
                  << std::setw(15) << getSeatStatusString(seat->status) << std::endl;
    }
    displaySeparator();
}

// Test CREATE operation
std::vector<std::shared_ptr<Model::Venue>> testCreateOperation(VenueModule& module) {
    displayHeader("CREATE OPERATION TEST");
    
    std::vector<std::shared_ptr<Model::Venue>> createdVenues;
    
    // Create first venue
    std::cout << "Creating first venue..." << std::endl;
    auto venue1 = module.createVenue(
        "Grand Concert Hall",
        "123 Music Street",
        "New York",
        "NY",
        "10001",
        "USA",
        5000,
        "A prestigious concert hall in downtown Manhattan",
        "info@grandconcerthall.com",
        "seatmap_grand_hall.png"
    );
    createdVenues.push_back(venue1);
    displayVenue(venue1);
    displaySeparator();
    
    // Create second venue
    std::cout << "Creating second venue..." << std::endl;
    auto venue2 = module.createVenue(
        "Riverside Amphitheater",
        "456 River Road",
        "Chicago",
        "IL",
        "60601",
        "USA",
        8000,
        "Outdoor amphitheater overlooking the river",
        "contact@riversideamphitheater.com"
    );
    createdVenues.push_back(venue2);
    displayVenue(venue2);
    displaySeparator();
    
    // Create third venue
    std::cout << "Creating third venue..." << std::endl;
    auto venue3 = module.createVenue(
        "City Arena",
        "789 Main Street",
        "Los Angeles",
        "CA",
        "90001",
        "USA",
        15000,
        "Multi-purpose arena for concerts and sporting events",
        "info@cityarena.com",
        "arena_layout.pdf"
    );
    createdVenues.push_back(venue3);
    displayVenue(venue3);
    displaySeparator();
    
    return createdVenues;
}

// Test adding seats to venues
void testAddSeats(VenueModule& module, const std::vector<std::shared_ptr<Model::Venue>>& testVenues) {
    displayHeader("ADD SEATS TEST");
    
    if (testVenues.empty()) {
        std::cout << "No test venues available for adding seats." << std::endl;
        return;
    }
    
    // Add seats to the first venue (multiple sections, rows, and columns)
    int venueId = testVenues[0]->id;
    std::cout << "Adding seats to venue ID " << venueId << " (" << testVenues[0]->name << "):" << std::endl;
    
    // Add VIP section seats
    for (char row = 'A'; row <= 'C'; row++) {
        for (int col = 1; col <= 10; col++) {
            std::string rowStr(1, row);
            std::string colStr = std::to_string(col);
            module.addSeat(venueId, "VIP", rowStr, colStr);
        }
    }
    
    // Add Regular section seats
    for (char row = 'D'; row <= 'J'; row++) {
        for (int col = 1; col <= 15; col++) {
            std::string rowStr(1, row);
            std::string colStr = std::to_string(col);
            module.addSeat(venueId, "Regular", rowStr, colStr);
        }
    }
    
    // Add Balcony section seats
    for (char row = 'K'; row <= 'M'; row++) {
        for (int col = 1; col <= 20; col++) {
            std::string rowStr(1, row);
            std::string colStr = std::to_string(col);
            module.addSeat(venueId, "Balcony", rowStr, colStr);
        }
    }
    
    std::cout << "Seats added successfully." << std::endl;
    
    // Show some seat statistics
    auto allSeats = module.getSeatsForVenue(venueId);
    std::cout << "\nTotal seats added: " << allSeats.size() << std::endl;
    
    // Count by type
    int vipSeats = 0, regularSeats = 0, balconySeats = 0;
    for (const auto& seat : allSeats) {
        if (seat->seat_type == "VIP") vipSeats++;
        else if (seat->seat_type == "Regular") regularSeats++;
        else if (seat->seat_type == "Balcony") balconySeats++;
    }
    
    std::cout << "VIP Seats: " << vipSeats << std::endl;
    std::cout << "Regular Seats: " << regularSeats << std::endl;
    std::cout << "Balcony Seats: " << balconySeats << std::endl;
    
    // Display a sample of seats
    std::cout << "\nSample of seats from each section:" << std::endl;
    
    // Find a seat from each section
    std::shared_ptr<Model::Seat> vipSample, regularSample, balconySample;
    for (const auto& seat : allSeats) {
        if (seat->seat_type == "VIP" && !vipSample) vipSample = seat;
        else if (seat->seat_type == "Regular" && !regularSample) regularSample = seat;
        else if (seat->seat_type == "Balcony" && !balconySample) balconySample = seat;
        
        if (vipSample && regularSample && balconySample) break;
    }
    
    std::vector<std::shared_ptr<Model::Seat>> sampleSeats;
    if (vipSample) sampleSeats.push_back(vipSample);
    if (regularSample) sampleSeats.push_back(regularSample);
    if (balconySample) sampleSeats.push_back(balconySample);
    
    displaySeatList(sampleSeats);
}

// Test READ operations
void testReadOperations(VenueModule& module, const std::vector<std::shared_ptr<Model::Venue>>& testVenues) {
    displayHeader("READ OPERATIONS TEST");
    
    // Get all venues
    std::cout << "1. Getting all venues:" << std::endl;
    auto allVenues = module.getAllVenues();
    displayVenueList(allVenues);
    
    // Get venue by ID
    if (!testVenues.empty()) {
        int idToFind = testVenues[0]->id;
        std::cout << "\n2. Getting venue by ID " << idToFind << ":" << std::endl;
        auto foundVenue = module.getVenueById(idToFind);
        if (foundVenue) {
            displayVenue(foundVenue);
        }
    }
    
    // Find venues by name (partial match)
    std::cout << "\n3. Finding venues by name (contains 'Arena'):" << std::endl;
    auto nameResults = module.findVenuesByName("Arena");
    displayVenueList(nameResults);
    
    // Find venues by city
    std::cout << "\n4. Finding venues by city (Chicago):" << std::endl;
    auto cityResults = module.findVenuesByCity("Chicago");
    displayVenueList(cityResults);
    
    // Find venues by minimum capacity
    std::cout << "\n5. Finding venues with capacity >= 10000:" << std::endl;
    auto capacityResults = module.findVenuesByCapacity(10000);
    displayVenueList(capacityResults);
    
    // Test not finding a venue
    std::cout << "\n6. Testing non-existent ID lookup:" << std::endl;
    auto nonExistentVenue = module.getVenueById(9999);
    if (!nonExistentVenue) {
        std::cout << "Correctly returned nullptr for non-existent ID 9999" << std::endl;
    }
}

// Test UPDATE operations
void testUpdateOperations(VenueModule& module, const std::vector<std::shared_ptr<Model::Venue>>& testVenues) {
    displayHeader("UPDATE OPERATIONS TEST");
    
    if (testVenues.empty()) {
        std::cout << "No test venues available for update testing." << std::endl;
        return;
    }
    
    // Update name and address
    int id1 = testVenues[0]->id;
    std::cout << "1. Updating name and address for ID " << id1 << ":" << std::endl;
    std::cout << "Before update:" << std::endl;
    displayVenue(module.getVenueById(id1));
    
    if (module.updateVenue(id1, "Updated Grand Concert Hall", "456 Updated Street")) {
        std::cout << "\nAfter update:" << std::endl;
        displayVenue(module.getVenueById(id1));
    } else {
        std::cout << "Update failed." << std::endl;
    }
    displaySeparator();
    
    // Update capacity and contact info
    if (testVenues.size() > 1) {
        int id2 = testVenues[1]->id;
        std::cout << "2. Updating capacity and contact info for ID " << id2 << ":" << std::endl;
        std::cout << "Before update:" << std::endl;
        displayVenue(module.getVenueById(id2));
        
        if (module.updateVenue(
            id2, 
            "",    // No name change
            "",    // No address change
            "",    // No city change
            "",    // No state change
            "",    // No zip change
            "",    // No country change
            10000, // New capacity
            "",    // No description change
            "newemail@example.com"  // New contact
        )) {
            std::cout << "\nAfter update:" << std::endl;
            displayVenue(module.getVenueById(id2));
        } else {
            std::cout << "Update failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Update seat status
    if (!testVenues.empty() && !module.getSeatsForVenue(testVenues[0]->id).empty()) {
        int venueId = testVenues[0]->id;
        auto seats = module.getSeatsForVenue(venueId);
        int seatId = seats[0]->seat_id; // Get first seat ID
        
        std::cout << "3. Updating seat status for venue " << venueId << ", seat " << seatId << ":" << std::endl;
        std::cout << "Before update:" << std::endl;
        displaySeatList({seats[0]});
        
        if (module.updateSeatStatus(venueId, seatId, Model::TicketStatus::RESERVED)) {
            std::cout << "\nAfter update:" << std::endl;
            auto updatedSeats = module.getSeatsForVenue(venueId);
            
            // Find the updated seat
            for (const auto& seat : updatedSeats) {
                if (seat->seat_id == seatId) {
                    displaySeatList({seat});
                    break;
                }
            }
        } else {
            std::cout << "Update failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Test updating non-existent venue
    std::cout << "4. Attempting to update non-existent venue (ID 9999):" << std::endl;
    bool nonExistentUpdate = module.updateVenue(9999, "This Should Fail");
    std::cout << "Update result: " << (nonExistentUpdate ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
}

// Test seat management operations
void testSeatOperations(VenueModule& module, const std::vector<std::shared_ptr<Model::Venue>>& testVenues) {
    displayHeader("SEAT MANAGEMENT TEST");
    
    if (testVenues.empty() || testVenues.size() < 2) {
        std::cout << "Not enough test venues available for seat management testing." << std::endl;
        return;
    }
    
    int venueId = testVenues[1]->id;
    std::cout << "1. Adding individual seats to venue ID " << venueId << ":" << std::endl;
    
    // Add a few individual seats
    module.addSeat(venueId, "Premium", "AA", "1");
    module.addSeat(venueId, "Premium", "AA", "2");
    module.addSeat(venueId, "Premium", "AA", "3");
    module.addSeat(venueId, "Standard", "BB", "1");
    module.addSeat(venueId, "Standard", "BB", "2");
    
    // Display all seats
    auto seats = module.getSeatsForVenue(venueId);
    displaySeatList(seats);
    displaySeparator();
    
    // Remove a seat
    if (!seats.empty()) {
        int seatToRemove = seats[0]->seat_id;
        std::cout << "2. Removing seat ID " << seatToRemove << " from venue ID " << venueId << ":" << std::endl;
        
        if (module.removeSeat(venueId, seatToRemove)) {
            std::cout << "Seat removed successfully." << std::endl;
            
            // Display remaining seats
            auto remainingSeats = module.getSeatsForVenue(venueId);
            std::cout << "\nRemaining seats:" << std::endl;
            displaySeatList(remainingSeats);
        } else {
            std::cout << "Seat removal failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Get available seats
    std::cout << "3. Getting available seats for venue ID " << venueId << ":" << std::endl;
    auto availableSeats = module.getAvailableSeats(venueId);
    displaySeatList(availableSeats);
    
    // Update some seats to be unavailable
    if (!availableSeats.empty()) {
        // Mark the first seat as sold
        int seatId = availableSeats[0]->seat_id;
        std::cout << "\n4. Marking seat ID " << seatId << " as SOLD:" << std::endl;
        
        if (module.updateSeatStatus(venueId, seatId, Model::TicketStatus::SOLD)) {
            std::cout << "Status updated successfully." << std::endl;
            
            // Check available seats again
            std::cout << "\nAvailable seats after update:" << std::endl;
            auto newAvailableSeats = module.getAvailableSeats(venueId);
            displaySeatList(newAvailableSeats);
            
            // Verify we have one less available seat
            std::cout << "Original available seat count: " << availableSeats.size() << std::endl;
            std::cout << "New available seat count: " << newAvailableSeats.size() << std::endl;
        } else {
            std::cout << "Status update failed." << std::endl;
        }
    }
}

// Test DELETE operations
void testDeleteOperations(VenueModule& module, std::vector<std::shared_ptr<Model::Venue>>& testVenues) {
    displayHeader("DELETE OPERATIONS TEST");
    
    if (testVenues.empty()) {
        std::cout << "No test venues available for delete testing." << std::endl;
        return;
    }
    
    // Delete the last created venue
    int idToDelete = testVenues.back()->id;
    std::cout << "1. Deleting venue with ID " << idToDelete << ":" << std::endl;
    displayVenue(module.getVenueById(idToDelete));
    
    if (module.deleteVenue(idToDelete)) {
        std::cout << "\nVenue successfully deleted." << std::endl;
        
        // Verify deletion
        auto verifyDelete = module.getVenueById(idToDelete);
        if (!verifyDelete) {
            std::cout << "Verification successful: Venue no longer exists." << std::endl;
        } else {
            std::cout << "Verification failed: Venue still exists!" << std::endl;
        }
        
        // Remove from our test list
        testVenues.pop_back();
    } else {
        std::cout << "Deletion failed." << std::endl;
    }
    displaySeparator();
    
    // Show remaining venues
    std::cout << "2. Remaining venues after deletion:" << std::endl;
    displayVenueList(module.getAllVenues());
    
    // Try to delete non-existent venue
    std::cout << "\n3. Attempting to delete non-existent venue (ID 9999):" << std::endl;
    bool nonExistentDelete = module.deleteVenue(9999);
    std::cout << "Delete result: " << (nonExistentDelete ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
}

// Main testing function
int main() {
    std::cout << "\n\n";
    displayHeader("VENUE MODULE COMPREHENSIVE TEST");
    std::cout << "Date: " << Model::DateTime::now().iso8601String << "\n\n";
    
    // Create module instance
    VenueModule module;
    
    // Vector to keep track of created venues for testing
    std::vector<std::shared_ptr<Model::Venue>> testVenues;
    
    try {
        // Test CREATE operations
        testVenues = testCreateOperation(module);
        std::cout << "\n\n";
        
        // Test adding seats to venues
        testAddSeats(module, testVenues);
        std::cout << "\n\n";
        
        // Test READ operations
        testReadOperations(module, testVenues);
        std::cout << "\n\n";
        
        // Test UPDATE operations
        testUpdateOperations(module, testVenues);
        std::cout << "\n\n";
        
        // Test seat management operations
        testSeatOperations(module, testVenues);
        std::cout << "\n\n";
        
        // Test DELETE operations
        testDeleteOperations(module, testVenues);
        
        displayHeader("TEST COMPLETED SUCCESSFULLY");
    }
    catch (const std::exception& e) {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
