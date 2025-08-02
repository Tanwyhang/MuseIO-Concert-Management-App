#include <iostream>
#include <string>
#include <limits>
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
#include "include/sponsorModule.hpp"
#include "include/attendeeModule.hpp"
#include "include/uiModule.hpp"

// Forward declarations for portal handlers
void runManagementPortal();
void runUserPortal();
void displayMainMenu();

void displayMainMenu() {
    // Display the main menu banner using UIManager strings
    std::cout << UIManager::MAIN_MENU;
}

void runManagementPortal() {
    bool logout = false;

    while (!logout) {
        
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
                break; // Handle invalid choice
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