#include "../include/models.h"
#include "../include/dataManager.h"
#include "../include/serializationUtils.h"
#include "../include/serializationImpl.h"

#include <iostream>
#include <memory>
#include <vector>

using namespace MuseIO;

int main() {
    // Create a data manager with the default "data" directory
    DataManager dataManager;
    
    // Create a venue
    auto venue = std::make_shared<Venue>(
        1, "Madison Square Garden", "4 Pennsylvania Plaza", "New York",
        "NY", "10001", "USA", 20000, "Famous arena in Manhattan",
        "contact@msg.com", "msg_seatmap.jpg"
    );
    
    // Create a concert
    auto startDate = DateTime("2025-08-15T19:30:00Z");
    auto endDate = DateTime("2025-08-15T23:00:00Z");
    auto concert = std::make_shared<Concert>(
        1, "Summer Rock Festival", "Annual rock music event",
        startDate, endDate, venue, EventStatus::SCHEDULED
    );
    
    // Add some performers to the concert
    auto performer1 = std::make_shared<Performer>(
        1, "The Rolling Stones", "Rock Band", 
        "contact@rollingstones.com", "Legendary rock band from England"
    );
    
    auto performer2 = std::make_shared<Performer>(
        2, "Foo Fighters", "Rock Band", 
        "contact@foofighters.com", "American rock band formed in 1994"
    );
    
    concert->addPerformer(performer1);
    concert->addPerformer(performer2);
    
    // Create and setup ticket information
    auto ticketInfo = std::make_shared<ConcertTicket>(
        99.99, 5000, DateTime("2025-05-01T10:00:00Z"), DateTime("2025-08-15T18:00:00Z")
    );
    concert->setTicketInfo(ticketInfo);
    
    // Create some attendees and tickets
    auto attendee1 = std::make_shared<Attendee>(
        1, "John Smith", "john@example.com", "555-123-4567"
    );
    
    auto attendee2 = std::make_shared<Attendee>(
        2, "Jane Doe", "jane@example.com", "555-987-6543", 
        AttendeeType::VIP
    );
    
    // Generate tickets for attendees
    auto ticket1 = ticketInfo->createTicket(1, "QR123456");
    auto ticket2 = ticketInfo->createTicket(2, "QR789012");
    
    attendee1->addTicket(ticket1);
    attendee2->addTicket(ticket2);
    
    // Add attendees to concert
    concert->addAttendee(attendee1);
    concert->addAttendee(attendee2);
    
    // Save all data
    std::cout << "Saving concert data...\n";
    
    // Add objects to data manager
    dataManager.addConcert(concert);
    
    // Save to files
    if (dataManager.saveConcerts()) {
        std::cout << "Concert data saved successfully.\n";
    } else {
        std::cout << "Failed to save concert data.\n";
    }
    
    // Clear and reload data to verify persistence
    std::cout << "\nReloading concert data...\n";
    
    // Create a new data manager to simulate restarting the application
    DataManager newDataManager;
    
    if (newDataManager.loadConcerts()) {
        std::cout << "Concert data loaded successfully.\n";
        
        // Get the loaded concert
        auto loadedConcerts = newDataManager.getAllConcerts();
        
        if (!loadedConcerts.empty()) {
            auto loadedConcert = loadedConcerts[0];
            
            // Display information to verify data was loaded correctly
            std::cout << "\nLoaded Concert Information:\n";
            std::cout << "ID: " << loadedConcert->getId() << "\n";
            std::cout << "Name: " << loadedConcert->getName() << "\n";
            std::cout << "Description: " << loadedConcert->getDescription() << "\n";
            std::cout << "Start Date: " << loadedConcert->getStartDateTime().toString() << "\n";
            std::cout << "End Date: " << loadedConcert->getEndDateTime().toString() << "\n";
            std::cout << "Status: " << static_cast<int>(loadedConcert->getEventStatus()) << "\n";
        } else {
            std::cout << "No concerts were loaded.\n";
        }
    } else {
        std::cout << "Failed to load concert data.\n";
    }
    
    return 0;
}
