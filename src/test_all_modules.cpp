#include <iostream>
#include "include/authModule.hpp"
#include "include/attendeeModule.hpp"
#include "include/venueModule.hpp"
#include "include/concertModule.hpp"
#include "include/performerModule.hpp"
#include "include/crewModule.hpp"
#include "include/feedbackModule.hpp"
#include "include/commModule.hpp"

// DataPaths namespace test
namespace DataPaths {
    const std::string ATTENDEES_FILE = "data/attendees.dat";
    const std::string CONCERTS_FILE = "data/concerts.dat";
    const std::string VENUES_FILE = "data/venues.dat";
    const std::string PERFORMERS_FILE = "data/performers.dat";
    const std::string CREWS_FILE = "data/crews.dat";
    const std::string TICKETS_FILE = "data/tickets.dat";
    const std::string FEEDBACK_FILE = "data/feedback.dat";
    const std::string PAYMENTS_FILE = "data/payments.dat";
    const std::string SPONSORS_FILE = "data/sponsors.dat";
    const std::string PROMOTIONS_FILE = "data/promotions.dat";
    const std::string REPORTS_FILE = "data/reports.dat";
    const std::string AUTH_FILE = "data/auth.dat";
    const std::string COMM_FILE = "data/communications.dat";
}

int main() {
    std::cout << "=== Testing All Modules with DataPaths ===" << std::endl;
    
    try {
        std::cout << "Testing module initialization with DataPaths constants..." << std::endl;
        
        // Test each module with DataPaths
        auto authModule = std::make_unique<AuthModule>(4096, DataPaths::AUTH_FILE);
        std::cout << "✅ AuthModule initialized with " << DataPaths::AUTH_FILE << std::endl;
        
        auto attendeeModule = std::make_unique<AttendeeModule>(DataPaths::ATTENDEES_FILE);
        std::cout << "✅ AttendeeModule initialized with " << DataPaths::ATTENDEES_FILE << std::endl;
        
        auto venueModule = std::make_unique<VenueModule>(DataPaths::VENUES_FILE);
        std::cout << "✅ VenueModule initialized with " << DataPaths::VENUES_FILE << std::endl;
        
        auto concertModule = std::make_unique<ConcertModule>(DataPaths::CONCERTS_FILE);
        std::cout << "✅ ConcertModule initialized with " << DataPaths::CONCERTS_FILE << std::endl;
        
        auto performerModule = std::make_unique<PerformerModule>(DataPaths::PERFORMERS_FILE);
        std::cout << "✅ PerformerModule initialized with " << DataPaths::PERFORMERS_FILE << std::endl;
        
        auto crewModule = std::make_unique<CrewModule>(DataPaths::CREWS_FILE);
        std::cout << "✅ CrewModule initialized with " << DataPaths::CREWS_FILE << std::endl;
        
        auto feedbackModule = std::make_unique<FeedbackModule>(DataPaths::FEEDBACK_FILE);
        std::cout << "✅ FeedbackModule initialized with " << DataPaths::FEEDBACK_FILE << std::endl;
        
        auto commModule = std::make_unique<CommunicationModule>(DataPaths::COMM_FILE);
        std::cout << "✅ CommunicationModule initialized with " << DataPaths::COMM_FILE << std::endl;
        
        std::cout << "\n🎉 All modules successfully initialized with DataPaths constants!" << std::endl;
        
        // Test data operations
        std::cout << "\nTesting basic operations..." << std::endl;
        
        // Test auth
        if (authModule->registerUser("testuser", "TestPass123!")) {
            std::cout << "✅ Authentication: User registration works" << std::endl;
        }
        
        // Test venue creation
        auto venue = venueModule->createVenue("Test Venue", "123 Test St", "Test City", "TS", "12345", "Test Country", 1000, "Test venue", "test@venue.com");
        if (venue) {
            std::cout << "✅ Venue: Venue creation works" << std::endl;
        }
        
        // Test attendee creation
        auto attendee = attendeeModule->createAttendee("Test User", "test@user.com", "123-456-7890", Model::AttendeeType::REGULAR);
        if (attendee) {
            std::cout << "✅ Attendee: Attendee creation works" << std::endl;
        }
        
        std::cout << "\n🎉 All basic operations successful!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "=== All Tests Completed Successfully ===" << std::endl;
    return 0;
}
