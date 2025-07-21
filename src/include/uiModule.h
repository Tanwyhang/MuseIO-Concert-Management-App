#pragma once
#include <string>

namespace UIManager {
    // Existing menu banners...
    const std::string MAIN_MENU = R"(
=============================================
    CONCERT MANAGEMENT SYSTEM - MAIN MENU
=============================================
1. Management Portal (Staff Access)
2. User Portal (Ticket Purchase & Feedback)
3. Exit Program
=============================================
)";

    const std::string MANAGEMENT_MENU = R"(
=============================================
       MANAGEMENT PORTAL DASHBOARD
=============================================
1. Concert Management
2. Ticket Configuration
3. Payment Monitoring
4. Analytics & Reports
5. User Management
6. System Settings
7. Switch to User Portal
8. Logout
=============================================
)";

    const std::string USER_MENU = R"(
=============================================
         USER PORTAL DASHBOARD
=============================================
1. Browse Concerts
2. Buy Tickets
3. My Tickets
4. Submit Feedback
5. Switch to Management Portal
6. Logout
=============================================
)";

    // Menu choice string constants
    inline const std::string MAIN_MENU_CHOICES =
        "1. Management Portal (Staff Access)\n"
        "2. User Portal (Ticket Purchase & Feedback)\n"
        "3. Exit Program\n"
        "=============================================\n";

    inline const std::string MANAGEMENT_PORTAL_CHOICES =
        "1. Concert Management\n"
        "2. Venue Configuration\n"
        "3. Crew Management\n"
        "4. Ticket Configuration\n"
        "5. Payment Monitoring\n"
        "6. Analytics & Reports\n"
        "7. Communication Tools\n"
        "8. System Settings\n"
        "9. Switch to User Portal\n"
        "10. Logout\n"
        "=============================================\n";

    inline const std::string USER_PORTAL_CHOICES =
        "1. Browse Concerts\n"
        "2. Buy Tickets\n"
        "3. View My Tickets\n"
        "4. Submit Feedback\n"
        "5. View Performer Info\n"
        "6. Switch to Management Portal\n"
        "7. Logout\n"
        "=============================================\n";
}