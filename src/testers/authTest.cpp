#include <iostream>
#include <string>
#include <memory>
#include "../include/models.hpp"
#include "../include/authModule.hpp"

// Utility function to display a separator line
void displaySeparator(char symbol = '-', int length = 50) {
    std::cout << std::string(length, symbol) << std::endl;
}

// Utility function to display a section header
void displayHeader(const std::string& title) {
    displaySeparator('=');
    std::cout << title << std::endl;
    displaySeparator('=');
}

// Utility function to map user type to string
std::string getUserTypeString(int userType) {
    switch (userType) {
        case 0: return "REGULAR";
        case 1: return "ADMIN";
        case 2: return "STAFF";
        default: return "UNKNOWN";
    }
}

// Test user registration
void testUserRegistration(AuthModule& auth) {
    displayHeader("USER REGISTRATION TEST");
    
    struct TestUser {
        std::string username;
        std::string password;
        int userType;
    };
    
    TestUser testUsers[] = {
        {"admin", "admin123", 1},
        {"john.doe", "password123", 0},
        {"jane.smith", "securepass", 0},
        {"staff.member", "staff456", 2}
    };
    
    for (const auto& user : testUsers) {
        std::cout << "Registering user: " << user.username << " (Type: " << getUserTypeString(user.userType) << ")" << std::endl;
        bool result = auth.registerUser(user.username, user.password, user.userType);
        std::cout << "Registration " << (result ? "successful" : "failed") << std::endl;
        displaySeparator('-', 30);
    }
    
    // Try registering a duplicate user
    std::cout << "Attempting to register duplicate user 'admin'..." << std::endl;
    bool dupResult = auth.registerUser("admin", "newpassword", 0);
    std::cout << "Registration " << (dupResult ? "successful (unexpected)" : "failed (expected)") << std::endl;
    
    // Report user count
    std::cout << "\nTotal registered users: " << auth.getUserCount() << std::endl;
}

// Test user authentication
void testUserAuthentication(AuthModule& auth) {
    displayHeader("USER AUTHENTICATION TEST");
    
    struct AuthTest {
        std::string username;
        std::string password;
        bool shouldSucceed;
    };
    
    AuthTest authTests[] = {
        {"admin", "admin123", true},
        {"admin", "wrongpass", false},
        {"john.doe", "password123", true},
        {"nonexistent", "anypass", false},
        {"jane.smith", "wrong", false},
        {"staff.member", "staff456", true}
    };
    
    for (const auto& test : authTests) {
        std::cout << "Authenticating user: " << test.username << std::endl;
        int userType = auth.authenticateUser(test.username, test.password);
        
        if (userType >= 0) {
            std::cout << "Authentication successful. User type: " << getUserTypeString(userType) << std::endl;
        } else {
            std::cout << "Authentication failed." << std::endl;
        }
        
        std::cout << "Expected result: " << (test.shouldSucceed ? "Success" : "Failure") << std::endl;
        displaySeparator('-', 30);
    }
}

// Test password change
void testPasswordChange(AuthModule& auth) {
    displayHeader("PASSWORD CHANGE TEST");
    
    // Change password for existing user
    std::cout << "Changing password for user 'john.doe'" << std::endl;
    bool changeResult = auth.changePassword("john.doe", "password123", "newpassword");
    std::cout << "Password change " << (changeResult ? "successful" : "failed") << std::endl;
    
    // Verify new password works
    std::cout << "\nVerifying authentication with new password:" << std::endl;
    int userType = auth.authenticateUser("john.doe", "newpassword");
    std::cout << "Authentication " << (userType >= 0 ? "successful" : "failed") << std::endl;
    
    // Verify old password doesn't work
    std::cout << "\nVerifying old password no longer works:" << std::endl;
    userType = auth.authenticateUser("john.doe", "password123");
    std::cout << "Authentication with old password " << (userType >= 0 ? "succeeded (unexpected)" : "failed (expected)") << std::endl;
    
    // Try changing with wrong password
    std::cout << "\nAttempting password change with incorrect old password:" << std::endl;
    bool failedChange = auth.changePassword("admin", "wrongpass", "newadminpass");
    std::cout << "Password change " << (failedChange ? "succeeded (unexpected)" : "failed (expected)") << std::endl;
}

// Test user deletion
void testUserDeletion(AuthModule& auth) {
    displayHeader("USER DELETION TEST");
    
    // Delete an existing user
    std::cout << "Deleting user 'jane.smith'" << std::endl;
    bool deleteResult = auth.deleteUser("jane.smith");
    std::cout << "User deletion " << (deleteResult ? "successful" : "failed") << std::endl;
    
    // Verify user no longer exists
    std::cout << "\nVerifying user no longer exists:" << std::endl;
    bool exists = auth.userExists("jane.smith");
    std::cout << "User 'jane.smith' " << (exists ? "still exists (unexpected)" : "doesn't exist (expected)") << std::endl;
    
    // Try authenticating deleted user
    std::cout << "\nAttempting to authenticate deleted user:" << std::endl;
    int userType = auth.authenticateUser("jane.smith", "securepass");
    std::cout << "Authentication " << (userType >= 0 ? "succeeded (unexpected)" : "failed (expected)") << std::endl;
    
    // Try deleting non-existent user
    std::cout << "\nAttempting to delete non-existent user:" << std::endl;
    bool nonExistDelete = auth.deleteUser("nonexistent");
    std::cout << "Deletion " << (nonExistDelete ? "succeeded (unexpected)" : "failed (expected)") << std::endl;
    
    // Report updated user count
    std::cout << "\nRemaining users: " << auth.getUserCount() << std::endl;
}

// Test credential persistence
void testCredentialPersistence(AuthModule& auth) {
    displayHeader("CREDENTIAL PERSISTENCE TEST");
    
    // Save credentials
    std::cout << "Saving credentials to file..." << std::endl;
    bool saveResult = auth.saveCredentials("../../data/auth.dat");
    std::cout << "Save " << (saveResult ? "successful" : "failed") << std::endl;
    
    // Create a new auth module
    std::cout << "\nCreating a new AuthModule instance..." << std::endl;
    AuthModule newAuth;
    std::cout << "Initial user count: " << newAuth.getUserCount() << std::endl;
    
    // Load credentials
    std::cout << "\nLoading credentials from file..." << std::endl;
    bool loadResult = newAuth.loadCredentials("../../data/auth.dat");
    std::cout << "Load " << (loadResult ? "successful" : "failed") << std::endl;
    
    // Verify user count
    std::cout << "User count after loading: " << newAuth.getUserCount() << std::endl;
    
    // Verify authentication still works
    std::cout << "\nVerifying authentication with loaded credentials:" << std::endl;
    int userType = newAuth.authenticateUser("admin", "admin123");
    std::cout << "Admin authentication " << (userType >= 0 ? "successful" : "failed") << std::endl;
    
    userType = newAuth.authenticateUser("john.doe", "newpassword");
    std::cout << "John Doe authentication " << (userType >= 0 ? "successful" : "failed") << std::endl;
}

// Interactive test mode
void interactiveTestMode(AuthModule& auth) {
    displayHeader("INTERACTIVE AUTH MODULE TEST");
    
    bool running = true;
    while (running) {
        std::cout << "\nInteractive Auth Module Tester\n";
        std::cout << "------------------------------\n";
        std::cout << "1. Register new user\n";
        std::cout << "2. Authenticate user\n";
        std::cout << "3. Change user password\n";
        std::cout << "4. Delete user\n";
        std::cout << "5. List all users\n";
        std::cout << "6. Save credentials to file\n";
        std::cout << "7. Load credentials from file\n";
        std::cout << "8. Run automated tests\n";
        std::cout << "0. Exit\n";
        std::cout << "------------------------------\n";
        std::cout << "Enter your choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear newline from input buffer
        
        std::string username, password, newPassword;
        int userType;
        bool result;
        
        switch (choice) {
            case 0: // Exit
                running = false;
                break;
                
            case 1: // Register
                std::cout << "\nREGISTER USER\n";
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);
                std::cout << "User Type (0=Regular, 1=Admin, 2=Crew): ";
                std::cin >> userType;
                std::cin.ignore();
                
                result = auth.registerUser(username, password, userType);
                std::cout << "\nRegistration " << (result ? "successful" : "failed") << "\n";
                break;
                
            case 2: // Authenticate
                std::cout << "\nAUTHENTICATE USER\n";
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);
                
                userType = auth.authenticateUser(username, password);
                if (userType >= 0) {
                    std::cout << "\nAuthentication successful. User type: " 
                              << getUserTypeString(userType) << "\n";
                } else {
                    std::cout << "\nAuthentication failed.\n";
                }
                break;
                
            case 3: // Change password
                std::cout << "\nCHANGE PASSWORD\n";
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Current password: ";
                std::getline(std::cin, password);
                std::cout << "New password: ";
                std::getline(std::cin, newPassword);
                
                result = auth.changePassword(username, password, newPassword);
                std::cout << "\nPassword change " << (result ? "successful" : "failed") << "\n";
                break;
                
            case 4: // Delete user
                std::cout << "\nDELETE USER\n";
                std::cout << "Username: ";
                std::getline(std::cin, username);
                
                result = auth.deleteUser(username);
                std::cout << "\nUser deletion " << (result ? "successful" : "failed") << "\n";
                break;
                
            case 5: // List users
                std::cout << "\nUSER LIST\n";
                std::cout << "Total users: " << auth.getUserCount() << "\n";
                // If your AuthModule has a listUsers() method, call it here
                // Otherwise this is just showing the count
                break;
                
            case 6: // Save credentials
                std::cout << "\nSAVE CREDENTIALS\n";
                std::cout << "File path (default: ../../data/auth.dat): ";
                std::getline(std::cin, username); // Reuse username variable for file path
                if (username.empty()) {
                    username = "../../data/auth.dat";
                }
                
                result = auth.saveCredentials(username);
                std::cout << "\nCredentials save " << (result ? "successful" : "failed") << "\n";
                break;
                
            case 7: // Load credentials
                std::cout << "\nLOAD CREDENTIALS\n";
                std::cout << "File path (default: ../../data/auth.dat): ";
                std::getline(std::cin, username); // Reuse username variable for file path
                if (username.empty()) {
                    username = "../../data/auth.dat";
                }
                
                result = auth.loadCredentials(username);
                std::cout << "\nCredentials load " << (result ? "successful" : "failed") << "\n";
                std::cout << "User count after loading: " << auth.getUserCount() << "\n";
                break;
                
            case 8: // Run automated tests
                // Run the original automated tests
                testUserRegistration(auth);
                std::cout << "\n\n";
                
                testUserAuthentication(auth);
                std::cout << "\n\n";
                
                testPasswordChange(auth);
                std::cout << "\n\n";
                
                testUserDeletion(auth);
                std::cout << "\n\n";
                
                testCredentialPersistence(auth);
                break;
                
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
                break;
        }
        
        if (running) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
    }
}

// Main testing function
int main(int argc, char* argv[]) {
    std::cout << "\n\n";
    displayHeader("AUTHENTICATION MODULE TEST");
    std::cout << "Date: " << Model::DateTime::now().iso8601String << "\n\n";
    
    // Create module instance
    AuthModule authModule;
    
    try {
        // Check if we should run in interactive mode
        if (argc > 1 && std::string(argv[1]) == "-i") {
            interactiveTestMode(authModule);
        } else {
            // Run automated tests
            testUserRegistration(authModule);
            std::cout << "\n\n";
            
            testUserAuthentication(authModule);
            std::cout << "\n\n";
            
            testPasswordChange(authModule);
            std::cout << "\n\n";
            
            testUserDeletion(authModule);
            std::cout << "\n\n";
            
            testCredentialPersistence(authModule);
            
            displayHeader("TEST COMPLETED SUCCESSFULLY");
            
            // Display hint about interactive mode
            std::cout << "\nTip: Run with -i flag for interactive testing mode.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
