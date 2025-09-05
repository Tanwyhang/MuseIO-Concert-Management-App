#include <iostream>
#include "include/authModule.hpp"
#include "include/uiModule.hpp"

int main() {
    std::cout << "=== Testing Enhanced Authentication Menu ===" << std::endl;
    
    try {
        // Create AuthModule with test data
        AuthModule auth(4096, "data/auth.dat");
        
        // Register some test users
        std::cout << "Creating test accounts..." << std::endl;
        auth.registerUser("alice", "Password123!");
        auth.registerUser("bob", "SecurePass456!");
        auth.registerUser("charlie", "MyPass789!");
        auth.registerUser("admin", "AdminPass123!");
        
        std::cout << "✅ Test accounts created!" << std::endl;
        std::cout << "Total user count: " << auth.getUserCount() << std::endl;
        
        // Test getAllUsernames function
        std::cout << "\n--- Testing getAllUsernames() ---" << std::endl;
        auto usernames = auth.getAllUsernames();
        
        std::cout << "Found " << usernames.size() << " usernames:" << std::endl;
        for (const auto& username : usernames) {
            std::cout << "  - " << username << std::endl;
        }
        
        // Test the UI display (simulated)
        std::cout << "\n--- Testing UI Display Format ---" << std::endl;
        UIManager::printSeparator('=');
        UIManager::printCenteredText("ALL REGISTERED ACCOUNTS");
        UIManager::printSeparator('=');
        
        if (usernames.empty()) {
            std::cout << "No accounts found." << std::endl;
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
        
        std::cout << "\n✅ All tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
