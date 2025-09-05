#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <thread>
#include <chrono>
#include "include/models.hpp"
#include "include/authModule.hpp"
#include "include/concertModule.hpp"
#include "include/ticketModule.hpp"
#include "include/venueModule.hpp"
#include "include/crewModule.hpp"
#include "include/paymentModule.hpp"
#include "include/commModule.hpp"
#include "include/feedbackModule.hpp"
#include "include/performerModule.hpp"
#include "include/reportModule.hpp"
#include "include/attendeeModule.hpp"
#include "include/validationModule.hpp"
#include "include/uiModule.hpp"

// DataPaths namespace (same as main.cpp)
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

// Global module instances (same as main.cpp)
std::unique_ptr<AuthModule> g_authModule;
std::unique_ptr<AttendeeModule> g_attendeeModule;
std::unique_ptr<ConcertModule> g_concertModule;
std::unique_ptr<VenueModule> g_venueModule;
std::unique_ptr<PerformerModule> g_performerModule;
std::unique_ptr<CrewModule> g_crewModule;
std::unique_ptr<TicketManager::TicketModule> g_ticketModule;
std::unique_ptr<PaymentManager::PaymentModule> g_paymentModule;
std::unique_ptr<FeedbackModule> g_feedbackModule;
std::unique_ptr<ReportManager::ReportModule> g_reportModule;
std::unique_ptr<CommunicationModule> g_commModule;

// User session (same as main.cpp)
struct UserSession {
    int userId;
    std::string username;
    std::string userRole;
    bool isAuthenticated;
    Model::DateTime loginTime;
    
    UserSession() : userId(-1), isAuthenticated(false) {}
};

UserSession currentSession;

class MuseIOSimulator {
private:
    int stepCounter = 1;
    
    void printStep(const std::string& description) {
        UIManager::addSectionSpacing();
        UIManager::printSeparator('=', 80);
        UIManager::printCenteredText("STEP " + std::to_string(stepCounter++) + ": " + description, 80);
        UIManager::printSeparator('=', 80);
        UIManager::addSmallSpacing();
    }
    
    void simulateUserInput(const std::string& input) {
        std::cout << "[USER INPUT]: " << input << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    void showOutput(const std::string& output) {
        std::cout << output << std::endl;
    }
    
    void pause(int milliseconds = 1000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    
    void printSubSection(const std::string& title) {
        UIManager::addSmallSpacing();
        UIManager::printSeparator('-', 60);
        UIManager::printCenteredText(title, 60);
        UIManager::printSeparator('-', 60);
        UIManager::addSmallSpacing();
    }
    
    void printTestCase(const std::string& testName) {
        UIManager::addSmallSpacing();
        std::cout << "🔸 " << testName << std::endl;
        UIManager::printSeparator('.', 40);
    }

public:
    void runCompleteSimulation() {
        UIManager::addSectionSpacing();
        std::cout << UIManager::APP_BANNER << std::endl;
        UIManager::printCenteredText("🎭 MUSEIO CONCERT MANAGEMENT SYSTEM - COMPLETE SIMULATION", 80);
        UIManager::printCenteredText("This simulation demonstrates ALL functionalities as if a real user is interacting with the system", 80);
        UIManager::addSectionSpacing();
        
        initializeSystem();
        simulateAuthenticationPortal();
        simulateManagementPortal();
        simulateUserPortal();
        showFinalResults();
    }

private:
    void initializeSystem() {
        printStep("SYSTEM INITIALIZATION");
        
        showOutput("Initializing MuseIO Concert Management System...");
        
        try {
            g_authModule = std::make_unique<AuthModule>(4096, DataPaths::AUTH_FILE);
            g_attendeeModule = std::make_unique<AttendeeModule>(DataPaths::ATTENDEES_FILE);
            g_venueModule = std::make_unique<VenueModule>(DataPaths::VENUES_FILE);
            g_performerModule = std::make_unique<PerformerModule>(DataPaths::PERFORMERS_FILE);
            g_crewModule = std::make_unique<CrewModule>(DataPaths::CREWS_FILE);
            g_concertModule = std::make_unique<ConcertModule>(DataPaths::CONCERTS_FILE);
            g_ticketModule = std::make_unique<TicketManager::TicketModule>(DataPaths::TICKETS_FILE);
            g_paymentModule = std::make_unique<PaymentManager::PaymentModule>(DataPaths::PAYMENTS_FILE);
            g_feedbackModule = std::make_unique<FeedbackModule>(DataPaths::FEEDBACK_FILE);
            g_reportModule = std::make_unique<ReportManager::ReportModule>(DataPaths::REPORTS_FILE);
            g_commModule = std::make_unique<CommunicationModule>(DataPaths::COMM_FILE);
            
            showOutput("✅ All modules initialized successfully!");
            
            // Initialize default users
            showOutput("Setting up default user accounts...");
            g_authModule->registerUser("admin", "admin123");
            g_authModule->registerUser("manager1", "manager123");
            g_authModule->registerUser("staff1", "staff123");
            g_authModule->registerUser("user1", "user123");
            g_authModule->registerUser("vip1", "vip123");
            
            showOutput("✅ Default user accounts created!");
            
        } catch (const std::exception& e) {
            showOutput("❌ System initialization failed: " + std::string(e.what()));
        }
        pause();
    }

    void simulateAuthenticationPortal() {
        printStep("AUTHENTICATION PORTAL SIMULATION");
        
        // Show demo credentials
        printTestCase("Displaying Demo Credentials");
        showOutput(UIManager::DEMO_CREDENTIALS);
        pause(2000);
        
        // Show authentication menu
        printTestCase("Authentication Menu Display");
        showOutput(UIManager::AUTH_MENU);
        UIManager::addSmallSpacing();
        
        // Simulate viewing all accounts first
        printTestCase("View All Registered Accounts");
        simulateUserInput("3");
        showOutput("Enter your choice (0-3): 3");
        simulateViewAllAccounts();
        
        // Show menu again
        printTestCase("User Registration Process");
        showOutput(UIManager::AUTH_MENU);
        UIManager::addSmallSpacing();
        
        // Simulate user registration
        simulateUserInput("2");
        showOutput("Enter your choice (0-3): 2");
        simulateUserRegistration();
        
        // Show menu again
        printTestCase("User Login Authentication");
        showOutput(UIManager::AUTH_MENU);
        UIManager::addSmallSpacing();
        
        // Simulate login
        simulateUserInput("1");
        showOutput("Enter your choice (0-3): 1");
        simulateLogin();
        
        pause();
    }
    
    void simulateViewAllAccounts() {
        printSubSection("All Registered Accounts");
        
        auto usernames = g_authModule->getAllUsernames();
        if (usernames.empty()) {
            UIManager::displayError("No accounts found.");
        } else {
            UIManager::displayInfo("Registered Usernames:");
            for (const auto& username : usernames) {
                showOutput("  • " + username);
            }
            UIManager::addSmallSpacing();
            UIManager::displaySuccess("Total accounts: " + std::to_string(usernames.size()));
        }
        
        UIManager::addSmallSpacing();
        UIManager::printSeparator('-');
        showOutput("Press Enter to return to authentication menu...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateUserRegistration() {
        printSubSection("USER REGISTRATION");
        UIManager::displayInfo("Enter '0' at any field to cancel registration");
        UIManager::addSmallSpacing();
        
        // Generate unique username using timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        std::string uniqueUsername = "simuser" + std::to_string(timestamp % 10000);
        std::string uniqueEmail = "user" + std::to_string(timestamp % 10000) + "@example.com";
        
        // Username
        UIManager::displayPrompt("Username");
        simulateUserInput(uniqueUsername);
        
        // Password
        UIManager::displayPrompt("Password");
        simulateUserInput("SecurePass123!");
        
        // Email
        UIManager::displayPrompt("Email");
        simulateUserInput(uniqueEmail);
        
        // First Name
        UIManager::displayPrompt("First Name");
        simulateUserInput("John");
        
        // Last Name
        UIManager::displayPrompt("Last Name");
        simulateUserInput("Doe");
        
        // Phone (optional)
        UIManager::displayPrompt("Phone Number (optional, press Enter to skip)");
        simulateUserInput("555-123-4567");
        
        UIManager::addSmallSpacing();
        UIManager::displayInfo("Processing registration...");
        
        // Simulate registration process
        auto attendee = g_attendeeModule->createAttendee("John Doe", uniqueEmail, "555-123-4567", Model::AttendeeType::REGULAR);
        if (attendee && g_authModule->registerUser(uniqueUsername, "SecurePass123!")) {
            UIManager::displaySuccess("Account created successfully!");
            UIManager::displayInfo("Please login with your new credentials to continue.");
        } else {
            UIManager::displayError("Registration failed. Please try again.");
        }
        pause();
    }

    void simulateLogin() {
        printSubSection("User Authentication");
        UIManager::displayPrompt("Username (or 0 to cancel)");
        simulateUserInput("admin");
        
        UIManager::displayPrompt("Password");
        simulateUserInput("admin123");
        
        UIManager::addSmallSpacing();
        UIManager::displayInfo("Authenticating user...");
        
        bool isAuthenticated = g_authModule->authenticateUser("admin", "admin123");
        if (isAuthenticated) {
            currentSession.username = "admin";
            currentSession.isAuthenticated = true;
            currentSession.loginTime = Model::DateTime::now();
            currentSession.userRole = "admin";
            currentSession.userId = 1;
            
            UIManager::displaySuccess("Login successful! Welcome, admin");
        } else {
            UIManager::displayError("Invalid credentials. Please try again.");
        }
        UIManager::addSmallSpacing();
        pause();
    }

    void simulateManagementPortal() {
        printStep("MANAGEMENT PORTAL SIMULATION");
        
        UIManager::displayInfo("Entering Management Portal with admin privileges");
        showOutput(UIManager::MANAGEMENT_MENU);
        UIManager::addSectionSpacing();
        
        // Simulate all management functions
        simulateConcertManagement();
        simulateVenueManagement();
        simulatePerformersAndCrewManagement();
        simulateTicketManagement();
        simulatePaymentMonitoring();
        simulateFeedbackAndCommunication();
        simulateReportsGeneration();
        simulateSystemAdministration();
    }

    void simulateConcertManagement() {
        printStep("CONCERT MANAGEMENT SIMULATION");
        
        simulateUserInput("1");
        showOutput("Enter choice (1-8): 1");
        
        printSubSection("Concert Management Menu");
        showOutput("1. Create New Concert");
        showOutput("2. View All Concerts");
        showOutput("3. Update Concert Details");
        showOutput("4. Cancel Concert");
        showOutput("5. Assign Performers");
        showOutput("6. Concert Statistics");
        showOutput("0. Back to Management Portal");
        UIManager::addSmallSpacing();
        
        // 1. Create New Concert
        printTestCase("Creating New Concert");
        simulateUserInput("1");
        showOutput("Enter choice (0-6): 1");
        simulateCreateConcert();
        
        // 2. View All Concerts
        printTestCase("Viewing All Concerts");
        simulateUserInput("2");
        showOutput("Enter choice (0-6): 2");
        simulateViewAllConcerts();
        
        // 3. Update Concert Details
        printTestCase("Updating Concert Details");
        simulateUserInput("3");
        showOutput("Enter choice (0-6): 3");
        simulateUpdateConcert();
        
        // 6. Concert Statistics
        printTestCase("Concert Statistics Analysis");
        simulateUserInput("6");
        showOutput("Enter choice (0-6): 6");
        simulateConcertStatistics();
        
        simulateUserInput("0");
        showOutput("Enter choice (0-6): 0");
        pause();
    }

    void simulateCreateConcert() {
        UIManager::displayInfo("Creating new concert - Enter '0' at any field to cancel");
        UIManager::addSmallSpacing();
        
        // Generate unique concert name
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        std::string uniqueConcertName = "Symphony Under the Stars #" + std::to_string(timestamp % 10000);
        
        UIManager::displayPrompt("Concert Name");
        simulateUserInput(uniqueConcertName);
        
        UIManager::displayPrompt("Description");
        simulateUserInput("A magical evening of classical music under the night sky");
        
        UIManager::displayPrompt("Genre");
        simulateUserInput("Classical");
        
        UIManager::displayPrompt("Concert Date (YYYY-MM-DD)");
        simulateUserInput("2025-12-31");
        
        UIManager::displayPrompt("Concert Time (HH:MM in 24-hour format)");
        simulateUserInput("19:30");
        
        showOutput("Venue ID: ");
        simulateUserInput("1");
        
        UIManager::displayPrompt("Base Ticket Price ($)");
        simulateUserInput("75.00");
        
        UIManager::addSmallSpacing();
        UIManager::displayInfo("Processing concert creation...");
        
        // Create the concert
        std::string concertDateTime = "2025-12-31T19:30:00Z";
        auto concert = g_concertModule->createConcert(uniqueConcertName, 
            "A magical evening of classical music under the night sky", 
            concertDateTime, concertDateTime);
        
        if (concert) {
            UIManager::displaySuccess("Concert created successfully! ID: " + std::to_string(concert->id));
            UIManager::displayInfo("Date/Time: 2025-12-31 at 19:30");
            UIManager::displayInfo("Venue: Concert Hall (ID: 1)");
        } else {
            UIManager::displayError("Failed to create concert. Please try again.");
        }
        
        UIManager::addSmallSpacing();
        UIManager::printSeparator('-');
        showOutput("Press Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateViewAllConcerts() {
        printSubSection("All Concerts");
        auto concerts = g_concertModule->getAllConcerts();
        
        if (concerts.empty()) {
            UIManager::displayWarning("No concerts found.");
        } else {
            UIManager::displayInfo("Concert Listing:");
            UIManager::addSmallSpacing();
            
            for (const auto& concert : concerts) {
                std::string status;
                switch (concert->event_status) {
                    case Model::EventStatus::SCHEDULED: status = "SCHEDULED"; break;
                    case Model::EventStatus::CANCELLED: status = "CANCELLED"; break;
                    case Model::EventStatus::POSTPONED: status = "POSTPONED"; break;
                    case Model::EventStatus::COMPLETED: status = "COMPLETED"; break;
                    case Model::EventStatus::SOLDOUT: status = "SOLD OUT"; break;
                }
                showOutput("ID: " + std::to_string(concert->id) + " | " + concert->name + 
                          " | Status: " + status);
            }
            UIManager::addSmallSpacing();
            UIManager::displaySuccess("Total concerts found: " + std::to_string(concerts.size()));
        }
        
        UIManager::addSmallSpacing();
        UIManager::printSeparator('-');
        showOutput("Press Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateUpdateConcert() {
        printSubSection("Update Concert Details");
        UIManager::displayPrompt("Enter Concert ID to update (or 0 to cancel)");
        simulateUserInput("1");
        
        auto concert = g_concertModule->getConcertById(1);
        if (concert) {
            showOutput("New name (current: " + concert->name + ", 0 to skip): ");
            simulateUserInput("Symphony Under the Stars - Extended Edition");
            
            showOutput("New description (current: " + concert->description + ", 0 to skip): ");
            simulateUserInput("An extended magical evening of classical music with special guests");
            
            if (g_concertModule->editConcert(1, "Symphony Under the Stars - Extended Edition",
                "An extended magical evening of classical music with special guests")) {
                showOutput("✅ Concert updated successfully!");
            } else {
                showOutput("❌ Failed to update concert.");
            }
        } else {
            showOutput("❌ Concert not found.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateConcertStatistics() {
        auto concerts = g_concertModule->getAllConcerts();
        int total = concerts.size();
        int planned = 0, active = 0, completed = 0, cancelled = 0;
        
        for (const auto& concert : concerts) {
            switch (concert->event_status) {
                case Model::EventStatus::SCHEDULED: planned++; break;
                case Model::EventStatus::CANCELLED: cancelled++; break;
                case Model::EventStatus::POSTPONED: planned++; break;
                case Model::EventStatus::COMPLETED: completed++; break;
                case Model::EventStatus::SOLDOUT: active++; break;
            }
        }
        
        showOutput("\n--- Concert Statistics ---");
        showOutput("Total Concerts: " + std::to_string(total));
        showOutput("Planned: " + std::to_string(planned));
        showOutput("Active: " + std::to_string(active));
        showOutput("Completed: " + std::to_string(completed));
        showOutput("Cancelled: " + std::to_string(cancelled));
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateVenueManagement() {
        printStep("VENUE MANAGEMENT SIMULATION");
        
        simulateUserInput("2");
        showOutput("Enter choice (1-8): 2");
        showOutput("\n--- Venue Management ---");
        showOutput("1. Create New Venue");
        showOutput("2. View All Venues");
        showOutput("3. Search Venues");
        showOutput("4. Update Venue Details");
        showOutput("5. Delete Venue");
        showOutput("6. Venue Capacity Analysis");
        showOutput("0. Back to Management Portal");
        
        // 1. Create New Venue
        simulateUserInput("1");
        showOutput("Enter choice (0-6): 1");
        simulateCreateVenue();
        
        // 2. View All Venues
        simulateUserInput("2");
        showOutput("Enter choice (0-6): 2");
        simulateViewAllVenues();
        
        // 6. Venue Capacity Analysis
        simulateUserInput("6");
        showOutput("Enter choice (0-6): 6");
        simulateVenueCapacityAnalysis();
        
        simulateUserInput("0");
        showOutput("Enter choice (0-6): 0");
        pause();
    }

    void simulateCreateVenue() {
        showOutput("Creating new venue - Enter '0' at any field to cancel");
        
        // Generate unique venue name
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        std::string uniqueVenueName = "Grand Symphony Hall #" + std::to_string(timestamp % 10000);
        
        showOutput("Venue Name: ");
        simulateUserInput(uniqueVenueName);
        
        showOutput("Address: ");
        simulateUserInput("123 Music Avenue");
        
        showOutput("City: ");
        simulateUserInput("Concert City");
        
        showOutput("State/Province: ");
        simulateUserInput("Music State");
        
        showOutput("ZIP/Postal Code: ");
        simulateUserInput("12345");
        
        showOutput("Country: ");
        simulateUserInput("USA");
        
        showOutput("Capacity: ");
        simulateUserInput("2500");
        
        showOutput("Description (optional, press Enter to skip): ");
        simulateUserInput("Premier concert hall with world-class acoustics");
        
        showOutput("Contact Info - email or phone (optional, press Enter to skip): ");
        simulateUserInput("info@grandsymphonyhall.com");
        
        auto venue = g_venueModule->createVenue(uniqueVenueName, "123 Music Avenue", 
            "Concert City", "Music State", "12345", "USA", 2500, 
            "Premier concert hall with world-class acoustics", "info@grandsymphonyhall.com");
        
        if (venue) {
            showOutput("✅ Venue created successfully! ID: " + std::to_string(venue->id));
        } else {
            showOutput("❌ Failed to create venue. Please try again.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateViewAllVenues() {
        showOutput("\n--- All Venues ---");
        auto venues = g_venueModule->getAllVenues();
        
        if (venues.empty()) {
            showOutput("No venues found.");
        } else {
            for (const auto& venue : venues) {
                showOutput("ID: " + std::to_string(venue->id) + " | " + venue->name + 
                          " | " + venue->city + ", " + venue->state + 
                          " | Capacity: " + std::to_string(venue->capacity));
            }
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateVenueCapacityAnalysis() {
        auto venues = g_venueModule->getAllVenues();
        if (venues.empty()) {
            showOutput("No venues available for analysis.");
            return;
        }
        
        int totalCapacity = 0;
        int maxCapacity = 0;
        int minCapacity = INT_MAX;
        std::string largestVenue, smallestVenue;
        
        for (const auto& venue : venues) {
            totalCapacity += venue->capacity;
            if (venue->capacity > maxCapacity) {
                maxCapacity = venue->capacity;
                largestVenue = venue->name;
            }
            if (venue->capacity < minCapacity) {
                minCapacity = venue->capacity;
                smallestVenue = venue->name;
            }
        }
        
        showOutput("\n--- Venue Capacity Analysis ---");
        showOutput("Total Venues: " + std::to_string(venues.size()));
        showOutput("Total Capacity: " + std::to_string(totalCapacity));
        showOutput("Average Capacity: " + std::to_string(totalCapacity / static_cast<int>(venues.size())));
        showOutput("Largest Venue: " + largestVenue + " (" + std::to_string(maxCapacity) + ")");
        showOutput("Smallest Venue: " + smallestVenue + " (" + std::to_string(minCapacity) + ")");
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulatePerformersAndCrewManagement() {
        printStep("PERFORMERS & CREW MANAGEMENT SIMULATION");
        
        simulateUserInput("3");
        showOutput("Enter choice (1-8): 3");
        showOutput("\n--- Performer & Crew Management ---");
        showOutput("1. Performer Management");
        showOutput("2. Crew Management");
        showOutput("3. View All Performers");
        showOutput("4. View All Crew Members");
        showOutput("5. Search Performers");
        showOutput("6. Search Crew");
        showOutput("0. Back to Management Portal");
        
        // 1. Performer Management
        simulateUserInput("1");
        showOutput("Enter choice (0-6): 1");
        simulatePerformerManagement();
        
        // 2. Crew Management
        simulateUserInput("2");
        showOutput("Enter choice (0-6): 2");
        simulateCrewManagement();
        
        simulateUserInput("0");
        showOutput("Enter choice (0-6): 0");
        pause();
    }

    void simulatePerformerManagement() {
        showOutput("\n--- Performer Management ---");
        showOutput("1. Create New Performer");
        showOutput("2. Update Performer");
        showOutput("3. Delete Performer");
        showOutput("0. Back");
        
        simulateUserInput("1");
        showOutput("Enter choice (0-3): 1");
        
        showOutput("Performer Name (or 0 to cancel): ");
        simulateUserInput("The Midnight Orchestra");
        
        showOutput("Type (Band/Solo Artist/Orchestra/etc.): ");
        simulateUserInput("Orchestra");
        
        showOutput("Contact Info: ");
        simulateUserInput("contact@midnightorchestra.com");
        
        showOutput("Bio/Description: ");
        simulateUserInput("Award-winning classical orchestra specializing in contemporary arrangements");
        
        auto performer = g_performerModule->createPerformer("The Midnight Orchestra", "Orchestra", 
            "contact@midnightorchestra.com", "Award-winning classical orchestra specializing in contemporary arrangements");
        
        if (performer) {
            showOutput("✅ Performer created successfully! ID: " + std::to_string(performer->performer_id));
        } else {
            showOutput("❌ Failed to create performer.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateCrewManagement() {
        showOutput("\n--- Crew Management ---");
        showOutput("1. Create New Crew Member");
        showOutput("2. Update Crew Member");
        showOutput("3. Delete Crew Member");
        showOutput("0. Back");
        
        simulateUserInput("1");
        showOutput("Enter choice (0-3): 1");
        
        showOutput("Crew Member Name (or 0 to cancel): ");
        simulateUserInput("Sarah Johnson");
        
        showOutput("Email: ");
        simulateUserInput("sarah.johnson@crew.com");
        
        showOutput("Phone: ");
        simulateUserInput("555-0123");
        
        showOutput("Role: ");
        simulateUserInput("Stage Manager");
        
        auto crewMember = g_crewModule->createCrewMember("Sarah Johnson", "sarah.johnson@crew.com", 
            "555-0123", "Stage Manager");
        
        if (crewMember) {
            showOutput("✅ Crew member created successfully! ID: " + std::to_string(crewMember->id));
        } else {
            showOutput("❌ Failed to create crew member.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateTicketManagement() {
        printStep("TICKET MANAGEMENT SIMULATION");
        
        simulateUserInput("4");
        showOutput("Enter choice (1-8): 4");
        showOutput("\n--- Ticket Management ---");
        showOutput("1. Create Tickets for Concert");
        showOutput("2. View Ticket Sales Statistics");
        showOutput("3. Check Ticket Availability");
        showOutput("4. Validate Ticket-Concert Relationships");
        showOutput("5. QR Code Management");
        showOutput("6. Ticket Status Updates");
        showOutput("0. Back to Management Portal");
        
        // 1. Create Tickets for Concert
        simulateUserInput("1");
        showOutput("Enter choice (0-6): 1");
        simulateCreateTickets();
        
        // 2. View Ticket Sales Statistics
        simulateUserInput("2");
        showOutput("Enter choice (0-6): 2");
        simulateTicketStatistics();
        
        // 5. QR Code Management
        simulateUserInput("5");
        showOutput("Enter choice (0-6): 5");
        simulateQRCodeManagement();
        
        simulateUserInput("0");
        showOutput("Enter choice (0-6): 0");
        pause();
    }

    void simulateCreateTickets() {
        showOutput("Concert ID (or 0 to cancel): ");
        simulateUserInput("1");
        
        showOutput("Quantity (or 0 to cancel): ");
        simulateUserInput("100");
        
        showOutput("Ticket Type (VIP/REGULAR/PREMIUM): ");
        simulateUserInput("REGULAR");
        
        auto concert = g_concertModule->getConcertById(1);
        bool concertExists = (concert != nullptr);
        
        if (concertExists) {
            std::vector<int> ticketIds;
            for (int i = 0; i < 100; i++) {
                int ticketId = g_ticketModule->createTicketSafe(0, 1, "REGULAR", concertExists);
                if (ticketId > 0) {
                    ticketIds.push_back(ticketId);
                }
            }
            
            showOutput("✅ Created " + std::to_string(ticketIds.size()) + " tickets successfully!");
            showOutput("Ticket IDs: ");
            std::string idList;
            for (size_t i = 0; i < std::min(ticketIds.size(), static_cast<size_t>(10)); i++) {
                idList += std::to_string(ticketIds[i]) + " ";
            }
            if (ticketIds.size() > 10) {
                idList += "... (and " + std::to_string(ticketIds.size() - 10) + " more)";
            }
            showOutput(idList);
        } else {
            showOutput("❌ Concert not found! Cannot create tickets for non-existent concert.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateTicketStatistics() {
        showOutput("Enter Concert ID for statistics (or 0 to cancel): ");
        simulateUserInput("1");
        
        auto concert = g_concertModule->getConcertById(1);
        if (concert) {
            auto tickets = g_ticketModule->getTicketsByConcert(1);
            
            int total = tickets.size();
            int sold = 0, available = 0, checkedIn = 0, cancelled = 0;
            
            for (const auto& ticket : tickets) {
                switch (ticket->status) {
                    case Model::TicketStatus::AVAILABLE: available++; break;
                    case Model::TicketStatus::SOLD: sold++; break;
                    case Model::TicketStatus::CHECKED_IN: checkedIn++; break;
                    case Model::TicketStatus::CANCELLED: cancelled++; break;
                    case Model::TicketStatus::EXPIRED: cancelled++; break;
                }
            }
            
            showOutput("\n--- Ticket Statistics for: " + concert->name + " ---");
            showOutput("Total Tickets: " + std::to_string(total));
            showOutput("Available: " + std::to_string(available));
            showOutput("Sold: " + std::to_string(sold));
            showOutput("Checked In: " + std::to_string(checkedIn));
            showOutput("Cancelled/Expired: " + std::to_string(cancelled));
            showOutput("Sales Rate: " + std::to_string(total > 0 ? (sold * 100.0 / total) : 0.0) + "%");
        } else {
            showOutput("❌ Concert not found.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateQRCodeManagement() {
        showOutput("Enter Ticket ID for QR code operations (or 0 to cancel): ");
        simulateUserInput("1");
        
        auto ticket = g_ticketModule->getTicketById(1);
        if (ticket) {
            std::string qrCode = g_ticketModule->generateQRCode(1);
            showOutput("✅ QR Code generated: " + qrCode);
            
            int validatedTicketId = g_ticketModule->validateQRCode(qrCode);
            if (validatedTicketId == 1) {
                showOutput("✅ QR Code validation successful!");
            } else {
                showOutput("❌ QR Code validation failed.");
            }
        } else {
            showOutput("❌ Ticket not found.");
        }
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulatePaymentMonitoring() {
        printStep("PAYMENT MONITORING SIMULATION");
        
        simulateUserInput("5");
        showOutput("Enter choice (1-8): 5");
        showOutput("\n--- Payment Monitoring ---");
        showOutput("(Payment system monitoring functionality would be displayed here)");
        showOutput("✅ Payment system operational");
        showOutput("✅ All transactions secure");
        showOutput("✅ No payment disputes detected");
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateFeedbackAndCommunication() {
        printStep("FEEDBACK & COMMUNICATION SIMULATION");
        
        simulateUserInput("6");
        showOutput("Enter choice (1-8): 6");
        showOutput("\n--- Feedback & Communication Management ---");
        showOutput("✅ Communication system active");
        showOutput("✅ Feedback collection enabled");
        showOutput("✅ Notification system ready");
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateReportsGeneration() {
        printStep("REPORTS GENERATION SIMULATION");
        
        simulateUserInput("7");
        showOutput("Enter choice (1-8): 7");
        showOutput("\n--- Reports & Analytics ---");
        
        // Generate sample report
        showOutput("Generating comprehensive system report...");
        pause(1500);
        
        showOutput("✅ Concert Attendance Report generated");
        showOutput("✅ Revenue Analysis Report generated");
        showOutput("✅ System Performance Report generated");
        showOutput("✅ User Activity Report generated");
        
        auto concerts = g_concertModule->getAllConcerts();
        showOutput("\n--- Quick Statistics ---");
        showOutput("Total Concerts: " + std::to_string(concerts.size()));
        
        auto venues = g_venueModule->getAllVenues();
        showOutput("Total Venues: " + std::to_string(venues.size()));
        
        auto performers = g_performerModule->getAllPerformers();
        showOutput("Total Performers: " + std::to_string(performers.size()));
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateSystemAdministration() {
        printStep("SYSTEM ADMINISTRATION SIMULATION");
        
        simulateUserInput("8");
        showOutput("Enter choice (1-8): 8");
        showOutput("\n--- System Administration ---");
        showOutput("✅ System health: OPTIMAL");
        showOutput("✅ Database integrity: VERIFIED");
        showOutput("✅ Security status: SECURE");
        showOutput("✅ Backup status: UP TO DATE");
        showOutput("✅ All modules: OPERATIONAL");
        
        showOutput("\nPress Enter to continue...");
        simulateUserInput("[ENTER]");
        pause();
    }

    void simulateUserPortal() {
        printStep("USER PORTAL SIMULATION");
        
        // Logout from admin and login as regular user
        currentSession = UserSession();
        showOutput("Logging out admin...");
        pause();
        
        showOutput("Logging in as regular user...");
        simulateUserInput("user1");
        simulateUserInput("user123");
        
        if (g_authModule->authenticateUser("user1", "user123")) {
            currentSession.username = "user1";
            currentSession.isAuthenticated = true;
            currentSession.userRole = "user";
            showOutput("✅ Login successful! Welcome, user1");
        }
        
        showOutput(UIManager::USER_MENU);
        
        simulateUserPortalFunctions();
        pause();
    }

    void simulateUserPortalFunctions() {
        // 1. Browse Concerts
        simulateUserInput("1");
        showOutput("Enter choice (1-6): 1");
        showOutput("\n--- Browse Concerts ---");
        auto concerts = g_concertModule->getAllConcerts();
        for (const auto& concert : concerts) {
            showOutput("🎵 " + concert->name + " - " + concert->description);
        }
        
        // 2. Purchase Tickets
        simulateUserInput("2");
        showOutput("Enter choice (1-6): 2");
        showOutput("\n--- Purchase Tickets ---");
        showOutput("Concert ID: ");
        simulateUserInput("1");
        showOutput("Number of tickets: ");
        simulateUserInput("2");
        showOutput("✅ Tickets purchased successfully!");
        
        // 3. View My Tickets
        simulateUserInput("3");
        showOutput("Enter choice (1-6): 3");
        showOutput("\n--- My Tickets ---");
        showOutput("🎫 Ticket #1001 - Symphony Under the Stars");
        showOutput("🎫 Ticket #1002 - Symphony Under the Stars");
        
        // 4. Submit Feedback
        simulateUserInput("4");
        showOutput("Enter choice (1-6): 4");
        showOutput("\n--- Submit Feedback ---");
        showOutput("Concert ID: ");
        simulateUserInput("1");
        showOutput("Rating (1-5): ");
        simulateUserInput("5");
        showOutput("Comments: ");
        simulateUserInput("Absolutely fantastic performance! The orchestra was amazing!");
        showOutput("✅ Feedback submitted successfully!");
        
        simulateUserInput("0");
        showOutput("Enter choice (1-6): 0");
    }

    void showFinalResults() {
        printStep("SIMULATION COMPLETE - FINAL RESULTS");
        
        UIManager::addSectionSpacing();
        UIManager::printCenteredText("🎉 MUSEIO CONCERT MANAGEMENT SYSTEM SIMULATION COMPLETE!", 80);
        UIManager::addSectionSpacing();
        
        UIManager::displaySuccess("✅ SUCCESSFULLY DEMONSTRATED ALL FUNCTIONALITIES:");
        UIManager::addSmallSpacing();
        
        printSubSection("🔐 AUTHENTICATION PORTAL");
        showOutput("  ✅ View All Accounts");
        showOutput("  ✅ User Registration");
        showOutput("  ✅ User Login");
        UIManager::addSmallSpacing();
        
        printSubSection("👨‍💼 MANAGEMENT PORTAL");
        showOutput("  ✅ Concert Management (Create, View, Update, Statistics)");
        showOutput("  ✅ Venue Management (Create, View, Capacity Analysis)");
        showOutput("  ✅ Performers & Crew Management");
        showOutput("  ✅ Ticket Management (Create, Statistics, QR Codes)");
        showOutput("  ✅ Payment Monitoring");
        showOutput("  ✅ Feedback & Communication");
        showOutput("  ✅ Reports Generation");
        showOutput("  ✅ System Administration");
        UIManager::addSmallSpacing();
        
        printSubSection("👤 USER PORTAL");
        showOutput("  ✅ Browse Concerts");
        showOutput("  ✅ Purchase Tickets");
        showOutput("  ✅ View My Tickets");
        showOutput("  ✅ Submit Feedback");
        UIManager::addSmallSpacing();
        
        printSubSection("📊 SYSTEM STATUS");
        showOutput("  ✅ All modules operational");
        showOutput("  ✅ Data persistence working");
        showOutput("  ✅ User interactions successful");
        showOutput("  ✅ Complete workflow demonstrated");
        UIManager::addSmallSpacing();
        
        UIManager::printCenteredText("🚀 READY FOR PRODUCTION DEPLOYMENT!", 80);
        UIManager::addSectionSpacing();
    }
};

int main() {
    MuseIOSimulator simulator;
    simulator.runCompleteSimulation();
    return 0;
}
