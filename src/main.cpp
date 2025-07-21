#include <iostream>
#include <string>
#include <limits>
#include "models.h"
#include "authModule.h"
#include "concertModule.h"
#include "ticketModule.h"
#include "venueModule.h"
#include "crewModule.h"
#include "paymentModule.h"
#include "commModule.h"
#include "feedbackModule.h"
#include "performerModule.h"
#include "reportModule.h"
#include "sponsorModule.h"
#include "attendeeModule.h"
#include "uiModule.h"

// Forward declarations for portal handlers
void runManagementPortal();
void runUserPortal();
void displayMainMenu();

void displayMainMenu() {
    //UIManager::displayBanner("CONCERT MANAGEMENT SYSTEM");
    // print using ui manager 
    // "1. Management Portal (Staff Access)\n"
    // "2. User Portal (Ticket Purchase & Feedback)\n"
    // "3. Exit Program\n"
    // "=============================================\n";
}

void runManagementPortal() {
    bool logout = false;

    while (!logout) {
        //UIManager::clearScreen();
        //UIManager::displayBanner("MANAGEMENT PORTAL");

        // "1. Concert Management\n"
        // "2. Venue Configuration\n"
        // "3. Crew Management\n"
        // "4. Ticket Configuration\n"
        // "5. Payment Monitoring\n"
        // "6. Analytics & Reports\n"
        // "7. Communication Tools\n"
        // "8. System Settings\n"
        // "9. Switch to User Portal\n"
        // "10. Logout\n"

        int choice;
        std::cout << "Enter your choice (1-10): ";
        std::cin >> choice;

        switch (choice) {
            case 1:
            // Concert management logic
            break;
            case 2:
            // Venue configuration logic
            break;
            case 3:
            // Crew management logic
            break;
            case 4:
            // Ticket configuration logic
            break;
            case 5:
            // Payment monitoring logic
            break;
            case 6:
            // Analytics & reports logic
            break;
            case 7:
            // Communication tools logic
            break;
            case 8:
            // System settings logic
            break;
            case 9:
            // Switch to user portal logic
            break;
            case 10:
            // Logout logic
            break;
            default:
            // Invalid choice logic
            break;
        }
    }
}


void runUserPortal() {

    //UserSession session = AuthManager::authenticateUser();
    // check if session is valid, if not return, if yes continue

    bool logout = false;

    while (!logout) {
        //UIManager::clearScreen();
        //UIManager::displayBanner("USER PORTAL");
        
        // cout << "1. Browse Concerts\n"
        //      << "2. Buy Tickets\n"
        //      << "3. View My Tickets\n"
        //      << "4. Submit Feedback\n"
        //      << "5. View Performer Info\n"
        //      << "6. Switch to Management Portal\n"
        //      << "7. Logout\n"
        //      << "=============================================\n";

        int choice;
        std::cout << "Enter your choice (1-7): ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                //ConcertManager::showUserConcerts();
                break;
            case 2:
                //TicketManager::startPurchaseFlow(session.userId);
                break;
            case 3:
                //TicketManager::showUserTickets(session.userId);
                break;
            case 4:
                //FeedbackManager::submitFeedback(session.userId);
                break;
            case 5:
                //PerformerManager::showBrowseMenu();
                break;
            case 6:
                // Switch to management portal logic
                break;
            case 7:
                logout = true;
                break;
            default:
                //UIManager::showError("Invalid choice");
        }
    }
}


int main() {
    while (true) {
        //UIManager::clearScreen();
        displayMainMenu();
        
        int choice;
        std::cout << "Enter your choice (1-3): ";
        
        // Robust input validation
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            //UIManager::showError("Invalid input. Please enter a number.");
            continue;
        }

        switch (choice) {
            case 1: // Run management portal
                break;
            case 2: // Run user portal
                break;
            case 3: // Exit program
                return 0;
            default:
                break; // UIManager::showError("Invalid choice. Please select a valid option.");
        }

        //UIManager::pressToContinue();
    }
}