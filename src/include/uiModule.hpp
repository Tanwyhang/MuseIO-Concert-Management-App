#pragma once
#include <string>
#include <iostream>
#include <iomanip>

namespace UIManager {
    // Spacing constants
    constexpr int SECTION_SPACING = 10;
    constexpr int SMALL_SPACING = 1;
    constexpr int BANNER_WIDTH = 60;

    // Application banner
    const std::string APP_BANNER = R"(
    ██████   ██████                             █████    ███████   
    ░░██████ ██████                             ░░███   ███░░░░░███ 
     ░███░█████░███  █████ ████  █████   ██████  ░███  ███     ░░███
     ░███░░███ ░███ ░░███ ░███  ███░░   ███░░███ ░███ ░███      ░███
     ░███ ░░░  ░███  ░███ ░███ ░░█████ ░███████  ░███ ░███      ░███
     ░███      ░███  ░███ ░███  ░░░░███░███░░░   ░███ ░░███     ███ 
     █████     █████ ░░████████ ██████ ░░██████  █████ ░░░███████░  
    ░░░░░     ░░░░░   ░░░░░░░░ ░░░░░░   ░░░░░░  ░░░░░    ░░░░░░░    
    )";

    // Menu content with proper spacing
    const std::string AUTH_MENU = R"(
┌─────────────────────────────┐
│          AUTH MENU          │
├─────────────────────────────┤
│ 1. Login                    │
│ 2. Register New Account     │
│ 3. View All Accounts        │
│ 0. Exit                     │
└─────────────────────────────┘
)";

    const std::string MAIN_MENU_OPTIONS = R"(
┌─────────────────────────────┐
│         MAIN MENU           │
├─────────────────────────────┤
│ 1. Management Portal        │
│    (Admin/Staff)            │
│ 2. User Portal              │
│    (General Access)         │
│ 0. Logout                   │
└─────────────────────────────┘
)";

    const std::string MANAGEMENT_MENU_OPTIONS = R"(
┌─────────────────────────────┐
│    MANAGEMENT MENU          │
├─────────────────────────────┤
│ 1. Concert Management       │
│ 2. Venue Management         │
│ 3. Crew Management          │
│ 4. Ticket Management        │
│ 5. Payment Management       │
│ 6. Feedback Management      │
│ 7. Reports & Analytics      │
│ 8. System Administration    │
│ 9. Switch User Portal       │
│ 10. Input Validation Demo   │
│ 0. Back to Main Menu        │
└─────────────────────────────┘
)";

    const std::string USER_MENU_OPTIONS = R"(
┌─────────────────────────────┐
│       USER MENU             │
├─────────────────────────────┤
│ 1. Browse Concerts          │
│ 2. Purchase Tickets         │
│ 3. View My Tickets          │
│ 4. Submit Feedback          │
│ 5. Performers & Venues      │
│ 6. Account & Profile        │
│ 7. Switch to Management     │
│ 0. Back to Main Menu        │
└─────────────────────────────┘
)";
    
    // Menu aliases for backward compatibility
    const std::string USER_MENU = USER_MENU_OPTIONS;
    const std::string MANAGEMENT_MENU = MANAGEMENT_MENU_OPTIONS;
    
    // Demo credentials display string
    const std::string DEMO_CREDENTIALS = R"(
═══════════════════════════════════════════════════════════════════════════════
                              DEMO LOGIN CREDENTIALS
═══════════════════════════════════════════════════════════════════════════════

┌─────────────┬─────────────┐
│ Username    │ Password    │
├─────────────┼─────────────┤
│ admin       │ admin123    │
│ manager1    │ manager123  │
│ staff1      │ staff123    │
│ user1       │ user123     │
│ vip1        │ vip123      │
└─────────────┴─────────────┘

Note: Use these credentials to test different user roles and permissions.
)";
    
    // Spacing utilities
    inline void addSectionSpacing() {
        for (int i = 0; i < SECTION_SPACING; ++i) {
            std::cout << "\n";
        }
    }
    
    inline void addSmallSpacing() {
        for (int i = 0; i < SMALL_SPACING; ++i) {
            std::cout << "\n";
        }
    }
    
    inline std::string getSectionSpacing() {
        return std::string(SECTION_SPACING, '\n');
    }
    
    inline std::string getSmallSpacing() {
        return std::string(SMALL_SPACING, '\n');
    }
    
    // Decorative elements
    inline void printSeparator(char ch = '=', int width = BANNER_WIDTH) {
        std::cout << std::string(width, ch) << std::endl;
    }
    
    inline std::string getSeparator(char ch = '=', int width = BANNER_WIDTH) {
        return std::string(width, ch);
    }
    
    inline void printCenteredText(const std::string& text, int width = BANNER_WIDTH) {
        int padding = (width - text.length()) / 2;
        if (padding > 0) {
            std::cout << std::string(padding, ' ') << text << std::endl;
        } else {
            std::cout << text << std::endl;
        }
    }
    
    // Enhanced banner functions
    inline void displayWelcomeBanner() {
        addSectionSpacing();
        std::cout << APP_BANNER << std::endl;
        printSeparator('*');
        printCenteredText("🎵 WELCOME TO MUSEIO CONCERT MANAGEMENT SYSTEM 🎵");
        printSeparator('*');
        addSmallSpacing();
    }
    inline void displayLoginBanner() {
        addSectionSpacing();
        printSeparator();
        printCenteredText("AUTHENTICATION PORTAL");
        printSeparator();
        addSmallSpacing();
    }
    
    inline void displayMainMenuBanner() {
        addSectionSpacing();
        printSeparator();
        printCenteredText("MAIN MENU");
        printSeparator();
        addSmallSpacing();
    }
    
    inline void displayManagementPortalBanner() {
        addSectionSpacing();
        printSeparator();
        printCenteredText("MANAGEMENT PORTAL DASHBOARD");
        printSeparator();
        addSmallSpacing();
    }
    
    inline void displayUserPortalBanner() {
        addSectionSpacing();
        printSeparator();
        printCenteredText("USER PORTAL DASHBOARD");
        printSeparator();
        addSmallSpacing();
    }
    
    // Status and feedback functions
    inline void displaySuccess(const std::string& message) {
        addSmallSpacing();
        std::cout << "✅ " << message << std::endl;
        addSmallSpacing();
    }
    
    inline void displayError(const std::string& message) {
        addSmallSpacing();
        std::cout << "❌ " << message << std::endl;
        addSmallSpacing();
    }
    
    inline void displayWarning(const std::string& message) {
        addSmallSpacing();
        std::cout << "⚠️  " << message << std::endl;
        addSmallSpacing();
    }
    
    inline void displayInfo(const std::string& message) {
        addSmallSpacing();
        std::cout << "ℹ️  " << message << std::endl;
        addSmallSpacing();
    }
    
    // Input prompts with consistent styling
    inline void displayPrompt(const std::string& prompt) {
        std::cout << "➤ " << prompt << ": ";
    }
    
    inline void displayChoicePrompt(const std::string& range = "") {
        std::string prompt = "Enter your choice";
        if (!range.empty()) {
            prompt += " (" + range + ")";
        }
        displayPrompt(prompt);
    }
    
    // Continue prompt
    inline void waitForContinue() {
        addSmallSpacing();
        std::cout << "Press Enter to continue...";
        std::cin.get();
        addSmallSpacing();
    }
    
    // Helper function to get current user role (to be implemented in main.cpp)
    std::string getCurrentUserRole();
    
    // Menu content with proper spacing
    // Helper function to get current user role (to be implemented in main.cpp)
    std::string getCurrentUserRole();
    
    // Default user credentials table
    inline void displayDefaultCredentials() {
        addSmallSpacing();
        printSeparator('-');
        printCenteredText("DEFAULT LOGIN CREDENTIALS");
        printSeparator('-');
        std::cout << "┌─────────────┬────────────┐\n";
        std::cout << "│ Username    │ Password   │\n";
        std::cout << "├─────────────┼────────────┤\n";
        std::cout << "│ admin       │ admin123   │\n";
        std::cout << "│ manager1    │ manager123 │\n";
        std::cout << "│ staff1      │ staff123   │\n";
        std::cout << "│ user1       │ user123    │\n";
        std::cout << "│ vip1        │ vip123     │\n";
        std::cout << "└─────────────┴────────────┘\n";
        addSmallSpacing();
    }
    
    // Enhanced menu display functions
    inline void displayAuthMenu() {
        displayLoginBanner();
        std::cout << AUTH_MENU << std::endl;
        addSmallSpacing();
        displayChoicePrompt("0-3");
    }
    
    inline void displayMainMenu() {
        displayMainMenuBanner();
        std::cout << MAIN_MENU_OPTIONS << std::endl;
        addSmallSpacing();
        displayChoicePrompt("0-2");
    }
    
    inline void displayManagementMenu() {
        displayManagementPortalBanner();
        std::cout << MANAGEMENT_MENU_OPTIONS << std::endl;
        addSmallSpacing();
        displayChoicePrompt("0-10");
    }
    
    inline void displayUserMenu() {
        displayUserPortalBanner();
        std::cout << USER_MENU_OPTIONS << std::endl;
        addSmallSpacing();
        displayChoicePrompt("0-7");
    }
}