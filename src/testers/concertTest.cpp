#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include "../include/models.hpp"
#include "../include/concertModule.hpp"

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

// Utility function for displaying event status
std::string getEventStatusString(Model::EventStatus status) {
    switch (status) {
        case Model::EventStatus::SCHEDULED:  return "SCHEDULED";
        case Model::EventStatus::CANCELLED:  return "CANCELLED";
        case Model::EventStatus::POSTPONED:  return "POSTPONED";
        case Model::EventStatus::COMPLETED:  return "COMPLETED";
        case Model::EventStatus::SOLDOUT:    return "SOLDOUT";
        default:                            return "UNKNOWN";
    }
}

// Detailed display of a single concert
void displayConcert(const std::shared_ptr<Model::Concert>& concert, bool detailed = true) {
    if (!concert) {
        std::cout << "[NULL CONCERT REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Concert ID: " << concert->id << std::endl;
    std::cout << "Name: " << concert->name << std::endl;
    
    if (detailed) {
        std::cout << "Description: " << concert->description << std::endl;
        std::cout << "Start Date/Time: " << concert->start_date_time.iso8601String << std::endl;
        std::cout << "End Date/Time: " << concert->end_date_time.iso8601String << std::endl;
        std::cout << "Status: " << getEventStatusString(concert->event_status) << std::endl;
        std::cout << "Created At: " << concert->created_at.iso8601String << std::endl;
        std::cout << "Updated At: " << concert->updated_at.iso8601String << std::endl;
        
        // Display ticket info if available
        if (concert->ticketInfo) {
            std::cout << "Ticket Info:" << std::endl;
            std::cout << "  Base Price: $" << concert->ticketInfo->base_price << std::endl;
            std::cout << "  Available: " << concert->ticketInfo->quantity_available << std::endl;
            std::cout << "  Sold: " << concert->ticketInfo->quantity_sold << std::endl;
            std::cout << "  Sale Start: " << concert->ticketInfo->start_sale_date_time.iso8601String << std::endl;
            std::cout << "  Sale End: " << concert->ticketInfo->end_sale_date_time.iso8601String << std::endl;
        } else {
            std::cout << "Ticket Info: Not yet set up" << std::endl;
        }
        
        // Display venue if available
        if (concert->venue) {
            std::cout << "Venue: " << concert->venue->name << " (ID: " << concert->venue->id << ")" << std::endl;
            std::cout << "Capacity: " << concert->venue->capacity << std::endl;
        } else {
            std::cout << "Venue: Not yet assigned" << std::endl;
        }
        
        // Display performer count
        std::cout << "Performers: " << concert->performers.size() << std::endl;
        
        // Display promotion count
        std::cout << "Promotions: " << concert->promotions.size() << std::endl;
        
        // Display show count (from communication logs)
        int showCount = 0;
        for (const auto& log : concert->comm_logs) {
            if (log->comm_type == "Show") {
                showCount++;
            }
        }
        std::cout << "Shows: " << showCount << std::endl;
    }
}

// Display a collection of concerts
void displayConcertList(const std::vector<std::shared_ptr<Model::Concert>>& concerts) {
    if (concerts.empty()) {
        std::cout << "No concerts found." << std::endl;
        return;
    }
    
    std::cout << "Found " << concerts.size() << " concert(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(5) << "ID"
              << std::setw(30) << "Name"
              << std::setw(15) << "Status"
              << std::setw(25) << "Start Date/Time" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& concert : concerts) {
        std::cout << std::setw(5) << concert->id
                  << std::setw(30) << concert->name
                  << std::setw(15) << getEventStatusString(concert->event_status)
                  << std::setw(25) << concert->start_date_time.iso8601String << std::endl;
    }
    displaySeparator();
}

// Test CREATE operation
std::vector<std::shared_ptr<Model::Concert>> testCreateOperation(ConcertModule& module) {
    displayHeader("CREATE OPERATION TEST");
    
    std::vector<std::shared_ptr<Model::Concert>> createdConcerts;
    
    // Create first concert
    std::cout << "Creating first concert..." << std::endl;
    auto concert1 = module.createConcert(
        "Summer Music Festival",
        "A three-day outdoor music festival featuring a variety of genres",
        "2025-08-15T14:00:00Z",
        "2025-08-17T23:00:00Z"
    );
    createdConcerts.push_back(concert1);
    displayConcert(concert1);
    displaySeparator();
    
    // Create second concert
    std::cout << "Creating second concert..." << std::endl;
    auto concert2 = module.createConcert(
        "Classical Symphony Night",
        "An evening of classical music performed by the Philharmonic Orchestra",
        "2025-09-10T19:00:00Z",
        "2025-09-10T22:00:00Z"
    );
    createdConcerts.push_back(concert2);
    displayConcert(concert2);
    displaySeparator();
    
    // Create third concert
    std::cout << "Creating third concert..." << std::endl;
    auto concert3 = module.createConcert(
        "Rock Legends Reunion",
        "Legendary rock bands reunite for a spectacular night",
        "2025-10-05T18:30:00Z",
        "2025-10-05T23:30:00Z"
    );
    createdConcerts.push_back(concert3);
    displayConcert(concert3);
    displaySeparator();
    
    return createdConcerts;
}

// Test READ operations
void testReadOperations(ConcertModule& module, const std::vector<std::shared_ptr<Model::Concert>>& testConcerts) {
    displayHeader("READ OPERATIONS TEST");
    
    // Get all concerts
    std::cout << "1. Getting all concerts:" << std::endl;
    auto allConcerts = module.getAllConcerts();
    displayConcertList(allConcerts);
    
    // Get concert by ID
    if (!testConcerts.empty()) {
        int idToFind = testConcerts[0]->id;
        std::cout << "\n2. Getting concert by ID " << idToFind << ":" << std::endl;
        auto foundConcert = module.getConcertById(idToFind);
        if (foundConcert) {
            displayConcert(foundConcert);
        }
    }
    
    // Find concerts by name (partial match)
    std::cout << "\n3. Finding concerts by name (contains 'Music'):" << std::endl;
    auto nameResults = module.findConcertsByName("Music");
    displayConcertList(nameResults);
    
    // Find concerts by date range
    std::cout << "\n4. Finding concerts in date range (August 2025):" << std::endl;
    auto dateResults = module.findConcertsByDateRange("2025-08-01T00:00:00Z", "2025-08-31T23:59:59Z");
    displayConcertList(dateResults);
    
    // Find concerts by status
    std::cout << "\n5. Finding concerts with SCHEDULED status:" << std::endl;
    auto statusResults = module.findConcertsByStatus(Model::EventStatus::SCHEDULED);
    displayConcertList(statusResults);
    
    // Test not finding a concert
    std::cout << "\n6. Testing non-existent ID lookup:" << std::endl;
    auto nonExistentConcert = module.getConcertById(9999);
    if (!nonExistentConcert) {
        std::cout << "Correctly returned nullptr for non-existent ID 9999" << std::endl;
    }
}

// Test UPDATE operations
void testUpdateOperations(ConcertModule& module, const std::vector<std::shared_ptr<Model::Concert>>& testConcerts) {
    displayHeader("UPDATE OPERATIONS TEST");
    
    if (testConcerts.empty()) {
        std::cout << "No test concerts available for update testing." << std::endl;
        return;
    }
    
    // Update name and description
    int id1 = testConcerts[0]->id;
    std::cout << "1. Updating name and description for ID " << id1 << ":" << std::endl;
    std::cout << "Before update:" << std::endl;
    displayConcert(module.getConcertById(id1));
    
    if (module.editConcert(id1, "Updated Summer Music Festival", "This is an updated description")) {
        std::cout << "\nAfter update:" << std::endl;
        displayConcert(module.getConcertById(id1));
    } else {
        std::cout << "Update failed." << std::endl;
    }
    displaySeparator();
    
    // Set up ticket info for a concert
    if (testConcerts.size() > 1) {
        int id2 = testConcerts[1]->id;
        std::cout << "2. Setting up ticket info for ID " << id2 << ":" << std::endl;
        std::cout << "Before update:" << std::endl;
        displayConcert(module.getConcertById(id2));
        
        if (module.setupTicketInfo(
            id2, 
            75.0,  // Base price
            500,   // Quantity
            "2025-08-15T00:00:00Z",  // Sale start
            "2025-09-09T23:59:59Z"   // Sale end
        )) {
            std::cout << "\nAfter update:" << std::endl;
            displayConcert(module.getConcertById(id2));
        } else {
            std::cout << "Update failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Add a show to a concert
    if (testConcerts.size() > 2) {
        int id3 = testConcerts[2]->id;
        std::cout << "3. Adding a show to concert ID " << id3 << ":" << std::endl;
        
        if (module.addShowToConcert(id3, "Opening Act: The Rockers", "2025-10-05T18:30:00Z")) {
            // Add a second show
            module.addShowToConcert(id3, "Main Event: Rock Legends", "2025-10-05T20:00:00Z");
            
            std::cout << "Shows added successfully." << std::endl;
            displayConcert(module.getConcertById(id3));
        } else {
            std::cout << "Adding show failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Test concert status changes
    if (!testConcerts.empty()) {
        int id = testConcerts[0]->id;
        std::cout << "4. Testing concert status changes for ID " << id << ":" << std::endl;
        
        std::cout << "Original status: " << getEventStatusString(module.getConcertById(id)->event_status) << std::endl;
        
        // Change to SOLDOUT
        if (module.changeConcertStatus(id, Model::EventStatus::SOLDOUT)) {
            std::cout << "Changed to SOLDOUT: " << getEventStatusString(module.getConcertById(id)->event_status) << std::endl;
        }
        
        // Change to CANCELLED
        if (module.changeConcertStatus(id, Model::EventStatus::CANCELLED)) {
            std::cout << "Changed to CANCELLED: " << getEventStatusString(module.getConcertById(id)->event_status) << std::endl;
        }
        
        // Change back to SCHEDULED
        if (module.changeConcertStatus(id, Model::EventStatus::SCHEDULED)) {
            std::cout << "Changed back to SCHEDULED: " << getEventStatusString(module.getConcertById(id)->event_status) << std::endl;
        }
    }
}

// Test specialized operations
void testSpecializedOperations(ConcertModule& module, const std::vector<std::shared_ptr<Model::Concert>>& testConcerts) {
    displayHeader("SPECIALIZED OPERATIONS TEST");
    
    if (testConcerts.empty()) {
        std::cout << "No test concerts available for specialized operations testing." << std::endl;
        return;
    }
    
    int id = testConcerts[0]->id;
    std::cout << "1. Testing concert lifecycle operations for ID " << id << ":" << std::endl;
    
    // Test starting a concert
    std::cout << "Attempting to start concert..." << std::endl;
    if (module.startConcert(id)) {
        std::cout << "Concert started successfully. New status: " 
                 << getEventStatusString(module.getConcertById(id)->event_status) << std::endl;
        
        // Test ending a concert and generating a report
        std::cout << "\nAttempting to end concert..." << std::endl;
        if (module.endConcert(id)) {
            std::cout << "Concert ended successfully and report generated." << std::endl;
            
            // Display generated report count
            auto concert = module.getConcertById(id);
            std::cout << "Number of reports: " << concert->reports.size() << std::endl;
        } else {
            std::cout << "Ending concert failed." << std::endl;
        }
    } else {
        std::cout << "Starting concert failed." << std::endl;
    }
    displaySeparator();
    
    // Test show management
    if (testConcerts.size() > 1) {
        int id2 = testConcerts[1]->id;
        std::cout << "2. Testing show management for concert ID " << id2 << ":" << std::endl;
        
        // Add shows
        std::cout << "Adding shows to concert..." << std::endl;
        module.addShowToConcert(id2, "Opening Performance", "2025-09-10T19:00:00Z");
        module.addShowToConcert(id2, "Main Symphony", "2025-09-10T19:45:00Z");
        module.addShowToConcert(id2, "Closing Piece", "2025-09-10T21:30:00Z");
        
        // Get the concert and find the show IDs
        auto concert = module.getConcertById(id2);
        
        // Find shows (communication logs with type "Show")
        std::cout << "Shows for concert:" << std::endl;
        int showToRemove = 0;
        int count = 1;
        
        for (const auto& log : concert->comm_logs) {
            if (log->comm_type == "Show") {
                std::cout << count << ". ID: " << log->comm_id << " - " << log->message_content << std::endl;
                
                // Store the ID of the first show for removal
                if (count == 1) {
                    showToRemove = log->comm_id;
                }
                
                count++;
            }
        }
        
        // Remove a show
        if (showToRemove > 0) {
            std::cout << "\nRemoving show with ID " << showToRemove << "..." << std::endl;
            if (module.removeShowFromConcert(id2, showToRemove)) {
                std::cout << "Show removed successfully." << std::endl;
                
                // Display remaining shows
                std::cout << "\nRemaining shows:" << std::endl;
                count = 1;
                
                concert = module.getConcertById(id2); // Refresh concert data
                for (const auto& log : concert->comm_logs) {
                    if (log->comm_type == "Show") {
                        std::cout << count << ". ID: " << log->comm_id << " - " << log->message_content << std::endl;
                        count++;
                    }
                }
            } else {
                std::cout << "Show removal failed." << std::endl;
            }
        }
    }
}

// Test DELETE operations
void testDeleteOperations(ConcertModule& module, std::vector<std::shared_ptr<Model::Concert>>& testConcerts) {
    displayHeader("DELETE OPERATIONS TEST");
    
    if (testConcerts.empty()) {
        std::cout << "No test concerts available for delete testing." << std::endl;
        return;
    }
    
    // Delete the last created concert
    int idToDelete = testConcerts.back()->id;
    std::cout << "1. Deleting concert with ID " << idToDelete << ":" << std::endl;
    displayConcert(module.getConcertById(idToDelete));
    
    if (module.deleteConcert(idToDelete)) {
        std::cout << "\nConcert successfully deleted." << std::endl;
        
        // Verify deletion
        auto verifyDelete = module.getConcertById(idToDelete);
        if (!verifyDelete) {
            std::cout << "Verification successful: Concert no longer exists." << std::endl;
        } else {
            std::cout << "Verification failed: Concert still exists!" << std::endl;
        }
        
        // Remove from our test list
        testConcerts.pop_back();
    } else {
        std::cout << "Deletion failed." << std::endl;
    }
    displaySeparator();
    
    // Show remaining concerts
    std::cout << "2. Remaining concerts after deletion:" << std::endl;
    displayConcertList(module.getAllConcerts());
    
    // Try to delete non-existent concert
    std::cout << "\n3. Attempting to delete non-existent concert (ID 9999):" << std::endl;
    bool nonExistentDelete = module.deleteConcert(9999);
    std::cout << "Delete result: " << (nonExistentDelete ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
}

// Main testing function
int main() {
    std::cout << "\n\n";
    displayHeader("CONCERT MODULE COMPREHENSIVE TEST");
    std::cout << "Date: " << Model::DateTime::now().iso8601String << "\n\n";
    
    // Create module instance
    ConcertModule module;
    
    // Vector to keep track of created concerts for testing
    std::vector<std::shared_ptr<Model::Concert>> testConcerts;
    
    try {
        // Test CREATE operations
        testConcerts = testCreateOperation(module);
        std::cout << "\n\n";
        
        // Test READ operations
        testReadOperations(module, testConcerts);
        std::cout << "\n\n";
        
        // Test UPDATE operations
        testUpdateOperations(module, testConcerts);
        std::cout << "\n\n";
        
        // Test specialized operations
        testSpecializedOperations(module, testConcerts);
        std::cout << "\n\n";
        
        // Test DELETE operations
        testDeleteOperations(module, testConcerts);
        
        displayHeader("TEST COMPLETED SUCCESSFULLY");
    }
    catch (const std::exception& e) {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
