#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <vector>
#include <algorithm>
#include <sstream>
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
#include "include/uiModule.hpp"

// DataPaths namespace for centralized file path management
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

// Global module instances using DataPaths
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

// User session management
struct UserSession {
    int userId;
    std::string username;
    std::string userRole;
    bool isAuthenticated;
    Model::DateTime loginTime;
    
    UserSession() : userId(-1), isAuthenticated(false) {}
};

UserSession currentSession;

// Forward declarations for portal handlers
void runManagementPortal();
void runUserPortal();
void displayMainMenu();
bool initializeModules();
void cleanupModules();
bool authenticateUser();
void displayAuthMenu();
bool registerNewUser();
void logout();

// Module initialization function
bool initializeModules() {
    try {
        std::cout << "Initializing MuseIO Concert Management System...\n";
        
        // Initialize modules in dependency order
        g_authModule = std::make_unique<AuthModule>(DataPaths::AUTH_FILE);
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
        
        std::cout << "✅ All modules initialized successfully!\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to initialize modules: " << e.what() << std::endl;
        return false;
    }
}

// Cleanup function
void cleanupModules() {
    std::cout << "Saving data and cleaning up...\n";
    
    // Modules will auto-save on destruction
    g_commModule.reset();
    g_reportModule.reset();
    g_feedbackModule.reset();
    g_paymentModule.reset();
    g_ticketModule.reset();
    g_concertModule.reset();
    g_crewModule.reset();
    g_performerModule.reset();
    g_venueModule.reset();
    g_attendeeModule.reset();
    g_authModule.reset();
    
    std::cout << "✅ Cleanup completed successfully!\n";
}

// Authentication system implementation
void displayAuthMenu() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "           MuseIO Concert Management" << std::endl;
    std::cout << "               Authentication" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Register New Account" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

bool authenticateUser() {
    while (true) {
        displayAuthMenu();
        
        int choice;
        std::cout << "Enter your choice (1-3): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "❌ Invalid input. Please enter a number.\n";
            continue;
        }
        
        std::cin.ignore(); // Clear the newline character
        
        switch (choice) {
            case 1: { // Login
                std::string username, password;
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);
                
                if (g_authModule->authenticateUser(username, password)) {
                    currentSession.username = username;
                    currentSession.isAuthenticated = true;
                    currentSession.loginTime = Model::DateTime::now();
                    
                    // Get user role and ID (simplified - would need actual implementation)
                    currentSession.userRole = "user"; // Default role
                    currentSession.userId = 1; // Would get from auth system
                    
                    std::cout << "✅ Login successful! Welcome, " << username << std::endl;
                    return true;
                } else {
                    std::cout << "❌ Invalid credentials. Please try again.\n";
                }
                break;
            }
            case 2: { // Register
                if (registerNewUser()) {
                    std::cout << "✅ Registration successful! Please login with your new credentials.\n";
                }
                break;
            }
            case 3: // Exit
                return false;
            default:
                std::cout << "❌ Invalid choice. Please select a valid option.\n";
        }
    }
}

bool registerNewUser() {
    std::string username, password, email, firstName, lastName;
    
    std::cout << "\n--- User Registration ---\n";
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "First Name: ";
    std::getline(std::cin, firstName);
    std::cout << "Last Name: ";
    std::getline(std::cin, lastName);
    
    // Create new attendee account
    int attendeeId = g_attendeeModule->createAttendee(firstName, lastName, email, "");
    if (attendeeId > 0) {
        // Register user credentials
        if (g_authModule->registerUser(username, password)) {
            std::cout << "✅ Account created successfully!\n";
            return true;
        } else {
            std::cout << "❌ Failed to create user credentials.\n";
            return false;
        }
    } else {
        std::cout << "❌ Failed to create user profile.\n";
        return false;
    }
}

void logout() {
    std::cout << "Logging out " << currentSession.username << "...\n";
    currentSession = UserSession(); // Reset session
    std::cout << "✅ Logged out successfully!\n";
}

void displayMainMenu() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           MuseIO Concert Management System" << std::endl;
    std::cout << "               Welcome, " << currentSession.username << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "1. Management Portal (Admin/Staff)" << std::endl;
    std::cout << "2. User Portal (Concert Attendee)" << std::endl;
    std::cout << "3. Logout" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void displayManagementMenu() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "                 MANAGEMENT PORTAL" << std::endl;
    std::cout << "                User: " << currentSession.username << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "1.  Concert Management" << std::endl;
    std::cout << "2.  Venue Configuration" << std::endl;
    std::cout << "3.  Performer & Crew Management" << std::endl;
    std::cout << "4.  Ticket Management & Analytics" << std::endl;
    std::cout << "5.  Payment & Financial Monitoring" << std::endl;
    std::cout << "6.  Feedback & Communication Tools" << std::endl;
    std::cout << "7.  Reports & Analytics" << std::endl;
    std::cout << "8.  System Administration" << std::endl;
    std::cout << "9.  Switch to User Portal" << std::endl;
    std::cout << "10. Return to Main Menu" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void displayUserMenu() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "                    USER PORTAL" << std::endl;
    std::cout << "                User: " << currentSession.username << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "1. Browse Available Concerts" << std::endl;
    std::cout << "2. Purchase Tickets" << std::endl;
    std::cout << "3. My Tickets & Reservations" << std::endl;
    std::cout << "4. Submit Feedback" << std::endl;
    std::cout << "5. Browse Performers & Venues" << std::endl;
    std::cout << "6. Account & Profile Management" << std::endl;
    std::cout << "7. Switch to Management Portal" << std::endl;
    std::cout << "8. Return to Main Menu" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

// Management Portal Functions
void manageConcerts();
void manageVenues();
void managePerformersAndCrew();
void manageTickets();
void monitorPayments();
void manageFeedbackAndComm();
void generateReports();
void systemAdministration();

// User Portal Functions
void browseConcerts();
void purchaseTickets();
void manageMyTickets();
void submitFeedback();
void browsePerformersVenues();
void manageProfile();

// Concert Management Implementation
void manageConcerts() {
    while (true) {
        std::cout << "\n--- Concert Management ---\n";
        std::cout << "1. Create New Concert\n";
        std::cout << "2. View All Concerts\n";
        std::cout << "3. Update Concert Details\n";
        std::cout << "4. Cancel Concert\n";
        std::cout << "5. Assign Performers\n";
        std::cout << "6. Concert Statistics\n";
        std::cout << "7. Back to Management Portal\n";
        
        int choice;
        std::cout << "Enter choice (1-7): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();
        
        switch (choice) {
            case 1: { // Create New Concert
                std::string name, description, genre;
                int venueId;
                double ticketPrice;
                
                std::cout << "Concert Name: ";
                std::getline(std::cin, name);
                std::cout << "Description: ";
                std::getline(std::cin, description);
                std::cout << "Genre: ";
                std::getline(std::cin, genre);
                std::cout << "Venue ID: ";
                std::cin >> venueId;
                std::cout << "Base Ticket Price: $";
                std::cin >> ticketPrice;
                std::cin.ignore();
                
                // Validate venue exists
                auto venue = g_venueModule->getVenueById(venueId);
                if (!venue) {
                    std::cout << "❌ Venue not found. Please check the venue ID.\n";
                    break;
                }
                
                Model::DateTime concertDate = Model::DateTime::now(); // Simplified - would get from user
                int concertId = g_concertModule->createConcert(name, concertDate, venueId, description);
                
                if (concertId > 0) {
                    std::cout << "✅ Concert created successfully! ID: " << concertId << std::endl;
                } else {
                    std::cout << "❌ Failed to create concert.\n";
                }
                break;
            }
            case 2: { // View All Concerts
                auto concerts = g_concertModule->getAllConcerts();
                std::cout << "\n--- All Concerts ---\n";
                for (const auto& concert : concerts) {
                    std::cout << "ID: " << concert->id << " | " << concert->name 
                              << " | Venue: " << (concert->venue ? std::to_string(concert->venue->id) : "N/A") << " | Status: ";
                    switch (concert->event_status) {
                        case Model::EventStatus::SCHEDULED: std::cout << "SCHEDULED"; break;
                        case Model::EventStatus::CANCELLED: std::cout << "CANCELLED"; break;
                        case Model::EventStatus::POSTPONED: std::cout << "POSTPONED"; break;
                        case Model::EventStatus::COMPLETED: std::cout << "COMPLETED"; break;
                        case Model::EventStatus::SOLDOUT: std::cout << "SOLD OUT"; break;
                    }
                    std::cout << std::endl;
                }
                break;
            }
            case 3: { // Update Concert Details
                int concertId;
                std::cout << "Enter Concert ID to update: ";
                std::cin >> concertId;
                std::cin.ignore();
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                std::string newName, newDescription;
                std::cout << "New name (current: " << concert->name << "): ";
                std::getline(std::cin, newName);
                std::cout << "New description (current: " << concert->description << "): ";
                std::getline(std::cin, newDescription);
                
                if (g_concertModule->updateConcert(concertId, 
                    newName.empty() ? concert->name : newName,
                    concert->start_date_time, 
                    (concert->venue ? concert->venue->id : 0),
                    newDescription.empty() ? concert->description : newDescription)) {
                    std::cout << "✅ Concert updated successfully!\n";
                } else {
                    std::cout << "❌ Failed to update concert.\n";
                }
                break;
            }
            case 4: { // Cancel Concert
                int concertId;
                std::cout << "Enter Concert ID to cancel: ";
                std::cin >> concertId;
                std::cin.ignore();
                
                std::string reason;
                std::cout << "Cancellation reason: ";
                std::getline(std::cin, reason);
                
                if (g_concertModule->cancelConcert(concertId, reason)) {
                    std::cout << "✅ Concert cancelled successfully!\n";
                } else {
                    std::cout << "❌ Failed to cancel concert.\n";
                }
                break;
            }
            case 5: { // Assign Performers
                int concertId, performerId;
                std::cout << "Concert ID: ";
                std::cin >> concertId;
                std::cout << "Performer ID: ";
                std::cin >> performerId;
                
                // This would require enhanced concert-performer relationship management
                std::cout << "⚠️ Performer assignment feature requires enhanced relationship management.\n";
                break;
            }
            case 6: { // Concert Statistics
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
                
                std::cout << "\n--- Concert Statistics ---\n";
                std::cout << "Total Concerts: " << total << std::endl;
                std::cout << "Planned: " << planned << std::endl;
                std::cout << "Active: " << active << std::endl;
                std::cout << "Completed: " << completed << std::endl;
                std::cout << "Cancelled: " << cancelled << std::endl;
                break;
            }
            case 7: // Back to Management Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

// Enhanced Ticket Management with Validation
void manageTickets() {
    while (true) {
        std::cout << "\n--- Ticket Management ---\n";
        std::cout << "1. Create Tickets for Concert\n";
        std::cout << "2. View Ticket Sales Statistics\n";
        std::cout << "3. Check Ticket Availability\n";
        std::cout << "4. Validate Ticket-Concert Relationships\n";
        std::cout << "5. QR Code Management\n";
        std::cout << "6. Ticket Status Updates\n";
        std::cout << "7. Back to Management Portal\n";
        
        int choice;
        std::cout << "Enter choice (1-7): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();
        
        switch (choice) {
            case 1: { // Create Tickets with Enhanced Validation
                int concertId, quantity;
                std::string ticketType;
                
                std::cout << "Concert ID: ";
                std::cin >> concertId;
                std::cout << "Quantity: ";
                std::cin >> quantity;
                std::cin.ignore();
                std::cout << "Ticket Type (VIP/REGULAR/PREMIUM): ";
                std::getline(std::cin, ticketType);
                
                // Enhanced validation - check concert exists
                auto concert = g_concertModule->getConcertById(concertId);
                bool concertExists = (concert != nullptr);
                
                if (!concertExists) {
                    std::cout << "❌ Concert not found! Cannot create tickets for non-existent concert.\n";
                    break;
                }
                
                // Use enhanced ticket creation with validation
                std::vector<int> ticketIds;
                for (int i = 0; i < quantity; i++) {
                    // Create ticket without attendee initially (will be assigned during purchase)
                    if (g_ticketModule->validateTicketCreationWithConcert(0, concertId, ticketType, concertExists)) {
                        int ticketId = g_ticketModule->createTicketSafe(0, concertId, ticketType, concertExists);
                        if (ticketId > 0) {
                            ticketIds.push_back(ticketId);
                        }
                    }
                }
                
                std::cout << "✅ Created " << ticketIds.size() << " tickets successfully!\n";
                std::cout << "Ticket IDs: ";
                for (int id : ticketIds) {
                    std::cout << id << " ";
                }
                std::cout << std::endl;
                break;
            }
            case 2: { // Ticket Sales Statistics
                int concertId;
                std::cout << "Enter Concert ID for statistics: ";
                std::cin >> concertId;
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                auto tickets = g_ticketModule->getTicketsByConcert(concertId);
                
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
                
                std::cout << "\n--- Ticket Statistics for: " << concert->name << " ---\n";
                std::cout << "Total Tickets: " << total << std::endl;
                std::cout << "Available: " << available << std::endl;
                std::cout << "Sold: " << sold << std::endl;
                std::cout << "Checked In: " << checkedIn << std::endl;
                std::cout << "Cancelled/Expired: " << cancelled << std::endl;
                std::cout << "Sales Rate: " << (total > 0 ? (sold * 100.0 / total) : 0) << "%\n";
                break;
            }
            case 3: { // Check Availability
                auto concerts = g_concertModule->getAllConcerts();
                std::cout << "\n--- Ticket Availability Summary ---\n";
                
                for (const auto& concert : concerts) {
                    if (concert->event_status == Model::EventStatus::SCHEDULED || 
                        concert->event_status == Model::EventStatus::SOLDOUT) {
                        auto concertTickets = g_ticketModule->getTicketsByConcert(concert->id);
                        int available = 0;
                        for (const auto& ticket : concertTickets) {
                            if (ticket->status == Model::TicketStatus::AVAILABLE) {
                                available++;
                            }
                        }
                        std::cout << concert->name << " (ID: " << concert->id 
                                  << ") - Available: " << available << " tickets\n";
                    }
                }
                break;
            }
            case 4: { // Validate Relationships
                std::cout << "Validating ticket-concert relationships...\n";
                int invalidCount = 0;
                
                // This would implement comprehensive validation
                auto allTickets = g_ticketModule->getAllTickets();
                for (const auto& ticket : allTickets) {
                    auto concert = g_concertModule->getConcertById(ticket->concert_id);
                    if (!concert) {
                        std::cout << "❌ Orphaned ticket found - ID: " << ticket->id 
                                  << " references non-existent concert: " << ticket->concert_id << std::endl;
                        invalidCount++;
                    }
                }
                
                if (invalidCount == 0) {
                    std::cout << "✅ All ticket-concert relationships are valid!\n";
                } else {
                    std::cout << "⚠️ Found " << invalidCount << " invalid relationships.\n";
                }
                break;
            }
            case 5: { // QR Code Management
                int ticketId;
                std::cout << "Enter Ticket ID for QR code operations: ";
                std::cin >> ticketId;
                
                auto ticket = g_ticketModule->getTicketById(ticketId);
                if (!ticket) {
                    std::cout << "❌ Ticket not found.\n";
                    break;
                }
                
                std::string qrCode = g_ticketModule->generateQRCode(ticketId);
                std::cout << "✅ QR Code generated: " << qrCode << std::endl;
                
                // Validate the QR code
                int validatedTicketId = g_ticketModule->validateQRCode(qrCode);
                if (validatedTicketId == ticketId) {
                    std::cout << "✅ QR Code validation successful!\n";
                } else {
                    std::cout << "❌ QR Code validation failed.\n";
                }
                break;
            }
            case 6: { // Ticket Status Updates
                int ticketId;
                std::cout << "Enter Ticket ID to update: ";
                std::cin >> ticketId;
                
                auto ticket = g_ticketModule->getTicketById(ticketId);
                if (!ticket) {
                    std::cout << "❌ Ticket not found.\n";
                    break;
                }
                
                std::cout << "Current status: ";
                switch (ticket->status) {
                    case Model::TicketStatus::AVAILABLE: std::cout << "AVAILABLE"; break;
                    case Model::TicketStatus::SOLD: std::cout << "SOLD"; break;
                    case Model::TicketStatus::CHECKED_IN: std::cout << "CHECKED_IN"; break;
                    case Model::TicketStatus::CANCELLED: std::cout << "CANCELLED"; break;
                    case Model::TicketStatus::EXPIRED: std::cout << "EXPIRED"; break;
                }
                std::cout << std::endl;
                
                std::cout << "New status (0=AVAILABLE, 1=SOLD, 2=CHECKED_IN, 3=CANCELLED, 4=EXPIRED): ";
                int statusChoice;
                std::cin >> statusChoice;
                
                Model::TicketStatus newStatus = static_cast<Model::TicketStatus>(statusChoice);
                if (g_ticketModule->updateTicketStatus(ticketId, newStatus)) {
                    std::cout << "✅ Ticket status updated successfully!\n";
                } else {
                    std::cout << "❌ Failed to update ticket status.\n";
                }
                break;
            }
            case 7: // Back
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

// Placeholder implementations for other management functions
void manageVenues() {
    std::cout << "🚧 Venue Management - Under Development\n";
    std::cout << "Available features: Venue creation, capacity management, scheduling\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void managePerformersAndCrew() {
    std::cout << "🚧 Performer & Crew Management - Under Development\n";
    std::cout << "Available features: Artist booking, crew assignment, coordination\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void monitorPayments() {
    std::cout << "🚧 Payment Monitoring - Under Development\n";
    std::cout << "Available features: Transaction tracking, financial reports\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void manageFeedbackAndComm() {
    while (true) {
        std::cout << "\n--- Feedback & Communication Management ---\n";
        std::cout << "1. View All Feedback\n";
        std::cout << "2. Check Critical/Urgent Feedback\n";
        std::cout << "3. Respond to Feedback\n";
        std::cout << "4. Communication Logs\n";
        std::cout << "5. Back to Management Portal\n";
        
        int choice;
        std::cout << "Enter choice (1-5): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        switch (choice) {
            case 1: { // View All Feedback
                auto allFeedback = g_feedbackModule->getAllFeedback();
                std::cout << "\n--- All Feedback ---\n";
                for (const auto& feedback : allFeedback) {
                    std::cout << "ID: " << feedback->id << " | Rating: " << feedback->rating 
                              << "/5 | " << feedback->content.substr(0, 50) << "...\n";
                }
                break;
            }
            case 2: { // Check Critical Feedback
                std::cout << "🚨 Checking for critical feedback with urgent keywords...\n";
                auto allFeedback = g_feedbackModule->getAllFeedback();
                bool foundCritical = false;
                
                // Critical keywords that trigger escalation
                std::vector<std::string> criticalKeywords = {
                    "terrible", "awful", "worst", "horrible", "unsafe", 
                    "dangerous", "emergency", "urgent", "complaint", "refund"
                };
                
                for (const auto& feedback : allFeedback) {
                    std::string content = feedback->content;
                    std::transform(content.begin(), content.end(), content.begin(), ::tolower);
                    
                    for (const auto& keyword : criticalKeywords) {
                        if (content.find(keyword) != std::string::npos) {
                            std::cout << "🚨 CRITICAL FEEDBACK DETECTED:\n";
                            std::cout << "   ID: " << feedback->id << " | Rating: " << feedback->rating 
                                      << "/5\n   Content: " << feedback->content << "\n";
                            std::cout << "   Keyword: '" << keyword << "' detected\n\n";
                            foundCritical = true;
                            break;
                        }
                    }
                }
                
                if (!foundCritical) {
                    std::cout << "✅ No critical feedback requiring immediate attention.\n";
                }
                break;
            }
            case 3: { // Respond to Feedback
                std::cout << "🚧 Feedback Response System - Under Development\n";
                break;
            }
            case 4: { // Communication Logs
                std::cout << "🚧 Communication Logs - Under Development\n";
                break;
            }
            case 5: // Back
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void generateReports() {
    std::cout << "🚧 Reports & Analytics - Under Development\n";
    std::cout << "Available features: Revenue reports, attendance analytics, performance metrics\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void systemAdministration() {
    std::cout << "🚧 System Administration - Under Development\n";
    std::cout << "Available features: User management, system settings, data backup\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

// Management Portal Main Function
void runManagementPortal() {
    while (true) {
        displayManagementMenu();
        
        int choice;
        std::cout << "Enter your choice (1-10): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        switch (choice) {
            case 1: // Concert Management
                manageConcerts();
                break;
            case 2: // Venue Configuration
                manageVenues();
                break;
            case 3: // Performer & Crew Management
                managePerformersAndCrew();
                break;
            case 4: // Ticket Management
                manageTickets();
                break;
            case 5: // Payment Monitoring
                monitorPayments();
                break;
            case 6: // Feedback & Communication
                manageFeedbackAndComm();
                break;
            case 7: // Reports & Analytics
                generateReports();
                break;
            case 8: // System Administration
                systemAdministration();
                break;
            case 9: // Switch to User Portal
                runUserPortal();
                break;
            case 10: // Return to Main Menu
                return;
            default:
                std::cout << "❌ Invalid choice. Please select a valid option.\n";
        }
    }
}


void runUserPortal() {
    while (true) {
        displayUserMenu();
        
        int choice;
        std::cout << "Enter your choice (1-8): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        switch (choice) {
            case 1: // Browse Available Concerts
                browseConcerts();
                break;
            case 2: // Purchase Tickets
                purchaseTickets();
                break;
            case 3: // My Tickets & Reservations
                manageMyTickets();
                break;
            case 4: // Submit Feedback
                submitFeedback();
                break;
            case 5: // Browse Performers & Venues
                browsePerformersVenues();
                break;
            case 6: // Account & Profile Management
                manageProfile();
                break;
            case 7: // Switch to Management Portal
                runManagementPortal();
                break;
            case 8: // Return to Main Menu
                return;
            default:
                std::cout << "❌ Invalid choice. Please select a valid option.\n";
        }
    }
}

// User Portal Functions Implementation
void browseConcerts() {
    while (true) {
        std::cout << "\n--- Browse Available Concerts ---\n";
        std::cout << "1. View All Upcoming Concerts\n";
        std::cout << "2. Search by Genre\n";
        std::cout << "3. Search by Venue\n";
        std::cout << "4. View Concert Details\n";
        std::cout << "5. Back to User Portal\n";
        
        int choice;
        std::cout << "Enter choice (1-5): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();
        
        switch (choice) {
            case 1: { // View All Upcoming Concerts
                auto concerts = g_concertModule->getAllConcerts();
                std::cout << "\n--- Upcoming Concerts ---\n";
                
                for (const auto& concert : concerts) {
                    if (concert->event_status == Model::EventStatus::SCHEDULED || 
                        concert->event_status == Model::EventStatus::SOLDOUT) {
                        
                        auto concertTickets = g_ticketModule->getTicketsByConcert(concert->id);
                        int available = 0;
                        for (const auto& ticket : concertTickets) {
                            if (ticket->status == Model::TicketStatus::AVAILABLE) {
                                available++;
                            }
                        }
                        
                        std::cout << "🎵 " << concert->name << " (ID: " << concert->id << ")\n";
                        std::cout << "   Description: " << concert->description << "\n";
                        std::cout << "   Date: " << concert->start_date_time.iso8601String << "\n";
                        std::cout << "   Venue: " << (concert->venue ? std::to_string(concert->venue->id) : "TBA") << "\n";
                        std::cout << "   Available Tickets: " << available << "\n";
                        std::cout << "   Status: ";
                        if (concert->event_status == Model::EventStatus::SOLDOUT) {
                            std::cout << "SOLD OUT";
                        } else {
                            std::cout << "TICKETS AVAILABLE";
                        }
                        std::cout << "\n\n";
                    }
                }
                break;
            }
            case 2: { // Search by Genre
                std::string genre;
                std::cout << "Enter genre to search for: ";
                std::getline(std::cin, genre);
                
                auto concerts = g_concertModule->searchConcertsByGenre(genre);
                std::cout << "\n--- Concerts in " << genre << " ---\n";
                
                for (const auto& concert : concerts) {
                    std::cout << "🎵 " << concert->name << " (ID: " << concert->id << ")\n";
                    std::cout << "   " << concert->description << "\n\n";
                }
                break;
            }
            case 3: { // Search by Venue
                int venueId;
                std::cout << "Enter venue ID: ";
                std::cin >> venueId;
                
                auto concerts = g_concertModule->getConcertsByVenue(venueId);
                std::cout << "\n--- Concerts at Venue " << venueId << " ---\n";
                
                for (const auto& concert : concerts) {
                    std::cout << "🎵 " << concert->name << " (ID: " << concert->id << ")\n";
                    std::cout << "   Date: " << concert->start_date_time.iso8601String << "\n\n";
                }
                break;
            }
            case 4: { // View Concert Details
                int concertId;
                std::cout << "Enter concert ID: ";
                std::cin >> concertId;
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                std::cout << "\n--- Concert Details ---\n";
                std::cout << "Name: " << concert->name << "\n";
                std::cout << "Description: " << concert->description << "\n";
                std::cout << "Start: " << concert->start_date_time.iso8601String << "\n";
                std::cout << "End: " << concert->end_date_time.iso8601String << "\n";
                std::cout << "Venue ID: " << (concert->venue ? std::to_string(concert->venue->id) : "TBA") << "\n";
                
                // Show ticket availability
                auto tickets = g_ticketModule->getTicketsByConcert(concertId);
                int available = 0, sold = 0;
                for (const auto& ticket : tickets) {
                    if (ticket->status == Model::TicketStatus::AVAILABLE) available++;
                    else if (ticket->status == Model::TicketStatus::SOLD) sold++;
                }
                std::cout << "Tickets Available: " << available << "\n";
                std::cout << "Tickets Sold: " << sold << "\n";
                break;
            }
            case 5: // Back
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void purchaseTickets() {
    while (true) {
        std::cout << "\n--- Purchase Tickets ---\n";
        std::cout << "1. Quick Purchase (Concert ID)\n";
        std::cout << "2. Browse and Purchase\n";
        std::cout << "3. Check Availability\n";
        std::cout << "4. Back to User Portal\n";
        
        int choice;
        std::cout << "Enter choice (1-4): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore();
        
        switch (choice) {
            case 1: { // Quick Purchase
                int concertId, quantity;
                std::string ticketType;
                
                std::cout << "Concert ID: ";
                std::cin >> concertId;
                std::cout << "Quantity: ";
                std::cin >> quantity;
                std::cin.ignore();
                std::cout << "Ticket Type (VIP/REGULAR/PREMIUM): ";
                std::getline(std::cin, ticketType);
                
                // Enhanced validation - check concert exists
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found!\n";
                    break;
                }
                
                // Use enhanced ticket creation with proper user association
                std::vector<int> purchasedTickets;
                for (int i = 0; i < quantity; i++) {
                    // Find an available ticket for this concert
                    auto availableTickets = g_ticketModule->getTicketsByConcert(concertId);
                    bool ticketPurchased = false;
                    
                    for (const auto& ticket : availableTickets) {
                        if (ticket->status == Model::TicketStatus::AVAILABLE && 
                            ticket->ticket_type == ticketType) {
                            
                            // Update ticket status and associate with user
                            if (g_ticketModule->updateTicketStatus(ticket->id, Model::TicketStatus::SOLD)) {
                                // Associate ticket with current user
                                auto attendee = g_attendeeModule->getAttendeeById(currentSession.userId);
                                if (attendee && g_ticketModule->setTicketAttendee(ticket->id, attendee)) {
                                    purchasedTickets.push_back(ticket->id);
                                    ticketPurchased = true;
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (!ticketPurchased) {
                        std::cout << "⚠️ Could not purchase ticket " << (i + 1) << " - no available tickets of type " << ticketType << "\n";
                        break;
                    }
                }
                
                if (!purchasedTickets.empty()) {
                    std::cout << "✅ Successfully purchased " << purchasedTickets.size() << " tickets!\n";
                    std::cout << "Ticket IDs: ";
                    for (int id : purchasedTickets) {
                        std::cout << id << " ";
                        // Generate QR code for each ticket
                        std::string qrCode = g_ticketModule->generateQRCode(id);
                        std::cout << "(QR: " << qrCode << ") ";
                    }
                    std::cout << "\n";
                } else {
                    std::cout << "❌ Failed to purchase any tickets.\n";
                }
                break;
            }
            case 2: { // Browse and Purchase
                std::cout << "🚧 Browse and Purchase - Redirecting to concert browser...\n";
                browseConcerts();
                break;
            }
            case 3: { // Check Availability
                int concertId;
                std::cout << "Enter Concert ID: ";
                std::cin >> concertId;
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                auto tickets = g_ticketModule->getTicketsByConcert(concertId);
                std::map<std::string, int> availability;
                
                for (const auto& ticket : tickets) {
                    if (ticket->status == Model::TicketStatus::AVAILABLE) {
                        availability[ticket->ticket_type]++;
                    }
                }
                
                std::cout << "\n--- Ticket Availability for " << concert->name << " ---\n";
                for (const auto& pair : availability) {
                    std::cout << pair.first << ": " << pair.second << " available\n";
                }
                break;
            }
            case 4: // Back
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void manageMyTickets() {
    while (true) {
        std::cout << "\n--- My Tickets & Reservations ---\n";
        std::cout << "1. View My Active Tickets\n";
        std::cout << "2. View Ticket Details\n";
        std::cout << "3. Generate QR Code\n";
        std::cout << "4. Check-in with QR Code\n";
        std::cout << "5. Cancel Ticket\n";
        std::cout << "6. Back to User Portal\n";
        
        int choice;
        std::cout << "Enter choice (1-6): ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        switch (choice) {
            case 1: { // View My Active Tickets
                auto myTickets = g_ticketModule->getActiveTicketsByAttendee(currentSession.userId);
                std::cout << "\n--- My Active Tickets ---\n";
                
                if (myTickets.empty()) {
                    std::cout << "You have no active tickets.\n";
                } else {
                    for (const auto& ticket : myTickets) {
                        auto concert = g_concertModule->getConcertById(ticket->concert_id);
                        std::cout << "🎫 Ticket ID: " << ticket->id << "\n";
                        std::cout << "   Concert: " << (concert ? concert->name : "Unknown") << "\n";
                        std::cout << "   Type: " << ticket->ticket_type << "\n";
                        std::cout << "   Status: ";
                        switch (ticket->status) {
                            case Model::TicketStatus::SOLD: std::cout << "PURCHASED"; break;
                            case Model::TicketStatus::CHECKED_IN: std::cout << "CHECKED IN"; break;
                            default: std::cout << "ACTIVE"; break;
                        }
                        std::cout << "\n   Purchase Date: " << ticket->purchase_date.iso8601String << "\n\n";
                    }
                }
                break;
            }
            case 2: { // View Ticket Details
                int ticketId;
                std::cout << "Enter Ticket ID: ";
                std::cin >> ticketId;
                
                auto ticket = g_ticketModule->getTicketById(ticketId);
                if (!ticket) {
                    std::cout << "❌ Ticket not found.\n";
                    break;
                }
                
                // Verify ticket belongs to user (simplified check)
                auto concert = g_concertModule->getConcertById(ticket->concert_id);
                
                std::cout << "\n--- Ticket Details ---\n";
                std::cout << "Ticket ID: " << ticket->id << "\n";
                std::cout << "Concert: " << (concert ? concert->name : "Unknown") << "\n";
                std::cout << "Type: " << ticket->ticket_type << "\n";
                std::cout << "Price: $" << ticket->price << "\n";
                std::cout << "Purchase Date: " << ticket->purchase_date.iso8601String << "\n";
                std::cout << "Concert Date: " << (concert ? concert->start_date_time.iso8601String : "TBA") << "\n";
                break;
            }
            case 3: { // Generate QR Code
                int ticketId;
                std::cout << "Enter Ticket ID: ";
                std::cin >> ticketId;
                
                auto ticket = g_ticketModule->getTicketById(ticketId);
                if (!ticket) {
                    std::cout << "❌ Ticket not found.\n";
                    break;
                }
                
                std::string qrCode = g_ticketModule->generateQRCode(ticketId);
                std::cout << "✅ QR Code for Ticket " << ticketId << ": " << qrCode << "\n";
                std::cout << "Present this code at the venue for entry.\n";
                break;
            }
            case 4: { // Check-in with QR Code
                std::string qrCode;
                std::cout << "Enter QR Code: ";
                std::cin.ignore();
                std::getline(std::cin, qrCode);
                
                if (g_ticketModule->checkInWithQRCode(qrCode)) {
                    std::cout << "✅ Check-in successful! Welcome to the concert!\n";
                } else {
                    std::cout << "❌ Check-in failed. Please verify your QR code.\n";
                }
                break;
            }
            case 5: { // Cancel Ticket
                int ticketId;
                std::cout << "Enter Ticket ID to cancel: ";
                std::cin >> ticketId;
                std::cin.ignore();
                
                std::string reason;
                std::cout << "Cancellation reason: ";
                std::getline(std::cin, reason);
                
                if (g_ticketModule->cancelTicket(ticketId, reason)) {
                    std::cout << "✅ Ticket cancelled successfully.\n";
                } else {
                    std::cout << "❌ Failed to cancel ticket.\n";
                }
                break;
            }
            case 6: // Back
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void submitFeedback() {
    std::cout << "\n--- Submit Feedback ---\n";
    
    int concertId, rating;
    std::string feedbackText;
    
    std::cout << "Concert ID (or 0 for general feedback): ";
    std::cin >> concertId;
    std::cout << "Rating (1-5): ";
    std::cin >> rating;
    std::cin.ignore();
    std::cout << "Your feedback: ";
    std::getline(std::cin, feedbackText);
    
    int feedbackId = g_feedbackModule->createFeedback(currentSession.userId, concertId, rating, feedbackText);
    
    if (feedbackId > 0) {
        std::cout << "✅ Thank you for your feedback! (ID: " << feedbackId << ")\n";
        
        // Check if feedback contains critical keywords
        std::string lowerText = feedbackText;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        
        std::vector<std::string> criticalKeywords = {
            "terrible", "awful", "worst", "horrible", "unsafe", 
            "dangerous", "emergency", "urgent", "complaint", "refund"
        };
        
        bool isCritical = false;
        for (const auto& keyword : criticalKeywords) {
            if (lowerText.find(keyword) != std::string::npos) {
                isCritical = true;
                break;
            }
        }
        
        if (isCritical || rating <= 2) {
            std::cout << "🚨 Your feedback has been flagged for urgent review.\n";
            std::cout << "A manager will contact you soon.\n";
        }
    } else {
        std::cout << "❌ Failed to submit feedback. Please try again.\n";
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

void browsePerformersVenues() {
    std::cout << "🚧 Browse Performers & Venues - Under Development\n";
    std::cout << "Available features: Artist profiles, venue information, upcoming shows\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void manageProfile() {
    std::cout << "🚧 Profile Management - Under Development\n";
    std::cout << "Available features: Update personal info, preferences, notification settings\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}


int main() {
    std::cout << "🎵 Welcome to MuseIO Concert Management System 🎵\n\n";
    
    // Initialize all modules
    if (!initializeModules()) {
        std::cerr << "Failed to initialize system. Exiting...\n";
        return -1;
    }
    
    // Authentication loop
    while (true) {
        if (!authenticateUser()) {
            std::cout << "Goodbye! Thank you for using MuseIO.\n";
            break;
        }
        
        // Main application loop after successful authentication
        while (currentSession.isAuthenticated) {
            displayMainMenu();
            
            int choice;
            std::cout << "Enter your choice (1-3): ";
            
            // Robust input validation
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "❌ Invalid input. Please enter a number.\n";
                continue;
            }

            switch (choice) {
                case 1: // Management Portal
                    std::cout << "Entering Management Portal...\n";
                    runManagementPortal();
                    break;
                case 2: // User Portal
                    std::cout << "Entering User Portal...\n";
                    runUserPortal();
                    break;
                case 3: // Logout
                    logout();
                    break;
                default:
                    std::cout << "❌ Invalid choice. Please select a valid option.\n";
            }
        }
    }
    
    // Cleanup before exit
    cleanupModules();
    return 0;
}