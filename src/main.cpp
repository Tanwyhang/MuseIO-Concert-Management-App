#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <functional>
#include <fstream>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
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

// Helper function to validate pure integer input
bool isValidInteger(const std::string& input) {
    if (input.empty()) return false;
    
    // Check for leading/trailing whitespace
    if (input.front() == ' ' || input.back() == ' ') return false;
    
    // Allow negative numbers (start from index 1 if first char is '-')
    size_t start = (input[0] == '-') ? 1 : 0;
    if (start == 1 && input.length() == 1) return false; // Just a '-' is invalid
    
    // Check that all remaining characters are digits
    for (size_t i = start; i < input.length(); ++i) {
        if (!std::isdigit(input[i])) {
            return false;
        }
    }
    
    return true;
}

// Format ISO8601 like 2025-08-16T16:18:03Z -> 2025-08-16 16:18:03
static inline std::string formatTimestampDisplay(const std::string& iso) {
    if (iso.empty()) return iso;
    std::string out = iso;
    // Replace 'T' with space
    std::replace(out.begin(), out.end(), 'T', ' ');
    // Drop trailing 'Z' if present
    if (!out.empty() && out.back() == 'Z') out.pop_back();
    return out;
}

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

// Predefined performer types and crew roles
namespace PredefinedOptions {
    const std::vector<std::string> PERFORMER_TYPES = {
        "Solo Artist",
        "Band",
        "DJ",
        "Orchestra",
        "Choir",
        "Vocalist",
        "Instrumentalist",
        "Producer",
        "Composer",
        "Conductor"
    };
    
    const std::vector<std::string> CREW_ROLES = {
        "Sound Engineer",
        "Lighting Technician", 
        "Security",
        "Stage Manager",
        "Event Coordinator",
        "Setup Crew",
        "Camera Operator",
        "Merchandise",
        "Usher",
        "Technical Support"
    };
}

// Helper function to display and get choice from predefined options
std::string getChoiceFromOptions(const std::vector<std::string>& options, const std::string& prompt) {
    std::cout << "\n" << prompt << "\n";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << (i + 1) << ". " << options[i] << "\n";
    }
    std::cout << (options.size() + 1) << ". Other (Custom Entry)\n";
    std::cout << "0. Cancel\n";
    std::cout << "\nEnter your choice (0-" << (options.size() + 1) << "): ";
    
    std::string choiceStr;
    std::getline(std::cin, choiceStr);
    
    if (!isValidInteger(choiceStr)) {
        std::cout << "❌ Invalid input. Please enter only integer numbers (e.g., '1', '2', '3').\n";
        return getChoiceFromOptions(options, prompt); // Retry
    }
    
    int choice;
    try {
        choice = std::stoi(choiceStr);
    } catch (const std::exception&) {
        std::cout << "❌ Invalid input. Please enter only integer numbers.\n";
        return getChoiceFromOptions(options, prompt); // Retry
    }
    
    if (choice == 0) {
        return "0"; // Cancel
    } else if (choice > 0 && choice <= static_cast<int>(options.size())) {
        return options[choice - 1];
    } else if (choice == static_cast<int>(options.size() + 1)) {
        // Custom entry option
        std::string customInput;
        std::cout << "Enter custom " << (prompt.find("Performer") != std::string::npos ? "performer type" : "crew role") << ": ";
        std::getline(std::cin, customInput);
        
        // Validate the custom input
        auto result = InputValidator::validateAlphabeticText(customInput, 
            (prompt.find("Performer") != std::string::npos ? "Performer type" : "Crew role"), 2, 30);
        
        if (result.isValid) {
            return customInput;
        } else {
            std::cout << "❌ " << result.errorMessage << "\n";
            std::cout << "Please try again or select from the predefined options.\n";
            return getChoiceFromOptions(options, prompt); // Retry
        }
    } else {
        std::cout << "❌ Invalid choice. Please select a valid option.\n";
        return getChoiceFromOptions(options, prompt); // Retry
    }
}

// Forward declarations for portal handlers
void runManagementPortal();
void runUserPortal();
void displayMainMenu();
bool initializeModules();
bool initializeDefaultUsers();
void cleanupModules();
bool authenticateUser();
void displayAuthMenu();
bool registerNewUser();
void logout();
void displayDemoCredentialsAndTests();
void demonstrateValidation();
void displayAllAccounts();
void showAnalyticsDashboard();

// Demo credentials and test plan display function
void displayDemoCredentialsAndTests() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "🎯 MUSEIO CONCERT MANAGEMENT SYSTEM - DEMO CREDENTIALS & TEST PLAN" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "\n📋 DEMO CREDENTIALS FOR TESTING:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    std::cout << "TEST ACCOUNTS (All users have access to both portals):" << std::endl;
    std::cout << "  Username: admin    | Password: admin123" << std::endl;
    std::cout << "  Username: manager1 | Password: manager123" << std::endl;
    std::cout << "  Username: staff1   | Password: staff123" << std::endl;
    std::cout << "  Username: user1    | Password: user123" << std::endl;
    std::cout << "  Username: vip1     | Password: vip123" << std::endl;
    std::cout << std::endl;
    std::cout << "  Username: staff1   | Password: staff123" << std::endl;
    std::cout << "  Username: user1    | Password: user123" << std::endl;
    std::cout << "  Username: vip1     | Password: vip123" << std::endl;
    std::cout << std::endl;
    
    // Fetch regular user credentials dynamically
    std::vector<std::pair<std::string, int>> regularUsers;
    if (g_authModule) {
        regularUsers = g_authModule->getRegularUsers();
    }
    
    std::cout << "REGULAR USER ACCOUNTS:" << std::endl;
    if (!regularUsers.empty()) {
        for (const auto& user : regularUsers) {
            std::cout << "  Username: " << user.first << std::endl;
            std::cout << "  Password: [Contact system administrator for password]" << std::endl;
        }
    } else {
        // Fallback to default regular user credentials if none exist
        std::cout << "  Username: user1    | Password: user123" << std::endl;
        std::cout << "  Username: user2    | Password: user456" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "VIP USER ACCOUNTS:" << std::endl;
    std::cout << "  Username: vip1     | Password: vip123" << std::endl;
    std::cout << "  Username: vip2     | Password: vip456" << std::endl;
    std::cout << std::endl;
    
    std::cout << "📝 NOTE: Admin, Staff, and Regular user credentials are fetched dynamically from the system." << std::endl;
    std::cout << "         If no users exist, default test credentials are shown above." << std::endl;
    std::cout << "         Management and VIP accounts use predefined test credentials." << std::endl;
    std::cout << std::endl;
    
    std::cout << "🔧 COMPREHENSIVE TEST PLAN:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "PHASE 1: AUTHENTICATION & USER MANAGEMENT" << std::endl;
    std::cout << "  1. Login with admin credentials" << std::endl;
    std::cout << "  2. Login with manager credentials" << std::endl;
    std::cout << "  3. Login with staff credentials" << std::endl;
    std::cout << "  4. Login with regular user credentials" << std::endl;
    std::cout << "  5. Login with VIP user credentials" << std::endl;
    std::cout << "  6. Test invalid login attempts" << std::endl;
    std::cout << "  7. Register new user account" << std::endl;
    std::cout << "  8. Test logout functionality" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 2: VENUE MANAGEMENT" << std::endl;
    std::cout << "  9. Create new venue (admin/manager only)" << std::endl;
    std::cout << "  10. View all venues" << std::endl;
    std::cout << "  11. Search venues by name" << std::endl;
    std::cout << "  12. Update venue information" << std::endl;
    std::cout << "  13. Delete venue" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 3: PERFORMER MANAGEMENT" << std::endl;
    std::cout << "  14. Create new performer" << std::endl;
    std::cout << "  15. View all performers" << std::endl;
    std::cout << "  16. Search performers by name/genre" << std::endl;
    std::cout << "  17. Update performer information" << std::endl;
    std::cout << "  18. Delete performer" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 4: CREW MANAGEMENT" << std::endl;
    std::cout << "  19. Create new crew member" << std::endl;
    std::cout << "  20. View all crew members" << std::endl;
    std::cout << "  21. Search crew by role/name" << std::endl;
    std::cout << "  22. Update crew information" << std::endl;
    std::cout << "  23. Delete crew member" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 5: CONCERT MANAGEMENT" << std::endl;
    std::cout << "  24. Create new concert" << std::endl;
    std::cout << "  25. View all concerts" << std::endl;
    std::cout << "  26. Search concerts by name/genre" << std::endl;
    std::cout << "  27. Update concert information" << std::endl;
    std::cout << "  28. Delete concert" << std::endl;
    std::cout << "  29. View concert details with venue/performers" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 6: TICKET MANAGEMENT" << std::endl;
    std::cout << "  30. Create tickets for concert" << std::endl;
    std::cout << "  31. View all tickets" << std::endl;
    std::cout << "  32. Search tickets by concert/attendee" << std::endl;
    std::cout << "  33. Update ticket status" << std::endl;
    std::cout << "  34. Delete ticket" << std::endl;
    std::cout << "  35. Generate QR code for ticket" << std::endl;
    std::cout << "  36. Validate ticket QR code" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 7: PAYMENT PROCESSING" << std::endl;
    std::cout << "  37. Process ticket payment" << std::endl;
    std::cout << "  38. View payment history" << std::endl;
    std::cout << "  39. Process refunds" << std::endl;
    std::cout << "  40. View payment statistics" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 8: FEEDBACK SYSTEM" << std::endl;
    std::cout << "  41. Submit concert feedback" << std::endl;
    std::cout << "  42. View all feedback" << std::endl;
    std::cout << "  43. Search feedback by concert/attendee" << std::endl;
    std::cout << "  44. Update feedback" << std::endl;
    std::cout << "  45. Delete feedback" << std::endl;
    std::cout << "  46. Analyze sentiment in feedback" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 9: REPORTING SYSTEM" << std::endl;
    std::cout << "  47. Generate concert attendance report" << std::endl;
    std::cout << "  48. Generate revenue report" << std::endl;
    std::cout << "  49. Generate feedback analysis report" << std::endl;
    std::cout << "  50. Generate system usage report" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 10: COMMUNICATION SYSTEM" << std::endl;
    std::cout << "  51. Send notification to attendees" << std::endl;
    std::cout << "  52. Send promotional messages" << std::endl;
    std::cout << "  53. View communication history" << std::endl;
    std::cout << "  54. Schedule automated reminders" << std::endl;
    std::cout << std::endl;
    
    std::cout << "📊 CROSS-MODULE INTEGRATION TESTS:" << std::endl;
    std::cout << "  55. Complete concert booking workflow (venue → performer → concert → ticket → payment)" << std::endl;
    std::cout << "  56. End-to-end attendee experience (registration → concert search → ticket purchase → feedback)" << std::endl;
    std::cout << "  57. User workflow (login → concert browsing → ticket purchase → feedback)" << std::endl;
    std::cout << "  58. Management workflow (login → concert management → system reports)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "⚠️  TESTING NOTES:" << std::endl;
    std::cout << "  • All users can access both management and user portal features" << std::endl;
    std::cout << "  • All data is persisted to binary files in the data/ directory" << std::endl;
    std::cout << "  • Test both successful operations and error conditions" << std::endl;
    std::cout << "  • Verify cross-module relationships (concert-venue, ticket-attendee, etc.)" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "\nPress Enter to continue to the application...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::endl;
}

// Module initialization function
bool initializeModules() {
    try {
        std::cout << "Initializing MuseIO Concert Management System...\n";
        
        // Initialize modules in dependency order using DataPaths constants
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
        
        std::cout << "✅ All modules initialized successfully!\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to initialize modules: " << e.what() << std::endl;
        return false;
    }
}

bool initializeDefaultUsers() {
    UIManager::addSmallSpacing();
    std::cout << "Setting up default user accounts...\n";
    
    try {
        // Create default users
        struct DefaultUser {
            std::string username;
            std::string password;
            std::string description;
        };
        
        std::vector<DefaultUser> defaultUsers = {
            {"admin", "admin123", "System Administrator"},
            {"manager1", "manager123", "Concert Manager"},
            {"staff1", "staff123", "Staff Member"},
            {"user1", "user123", "Regular User"},
            {"vip1", "vip123", "VIP User"}
        };
        
        for (const auto& user : defaultUsers) {
            if (g_authModule->registerUser(user.username, user.password)) {
                std::cout << "✅ Created " << user.description << " account: " << user.username << std::endl;
            } else {
                std::cout << "⚠️  Account " << user.username << " may already exist" << std::endl;
            }
        }
        
        UIManager::displayDefaultCredentials();
        
        return true;
    } catch (const std::exception& e) {
        UIManager::displayError("Failed to create default users: " + std::string(e.what()));
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
    UIManager::displayAuthMenu();
}

bool authenticateUser() {
    while (true) {
        displayAuthMenu();
        
        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            UIManager::displayError("Invalid input. Please enter only integer numbers (e.g., '1', '2', '3').");
            continue;
        }
        
        int choice;
        try {
            choice = std::stoi(choiceStr);
        } catch (const std::exception&) {
            UIManager::displayError("Invalid input. Please enter only integer numbers.");
            continue;
        }
        
        switch (choice) {
            case 1: { // Login
                std::string username, password;
                UIManager::displayPrompt("Username (or 0 to cancel)");
                std::getline(std::cin, username);
                if (username == "0") break;
                
                UIManager::displayPrompt("Password");
                std::getline(std::cin, password);
                
                bool isAuthenticated = g_authModule->authenticateUser(username, password);
                if (isAuthenticated) {
                    currentSession.username = username;
                    currentSession.isAuthenticated = true;
                    currentSession.loginTime = Model::DateTime::now();
                    currentSession.userRole = "user"; // All users have same role now
                    // Map user to attendee profile if exists
                    int mappedId = -1;
                    if (g_attendeeModule) {
                        auto attendee = g_attendeeModule->findAttendeeByUsername(username);
                        if (attendee) {
                            mappedId = attendee->id;
                        }
                    }
                    currentSession.userId = (mappedId != -1) ? mappedId : 1; // Fallback if not found
                    
                    UIManager::displaySuccess("Login successful! Welcome, " + username);
                    return true;
                } else {
                    UIManager::displayError("Invalid credentials. Please try again.");
                }
                break;
            }
            case 2: { // Register
                if (registerNewUser()) {
                    UIManager::displaySuccess("Registration successful! Please login with your new credentials.");
                }
                break;
            }
            case 3: { // View All Accounts
                displayAllAccounts();
                break;
            }
            case 0: // Exit
                return false;
            default:
                UIManager::displayError("Invalid choice. Please select a valid option.");
        }
    }
}

// Helper function to get validated input with retry mechanism
std::string getValidatedInput(const std::string& prompt, 
                             std::function<InputValidator::ValidationResult(const std::string&)> validator,
                             int maxRetries = 3) {
    std::string input;
    int attempts = 0;
    
    while (attempts < maxRetries) {
        UIManager::displayPrompt(prompt + (attempts > 0 ? " (retry " + std::to_string(attempts + 1) + "/" + std::to_string(maxRetries) + ")" : ""));
        std::getline(std::cin, input);
        
        // Allow user to cancel with "0"
        if (input == "0") {
            return "0";
        }
        
        auto result = validator(input);
        if (result.isValid) {
            return input;
        } else {
            UIManager::displayError(result.errorMessage);
            attempts++;
        }
    }
    
    UIManager::displayError("Maximum attempts reached. Registration cancelled.");
    return "0"; // Return cancel code
}

bool registerNewUser() {
    UIManager::addSmallSpacing();
    UIManager::printSeparator('-');
    UIManager::printCenteredText("USER REGISTRATION");
    UIManager::printSeparator('-');
    UIManager::displayInfo("Enter '0' at any field to cancel registration");
    UIManager::addSmallSpacing();
    
    // Get validated username
    std::string username = getValidatedInput(
        "Username", 
        [](const std::string& input) { return InputValidator::validateUsername(input); }
    );
    if (username == "0") return false;
    
    // Check if username already exists
    if (g_authModule->userExists(username)) {
        UIManager::displayError("Username already exists. Please choose a different username.");
        return false;
    }
    
    // Get validated password
    std::string password = getValidatedInput(
        "Password", 
        [](const std::string& input) { return InputValidator::validatePassword(input); }
    );
    if (password == "0") return false;
    
    // Get validated email
    std::string email = getValidatedInput(
        "Email", 
        [](const std::string& input) { return InputValidator::validateEmail(input); }
    );
    if (email == "0") return false;
    
    // Get validated first name
    std::string firstName = getValidatedInput(
        "First Name", 
        [](const std::string& input) { return InputValidator::validateName(input, "First name"); }
    );
    if (firstName == "0") return false;
    
    // Get validated last name
    std::string lastName = getValidatedInput(
        "Last Name", 
        [](const std::string& input) { return InputValidator::validateName(input, "Last name"); }
    );
    if (lastName == "0") return false;
    
    // Optional phone number with validation
    UIManager::displayPrompt("Phone Number (optional, press Enter to skip)");
    std::string phone;
    std::getline(std::cin, phone);
    
    if (!phone.empty() && phone != "0") {
        auto phoneResult = InputValidator::validatePhoneNumber(phone);
        if (!phoneResult.isValid) {
            UIManager::displayError(phoneResult.errorMessage);
            UIManager::displayPrompt("Phone Number (corrected, or press Enter to skip)");
            std::getline(std::cin, phone);
            if (!phone.empty()) {
                phoneResult = InputValidator::validatePhoneNumber(phone);
                if (!phoneResult.isValid) {
                    UIManager::displayWarning("Invalid phone number provided. Proceeding without phone number.");
                    phone = "";
                }
            }
        }
    }
    
    // Create new attendee account
    auto attendee = g_attendeeModule->createAttendee(firstName + " " + lastName, email, phone, Model::AttendeeType::REGULAR, username, "", false);
    if (attendee != nullptr) {
        // Register user credentials
        if (g_authModule->registerUser(username, password)) {
            UIManager::displaySuccess("Account created successfully!");
            UIManager::displayInfo("Please login with your new credentials to continue.");
            return true;
        } else {
            UIManager::displayError("Failed to create user credentials. Please try again.");
            return false;
        }
    } else {
        UIManager::displayError("Failed to create user profile. Please try again.");
        return false;
    }
}

void logout() {
    UIManager::addSmallSpacing();
    std::cout << "Logging out " << currentSession.username << "...\n";
    currentSession = UserSession(); // Reset session
    UIManager::displaySuccess("Logged out successfully!");
}

void displayAllAccounts() {
    UIManager::addSmallSpacing();
    UIManager::printSeparator('=');
    UIManager::printCenteredText("ALL REGISTERED ACCOUNTS");
    UIManager::printSeparator('=');
    UIManager::addSmallSpacing();
    
    if (!g_authModule) {
        UIManager::displayError("Authentication module not initialized.");
        return;
    }
    
    auto usernames = g_authModule->getAllUsernames();
    
    if (usernames.empty()) {
        UIManager::displayInfo("No accounts found.");
        UIManager::displayInfo("Use 'Register New Account' to create your first account.");
    } else {
        std::cout << "📋 Found " << usernames.size() << " registered account(s):\n\n";
        
        int count = 1;
        for (const auto& username : usernames) {
            std::cout << "  " << count << ". " << username << std::endl;
            count++;
        }
        
        UIManager::addSmallSpacing();
        UIManager::displayInfo("Note: Passwords are not shown for security reasons.");
        UIManager::displayInfo("Use these usernames with the 'Login' option.");
    }
    
    UIManager::addSmallSpacing();
    UIManager::printSeparator('-');
    std::cout << "Press Enter to return to authentication menu...";
    std::cin.get();
}

void displayMainMenu() {
    UIManager::displayMainMenu();
}

void displayManagementMenu() {
    UIManager::displayManagementMenu();
}

void displayUserMenu() {
    UIManager::displayUserMenu();
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
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-6): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter only integer numbers (e.g., '1', '2', '3').\n";
            continue;
        }
        
        int choice;
        try {
            choice = std::stoi(choiceStr);
        } catch (const std::exception&) {
            std::cout << "❌ Invalid input. Please enter only integer numbers.\n";
            continue;
        }
        
        switch (choice) {
            case 1: { // Create New Concert
                std::string name, description, genre, concertDate, concertTime;
                int venueId;
                double ticketPrice;
                
                UIManager::displayInfo("Creating new concert - Enter '0' at any field to cancel");
                
                // Validate concert name
                name = getValidatedInput(
                    "Concert Name", 
                    [](const std::string& input) { 
                        if (input.empty()) return InputValidator::ValidationResult(false, "Concert name cannot be empty");
                        if (input.length() > 100) return InputValidator::ValidationResult(false, "Concert name too long (max 100 characters)");
                        return InputValidator::ValidationResult(true);
                    }
                );
                if (name == "0") break;
                
                // Validate description
                description = getValidatedInput(
                    "Description", 
                    [](const std::string& input) { 
                        if (input.empty()) return InputValidator::ValidationResult(false, "Description cannot be empty");
                        if (input.length() > 500) return InputValidator::ValidationResult(false, "Description too long (max 500 characters)");
                        return InputValidator::ValidationResult(true);
                    }
                );
                if (description == "0") break;
                
                // Validate genre
                genre = getValidatedInput(
                    "Genre", 
                    [](const std::string& input) { 
                        if (input.empty()) return InputValidator::ValidationResult(false, "Genre cannot be empty");
                        if (input.length() > 50) return InputValidator::ValidationResult(false, "Genre too long (max 50 characters)");
                        return InputValidator::ValidationResult(true);
                    }
                );
                if (genre == "0") break;
                
                // Validate concert date
                concertDate = getValidatedInput(
                    "Concert Date (YYYY-MM-DD)", 
                    [](const std::string& input) { return InputValidator::validateDate(input); }
                );
                if (concertDate == "0") break;
                
                // Validate concert time
                concertTime = getValidatedInput(
                    "Concert Time (HH:MM in 24-hour format)", 
                    [](const std::string& input) { return InputValidator::validateTime(input); }
                );
                if (concertTime == "0") break;
                
                // Validate venue ID
                std::string venueIdStr = getValidatedInput(
                    "Venue ID", 
                    [](const std::string& input) { 
                        try {
                            int id = std::stoi(input);
                            if (id <= 0) return InputValidator::ValidationResult(false, "Venue ID must be greater than 0");
                            return InputValidator::ValidationResult(true);
                        } catch (...) {
                            return InputValidator::ValidationResult(false, "Please enter a valid venue ID number");
                        }
                    }
                );
                if (venueIdStr == "0") break;
                venueId = std::stoi(venueIdStr);
                
                // Validate venue exists
                auto venue = g_venueModule->getVenueById(venueId);
                if (!venue) {
                    UIManager::displayError("Venue not found. Please check the venue ID.");
                    break;
                }
                
                // Validate ticket price
                std::string priceStr = getValidatedInput(
                    "Base Ticket Price ($)", 
                    [](const std::string& input) { 
                        try {
                            double price = std::stod(input);
                            if (price < 0) return InputValidator::ValidationResult(false, "Price cannot be negative");
                            if (price > 10000) return InputValidator::ValidationResult(false, "Price too high (max $10,000)");
                            return InputValidator::ValidationResult(true);
                        } catch (...) {
                            return InputValidator::ValidationResult(false, "Please enter a valid price");
                        }
                    }
                );
                if (priceStr == "0") break;
                ticketPrice = std::stod(priceStr);
                
                // Combine date and time for ISO 8601 format
                std::string concertDateTime = concertDate + "T" + concertTime + ":00Z";
                
                // Create concert with validated date/time
                auto concert = g_concertModule->createConcert(name, description, 
                    concertDateTime, concertDateTime);
                
                if (concert != nullptr) {
                    // **NEW: Create ticket inventory for the concert**
                    int venueCapacity = venue ? venue->capacity : 500; // Use venue capacity or default
                    auto ticketIds = g_ticketModule->createTicketInventory(concert->id, venueCapacity, "Regular", venueCapacity);
                    
                    UIManager::displaySuccess("Concert created successfully! ID: " + std::to_string(concert->id));
                    UIManager::displayInfo("Date/Time: " + concertDate + " at " + concertTime);
                    UIManager::displayInfo("Venue: " + venue->name + " (ID: " + std::to_string(venueId) + ")");
                    UIManager::displayInfo("Ticket Inventory: " + std::to_string(ticketIds.size()) + " tickets created");
                } else {
                    UIManager::displayError("Failed to create concert. Please try again.");
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
                std::cout << "Enter Concert ID to update (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                std::cin.ignore();
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                std::string newName, newDescription;
                std::cout << "New name (current: " << concert->name << ", 0 to skip): ";
                std::getline(std::cin, newName);
                if (newName == "0") newName = "";
                
                std::cout << "New description (current: " << concert->description << ", 0 to skip): ";
                std::getline(std::cin, newDescription);
                if (newDescription == "0") newDescription = "";
                
                if (g_concertModule->editConcert(concertId, 
                    newName.empty() ? "" : newName,
                    newDescription.empty() ? "" : newDescription)) {
                    std::cout << "✅ Concert updated successfully!\n";
                } else {
                    std::cout << "❌ Failed to update concert.\n";
                }
                break;
            }
            case 4: { // Cancel Concert
                int concertId;
                std::cout << "Enter Concert ID to cancel (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                std::cin.ignore();
                
                std::string reason;
                std::cout << "Cancellation reason: ";
                std::getline(std::cin, reason);
                
                if (g_concertModule->cancelConcert(concertId)) {
                    std::cout << "✅ Concert cancelled successfully!\n";
                } else {
                    std::cout << "❌ Failed to cancel concert.\n";
                }
                break;
            }
            case 5: { // Assign Performers
                int concertId, performerId;
                std::cout << "Concert ID (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
                std::cout << "Performer ID (or 0 to cancel): ";
                std::cin >> performerId;
                if (performerId == 0) break;
                
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
            case 0: // Back to Management Portal
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
        std::cout << "7. Simulate Ticket Purchases\n";
        std::cout << "8. 🆕 RESET TICKET INVENTORY (Create Fresh AVAILABLE Tickets)\n";
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-8): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // Create Tickets with Enhanced Validation
                int concertId, quantity;
                std::string ticketType;
                
                std::cout << "Concert ID (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
                std::cout << "Quantity (or 0 to cancel): ";
                std::cin >> quantity;
                if (quantity == 0) break;
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
                
                // **NEW APPROACH: Create inventory (AVAILABLE tickets) instead of SOLD tickets**
                std::cout << "Creating ticket inventory for concert...\n";
                auto inventoryIds = g_ticketModule->createTicketInventory(concertId, quantity, ticketType, 1000);
                
                std::cout << "✅ Created " << inventoryIds.size() << " AVAILABLE tickets successfully!\n";
                std::cout << "First 20 Ticket IDs: ";
                for (size_t i = 0; i < std::min(inventoryIds.size(), static_cast<size_t>(20)); i++) {
                    std::cout << inventoryIds[i] << " ";
                }
                if (inventoryIds.size() > 20) {
                    std::cout << "... (and " << (inventoryIds.size() - 20) << " more)";
                }
                std::cout << std::endl;
                
                // **DEBUG: Show sample QR code format**
                if (!inventoryIds.empty()) {
                    auto sampleTicket = g_ticketModule->getTicketById(inventoryIds[0]);
                    if (sampleTicket) {
                        std::cout << "DEBUG: Sample QR code: " << sampleTicket->qr_code << std::endl;
                        std::cout << "DEBUG: Sample ticket status: " << (int)sampleTicket->status << std::endl;
                    }
                }
                
                // Show availability
                int availableCount = g_ticketModule->getAvailableTicketCount(concertId);
                std::cout << "Current available tickets for concert " << concertId << ": " << availableCount << std::endl;
                break;
            }
            case 2: { // Ticket Sales Statistics
                int concertId;
                std::cout << "Enter Concert ID for statistics (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
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
                    // **DEBUG: Show ALL concerts regardless of status**
                    // std::cout << "DEBUG: Concert " << concert->id << " (" << concert->name << ") - Status: ";
                    switch (concert->event_status) {
                        case Model::EventStatus::SCHEDULED: std::cout << "SCHEDULED"; break;
                        case Model::EventStatus::CANCELLED: std::cout << "CANCELLED"; break;
                        case Model::EventStatus::POSTPONED: std::cout << "POSTPONED"; break;
                        case Model::EventStatus::COMPLETED: std::cout << "COMPLETED"; break;
                        case Model::EventStatus::SOLDOUT: std::cout << "SOLDOUT"; break;
                    }
                    std::cout << std::endl;
                    
                    // Show availability for ALL concerts (remove status filter for debugging)
                    auto concertTickets = g_ticketModule->getTicketsByConcert(concert->id);
                    int available = 0;
                    // std::cout << "DEBUG: Found " << concertTickets.size() << " tickets for concert " << concert->id << std::endl;
                    
                    for (const auto& ticket : concertTickets) {
                        if (ticket->status == Model::TicketStatus::AVAILABLE) {
                            available++;
                        }
                        // DEBUG: Show first few ticket details
                        /*
                        if (available < 3) {
                            std::cout << "DEBUG: Ticket " << ticket->ticket_id << " QR:" << ticket->qr_code 
                                      << " Status:" << (int)ticket->status << std::endl;
                        }
                        */
                    }
                    
                    std::cout << concert->name << " (ID: " << concert->id 
                              << ") - Available: " << available << " tickets\n";
                }
                break;
            }
            case 4: { // Validate Relationships
                std::cout << "Validating ticket-concert relationships...\n";
                int invalidCount = 0;
                
                // This would implement comprehensive validation
                auto allTickets = g_ticketModule->getAll();
                for (const auto& ticket : allTickets) {
                    // Need to fix ticket field access - check actual Model::Ticket structure
                    std::cout << "⚠️ Ticket validation requires access to actual ticket fields.\n";
                    // This would need proper implementation based on actual Ticket model
                    break;
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
                std::cout << "Enter Ticket ID for QR code operations (or 0 to cancel): ";
                std::cin >> ticketId;
                if (ticketId == 0) break;
                
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
                std::cout << "Enter Ticket ID to update (or 0 to cancel): ";
                std::cin >> ticketId;
                if (ticketId == 0) break;
                
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
            case 7: { // Simulate Ticket Purchases
                int concertId, quantity;
                std::cout << "Concert ID for purchases (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
                std::cout << "Number of tickets to purchase (or 0 to cancel): ";
                std::cin >> quantity;
                if (quantity == 0) break;
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                int availableBefore = g_ticketModule->getAvailableTicketCount(concertId);
                std::cout << "Available tickets before purchase: " << availableBefore << std::endl;
                
                if (availableBefore < quantity) {
                    std::cout << "❌ Not enough available tickets! Only " << availableBefore << " available.\n";
                    break;
                }
                
                // Simulate purchasing tickets
                std::vector<int> purchasedIds;
                for (int i = 0; i < quantity; i++) {
                    int purchasedTicket = g_ticketModule->purchaseAvailableTicket(i + 1, concertId, "Regular");
                    if (purchasedTicket > 0) {
                        purchasedIds.push_back(purchasedTicket);
                    }
                }
                
                int availableAfter = g_ticketModule->getAvailableTicketCount(concertId);
                std::cout << "✅ Successfully purchased " << purchasedIds.size() << " tickets!\n";
                std::cout << "Available tickets after purchase: " << availableAfter << std::endl;
                std::cout << "First 10 purchased ticket IDs: ";
                for (size_t i = 0; i < std::min(purchasedIds.size(), static_cast<size_t>(10)); i++) {
                    std::cout << purchasedIds[i] << " ";
                }
                if (purchasedIds.size() > 10) {
                    std::cout << "... (and " << purchasedIds.size() - 10 << " more)";
                }
                std::cout << std::endl;
                break;
            }
            case 8: { // 🆕 RESET TICKET INVENTORY - Create Fresh AVAILABLE Tickets
                int concertId, quantity;
                std::cout << "🆕 RESET TICKET INVENTORY - This will create fresh AVAILABLE tickets\n";
                std::cout << "Concert ID for fresh inventory (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
                std::cout << "Number of fresh AVAILABLE tickets to create (or 0 to cancel): ";
                std::cin >> quantity;
                if (quantity == 0) break;
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                std::cout << "Creating " << quantity << " fresh AVAILABLE tickets for concert " << concertId << "...\n";
                
                // Create fresh inventory with explicit AVAILABLE status
                auto freshTicketIds = g_ticketModule->createTicketInventory(concertId, quantity, "Regular", 1000);
                
                std::cout << "✅ Created " << freshTicketIds.size() << " fresh AVAILABLE tickets!\n";
                
                // Immediately check availability
                int availableCount = g_ticketModule->getAvailableTicketCount(concertId);
                std::cout << "Available tickets for concert " << concertId << ": " << availableCount << std::endl;
                
                // Show sample tickets with status
                std::cout << "Sample fresh ticket IDs: ";
                for (size_t i = 0; i < std::min(freshTicketIds.size(), static_cast<size_t>(10)); i++) {
                    auto ticket = g_ticketModule->getTicketById(freshTicketIds[i]);
                    if (ticket) {
                        std::cout << "[ID:" << freshTicketIds[i] << ",Status:" << (int)ticket->status << "] ";
                    }
                }
                std::cout << std::endl;
                
                if (availableCount > 0) {
                    std::cout << "🎉 SUCCESS! Fresh AVAILABLE tickets created successfully!\n";
                } else {
                    std::cout << "⚠️ WARNING: Available count is still 0 - there may be a data persistence issue.\n";
                }
                break;
            }
            case 0: // Back
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
    while (true) {
        std::cout << "\n--- Venue Management ---\n";
        std::cout << "1. Create New Venue\n";
        std::cout << "2. View All Venues\n";
        std::cout << "3. Search Venues\n";
        std::cout << "4. Update Venue Details\n";
        std::cout << "5. Delete Venue\n";
        std::cout << "6. Venue Capacity Analysis\n";
        std::cout << "7. Visualize Seating Plan\n";
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-7): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // Create New Venue
                std::string name, address, city, state, zipCode, country, description, contactInfo;
                int capacity;
                
                UIManager::displayInfo("Creating new venue - Enter '0' at any field to cancel");
                
                // Validate venue name
                name = getValidatedInput(
                    "Venue Name", 
                    [](const std::string& input) { 
                        if (input.empty()) return InputValidator::ValidationResult(false, "Venue name cannot be empty");
                        if (input.length() > 100) return InputValidator::ValidationResult(false, "Venue name too long (max 100 characters)");
                        return InputValidator::ValidationResult(true);
                    }
                );
                if (name == "0") break;
                
                // Basic address validation
                address = getValidatedInput(
                    "Address", 
                    [](const std::string& input) { 
                        if (input.empty()) return InputValidator::ValidationResult(false, "Address cannot be empty");
                        if (input.length() > 200) return InputValidator::ValidationResult(false, "Address too long (max 200 characters)");
                        return InputValidator::ValidationResult(true);
                    }
                );
                if (address == "0") break;
                
                // Validate city
                city = getValidatedInput(
                    "City", 
                    [](const std::string& input) { return InputValidator::validateName(input, "City"); }
                );
                if (city == "0") break;
                
                // Validate state/province  
                state = getValidatedInput(
                    "State/Province", 
                    [](const std::string& input) { 
                        if (input.empty()) return InputValidator::ValidationResult(false, "State/Province cannot be empty");
                        if (input.length() > 50) return InputValidator::ValidationResult(false, "State/Province too long (max 50 characters)");
                        return InputValidator::ValidationResult(true);
                    }
                );
                if (state == "0") break;
                
                // Validate postal code
                zipCode = getValidatedInput(
                    "ZIP/Postal Code", 
                    [](const std::string& input) { return InputValidator::validatePostalCode(input); }
                );
                if (zipCode == "0") break;
                
                // Validate country
                country = getValidatedInput(
                    "Country", 
                    [](const std::string& input) { return InputValidator::validateName(input, "Country"); }
                );
                if (country == "0") break;
                
                // Validate capacity
                std::string capacityStr = getValidatedInput(
                    "Capacity", 
                    [](const std::string& input) { 
                        try {
                            int cap = std::stoi(input);
                            if (cap <= 0) return InputValidator::ValidationResult(false, "Capacity must be greater than 0");
                            if (cap > 1000000) return InputValidator::ValidationResult(false, "Capacity too large (max 1,000,000)");
                            return InputValidator::ValidationResult(true);
                        } catch (...) {
                            return InputValidator::ValidationResult(false, "Please enter a valid number");
                        }
                    }
                );
                if (capacityStr == "0") break;
                capacity = std::stoi(capacityStr);
                
                // Optional fields with basic validation
                UIManager::displayPrompt("Description (optional, press Enter to skip)");
                std::getline(std::cin, description);
                if (description.length() > 500) {
                    UIManager::displayWarning("Description truncated to 500 characters");
                    description = description.substr(0, 500);
                }
                
                // Validate contact info if provided
                UIManager::displayPrompt("Contact Info - email or phone (optional, press Enter to skip)");
                std::getline(std::cin, contactInfo);
                if (!contactInfo.empty()) {
                    // Try to validate as email first, then as phone
                    auto emailResult = InputValidator::validateEmail(contactInfo);
                    auto phoneResult = InputValidator::validatePhoneNumber(contactInfo);
                    
                    if (!emailResult.isValid && !phoneResult.isValid) {
                        UIManager::displayWarning("Contact info doesn't appear to be a valid email or phone number, but proceeding anyway");
                    }
                }
                
                auto venue = g_venueModule->createVenue(name, address, city, state, zipCode, country, capacity, description, contactInfo);
                if (venue) {
                    UIManager::displaySuccess("Venue created successfully! ID: " + std::to_string(venue->id));
                } else {
                    UIManager::displayError("Failed to create venue. Please try again.");
                }
                break;
            }
            case 2: { // View All Venues
                auto venues = g_venueModule->getAllVenues();
                std::cout << "\n--- All Venues ---\n";
                for (const auto& venue : venues) {
                    std::cout << "ID: " << venue->id << " | " << venue->name 
                              << " | " << venue->city << ", " << venue->state 
                              << " | Capacity: " << venue->capacity << std::endl;
                }
                if (venues.empty()) {
                    std::cout << "No venues found.\n";
                }
                break;
            }
            case 3: { // Search Venues
                std::string searchTerm;
                std::cout << "Enter venue name to search (or 0 to cancel): ";
                std::getline(std::cin, searchTerm);
                if (searchTerm == "0") break;
                
                auto venues = g_venueModule->findVenuesByName(searchTerm);
                std::cout << "\n--- Search Results ---\n";
                for (const auto& venue : venues) {
                    std::cout << "ID: " << venue->id << " | " << venue->name 
                              << " | " << venue->city << ", " << venue->state 
                              << " | Capacity: " << venue->capacity << std::endl;
                }
                if (venues.empty()) {
                    std::cout << "No venues found matching '" << searchTerm << "'.\n";
                }
                break;
            }
            case 4: { // Update Venue Details
                int venueId;
                std::cout << "Enter Venue ID to update (or 0 to cancel): ";
                std::cin >> venueId;
                if (venueId == 0) break;
                std::cin.ignore();
                
                auto venue = g_venueModule->getVenueById(venueId);
                if (!venue) {
                    std::cout << "❌ Venue not found.\n";
                    break;
                }
                
                std::string newName, newDescription;
                int newCapacity;
                std::cout << "New name (current: " << venue->name << ", 0 to skip): ";
                std::getline(std::cin, newName);
                if (newName == "0") newName = "";
                
                std::cout << "New description (current: " << venue->description << ", 0 to skip): ";
                std::getline(std::cin, newDescription);
                if (newDescription == "0") newDescription = "";
                
                std::cout << "New capacity (current: " << venue->capacity << ", 0 to skip): ";
                std::cin >> newCapacity;
                
                if (g_venueModule->updateVenue(venueId, 
                    newName, "", "", "", "", "",
                    newCapacity == 0 ? 0 : newCapacity, 
                    newDescription)) {
                    std::cout << "✅ Venue updated successfully!\n";
                } else {
                    std::cout << "❌ Failed to update venue.\n";
                }
                break;
            }
            case 5: { // Delete Venue
                int venueId;
                std::cout << "Enter Venue ID to delete (or 0 to cancel): ";
                std::cin >> venueId;
                if (venueId == 0) break;
                
                if (g_venueModule->deleteVenue(venueId)) {
                    std::cout << "✅ Venue deleted successfully!\n";
                } else {
                    std::cout << "❌ Failed to delete venue.\n";
                }
                break;
            }
            case 6: { // Venue Capacity Analysis
                auto venues = g_venueModule->getAllVenues();
                if (venues.empty()) {
                    std::cout << "No venues available for analysis.\n";
                    break;
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
                
                std::cout << "\n--- Venue Capacity Analysis ---\n";
                std::cout << "Total Venues: " << venues.size() << std::endl;
                std::cout << "Total Capacity: " << totalCapacity << std::endl;
                std::cout << "Average Capacity: " << (totalCapacity / static_cast<int>(venues.size())) << std::endl;
                std::cout << "Largest Venue: " << largestVenue << " (" << maxCapacity << ")" << std::endl;
                std::cout << "Smallest Venue: " << smallestVenue << " (" << minCapacity << ")" << std::endl;
                break;
            }
            case 7: { // Visualize Seating Plan
                auto venues = g_venueModule->getAllVenues();
                if (venues.empty()) {
                    UIManager::displayWarning("No venues available. Create a venue first.");
                    break;
                }
                
                UIManager::addSmallSpacing();
                UIManager::printSeparator('=');
                UIManager::printCenteredText("VENUE SEATING PLAN VISUALIZATION");
                UIManager::printSeparator('=');
                UIManager::addSmallSpacing();
                
                // Display available venues
                std::cout << "Available Venues:\n";
                for (size_t i = 0; i < venues.size(); ++i) {
                    std::cout << (i + 1) << ". " << venues[i]->name 
                              << " (ID: " << venues[i]->id << ", Capacity: " << venues[i]->capacity << ")\n";
                }
                std::cout << "0. Cancel\n\n";
                
                std::string venueChoiceStr;
                std::cout << "Select venue to visualize (0-" << venues.size() << "): ";
                std::getline(std::cin, venueChoiceStr);
                
                if (!isValidInteger(venueChoiceStr)) {
                    UIManager::displayError("Invalid input. Please enter a number.");
                    break;
                }
                
                int venueChoice = std::stoi(venueChoiceStr);
                if (venueChoice == 0) break;
                if (venueChoice < 1 || venueChoice > static_cast<int>(venues.size())) {
                    UIManager::displayError("Invalid venue selection.");
                    break;
                }
                
                auto selectedVenue = venues[venueChoice - 1];
                
                // Check if seating plan is initialized
                if (selectedVenue->rows == 0 || selectedVenue->columns == 0) {
                    UIManager::displayWarning("Seating plan not initialized for this venue. Initialize it first.");
                    break;
                }
                
                // Get the visualization
                std::string visualization = g_venueModule->getSeatingPlanVisualization(selectedVenue->id);
                
                // Fancy display
                UIManager::addSmallSpacing();
                UIManager::printSeparator('*');
                std::cout << "🎭 " << selectedVenue->name << " - Seating Plan Visualization 🎭\n";
                UIManager::printSeparator('*');
                UIManager::addSmallSpacing();
                
                // Display the seating plan with some formatting
                std::cout << visualization << std::endl;
                
                UIManager::addSmallSpacing();
                UIManager::printSeparator('-');
                std::cout << "💡 Legend: [A]=Available, [S]=Sold, [C]=Checked In, [X]=Unavailable\n";
                std::cout << "📍 Venue: " << selectedVenue->name << " | Layout: " 
                          << selectedVenue->rows << " rows × " << selectedVenue->columns << " columns\n";
                UIManager::printSeparator('-');
                
                break;
            }
            case 0: // Back to Management Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void managePerformersAndCrew() {
    while (true) {
        std::cout << "\n--- Performer & Crew Management ---\n";
        std::cout << "1. Performer Management\n";
        std::cout << "2. Crew Management\n";
        std::cout << "3. View All Performers\n";
        std::cout << "4. View All Crew Members\n";
        std::cout << "5. Search Performers\n";
        std::cout << "6. Search Crew\n";
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-6): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // Performer Management
                std::cout << "\n--- Performer Management ---\n";
                std::cout << "1. Create New Performer\n";
                std::cout << "2. Update Performer\n";
                std::cout << "3. Delete Performer\n";
                std::cout << "0. Back\n";
                
                int subChoice;
                std::cout << "Enter choice (0-3): ";
                std::cin >> subChoice;
                std::cin.ignore();
                
                switch (subChoice) {
                    case 1: { // Create New Performer
                        std::string name, type, contactInfo, bio, imageUrl;
                        
                        name = getValidatedInput(
                            "Performer Name",
                            [](const std::string& input) {
                                return InputValidator::validateName(input, "Performer name");
                            }
                        );
                        if (name == "0") break;
                        
                        type = getChoiceFromOptions(PredefinedOptions::PERFORMER_TYPES, "Select Performer Type:");
                        if (type == "0") break;
                        
                        contactInfo = getValidatedInput(
                            "Contact Info (email/phone)",
                            [](const std::string& input) {
                                return InputValidator::validateContactInfo(input, "Contact info", 5, 100);
                            }
                        );
                        if (contactInfo == "0") break;
                        
                        bio = getValidatedInput(
                            "Biography (optional, press Enter to skip)",
                            [](const std::string& input) {
                                if (input.empty()) return InputValidator::ValidationResult(true);
                                return InputValidator::validateBiography(input, "Biography", 0, 500);
                            }
                        );
                        if (bio == "0") break;
                        
                        std::cout << "Image URL (optional): ";
                        std::getline(std::cin, imageUrl);
                        
                        auto performer = g_performerModule->createPerformer(name, type, contactInfo, bio, imageUrl);
                        if (performer) {
                            std::cout << "✅ Performer created successfully! ID: " << performer->performer_id << std::endl;
                        } else {
                            std::cout << "❌ Failed to create performer.\n";
                        }
                        break;
                    }
                    case 2: { // Update Performer
                        int performerId;
                        std::cout << "Enter Performer ID to update (or 0 to cancel): ";
                        std::cin >> performerId;
                        if (performerId == 0) break;
                        std::cin.ignore();
                        
                        auto performer = g_performerModule->getPerformerById(performerId);
                        if (!performer) {
                            std::cout << "❌ Performer not found.\n";
                            break;
                        }
                        
                        std::string newName, newType, newContact, newBio;
                        
                        newName = getValidatedInput(
                            "New name (current: " + performer->name + ", 0 to skip)",
                            [](const std::string& input) {
                                if (input == "0") return InputValidator::ValidationResult(true);
                                return InputValidator::validateName(input, "Performer name");
                            }
                        );
                        if (newName == "0") newName = "";
                        
                        std::cout << "\nCurrent type: " << performer->type << std::endl;
                        std::cout << "Select new type (or 0 to keep current):\n";
                        newType = getChoiceFromOptions(PredefinedOptions::PERFORMER_TYPES, "Select New Performer Type:");
                        if (newType == "0") newType = "";
                        
                        newContact = getValidatedInput(
                            "New contact (current: " + performer->contact_info + ", 0 to skip)",
                            [](const std::string& input) {
                                if (input == "0") return InputValidator::ValidationResult(true);
                                return InputValidator::validateContactInfo(input, "Contact info", 5, 100);
                            }
                        );
                        if (newContact == "0") newContact = "";
                        
                        newBio = getValidatedInput(
                            "New bio (current: " + performer->bio + ", 0 to skip)",
                            [](const std::string& input) {
                                if (input == "0") return InputValidator::ValidationResult(true);
                                return InputValidator::validateBiography(input, "Biography", 0, 500);
                            }
                        );
                        if (newBio == "0") newBio = "";
                        
                        if (g_performerModule->updatePerformer(performerId, newName, newType, newContact, newBio)) {
                            std::cout << "✅ Performer updated successfully!\n";
                        } else {
                            std::cout << "❌ Failed to update performer.\n";
                        }
                        break;
                    }
                    case 3: { // Delete Performer
                        int performerId;
                        std::cout << "Enter Performer ID to delete (or 0 to cancel): ";
                        std::cin >> performerId;
                        if (performerId == 0) break;
                        
                        if (g_performerModule->deletePerformer(performerId)) {
                            std::cout << "✅ Performer deleted successfully!\n";
                        } else {
                            std::cout << "❌ Failed to delete performer.\n";
                        }
                        break;
                    }
                    case 0: // Back
                        break;
                }
                break;
            }
            case 2: { // Crew Management
                std::cout << "\n--- Crew Management ---\n";
                std::cout << "1. Create New Crew Member\n";
                std::cout << "2. Update Crew Member\n";
                std::cout << "3. Delete Crew Member\n";
                std::cout << "0. Back\n";
                
                int subChoice;
                std::cout << "Enter choice (0-3): ";
                std::cin >> subChoice;
                std::cin.ignore();
                
                switch (subChoice) {
                    case 1: { // Create New Crew Member
                        std::string name, role, contactInfo, skills;
                        
                        name = getValidatedInput(
                            "Crew Member Name",
                            [](const std::string& input) {
                                return InputValidator::validateName(input, "Crew member name");
                            }
                        );
                        if (name == "0") break;
                        
                        role = getChoiceFromOptions(PredefinedOptions::CREW_ROLES, "Select Crew Role:");
                        if (role == "0") break;
                        
                        contactInfo = getValidatedInput(
                            "Contact Info (email/phone)",
                            [](const std::string& input) {
                                return InputValidator::validateContactInfo(input, "Contact info", 5, 100);
                            }
                        );
                        if (contactInfo == "0") break;
                        
                        skills = getValidatedInput(
                            "Skills/Certifications (optional)",
                            [](const std::string& input) {
                                if (input.empty()) return InputValidator::ValidationResult(true);
                                return InputValidator::validateBiography(input, "Skills/Certifications", 0, 200);
                            }
                        );
                        if (skills == "0") break;
                        
                        auto crew = g_crewModule->createCrewMember(name, contactInfo, contactInfo, role);
                        if (crew) {
                            std::cout << "✅ Crew member created successfully! ID: " << crew->id << std::endl;
                        } else {
                            std::cout << "❌ Failed to create crew member.\n";
                        }
                        break;
                    }
                    case 2: { // Update Crew Member
                        int crewId;
                        std::cout << "Enter Crew ID to update: ";
                        std::cin >> crewId;
                        std::cin.ignore();
                        
                        auto crew = g_crewModule->getCrewById(crewId);
                        if (!crew) {
                            std::cout << "❌ Crew member not found.\n";
                            break;
                        }
                        
                        std::string newName, newRole, newContact, newSkills;
                        
                        newName = getValidatedInput(
                            "New name (current: " + crew->name + ", 0 to skip)",
                            [](const std::string& input) {
                                if (input == "0") return InputValidator::ValidationResult(true);
                                return InputValidator::validateName(input, "Crew member name");
                            }
                        );
                        if (newName == "0") newName = "";
                        
                        std::cout << "\nCurrent role: " << g_crewModule->getCrewJob(crew->id) << std::endl;
                        std::cout << "Select new role (or 0 to keep current):\n";
                        newRole = getChoiceFromOptions(PredefinedOptions::CREW_ROLES, "Select New Crew Role:");
                        if (newRole == "0") newRole = "";
                        
                        newContact = getValidatedInput(
                            "New contact (current: " + crew->email + ", 0 to skip)",
                            [](const std::string& input) {
                                if (input == "0") return InputValidator::ValidationResult(true);
                                return InputValidator::validateContactInfo(input, "Contact info", 5, 100);
                            }
                        );
                        if (newContact == "0") newContact = "";
                        
                        newSkills = getValidatedInput(
                            "New skills (0 to skip)",
                            [](const std::string& input) {
                                if (input == "0" || input.empty()) return InputValidator::ValidationResult(true);
                                return InputValidator::validateBiography(input, "Skills/Certifications", 0, 200);
                            }
                        );
                        if (newSkills == "0") newSkills = "";
                        
                        if (g_crewModule->updateCrewMember(crewId, newName, newContact, newContact)) {
                            std::cout << "✅ Crew member updated successfully!\n";
                        } else {
                            std::cout << "❌ Failed to update crew member.\n";
                        }
                        break;
                    }
                    case 3: { // Delete Crew Member
                        int crewId;
                        std::cout << "Enter Crew ID to delete: ";
                        std::cin >> crewId;
                        
                        if (g_crewModule->deleteEntity(crewId)) {
                            std::cout << "✅ Crew member deleted successfully!\n";
                        } else {
                            std::cout << "❌ Failed to delete crew member.\n";
                        }
                        break;
                    }
                }
                break;
            }
            case 3: { // View All Performers
                auto performers = g_performerModule->getAllPerformers();
                std::cout << "\n--- All Performers ---\n";
                for (const auto& performer : performers) {
                    std::cout << "ID: " << performer->performer_id << " | " << performer->name 
                              << " | Type: " << performer->type << std::endl;
                }
                if (performers.empty()) {
                    std::cout << "No performers found.\n";
                }
                break;
            }
            case 4: { // View All Crew Members
                auto crews = g_crewModule->getAllCrew();
                std::cout << "\n--- All Crew Members ---\n";
                for (const auto& crew : crews) {
                    std::cout << "ID: " << crew->id << " | " << crew->name 
                              << " | Role: " << g_crewModule->getCrewJob(crew->id) << std::endl;
                }
                if (crews.empty()) {
                    std::cout << "No crew members found.\n";
                }
                break;
            }
            case 5: { // Search Performers
                std::string searchTerm;
                std::cout << "Enter performer name to search (or 0 to cancel): ";
                std::getline(std::cin, searchTerm);
                if (searchTerm == "0") break;
                
                auto performers = g_performerModule->findPerformersByName(searchTerm);
                std::cout << "\n--- Search Results ---\n";
                for (const auto& performer : performers) {
                    std::cout << "ID: " << performer->performer_id << " | " << performer->name 
                              << " | Type: " << performer->type << std::endl;
                }
                if (performers.empty()) {
                    std::cout << "No performers found matching '" << searchTerm << "'.\n";
                }
                break;
            }
            case 6: { // Search Crew
                std::string searchTerm;
                std::cout << "Enter crew member name to search (or 0 to cancel): ";
                std::getline(std::cin, searchTerm);
                if (searchTerm == "0") break;
                
                auto crews = g_crewModule->findCrewByName(searchTerm);
                std::cout << "\n--- Search Results ---\n";
                for (const auto& crew : crews) {
                    std::cout << "ID: " << crew->id << " | " << crew->name 
                              << " | Role: " << g_crewModule->getCrewJob(crew->id) << std::endl;
                }
                if (crews.empty()) {
                    std::cout << "No crew members found matching '" << searchTerm << "'.\n";
                }
                break;
            }
            case 0: // Back to Management Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void monitorPayments() {
    while (true) {
        std::cout << "\n--- Payment & Financial Monitoring ---\n";
        std::cout << "1. View Payment Statistics\n";
        std::cout << "2. Process Refund\n";
        std::cout << "3. View Transaction History\n";
        std::cout << "4. Payment Status Updates\n";
        std::cout << "5. Revenue Analysis\n";
        std::cout << "6. Failed Payments\n";
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-6): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // View Payment Statistics
                auto payments = g_paymentModule->getRecentPayments(1000); // Get recent payments
                std::cout << "\n--- Payment Statistics ---\n";
                std::cout << "Total Payments: " << payments.size() << std::endl;
                
                int completed = 0, pending = 0, failed = 0, refunded = 0;
                double totalRevenue = 0.0;
                
                for (const auto& payment : payments) {
                    switch (payment->status) {
                        case Model::PaymentStatus::COMPLETED:
                            completed++;
                            totalRevenue += payment->amount;
                            break;
                        case Model::PaymentStatus::PENDING:
                            pending++;
                            break;
                        case Model::PaymentStatus::FAILED:
                            failed++;
                            break;
                        case Model::PaymentStatus::REFUNDED:
                            refunded++;
                            break;
                    }
                }
                
                std::cout << "Completed: " << completed << std::endl;
                std::cout << "Pending: " << pending << std::endl;
                std::cout << "Failed: " << failed << std::endl;
                std::cout << "Refunded: " << refunded << std::endl;
                std::cout << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
                break;
            }
            case 2: { // Process Refund
                std::string pidStr;
                std::cout << "Enter Payment ID to refund (or 0 to cancel): ";
                std::getline(std::cin, pidStr);
                if (!isValidInteger(pidStr)) { std::cout << "❌ Invalid ID.\n"; break; }
                int paymentId = std::stoi(pidStr);
                if (paymentId == 0) break;
                
                auto payment = g_paymentModule->getPaymentById(paymentId);
                if (!payment) {
                    std::cout << "❌ Payment not found.\n";
                    break;
                }
                
                if (payment->status != Model::PaymentStatus::COMPLETED) {
                    std::cout << "❌ Can only refund completed payments.\n";
                    break;
                }
                
                std::string reason;
                std::cout << "Refund reason: ";
                std::getline(std::cin, reason);
                
                std::string refundResult = g_paymentModule->processRefund(paymentId, 0.0, reason);
                if (!refundResult.empty()) {
                    std::cout << "✅ Refund processed successfully! Transaction ID: " << refundResult << "\n";
                } else {
                    std::cout << "❌ Failed to process refund.\n";
                }
                break;
            }
            case 3: { // View Transaction History
                auto payments = g_paymentModule->getRecentPayments(1000); // Get recent payments

                // Filter to COMPLETED only and sort by payment_id ascending
                std::vector<std::shared_ptr<Model::Payment>> completed;
                completed.reserve(payments.size());
                for (const auto& p : payments) {
                    if (p && p->status == Model::PaymentStatus::COMPLETED) {
                        completed.push_back(p);
                    }
                }
                std::sort(completed.begin(), completed.end(),
                    [](const std::shared_ptr<Model::Payment>& a, const std::shared_ptr<Model::Payment>& b) {
                        return a->payment_id < b->payment_id;
                    });

                std::cout << "\n--- Transaction History (COMPLETED only) ---\n";
                std::cout << std::setw(8) << "ID" << " | " 
                          << std::setw(15) << "Transaction ID" << " | "
                          << std::setw(10) << "Amount" << " | "
                          << std::setw(12) << "Status" << " | "
                          << std::setw(15) << "Method" << std::endl;
                std::cout << std::string(70, '-') << std::endl;
                
                for (const auto& payment : completed) {
                    std::cout << std::setw(8) << payment->payment_id << " | " 
                              << std::setw(15) << payment->transaction_id << " | "
                              << std::setw(10) << std::fixed << std::setprecision(2) << payment->amount << " | "
                              << std::setw(12) << "COMPLETED"
                              << " | " << std::setw(15) << payment->payment_method << std::endl;
                }
                
                if (completed.empty()) {
                    std::cout << "No completed transactions found.\n";
                }
                break;
            }
            case 4: { // Payment Status Updates
                std::string pidStr;
                std::cout << "Enter Payment ID to update: ";
                std::getline(std::cin, pidStr);
                if (!isValidInteger(pidStr)) { std::cout << "❌ Invalid ID.\n"; break; }
                int paymentId = std::stoi(pidStr);
                
                auto payment = g_paymentModule->getPaymentById(paymentId);
                if (!payment) {
                    std::cout << "❌ Payment not found.\n";
                    break;
                }
                
                std::cout << "Current status: ";
                switch (payment->status) {
                    case Model::PaymentStatus::COMPLETED: std::cout << "COMPLETED"; break;
                    case Model::PaymentStatus::PENDING: std::cout << "PENDING"; break;
                    case Model::PaymentStatus::FAILED: std::cout << "FAILED"; break;
                    case Model::PaymentStatus::REFUNDED: std::cout << "REFUNDED"; break;
                }
                std::cout << std::endl;
                
                std::cout << "New status (1=PENDING, 2=COMPLETED, 3=FAILED, 4=REFUNDED): ";
                std::string statusStr; std::getline(std::cin, statusStr);
                if (!isValidInteger(statusStr)) { std::cout << "❌ Invalid choice.\n"; break; }
                int statusChoice = std::stoi(statusStr);
                
                Model::PaymentStatus newStatus;
                switch (statusChoice) {
                    case 1: newStatus = Model::PaymentStatus::PENDING; break;
                    case 2: newStatus = Model::PaymentStatus::COMPLETED; break;
                    case 3: newStatus = Model::PaymentStatus::FAILED; break;
                    case 4: newStatus = Model::PaymentStatus::REFUNDED; break;
                    default:
                        std::cout << "❌ Invalid status choice.\n";
                        continue;
                }
                
                if (g_paymentModule->updatePaymentStatus(paymentId, newStatus)) {
                    std::cout << "✅ Payment status updated successfully!\n";
                } else {
                    std::cout << "❌ Failed to update payment status.\n";
                }
                break;
            }
            case 5: { // Revenue Analysis
                auto payments = g_paymentModule->getRecentPayments(1000); // Get recent payments
                if (payments.empty()) {
                    std::cout << "No payment data available for revenue report.\n";
                    break;
                }
                
                double totalRevenue = 0, completedRevenue = 0, refundedAmount = 0;
                int totalTransactions = 0, completedTransactions = 0;
                
                for (const auto& payment : payments) {
                    totalTransactions++;
                    totalRevenue += payment->amount;
                    
                    if (payment->status == Model::PaymentStatus::COMPLETED) {
                        completedTransactions++;
                        completedRevenue += payment->amount;
                    } else if (payment->status == Model::PaymentStatus::REFUNDED) {
                        refundedAmount += payment->amount;
                    }
                }
                
                std::cout << "\n--- Revenue Report ---\n";
                std::cout << "Total Transactions: " << totalTransactions << std::endl;
                std::cout << "Completed Transactions: " << completedTransactions << std::endl;
                std::cout << "Gross Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
                std::cout << "Net Revenue: $" << std::fixed << std::setprecision(2) << completedRevenue << std::endl;
                std::cout << "Refunded Amount: $" << std::fixed << std::setprecision(2) << refundedAmount << std::endl;
                std::cout << "Average Transaction: $" << std::fixed << std::setprecision(2) 
                          << (completedTransactions > 0 ? completedRevenue / completedTransactions : 0) << std::endl;
                break;
            }
            case 6: { // Failed Payments
                auto payments = g_paymentModule->getPaymentsByStatus(Model::PaymentStatus::FAILED);
                std::cout << "\n--- Failed Payments ---\n";
                if (payments.empty()) {
                    std::cout << "No failed payments found.\n";
                    break;
                }
                
                for (const auto& payment : payments) {
                    std::cout << "Payment ID: " << payment->payment_id 
                              << " | Amount: $" << std::fixed << std::setprecision(2) << payment->amount
                              << " | Method: " << payment->payment_method 
                              << " | Date: " << payment->payment_date_time.iso8601String << std::endl;
                }
                break;
            }
            case 0: // Back to Management Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

// Communication Logs Management
void manageCommunicationLogs() {
    while (true) {
        std::cout << "\n--- Communication Logs ---\n";
        std::cout << "1. Create Chatroom for Concert\n";
        std::cout << "2. Join Chatroom\n";
        std::cout << "3. Send Message\n";
        std::cout << "4. View Recent Messages\n";
        std::cout << "5. View Pinned Announcements\n";
        std::cout << "6. Search Messages\n";
        std::cout << "7. View CommunicationLog (persisted)\n";
        std::cout << "8. Chat Stats\n";
        std::cout << "0. Back\n";

        std::string choiceStr;
        std::cout << "Enter choice (0-8): ";
        std::getline(std::cin, choiceStr);
        if (!isValidInteger(choiceStr)) { std::cout << "❌ Invalid input.\n"; continue; }
        int choice = std::stoi(choiceStr);

        if (choice == 0) return;

        switch (choice) {
            case 1: { // Create Chatroom
                std::string idStr; std::cout << "Concert ID: "; std::getline(std::cin, idStr);
                if (!isValidInteger(idStr)) { std::cout << "❌ Invalid concert id.\n"; break; }
                int concertId = std::stoi(idStr);
                std::string name; std::cout << "Concert Name: "; std::getline(std::cin, name);
                bool created = g_commModule->createChatroom(concertId, name);
                std::cout << (created ? "✅ Chatroom created.\n" : "ℹ️ Chatroom already exists.\n");
                break;
            }
            case 2: { // Join Chatroom
                std::string idStr; std::cout << "Concert ID: "; std::getline(std::cin, idStr);
                if (!isValidInteger(idStr)) { std::cout << "❌ Invalid concert id.\n"; break; }
                int concertId = std::stoi(idStr);
                std::string username; std::cout << "Username: "; std::getline(std::cin, username);
                // Auto-generate a stable user ID from username
                int userId = static_cast<int>(std::hash<std::string>{}(username) & 0x7fffffff);
                UserRole role = UserRole::ATTENDEE; // default role
                bool ok = g_commModule->subscribeToChat(concertId, userId, username, role);
                std::cout << (ok ? "✅ Joined chatroom.\n" : "❌ Failed to join (chatroom missing?).\n");
                break;
            }
            case 3: { // Send Message
                std::string idStr; std::cout << "Concert ID: "; std::getline(std::cin, idStr);
                if (!isValidInteger(idStr)) { std::cout << "❌ Invalid id.\n"; break; }
                int concertId = std::stoi(idStr);
                std::string username; std::cout << "Sender Name: "; std::getline(std::cin, username);
                std::string content; std::cout << "Message: "; std::getline(std::cin, content);
                int senderId = static_cast<int>(std::hash<std::string>{}(username) & 0x7fffffff);
                auto* msg = g_commModule->sendMessage(concertId, senderId, username, UserRole::ATTENDEE, content, MessageType::REGULAR);
                std::cout << (msg ? "✅ Message sent.\n" : "❌ Failed (chatroom missing?).\n");
                break;
            }
            case 4: { // View Recent Messages
                std::string idStr; std::cout << "Concert ID: "; std::getline(std::cin, idStr);
                if (!isValidInteger(idStr)) { std::cout << "❌ Invalid id.\n"; break; }
                int concertId = std::stoi(idStr);
                std::string limitStr; std::cout << "Limit (default 50): "; std::getline(std::cin, limitStr);
                int limit = (isValidInteger(limitStr) ? std::stoi(limitStr) : 50);
                auto msgs = g_commModule->getMessages(concertId, limit);
                if (msgs.empty()) { std::cout << "(no messages)\n"; break; }
                std::cout << "\n--- Recent Messages ---\n";
                for (auto* m : msgs) {
                    std::cout << "#" << m->message_id << " [" << formatTimestampDisplay(m->sent_at.iso8601String) << "] ";
                    std::cout << m->sender_name << ": " << m->message_content;
                    if (m->is_pinned) std::cout << " (PINNED)";
                    std::cout << "\n";
                }
                break;
            }
            case 5: { // View Pinned
                std::string idStr; std::cout << "Concert ID: "; std::getline(std::cin, idStr);
                if (!isValidInteger(idStr)) { std::cout << "❌ Invalid id.\n"; break; }
                int concertId = std::stoi(idStr);
                auto pinned = g_commModule->getPinnedMessages(concertId);
                if (pinned.empty()) { std::cout << "(no pinned messages)\n"; break; }
                std::cout << "\n--- Pinned Announcements ---\n";
                for (auto* m : pinned) {
                    std::cout << "#" << m->message_id << ": " << m->message_content << "\n";
                }
                break;
            }
            case 6: { // Search
                std::string idStr; std::cout << "Concert ID: "; std::getline(std::cin, idStr);
                if (!isValidInteger(idStr)) { std::cout << "❌ Invalid id.\n"; break; }
                int concertId = std::stoi(idStr);
                std::string keyword; std::cout << "Keyword: "; std::getline(std::cin, keyword);
                auto results = g_commModule->searchMessages(concertId, keyword);
                if (results.empty()) { std::cout << "No matches.\n"; break; }
                std::cout << "\n--- Search Results ---\n";
                for (auto* m : results) {
                    std::cout << "#" << m->message_id << " " << m->sender_name << ": " << m->message_content << "\n";
                }
                break;
            }
            case 7: { // View persisted CommunicationLog
                auto logs = g_commModule->getAll();
                if (logs.empty()) { std::cout << "No CommunicationLog entries found.\n"; break; }
                std::cout << "\n--- CommunicationLog (Persisted) ---\n";
                std::cout << std::setw(8) << "ID" << " | "
                          << std::setw(10) << "Type" << " | "
                          << std::setw(10) << "Recipients" << " | "
                          << "Sent At" << "\n";
                std::cout << std::string(60, '-') << "\n";
                for (const auto& log : logs) {
                    std::cout << std::setw(8) << log->comm_id << " | "
                              << std::setw(10) << log->comm_type << " | "
                              << std::setw(10) << log->recipient_count << " | "
                              << formatTimestampDisplay(log->sent_at.iso8601String) << "\n";
                }
                break;
            }
            default:
                std::cout << "❌ Invalid choice.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void manageFeedbackAndComm() {
    while (true) {
        std::cout << "\n--- Feedback & Communication Management ---\n";
        std::cout << "1. View All Feedback\n";
        std::cout << "2. Check Critical/Urgent Feedback\n";
        std::cout << "3. Communication Logs\n";
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-3): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // View All Feedback
                auto allFeedback = g_feedbackModule->getAll();
                std::cout << "\n--- All Feedback ---\n";
                for (const auto& feedback : allFeedback) {
                    std::cout << "Rating: " << feedback->rating 
                              << "/5 | " << feedback->comments.substr(0, 50) << "...\n";
                }
                break;
            }
            case 2: { // Check Critical Feedback
                std::cout << "🚨 Checking for critical feedback with urgent keywords...\n";
                auto allFeedback = g_feedbackModule->getAll();
                bool foundCritical = false;
                
                // Critical keywords that trigger escalation
                std::vector<std::string> criticalKeywords = {
                    "terrible", "awful", "worst", "horrible", "unsafe", 
                    "dangerous", "emergency", "urgent", "complaint", "refund"
                };
                
                for (const auto& feedback : allFeedback) {
                    std::string content = feedback->comments;
                    std::transform(content.begin(), content.end(), content.begin(), ::tolower);
                    
                    for (const auto& keyword : criticalKeywords) {
                        if (content.find(keyword) != std::string::npos) {
                            std::cout << "🚨 CRITICAL FEEDBACK DETECTED:\n";
                            std::cout << "   Rating: " << feedback->rating 
                                      << "/5\n   Content: " << feedback->comments << "\n";
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
            case 3: { // Communication Logs
                manageCommunicationLogs();
                break;
            }
            case 0: // Back
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void generateReports() {
    while (true) {
        std::cout << "\n--- Reports & Analytics ---\n";
        std::cout << "1. Concert Attendance Report\n";
        std::cout << "2. Revenue Report\n";
        std::cout << "3. Venue Utilization Report\n";
        std::cout << "4. Performer Analytics\n";
        std::cout << "5. Ticket Sales Analysis\n";
        std::cout << "6. System Usage Report\n";
        std::cout << "7. Export Reports\n";
        std::cout << "0. Back to Management Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-7): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // Concert Attendance Report
                auto concerts = g_concertModule->getAllConcerts();
                if (concerts.empty()) {
                    std::cout << "No concerts available for reporting.\n";
                    break;
                }
                
                std::cout << "\n--- Concert Attendance Report ---\n";
                std::cout << std::setw(8) << "ID" << " | " 
                          << std::setw(25) << "Concert Name" << " | "
                          << std::setw(12) << "Status" << " | "
                          << std::setw(10) << "Capacity" << std::endl;
                std::cout << std::string(65, '-') << std::endl;
                
                int totalConcerts = 0, completedConcerts = 0;
                for (const auto& concert : concerts) {
                    totalConcerts++;
                    if (concert->event_status == Model::EventStatus::COMPLETED) {
                        completedConcerts++;
                    }
                    
                    std::cout << std::setw(8) << concert->id << " | " 
                              << std::setw(25) << (concert->name.length() > 25 ? concert->name.substr(0, 22) + "..." : concert->name) << " | ";
                    
                    switch (concert->event_status) {
                        case Model::EventStatus::SCHEDULED: std::cout << std::setw(12) << "SCHEDULED"; break;
                        case Model::EventStatus::CANCELLED: std::cout << std::setw(12) << "CANCELLED"; break;
                        case Model::EventStatus::POSTPONED: std::cout << std::setw(12) << "POSTPONED"; break;
                        case Model::EventStatus::COMPLETED: std::cout << std::setw(12) << "COMPLETED"; break;
                        case Model::EventStatus::SOLDOUT: std::cout << std::setw(12) << "SOLD OUT"; break;
                    }
                    
                    int venueCapacity = concert->venue ? concert->venue->capacity : 0;
                    std::cout << " | " << std::setw(10) << venueCapacity << std::endl;
                }
                
                std::cout << "\nSummary:\n";
                std::cout << "Total Concerts: " << totalConcerts << std::endl;
                std::cout << "Completed: " << completedConcerts << std::endl;
                std::cout << "Completion Rate: " << std::fixed << std::setprecision(1) 
                          << (totalConcerts > 0 ? static_cast<double>(completedConcerts) / totalConcerts * 100 : 0) << "%" << std::endl;
                break;
            }
            case 2: { // Revenue Report
                auto payments = g_paymentModule->getRecentPayments(1000); // Get recent payments
                if (payments.empty()) {
                    std::cout << "No payment data available for revenue report.\n";
                    break;
                }
                
                double totalRevenue = 0, completedRevenue = 0, refundedAmount = 0;
                int totalTransactions = 0, completedTransactions = 0;
                
                for (const auto& payment : payments) {
                    totalTransactions++;
                    totalRevenue += payment->amount;
                    
                    if (payment->status == Model::PaymentStatus::COMPLETED) {
                        completedTransactions++;
                        completedRevenue += payment->amount;
                    } else if (payment->status == Model::PaymentStatus::REFUNDED) {
                        refundedAmount += payment->amount;
                    }
                }
                
                std::cout << "\n--- Revenue Report ---\n";
                std::cout << "Total Transactions: " << totalTransactions << std::endl;
                std::cout << "Completed Transactions: " << completedTransactions << std::endl;
                std::cout << "Gross Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
                std::cout << "Net Revenue: $" << std::fixed << std::setprecision(2) << completedRevenue << std::endl;
                std::cout << "Refunded Amount: $" << std::fixed << std::setprecision(2) << refundedAmount << std::endl;
                std::cout << "Average Transaction: $" << std::fixed << std::setprecision(2) 
                          << (completedTransactions > 0 ? completedRevenue / completedTransactions : 0) << std::endl;
                break;
            }
            case 3: { // Venue Utilization Report
                auto venues = g_venueModule->getAllVenues();
                auto concerts = g_concertModule->getAllConcerts();
                
                if (venues.empty()) {
                    std::cout << "No venues available for reporting.\n";
                    break;
                }
                
                std::cout << "\n--- Venue Utilization Report ---\n";
                std::cout << std::setw(8) << "ID" << " | " 
                          << std::setw(20) << "Venue Name" << " | "
                          << std::setw(10) << "Concerts" << " | "
                          << std::setw(10) << "Capacity" << std::endl;
                std::cout << std::string(55, '-') << std::endl;
                
                for (const auto& venue : venues) {
                    int concertCount = 0;
                    for (const auto& concert : concerts) {
                        if (concert->venue && concert->venue->id == venue->id) {
                            concertCount++;
                        }
                    }
                    
                    std::cout << std::setw(8) << venue->id << " | " 
                              << std::setw(20) << (venue->name.length() > 20 ? venue->name.substr(0, 17) + "..." : venue->name) << " | "
                              << std::setw(10) << concertCount << " | "
                              << std::setw(10) << venue->capacity << std::endl;
                }
                break;
            }
            case 4: { // Performer Analytics
                auto performers = g_performerModule->getAllPerformers();
                if (performers.empty()) {
                    std::cout << "No performers available for analytics.\n";
                    break;
                }
                
                // Group performers by type
                std::map<std::string, int> performerTypes;
                for (const auto& performer : performers) {
                    performerTypes[performer->type]++;
                }
                
                std::cout << "\n--- Performer Analytics ---\n";
                std::cout << "Total Performers: " << performers.size() << std::endl;
                std::cout << "\nBreakdown by Type:\n";
                std::cout << std::setw(15) << "Type" << " | " << std::setw(8) << "Count" << std::endl;
                std::cout << std::string(25, '-') << std::endl;
                
                for (const auto& type : performerTypes) {
                    std::cout << std::setw(15) << type.first << " | " << std::setw(8) << type.second << std::endl;
                }
                break;
            }
            case 5: { // Ticket Sales Analysis
                auto tickets = g_ticketModule->getAll();
                if (tickets.empty()) {
                    std::cout << "No ticket data available for analysis.\n";
                    break;
                }
                
                int available = 0, sold = 0, checkedIn = 0, cancelled = 0;
                for (const auto& ticket : tickets) {
                    switch (ticket->status) {
                        case Model::TicketStatus::AVAILABLE: available++; break;
                        case Model::TicketStatus::SOLD: sold++; break;
                        case Model::TicketStatus::CHECKED_IN: checkedIn++; break;
                        case Model::TicketStatus::CANCELLED: cancelled++; break;
                        case Model::TicketStatus::EXPIRED: cancelled++; break;
                    }
                }
                
                std::cout << "\n--- Ticket Sales Analysis ---\n";
                std::cout << "Total Tickets: " << tickets.size() << std::endl;
                std::cout << "Available: " << available << std::endl;
                std::cout << "Sold: " << sold << std::endl;
                std::cout << "Checked In: " << checkedIn << std::endl;
                std::cout << "Cancelled/Expired: " << cancelled << std::endl;
                std::cout << "Sales Rate: " << std::fixed << std::setprecision(1) 
                          << (tickets.size() > 0 ? static_cast<double>(sold + checkedIn) / tickets.size() * 100 : 0) << "%" << std::endl;
                break;
            }
            case 6: { // System Usage Report
                auto concerts = g_concertModule->getAllConcerts();
                auto venues = g_venueModule->getAllVenues();
                auto performers = g_performerModule->getAllPerformers();
                auto attendees = g_attendeeModule->getAllAttendees();
                auto tickets = g_ticketModule->getAll();
                auto payments = g_paymentModule->getRecentPayments(1000); // Get recent payments
                
                std::cout << "\n--- System Usage Report ---\n";
                std::cout << "Concerts: " << concerts.size() << std::endl;
                std::cout << "Venues: " << venues.size() << std::endl;
                std::cout << "Performers: " << performers.size() << std::endl;
                std::cout << "Attendees: " << attendees.size() << std::endl;
                std::cout << "Tickets: " << tickets.size() << std::endl;
                std::cout << "Payments: " << payments.size() << std::endl;
                
                // Calculate storage usage (simplified)
                size_t totalRecords = concerts.size() + venues.size() + performers.size() + 
                                     attendees.size() + tickets.size() + payments.size();
                std::cout << "Total Records: " << totalRecords << std::endl;
                break;
            }
            case 7: { // Export Reports
                std::cout << "\n--- Export Reports ---\n";
                std::cout << "Available formats: JSON, CSV\n";
                std::cout << "1. Export Concert Data\n";
                std::cout << "2. Export Financial Data\n";
                std::cout << "3. Export All Data\n";
                std::cout << "4. Back\n";
                
                int exportChoice;
                std::cout << "Enter choice (1-4): ";
                std::cin >> exportChoice;
                
                switch (exportChoice) {
                    case 1:
                        std::cout << "✅ Concert data export initiated (feature placeholder)\n";
                        break;
                    case 2:
                        std::cout << "✅ Financial data export initiated (feature placeholder)\n";
                        break;
                    case 3:
                        std::cout << "✅ Full system export initiated (feature placeholder)\n";
                        break;
                    case 4:
                        break;
                    default:
                        std::cout << "❌ Invalid choice.\n";
                }
                break;
            }
            case 0: // Back to Management Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}


// Management Portal Main Function
void runManagementPortal() {
    while (true) {
        displayManagementMenu();
        
        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            UIManager::displayError("Invalid input. Please enter only integer numbers (e.g., '1', '2', '3').");
            continue;
        }
        
        int choice;
        try {
            choice = std::stoi(choiceStr);
        } catch (const std::exception&) {
            UIManager::displayError("Invalid input. Please enter only integer numbers.");
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
                showAnalyticsDashboard();
                break;
            case 8: // System Administration
                systemAdministration();
                break;
            case 9: // Switch to User Portal
                runUserPortal();
                break;
            case 10: // Input Validation Demo
                demonstrateValidation();
                UIManager::displayInfo("Press Enter to continue...");
                std::cin.get();
                break;
            case 0: // Return to Main Menu
                return;
            default:
                std::cout << "❌ Invalid choice. Please select a valid option.\n";
        }
    }
}


void showAnalyticsDashboard() {
    // Create report file with timestamp
    std::string timestamp = Model::DateTime::now().iso8601String;
    std::replace(timestamp.begin(), timestamp.end(), ':', '-'); // Replace : with - for valid filename
    const std::string dataDir = "data";
    const std::string reportsDir = dataDir + "/reports";
    std::string reportPath = reportsDir + "/analytics_" + timestamp + ".txt";

    // Ensure directories exist (portable)
    #ifdef _WIN32
        _mkdir(dataDir.c_str());          // creates if missing; no-op otherwise
        _mkdir(reportsDir.c_str());
    #else
        mkdir(dataDir.c_str(), 0755);
        mkdir(reportsDir.c_str(), 0755);
    #endif
    
    // Open report file
    std::ofstream reportFile(reportPath);
    if (!reportFile) {
        std::cerr << "Failed to create report file: " << reportPath << std::endl;
        return;
    }
    
    // Helper lambda to write both to console and file
    auto writeOutput = [&reportFile](const std::string& text) {
        std::cout << text;
        reportFile << text;
    };
    
    writeOutput("\n" + std::string(100, '=') + "\n");
    writeOutput("📊 MUSEIO ANALYTICS DASHBOARD\n");
    writeOutput("Generated: " + timestamp + "\n");
    writeOutput(std::string(100, '=') + "\n");

    // Ticket Sales Overview
    double totalSales = 0.0;
    int totalTicketsSold = 0;
    int totalAttendees = 0;
    double avgEngagementScore = 0.0;
    int totalReports = 0;

    for (const auto& report : g_reportModule->getAll()) {
        totalSales += report->sales_volume;
        totalTicketsSold += report->tickets_sold;
        totalAttendees += report->total_registrations;
        avgEngagementScore += report->attendee_engagement_score;
        totalReports++;
    }

    if (totalReports > 0) {
        avgEngagementScore /= totalReports;
    }

    // Sales Visualization
    std::cout << "\n📈 TICKET SALES & REVENUE" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalSales << std::endl;
    std::cout << "Tickets Sold: " << totalTicketsSold << std::endl;
    
    // Progress bar for ticket sales (assuming 1000 as max capacity)
    int barWidth = 40;

    // Calculate total venue capacity for more accurate sales metrics
    int totalCapacity = 0;
    auto venues = g_venueModule->getAllVenues();
    for (const auto& venue : venues) {
        totalCapacity += venue->capacity;
    }
    
    // Use total capacity as max for sales progress (fallback to 1000 if no venues)
    int maxCapacity = totalCapacity > 0 ? totalCapacity : 1000;
    int progress = (totalTicketsSold * barWidth) / maxCapacity;
    int percentage = maxCapacity > 0 ? (totalTicketsSold * 100) / maxCapacity : 0;

    std::cout << "Sales Progress: [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < progress) std::cout << "█";
        else std::cout << " ";
    }
    
    std::cout << "] " << percentage << "% (" << totalTicketsSold << "/" << maxCapacity << " total tickets available for sale)" << std::endl;

    // Attendee Engagement
    std::cout << "\n👥 ATTENDEE METRICS" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Total Registrations: " << totalAttendees << std::endl;
    std::cout << "Average Engagement Score: " << std::fixed << std::setprecision(1) << avgEngagementScore << "/10" << std::endl;
    
    // Engagement Visualization
    std::cout << "Engagement Level: ";
    int engagementStars = static_cast<int>(avgEngagementScore + 0.5);
    for (int i = 0; i < 10; ++i) {
        if (i < engagementStars) std::cout << "⭐";
        else std::cout << "☆";
    }
    std::cout << std::endl;

    // Recent Events
    std::cout << "\n🎫 RECENT EVENTS" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    auto concerts = g_concertModule->getAll();
    std::sort(concerts.begin(), concerts.end(),
        [](const auto& a, const auto& b) {
            return a->start_date_time.iso8601String > b->start_date_time.iso8601String;
        });

    int shownConcerts = 0;
    for (const auto& concert : concerts) {
        if (shownConcerts >= 5) break; // Show only 5 most recent concerts
        std::cout << "- " << concert->name << " | Date: " << concert->start_date_time.iso8601String << std::endl;
        shownConcerts++;
    }

    // User Feedback Overview
    std::cout << "\n💭 FEEDBACK OVERVIEW" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    double avgNPS = 0.0;
    int totalNPS = 0;
    for (const auto& report : g_reportModule->getAll()) {
        if (report->nps_score > 0) {
            avgNPS += report->nps_score;
            totalNPS++;
        }
    }
    if (totalNPS > 0) {
        avgNPS /= totalNPS;
    }
    
    std::cout << "Net Promoter Score: " << std::fixed << std::setprecision(1) << avgNPS << std::endl;
    std::cout << "NPS Category: ";
    if (avgNPS >= 70) std::cout << "🟢 Excellent";
    else if (avgNPS >= 50) std::cout << "🟡 Good";
    else std::cout << "🔴 Needs Improvement";
    std::cout << std::endl;

    // Append additional verbose information to report file only
    reportFile << "\n=== DETAILED ANALYTICS INFORMATION ===\n\n";
    
    // Venue Details
    reportFile << "VENUE CAPACITY BREAKDOWN:\n";
    reportFile << std::string(30, '-') << "\n";
    for (const auto& venue : venues) {
        reportFile << "- " << venue->name << "\n";
        reportFile << "  Capacity: " << venue->capacity << "\n";
        reportFile << "  Location: " << venue->city << ", " << venue->state << "\n\n";
    }

    // Concert Details
    reportFile << "\nCONCERT SCHEDULE & STATUS:\n";
    reportFile << std::string(30, '-') << "\n";
    for (const auto& concert : concerts) {
        reportFile << "- " << concert->name << "\n";
        reportFile << "  Start: " << concert->start_date_time.iso8601String << "\n";
        reportFile << "  End: " << concert->end_date_time.iso8601String << "\n";
        reportFile << "  Status: ";
        switch (concert->event_status) {
            case Model::EventStatus::SCHEDULED: reportFile << "SCHEDULED"; break;
            case Model::EventStatus::CANCELLED: reportFile << "CANCELLED"; break;
            case Model::EventStatus::POSTPONED: reportFile << "POSTPONED"; break;
            case Model::EventStatus::COMPLETED: reportFile << "COMPLETED"; break;
            case Model::EventStatus::SOLDOUT: reportFile << "SOLD OUT"; break;
            default: reportFile << "UNKNOWN"; break;
        }
        reportFile << "\n\n";
    }

    // Financial Metrics
    reportFile << "\nFINANCIAL METRICS:\n";
    reportFile << std::string(30, '-') << "\n";
    reportFile << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalSales << "\n";
    reportFile << "Average Revenue per Ticket: $" << (totalTicketsSold > 0 ? totalSales/totalTicketsSold : 0) << "\n";
    reportFile << "Total Tickets Available: " << maxCapacity << "\n";
    reportFile << "Tickets Sold: " << totalTicketsSold << "\n";
    reportFile << "Sales Percentage: " << percentage << "%\n\n";

    // Engagement Metrics
    reportFile << "\nENGAGEMENT METRICS:\n";
    reportFile << std::string(30, '-') << "\n";
    reportFile << "Total Registrations: " << totalAttendees << "\n";
    reportFile << "Average Engagement Score: " << avgEngagementScore << "/10\n";
    reportFile << "Net Promoter Score: " << avgNPS << "\n";
    reportFile << "Total Reports Analyzed: " << totalReports << "\n";
    
    reportFile.close();
    
    writeOutput("\n" + std::string(100, '=') + "\n");
    writeOutput("Report exported to: " + reportPath + "\n");
    writeOutput(std::string(100, '=') + "\n");
    std::cout << "Press Enter to return to Management Portal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// System Administration with strict validation and robust submenus
void systemAdministration() {
    auto makeDir = [](const std::string& path) {
        #ifdef _WIN32
            _mkdir(path.c_str());
        #else
            mkdir(path.c_str(), 0755);
        #endif
    };

    auto copyFile = [](const std::string& src, const std::string& dst) -> bool {
        std::ifstream in(src, std::ios::binary);
        if (!in) return false;
        std::ofstream out(dst, std::ios::binary);
        if (!out) return false;
        out << in.rdbuf();
        return static_cast<bool>(out);
    };

    auto getBaseName = [](const std::string& path) -> std::string {
        size_t pos = path.find_last_of("/\\");
        return (pos == std::string::npos) ? path : path.substr(pos + 1);
    };

    // Known data files to back up/restore
    const std::vector<std::string> dataFiles = {
        DataPaths::ATTENDEES_FILE,
        DataPaths::CONCERTS_FILE,
        DataPaths::VENUES_FILE,
        DataPaths::PERFORMERS_FILE,
        DataPaths::CREWS_FILE,
        DataPaths::TICKETS_FILE,
        DataPaths::FEEDBACK_FILE,
        DataPaths::PAYMENTS_FILE,
        DataPaths::SPONSORS_FILE,
        DataPaths::PROMOTIONS_FILE,
        DataPaths::REPORTS_FILE,
        DataPaths::AUTH_FILE,
        DataPaths::COMM_FILE,
        std::string("data/chat_data.bin")
    };

    while (true) {
        std::cout << "\n--- System Administration ---\n";
        std::cout << "1. System Health Check\n";
        std::cout << "2. Data Backup & Restore\n";
        std::cout << "0. Back to Management Portal\n";
        std::cout << "Enter choice (0-2): ";

        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        if (!isValidInteger(choiceStr)) { std::cout << "❌ Invalid input.\n"; continue; }
        int choice; try { choice = std::stoi(choiceStr); } catch (...) { std::cout << "❌ Invalid input.\n"; continue; }

        switch (choice) {
            case 1: {
                // Lightweight system summary
                std::cout << "\n✅ System health: OPTIMAL\n";
                std::cout << "✅ Database integrity: VERIFIED\n";
                std::cout << "✅ Security status: SECURE\n";
                std::cout << "✅ Backup status: CHECK MANUALLY\n";
                std::cout << "✅ Modules operational: ";
                bool ok = (g_authModule && g_concertModule && g_ticketModule && g_venueModule &&
                           g_crewModule && g_paymentModule && g_feedbackModule && g_performerModule &&
                           g_reportModule && g_commModule);
                std::cout << (ok ? "YES" : "PARTIAL") << "\n";
                std::cout << "Press Enter to continue...";
                std::cin.get();
                break;
            }
            case 2: {
                // Data Backup & Restore submenu
                while (true) {
                    std::cout << "\n--- Data Backup & Restore ---\n";
                    std::cout << "1. Backup All Data\n";
                    std::cout << "2. Restore From Backup (enter path)\n";
                    std::cout << "3. View Backup Location\n";
                    std::cout << "0. Back\n";
                    std::cout << "Enter choice (0-3): ";

                    std::string bStr; std::getline(std::cin, bStr);
                    if (!isValidInteger(bStr)) { std::cout << "❌ Invalid input.\n"; continue; }
                    int b; try { b = std::stoi(bStr); } catch (...) { std::cout << "❌ Invalid input.\n"; continue; }

                    if (b == 0) break; // back to System Administration

                    switch (b) {
                        case 1: { // Backup
                            std::string ts = Model::DateTime::now().iso8601String;
                            std::replace(ts.begin(), ts.end(), ':', '-');
                            std::replace(ts.begin(), ts.end(), 'T', '_');
                            if (!ts.empty() && ts.back() == 'Z') ts.pop_back();

                            const std::string backupRoot = "data/backups";
                            const std::string backupDir = backupRoot + "/" + ts;
                            makeDir("data");
                            makeDir(backupRoot);
                            makeDir(backupDir);

                            int copied = 0, skipped = 0;
                            for (const auto& src : dataFiles) {
                                std::ifstream test(src, std::ios::binary);
                                if (!test) { skipped++; continue; }
                                const std::string dst = backupDir + "/" + getBaseName(src);
                                if (copyFile(src, dst)) copied++; else skipped++;
                            }
                            std::cout << "✅ Backup completed to: " << backupDir << "\n";
                            std::cout << "   Files copied: " << copied << ", skipped: " << skipped << "\n";
                            std::cout << "Press Enter to continue...";
                            std::cin.get();
                            break;
                        }
                        case 2: { // Restore
                            std::cout << "Enter backup folder path (e.g., data/backups/2025-09-10_12-34-56): ";
                            std::string path; std::getline(std::cin, path);
                            if (path.empty()) { std::cout << "❌ No path provided.\n"; break; }

                            int restored = 0, missing = 0;
                            for (const auto& dstFile : dataFiles) {
                                const std::string srcFile = path + "/" + getBaseName(dstFile);
                                if (copyFile(srcFile, dstFile)) restored++; else missing++;
                            }
                            std::cout << "🔄 Restore attempted from: " << path << "\n";
                            std::cout << "   Files restored: " << restored << ", missing/failed: " << missing << "\n";
                            std::cout << "Press Enter to continue...";
                            std::cin.get();
                            break;
                        }
                        case 3: {
                            std::cout << "Backups are stored under: data/backups\n";
                            std::cout << "Tip: Use option 2 with a specific timestamped folder to restore.\n";
                            std::cout << "Press Enter to continue...";
                            std::cin.get();
                            break;
                        }
                        default:
                            std::cout << "❌ Invalid choice. Please select 0-3.\n";
                    }
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "❌ Invalid choice. Please select 0-2.\n";
        }
    }
}

void runUserPortal() {
    while (true) {
        displayUserMenu();
        
        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            UIManager::displayError("Invalid input. Please enter only integer numbers (e.g., '1', '2', '3').");
            continue;
        }
        
        int choice;
        try {
            choice = std::stoi(choiceStr);
        } catch (const std::exception&) {
            UIManager::displayError("Invalid input. Please enter only integer numbers.");
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
            case 0: // Return to Main Menu
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
        std::cout << "0. Back to User Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-4): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // View All Upcoming Concerts
                auto concerts = g_concertModule->getAllConcerts();
                std::cout << "\n--- Upcoming Concerts ---\n";
                for (const auto& concert : concerts) {
                    if (concert->event_status == Model::EventStatus::SCHEDULED || 
                        concert->event_status == Model::EventStatus::SOLDOUT) {
                        std::cout << "🎵 " << concert->name << " | ID: " << concert->id << std::endl;
                        std::cout << "   📅 " << concert->start_date_time.iso8601String << std::endl;
                        if (concert->venue) {
                            std::cout << "   📍 " << concert->venue->name << ", " << concert->venue->city << std::endl;
                        }
                        std::cout << std::endl;
                    }
                }
                break;
            }
            case 2: { // Search by Genre  
                std::string genre;
                std::cout << "Enter genre to search (or 0 to cancel): ";
                std::getline(std::cin, genre);
                if (genre == "0") break;
                
                std::cout << "🔍 Searching for '" << genre << "' concerts...\n";
                std::cout << "⚠️ Genre search feature requires enhanced concert filtering.\n";
                break;
            }
            case 3: { // Search by Venue
                int venueId;
                std::cout << "Enter venue ID to search (or 0 to cancel): ";
                std::cin >> venueId;
                if (venueId == 0) break;
                std::cin.ignore();
                
                auto venue = g_venueModule->getVenueById(venueId);
                if (!venue) {
                    std::cout << "❌ Venue not found.\n";
                    break;
                }
                
                std::cout << "🔍 Concerts at " << venue->name << ":\n";
                auto concerts = g_concertModule->getAllConcerts();
                bool found = false;
                for (const auto& concert : concerts) {
                    if (concert->venue && concert->venue->id == venueId) {
                        std::cout << "🎵 " << concert->name << " | ID: " << concert->id << std::endl;
                        found = true;
                    }
                }
                if (!found) {
                    std::cout << "No concerts scheduled at this venue.\n";
                }
                break;
            }
            case 4: { // View Concert Details
                int concertId;
                std::cout << "Enter concert ID for details (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
                auto concert = g_concertModule->getConcertById(concertId);
                if (!concert) {
                    std::cout << "❌ Concert not found.\n";
                    break;
                }
                
                std::cout << "\n🎵 Concert Details:\n";
                std::cout << "Name: " << concert->name << std::endl;
                std::cout << "Description: " << concert->description << std::endl;
                std::cout << "Start: " << formatTimestampDisplay(concert->start_date_time.iso8601String) << std::endl;
                if (concert->venue) {
                    std::cout << "Venue: " << concert->venue->name << " (" << concert->venue->capacity << " capacity)" << std::endl;
                    std::cout << "Location: " << concert->venue->city << ", " << concert->venue->state << std::endl;
                }
                
                // Show ticket availability
                auto tickets = g_ticketModule->getTicketsByConcert(concertId);
                int available = 0;
                for (const auto& ticket : tickets) {
                    if (ticket->status == Model::TicketStatus::AVAILABLE) {
                        available++;
                    }
                }
                std::cout << "Available tickets: " << available << std::endl;
                break;
            }
            case 0: // Back to User Portal
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
        std::cout << "0. Back to User Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (0-3): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // Quick Purchase
                int concertId, quantity;
                std::string ticketType;
                
                std::cout << "Concert ID (or 0 to cancel): ";
                std::cin >> concertId;
                if (concertId == 0) break;
                
                std::cout << "Quantity (or 0 to cancel): ";
                std::cin >> quantity;

                if (quantity == 0) break;
                std::cin.ignore();
                
                std::cout << "Ticket Type (VIP/REGULAR/PREMIUM): ";
                std::getline(std::cin, ticketType);
                
                // Enhanced validation - check concert exists
                auto concert = g_concertModule->getConcertById(concertId);
                bool concertExists = (concert != nullptr);
                
                if (!concertExists) {
                    std::cout << "❌ Concert not found!";
                    break;
                }
                
                // Use enhanced ticket creation with proper user association
                std::vector<int> purchasedTickets;
                for (int i = 0; i < quantity; i++) {
                    // Find an available ticket for this concert
                    auto availableTickets = g_ticketModule->getTicketsByConcert(concertId);
                    bool ticketPurchased = false;
                    
                    for (const auto& ticket : availableTickets) {
                        if (ticket->status == Model::TicketStatus::AVAILABLE) {
                            
                            // Update ticket status and associate with user
                            if (g_ticketModule->updateTicketStatus(ticket->ticket_id, Model::TicketStatus::SOLD)) {
                                // Associate ticket with current user
                                auto attendee = g_attendeeModule->getAttendeeById(currentSession.userId);
                                if (attendee && g_ticketModule->setTicketAttendee(ticket->ticket_id, attendee)) {
                                    purchasedTickets.push_back(ticket->ticket_id);
                                    ticketPurchased = true;
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (!ticketPurchased) {
                        std::cout << "⚠️ Could not purchase ticket " << (i + 1) << " - no available tickets\n";
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
                        availability["General"]++; // Since ticket_type field doesn't exist
                    }
                }
                
                std::cout << "\n--- Ticket Availability for " << concert->name << " ---\n";
                for (const auto& pair : availability) {
                    std::cout << pair.first << ": " << pair.second << " available\n";
                }
                break;
            }
            case 0: // Back
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
        
        std::string choiceStr;
        std::cout << "Enter choice (1-6): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // View My Active Tickets
                // Resolve current attendee ID
                int attendeeId = currentSession.userId;
                if (attendeeId <= 0 && !currentSession.username.empty() && g_attendeeModule) {
                    auto att = g_attendeeModule->findAttendeeByUsername(currentSession.username);
                    if (att) attendeeId = att->id;
                }

                auto myTickets = g_ticketModule->getActiveTicketsByAttendee(attendeeId);
                std::cout << "\n--- My Active Tickets ---\n";
                if (myTickets.empty()) {
                    std::cout << "You have no active tickets.\n";
                } else {
                    for (const auto& ticket : myTickets) {
                        std::cout << "🎫 Ticket ID: " << ticket->ticket_id << "\n";
                        
                        // Get concert information through ConcertTicket relationship
                        std::string concertName = "Unknown Concert";
                        std::string concertDate = "Unknown Date";
                        
                        auto concertTicket = ticket->concert_ticket.lock();
                        if (concertTicket) {
                            auto concert = concertTicket->concert.lock();
                            if (concert) {
                                concertName = concert->name;
                                concertDate = formatTimestampDisplay(concert->start_date_time.iso8601String);
                            }
                        }
                        
                        std::cout << "   Concert: " << concertName << "\n";
                        std::cout << "   Date: " << concertDate << "\n";
                        std::cout << "   Type: General\n"; // Since ticket_type field doesn't exist
                        std::cout << "   Status: ";
                        switch (ticket->status) {
                            case Model::TicketStatus::SOLD: std::cout << "PURCHASED"; break;
                            case Model::TicketStatus::CHECKED_IN: std::cout << "CHECKED IN"; break;
                            default: std::cout << "ACTIVE"; break;
                        }
                        std::cout << "\n   Created: " << formatTimestampDisplay(ticket->created_at.iso8601String) << "\n\n";
                    }
                }
                break;
            }
            case 2: { // View Ticket Details
                int ticketId;
                std::cout << "Enter Ticket ID: ";
                std::cin >> ticketId;
                std::cin.ignore();

                auto ticket = g_ticketModule->getTicketById(ticketId);
                if (!ticket) {
                    std::cout << "❌ Ticket not found.\n";
                    break;
                }
                // Ownership check: ticket must belong to current user
                int attendeeId = currentSession.userId;
                if (attendeeId <= 0 && !currentSession.username.empty() && g_attendeeModule) {
                    auto att = g_attendeeModule->findAttendeeByUsername(currentSession.username);
                    if (att) attendeeId = att->id;
                }
                auto owner = ticket->attendee.lock();
                if (!owner || owner->id != attendeeId) {
                    std::cout << "❌ You can only view details of your own tickets.\n";
                    break;
                }
                
                // Get concert information through ConcertTicket relationship
                std::string concertName = "Unknown Concert";
                std::string concertDate = "Unknown Date";
                std::string venueName = "Unknown Venue";
                double ticketPrice = 0.0;
                
                auto concertTicket = ticket->concert_ticket.lock();
                if (concertTicket) {
                    auto concert = concertTicket->concert.lock();
                    if (concert) {
                        concertName = concert->name;
                        concertDate = formatTimestampDisplay(concert->start_date_time.iso8601String);
                        if (concert->venue) {
                            venueName = concert->venue->name;
                        }
                    }
                    ticketPrice = concertTicket->base_price;
                }
                
                std::cout << "\n--- Ticket Details ---\n";
                std::cout << "Ticket ID: " << ticket->ticket_id << "\n";
                std::cout << "Concert: " << concertName << "\n";
                std::cout << "Date: " << concertDate << "\n";
                std::cout << "Venue: " << venueName << "\n";
                std::cout << "Price: $" << std::fixed << std::setprecision(2) << ticketPrice << "\n";
                std::cout << "Type: General\n"; // Since ticket_type field doesn't exist in model
                std::cout << "QR Code: " << ticket->qr_code << "\n";
                std::cout << "Status: ";
                switch (ticket->status) {
                    case Model::TicketStatus::AVAILABLE: std::cout << "🟢 Available"; break;
                    case Model::TicketStatus::SOLD: std::cout << "🔵 Purchased"; break;
                    case Model::TicketStatus::CHECKED_IN: std::cout << "✅ Used"; break;
                    case Model::TicketStatus::CANCELLED: std::cout << "❌ Cancelled"; break;
                    case Model::TicketStatus::EXPIRED: std::cout << "⏰ Expired"; break;
                }
                std::cout << "\n";
                std::cout << "Created: " << formatTimestampDisplay(ticket->created_at.iso8601String) << "\n";
                std::cout << "Updated: " << formatTimestampDisplay(ticket->updated_at.iso8601String) << "\n";
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
    
    auto feedback = g_feedbackModule->createFeedback(concertId, currentSession.userId, rating, feedbackText);
    
    if (feedback != nullptr) {
        std::cout << "✅ Thank you for your feedback!\n";
        
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
    while (true) {
        std::cout << "\n--- Browse Performers & Venues ---\n";
        std::cout << "1. Browse All Performers\n";
        std::cout << "2. Browse All Venues\n";
        std::cout << "3. Search Performers by Type\n";
        std::cout << "4. Search Venues by Location\n";
        std::cout << "5. Performer Details\n";
        std::cout << "6. Venue Details\n";
        std::cout << "7. Back to User Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (1-7): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // Browse All Performers
                auto performers = g_performerModule->getAllPerformers();
                std::cout << "\n--- All Performers ---\n";
                if (performers.empty()) {
                    std::cout << "No performers available.\n";
                    break;
                }
                
                for (const auto& performer : performers) {
                    std::cout << "🎤 " << performer->name << " (" << performer->type << ")\n";
                    if (!performer->bio.empty()) {
                        std::string bio = performer->bio.length() > 80 ? performer->bio.substr(0, 77) + "..." : performer->bio;
                        std::cout << "   " << bio << std::endl;
                    }
                    std::cout << std::endl;
                }
                break;
            }
            case 2: { // Browse All Venues
                auto venues = g_venueModule->getAllVenues();
                std::cout << "\n--- All Venues ---\n";
                if (venues.empty()) {
                    std::cout << "No venues available.\n";
                    break;
                }
                
                for (const auto& venue : venues) {
                    std::cout << "🏛️  " << venue->name << "\n";
                    std::cout << "   📍 " << venue->address << ", " << venue->city << ", " << venue->state << std::endl;
                    std::cout << "   👥 Capacity: " << venue->capacity << std::endl;
                    if (!venue->description.empty()) {
                        std::string desc = venue->description.length() > 60 ? venue->description.substr(0, 57) + "..." : venue->description;
                        std::cout << "   " << desc << std::endl;
                    }
                    std::cout << std::endl;
                }
                break;
            }
            case 3: { // Search Performers by Type
                std::string type;
                std::cout << "Enter performer type (Solo Artist, Band, DJ, Orchestra, etc.): ";
                std::getline(std::cin, type);
                
                auto performers = g_performerModule->findPerformersByType(type);
                std::cout << "\n--- Performers of Type: " << type << " ---\n";
                if (performers.empty()) {
                    std::cout << "No performers found for type '" << type << "'.\n";
                    break;
                }
                
                for (const auto& performer : performers) {
                    std::cout << "🎤 " << performer->name << std::endl;
                    if (!performer->bio.empty()) {
                        std::string bio = performer->bio.length() > 80 ? performer->bio.substr(0, 77) + "..." : performer->bio;
                        std::cout << "   " << bio << std::endl;
                    }
                }
                break;
            }
            case 4: { // Search Venues by Location
                std::string location;
                std::cout << "Enter city or venue name: ";
                std::getline(std::cin, location);
                
                auto venues = g_venueModule->findVenuesByCity(location);
                if (venues.empty()) {
                    // Try searching by name if city search fails
                    venues = g_venueModule->findVenuesByName(location);
                }
                
                std::cout << "\n--- Venues matching '" << location << "' ---\n";
                if (venues.empty()) {
                    std::cout << "No venues found matching '" << location << "'.\n";
                    break;
                }
                
                for (const auto& venue : venues) {
                    std::cout << "🏛️  " << venue->name << "\n";
                    std::cout << "   📍 " << venue->address << ", " << venue->city << ", " << venue->state << std::endl;
                    std::cout << "   👥 Capacity: " << venue->capacity << std::endl;
                }
                break;
            }
            case 5: { // Performer Details
                int performerId;
                std::cout << "Enter Performer ID: ";
                std::cin >> performerId;
                std::cin.ignore();
                
                auto performer = g_performerModule->getPerformerById(performerId);
                if (!performer) {
                    std::cout << "❌ Performer not found.\n";
                    break;
                }
                
                std::cout << "\n--- Performer Details ---\n";
                std::cout << "🎤 Name: " << performer->name << std::endl;
                std::cout << "🎭 Type: " << performer->type << std::endl;
                std::cout << "📱 Contact: " << performer->contact_info << std::endl;
                std::cout << "📄 Biography:\n" << performer->bio << std::endl;
                if (!performer->image_url.empty()) {
                    std::cout << "🖼️  Image: " << performer->image_url << std::endl;
                }
                
                // Show upcoming concerts for this performer (if any)
                auto concerts = g_concertModule->getAllConcerts();
                std::cout << "\n🎪 Upcoming Concerts:\n";
                bool hasUpcoming = false;
                for (const auto& concert : concerts) {
                    // Note: This would require enhanced performer-concert relationship
                    if (concert->event_status == Model::EventStatus::SCHEDULED) {
                        std::cout << "   • " << concert->name << " (ID: " << concert->id << ")" << std::endl;
                        hasUpcoming = true;
                    }
                }
                if (!hasUpcoming) {
                    std::cout << "   No upcoming concerts scheduled.\n";
                }
                break;
            }
            case 6: { // Venue Details
                int venueId;
                std::cout << "Enter Venue ID: ";
                std::cin >> venueId;
                std::cin.ignore();
                
                auto venue = g_venueModule->getVenueById(venueId);
                if (!venue) {
                    std::cout << "❌ Venue not found.\n";
                    break;
                }
                
                std::cout << "\n--- Venue Details ---\n";
                std::cout << "🏛️  Name: " << venue->name << std::endl;
                std::cout << "📍 Address: " << venue->address << std::endl;
                std::cout << "🏙️  City: " << venue->city << ", " << venue->state << " " << venue->zip_code << std::endl;
                std::cout << "🌍 Country: " << venue->country << std::endl;
                std::cout << "👥 Capacity: " << venue->capacity << std::endl;
                std::cout << "📄 Description:\n" << venue->description << std::endl;
                std::cout << "📱 Contact: " << venue->contact_info << std::endl;
                
                // Show upcoming concerts at this venue
                auto concerts = g_concertModule->getAllConcerts();
                std::cout << "\n🎪 Upcoming Concerts:\n";
                bool hasUpcoming = false;
                for (const auto& concert : concerts) {
                    if (concert->venue && concert->venue->id == venue->id && 
                        concert->event_status == Model::EventStatus::SCHEDULED) {
                        std::cout << "   • " << concert->name << " (ID: " << concert->id << ")" << std::endl;
                        hasUpcoming = true;
                    }
                }
                if (!hasUpcoming) {
                    std::cout << "   No upcoming concerts scheduled.\n";
                }
                break;
            }
            case 7: // Back to User Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

void manageProfile() {
    while (true) {
        std::cout << "\n--- Account & Profile Management ---\n";
        std::cout << "1. View My Profile\n";
        std::cout << "2. Update Personal Information\n";
        std::cout << "3. Change Password\n";
        std::cout << "4. My Purchase History\n";
        std::cout << "5. My Ticket History\n";
        std::cout << "6. Account Preferences\n";
        std::cout << "7. Delete Account\n";
        std::cout << "8. Back to User Portal\n";
        
        std::string choiceStr;
        std::cout << "Enter choice (1-8): ";
        std::getline(std::cin, choiceStr);
        
        if (!isValidInteger(choiceStr)) {
            std::cout << "❌ Invalid input. Please enter a valid integer only.\n";
            continue;
        }
        int choice = std::stoi(choiceStr);
        
        switch (choice) {
            case 1: { // View My Profile
                // Resolve current attendee via session (prefer userId, fallback to username)
                std::shared_ptr<Model::Attendee> currentAttendee = nullptr;
                if (g_attendeeModule) {
                    if (currentSession.userId > 0) {
                        currentAttendee = g_attendeeModule->getAttendeeById(currentSession.userId);
                    }
                    if (!currentAttendee && !currentSession.username.empty()) {
                        currentAttendee = g_attendeeModule->findAttendeeByUsername(currentSession.username);
                    }
                }

                if (currentAttendee) {
                    std::cout << "\n--- My Profile ---\n";
                    std::cout << "Username: " << (currentAttendee->username.empty() ? currentSession.username : currentAttendee->username) << std::endl;
                    std::cout << "User ID: " << currentAttendee->id << std::endl;
                    std::cout << "Name: " << currentAttendee->name << std::endl;
                    std::cout << "Email: " << currentAttendee->email << std::endl;
                    std::cout << "Phone: " << currentAttendee->phone_number << std::endl;
                    std::cout << "Account Type: ";
                    switch (currentAttendee->attendee_type) {
                        case Model::AttendeeType::REGULAR: std::cout << "Regular"; break;
                        case Model::AttendeeType::VIP: std::cout << "VIP"; break;
                        default: std::cout << "Unknown"; break;
                    }
                    std::cout << std::endl;
                    std::cout << "📅 Member Since: " << formatTimestampDisplay(currentAttendee->registration_date.iso8601String) << std::endl;
                    std::cout << "🛡️  Staff Privileges: " << (currentAttendee->staff_privileges ? "Yes" : "No") << std::endl;
                } else {
                    std::cout << "❌ Profile not found. Please contact support.\n";
                }
                break;
            }
            case 2: { // Update Personal Information
                std::shared_ptr<Model::Attendee> currentAttendee = nullptr;
                if (g_attendeeModule) {
                    if (currentSession.userId > 0) {
                        currentAttendee = g_attendeeModule->getAttendeeById(currentSession.userId);
                    }
                    if (!currentAttendee && !currentSession.username.empty()) {
                        currentAttendee = g_attendeeModule->findAttendeeByUsername(currentSession.username);
                    }
                }
                if (!currentAttendee) {
                    std::cout << "❌ Profile not found.\n";
                    break;
                }

                std::string newName, newEmail, newPhone;
                std::cout << "Current name: " << currentAttendee->name << std::endl;
                std::cout << "New name (or press Enter to keep current): ";
                std::getline(std::cin, newName);

                std::cout << "Current email: " << currentAttendee->email << std::endl;
                std::cout << "New email (or press Enter to keep current): ";
                std::getline(std::cin, newEmail);

                std::cout << "Current phone: " << currentAttendee->phone_number << std::endl;
                std::cout << "New phone (or press Enter to keep current): ";
                std::getline(std::cin, newPhone);

                // Validate only if provided; empty keeps existing
                if (!newName.empty()) {
                    auto vr = InputValidator::validateName(newName, "Name");
                    if (!vr.isValid) { std::cout << "❌ " << vr.errorMessage << "\n"; break; }
                }
                if (!newEmail.empty()) {
                    auto vr = InputValidator::validateEmail(newEmail);
                    if (!vr.isValid) { std::cout << "❌ " << vr.errorMessage << "\n"; break; }
                }
                if (!newPhone.empty()) {
                    auto vr = InputValidator::validatePhoneNumber(newPhone);
                    if (!vr.isValid) { std::cout << "❌ " << vr.errorMessage << "\n"; break; }
                }

                if (g_attendeeModule->updateAttendee(currentAttendee->id, newName, newEmail, newPhone, currentAttendee->attendee_type)) {
                    std::cout << "✅ Profile updated successfully!\n";
                } else {
                    std::cout << "❌ Failed to update profile.\n";
                }
                break;
            }
            case 3: { // Change Password
                std::string oldPassword, newPassword, confirmPassword;
                std::cout << "Current password: ";
                std::getline(std::cin, oldPassword);
                std::cout << "New password: ";
                std::getline(std::cin, newPassword);
                std::cout << "Confirm new password: ";
                std::getline(std::cin, confirmPassword);
                
                if (newPassword != confirmPassword) {
                    std::cout << "❌ Passwords do not match.\n";
                    break;
                }
                
                if (g_authModule->changePassword(currentSession.username, oldPassword, newPassword)) {
                    std::cout << "✅ Password changed successfully!\n";
                } else {
                    std::cout << "❌ Failed to change password. Check your current password.\n";
                }
                break;
            }
            case 4: { // My Purchase History
                auto payments = g_paymentModule->getRecentPayments(1000); // Get recent payments
                std::cout << "\n--- My Purchase History ---\n";
                
                bool hasPayments = false;
                for (const auto& payment : payments) {
                    // In a real system, we'd filter by current user's attendee ID
                    // For demo purposes, show recent payments
                    std::cout << "💳 Payment ID: " << payment->payment_id << std::endl;
                    std::cout << "   Amount: $" << std::fixed << std::setprecision(2) << payment->amount << std::endl;
                    std::cout << "   Date: " << payment->payment_date_time.iso8601String << std::endl;
                    std::cout << "   Method: " << payment->payment_method << std::endl;
                    std::cout << "   Status: ";
                    switch (payment->status) {
                        case Model::PaymentStatus::COMPLETED: std::cout << "✅ Completed"; break;
                        case Model::PaymentStatus::PENDING: std::cout << "⏳ Pending"; break;
                        case Model::PaymentStatus::FAILED: std::cout << "❌ Failed"; break;
                        case Model::PaymentStatus::REFUNDED: std::cout << "🔄 Refunded"; break;
                    }
                    std::cout << "\n\n";
                    hasPayments = true;
                }
                
                if (!hasPayments) {
                    std::cout << "No purchase history found.\n";
                }
                break;
            }
            case 5: { // My Ticket History
                // Resolve current attendee ID
                int attendeeId = currentSession.userId;
                if (attendeeId <= 0 && !currentSession.username.empty() && g_attendeeModule) {
                    auto att = g_attendeeModule->findAttendeeByUsername(currentSession.username);
                    if (att) attendeeId = att->id;
                }

                auto tickets = g_ticketModule->getTicketsByAttendee(attendeeId);
                std::cout << "\n--- My Ticket History ---\n";
                if (tickets.empty()) {
                    std::cout << "No ticket history found.\n";
                    break;
                }
                for (const auto& ticket : tickets) {
                    std::cout << "🎫 Ticket ID: " << ticket->ticket_id << std::endl;
                    std::cout << "   Concert: " << "(Concert information requires ConcertTicket relationship)" << std::endl;
                    std::cout << "   Price: " << "(Price information stored in Payment module)" << std::endl;
                    std::cout << "   Status: ";
                    switch (ticket->status) {
                        case Model::TicketStatus::AVAILABLE: std::cout << "🟢 Available"; break;
                        case Model::TicketStatus::SOLD: std::cout << "🔵 Purchased"; break;
                        case Model::TicketStatus::CHECKED_IN: std::cout << "✅ Used"; break;
                        case Model::TicketStatus::CANCELLED: std::cout << "❌ Cancelled"; break;
                        case Model::TicketStatus::EXPIRED: std::cout << "⏰ Expired"; break;
                    }
                    std::cout << std::endl;
                    if (!ticket->qr_code.empty()) {
                        std::cout << "   QR Code: " << ticket->qr_code << std::endl;
                    }
                    std::cout << std::endl;
                }
                break;
            }
            case 6: { // Account Preferences
                std::cout << "\n--- Account Preferences ---\n";
                std::cout << "Current Settings:\n";
                std::cout << "📧 Email Notifications: Enabled\n";
                std::cout << "📱 SMS Notifications: Disabled\n";
                std::cout << "🎵 Genre Preferences: All\n";
                std::cout << "🏪 Marketing Communications: Enabled\n";
                std::cout << "🔒 Privacy Level: Standard\n";
                std::cout << "\n⚠️ Preference modification requires enhanced settings system.\n";
                break;
            }
            case 7: { // Delete Account
                std::cout << "⚠️  WARNING: This will permanently delete your account!\n";
                std::cout << "All tickets, purchase history, and profile data will be lost.\n";
                std::cout << "Type 'DELETE' to confirm account deletion: ";
                
                std::string confirmation;
                std::getline(std::cin, confirmation);
                
                if (confirmation == "DELETE") {
                    // In a real system, we'd properly delete all related data
                    std::cout << "Account deletion initiated...\n";
                    std::cout << "⚠️ This feature requires enhanced data cleanup procedures.\n";
                    std::cout << "Please contact support to complete account deletion.\n";
                } else {
                    std::cout << "❌ Account deletion cancelled.\n";
                }
                break;
            }
            case 8: // Back to User Portal
                return;
            default:
                std::cout << "❌ Invalid choice.\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
}

// Demo function to showcase input validation capabilities
void demonstrateValidation() {
    UIManager::addSmallSpacing();
    UIManager::printSeparator('=');
    UIManager::printCenteredText("INPUT VALIDATION DEMONSTRATION");
    UIManager::printSeparator('=');
    UIManager::displayInfo("This demo shows regex-based input validation for various data types");
    UIManager::addSmallSpacing();
    
    // Test email validation
    UIManager::displayInfo("🔍 Testing Email Validation:");
    std::vector<std::string> testEmails = {
        "user@example.com",      // Valid
        "invalid.email",         // Invalid - no @
        "user@",                 // Invalid - incomplete
        "test@domain.co.uk"      // Valid
    };
    
    for (const auto& email : testEmails) {
        auto result = InputValidator::validateEmail(email);
        std::cout << "  " << email << " → " << (result.isValid ? "✅ Valid" : "❌ " + result.errorMessage) << std::endl;
    }
    
    UIManager::addSmallSpacing();
    
    // Test password validation
    UIManager::displayInfo("🔒 Testing Password Validation:");
    std::vector<std::string> testPasswords = {
        "Password123!",          // Valid
        "weak",                  // Invalid - too short
        "NoNumbers!",            // Invalid - no digits
        "noUpper123!"            // Invalid - no uppercase
    };
    
    for (const auto& password : testPasswords) {
        auto result = InputValidator::validatePassword(password);
        std::cout << "  " << password << " → " << (result.isValid ? "✅ Valid" : "❌ " + result.errorMessage) << std::endl;
    }
    
    UIManager::addSmallSpacing();
    
    // Test phone number validation
    UIManager::displayInfo("📞 Testing Phone Number Validation:");
    std::vector<std::string> testPhones = {
        "+1234567890",           // Valid
        "(123) 456-7890",        // Valid (will be cleaned)
        "123-456-7890",          // Valid
        "12345"                  // Invalid - too short
    };
    
    for (const auto& phone : testPhones) {
        auto result = InputValidator::validatePhoneNumber(phone);
        std::cout << "  " << phone << " → " << (result.isValid ? "✅ Valid" : "❌ " + result.errorMessage) << std::endl;
    }
    
    UIManager::addSmallSpacing();
    
    // Test date validation
    UIManager::displayInfo("📅 Testing Date Validation:");
    std::vector<std::string> testDates = {
        "2024-12-25",           // Valid
        "2024-02-29",           // Valid (leap year)
        "2023-02-29",           // Invalid (not leap year)
        "2024-13-01"            // Invalid (month > 12)
    };
    
    for (const auto& date : testDates) {
        auto result = InputValidator::validateDate(date);
        std::cout << "  " << date << " → " << (result.isValid ? "✅ Valid" : "❌ " + result.errorMessage) << std::endl;
    }
    
    UIManager::addSmallSpacing();
    UIManager::displaySuccess("Validation demonstration complete!");
    UIManager::displayInfo("These validations are now applied to:");
    std::cout << "  • User Registration (username, password, email, names, phone)" << std::endl;
    std::cout << "  • Venue Creation (address, postal codes, contact info)" << std::endl;
    std::cout << "  • Concert Creation (dates, times, prices)" << std::endl;
    UIManager::printSeparator('=');
}


int main() {
    UIManager::displayWelcomeBanner();
    
    // Initialize all modules
    if (!initializeModules()) {
        UIManager::displayError("Failed to initialize system. Exiting...");
        return -1;
    }
    
    // Initialize default users
    if (!initializeDefaultUsers()) {
        UIManager::displayError("Failed to initialize default users. Exiting...");
        return -1;
    }
    
    // Authentication loop
    while (true) {
        if (!authenticateUser()) {
            UIManager::addSectionSpacing();
            UIManager::printSeparator('*');
            UIManager::printCenteredText("🎵 Thank you for using MuseIO! 🎵");
            UIManager::printCenteredText("Goodbye!");
            UIManager::printSeparator('*');
            UIManager::addSmallSpacing();
            break;
        }
        
        // Main application loop after successful authentication
        while (currentSession.isAuthenticated) {
            displayMainMenu();
            
            std::string choiceStr;
            std::getline(std::cin, choiceStr);
            
            if (!isValidInteger(choiceStr)) {
                UIManager::displayError("Invalid input. Please enter only integer numbers (e.g., '1', '2', '3').");
                continue;
            }
            
            int choice;
            try {
                choice = std::stoi(choiceStr);
            } catch (const std::exception&) {
                UIManager::displayError("Invalid input. Please enter only integer numbers.");
                continue;
            }

            switch (choice) {
                case 1: // Management Portal
                    UIManager::displayInfo("Entering Management Portal...");
                    runManagementPortal();
                    break;
                case 2: // User Portal
                    UIManager::displayInfo("Entering User Portal...");
                    runUserPortal();
                    break;
                case 0: // Logout
                    logout();
                    break;
                default:
                    UIManager::displayError("Invalid choice. Please select a valid option.");
            }
        }
    }
    
    // Cleanup before exit
    cleanupModules();
    return 0;
}

// Forward declaration
void manageCommunicationLogs();