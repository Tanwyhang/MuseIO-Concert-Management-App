#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include "../include/models.hpp"
#include "../include/attendeeModule.hpp"

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

// Utility function for displaying attendee types
std::string getAttendeeTypeString(Model::AttendeeType type) {
    switch (type) {
        case Model::AttendeeType::REGULAR:    return "REGULAR";
        case Model::AttendeeType::VIP:        return "VIP";
        case Model::AttendeeType::STAFF:      return "STAFF";
        case Model::AttendeeType::PRESS:      return "PRESS";
        case Model::AttendeeType::PERFORMER:  return "PERFORMER";
        default:                             return "UNKNOWN";
    }
}

// Detailed display of a single attendee
void displayAttendee(const std::shared_ptr<Model::Attendee>& attendee, bool detailed = true) {
    if (!attendee) {
        std::cout << "[NULL ATTENDEE REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Attendee ID: " << attendee->getId() << std::endl;
    std::cout << "Name: " << attendee->name << std::endl;
    
    if (detailed) {
        std::cout << "Email: " << attendee->email << std::endl;
        std::cout << "Phone: " << attendee->phone_number << std::endl;
        std::cout << "Type: " << getAttendeeTypeString(attendee->attendee_type) << std::endl;
        std::cout << "Registration Date: " << attendee->registration_date.iso8601String << std::endl;
        std::cout << "Username: " << (attendee->username.empty() ? "[None]" : attendee->username) << std::endl;
        std::cout << "Staff Privileges: " << (attendee->staff_privileges ? "YES" : "NO") << std::endl;
    }
}

// Display a collection of attendees
void displayAttendeeList(const std::vector<std::shared_ptr<Model::Attendee>>& attendees) {
    if (attendees.empty()) {
        std::cout << "No attendees found." << std::endl;
        return;
    }
    
    std::cout << "Found " << attendees.size() << " attendee(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(5) << "ID"
              << std::setw(25) << "Name"
              << std::setw(30) << "Email"
              << std::setw(15) << "Type"
              << std::setw(30) << "Registration Date" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& attendee : attendees) {
        std::cout << std::setw(5) << attendee->getId()
                  << std::setw(25) << attendee->name
                  << std::setw(30) << attendee->email
                  << std::setw(15) << getAttendeeTypeString(attendee->attendee_type)
                  << std::setw(30) << attendee->registration_date.iso8601String << std::endl;
    }
    displaySeparator();
}

// Test CREATE operation
std::vector<std::shared_ptr<Model::Attendee>> testCreateOperation(AttendeeModule& module) {
    displayHeader("CREATE OPERATION TEST");
    
    std::vector<std::shared_ptr<Model::Attendee>> createdAttendees;
    
    // Create regular attendee
    std::cout << "Creating regular attendee..." << std::endl;
    auto attendee1 = module.createAttendee(
        "John Doe",
        "john.doe@example.com",
        "555-123-4567",
        Model::AttendeeType::REGULAR,
        "johndoe",
        "password123"
    );
    createdAttendees.push_back(attendee1);
    displayAttendee(attendee1);
    displaySeparator();
    
    // Create VIP attendee
    std::cout << "Creating VIP attendee..." << std::endl;
    auto attendee2 = module.createAttendee(
        "Jane Smith",
        "jane.smith@example.com",
        "555-987-6543",
        Model::AttendeeType::VIP,
        "janesmith",
        "vippass"
    );
    createdAttendees.push_back(attendee2);
    displayAttendee(attendee2);
    displaySeparator();
    
    // Create staff attendee
    std::cout << "Creating staff attendee..." << std::endl;
    auto attendee3 = module.createAttendee(
        "Robert Johnson",
        "robert.johnson@example.com",
        "555-555-5555",
        Model::AttendeeType::STAFF,
        "robertj",
        "staffpass",
        true
    );
    createdAttendees.push_back(attendee3);
    displayAttendee(attendee3);
    displaySeparator();
    
    // Create press attendee
    std::cout << "Creating press attendee..." << std::endl;
    auto attendee4 = module.createAttendee(
        "Maria Garcia",
        "maria.garcia@press.com",
        "555-222-3333",
        Model::AttendeeType::PRESS,
        "mariag",
        "presspass"
    );
    createdAttendees.push_back(attendee4);
    displayAttendee(attendee4);
    
    return createdAttendees;
}

// Test READ operations
void testReadOperations(AttendeeModule& module, const std::vector<std::shared_ptr<Model::Attendee>>& testAttendees) {
    displayHeader("READ OPERATIONS TEST");
    
    // Get all attendees
    std::cout << "1. Getting all attendees:" << std::endl;
    auto allAttendees = module.getAllAttendees();
    displayAttendeeList(allAttendees);
    
    // Get attendee by ID
    if (!testAttendees.empty()) {
        int idToFind = testAttendees[0]->getId();
        std::cout << "\n2. Getting attendee by ID " << idToFind << ":" << std::endl;
        auto foundAttendee = module.getAttendeeById(idToFind);
        if (foundAttendee) {
            displayAttendee(foundAttendee);
        }
    }
    
    // Find attendees by name (partial match)
    std::cout << "\n3. Finding attendees by name (contains 'John'):" << std::endl;
    auto nameResults = module.findAttendeesByName("John");
    displayAttendeeList(nameResults);
    
    // Find attendee by email (exact match)
    if (testAttendees.size() > 1) {
        std::string emailToFind = testAttendees[1]->email;
        std::cout << "\n4. Finding attendee by exact email '" << emailToFind << "':" << std::endl;
        auto emailResult = module.findAttendeeByEmail(emailToFind);
        if (emailResult) {
            displayAttendee(emailResult);
        } else {
            std::cout << "No attendee found with email: " << emailToFind << std::endl;
        }
    }
    
    // Test not finding an attendee
    std::cout << "\n5. Testing non-existent ID lookup:" << std::endl;
    auto nonExistentAttendee = module.getAttendeeById(9999);
    if (!nonExistentAttendee) {
        std::cout << "Correctly returned nullptr for non-existent ID 9999" << std::endl;
    }
    
    // Test not finding an email
    std::cout << "\n6. Testing non-existent email lookup:" << std::endl;
    auto nonExistentEmail = module.findAttendeeByEmail("nonexistent@example.com");
    if (!nonExistentEmail) {
        std::cout << "Correctly returned nullptr for non-existent email" << std::endl;
    }
}

// Test UPDATE operations
void testUpdateOperations(AttendeeModule& module, const std::vector<std::shared_ptr<Model::Attendee>>& testAttendees) {
    displayHeader("UPDATE OPERATIONS TEST");
    
    if (testAttendees.empty()) {
        std::cout << "No test attendees available for update testing." << std::endl;
        return;
    }
    
    // Update name only
    int id1 = testAttendees[0]->getId();
    std::cout << "1. Updating name only for ID " << id1 << ":" << std::endl;
    std::cout << "Before update:" << std::endl;
    displayAttendee(module.getAttendeeById(id1));
    
    if (module.updateAttendee(id1, "John Doe Updated")) {
        std::cout << "\nAfter update:" << std::endl;
        displayAttendee(module.getAttendeeById(id1));
    } else {
        std::cout << "Update failed." << std::endl;
    }
    displaySeparator();
    
    // Update email and phone
    if (testAttendees.size() > 1) {
        int id2 = testAttendees[1]->getId();
        std::cout << "2. Updating email and phone for ID " << id2 << ":" << std::endl;
        std::cout << "Before update:" << std::endl;
        displayAttendee(module.getAttendeeById(id2));
        
        if (module.updateAttendee(id2, "", "jane.updated@example.com", "555-111-2222")) {
            std::cout << "\nAfter update:" << std::endl;
            displayAttendee(module.getAttendeeById(id2));
        } else {
            std::cout << "Update failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Update attendee type
    if (testAttendees.size() > 2) {
        int id3 = testAttendees[2]->getId();
        std::cout << "3. Updating attendee type for ID " << id3 << ":" << std::endl;
        std::cout << "Before update:" << std::endl;
        displayAttendee(module.getAttendeeById(id3));
        
        if (module.updateAttendee(id3, "", "", "", Model::AttendeeType::VIP)) {
            std::cout << "\nAfter update:" << std::endl;
            displayAttendee(module.getAttendeeById(id3));
        } else {
            std::cout << "Update failed." << std::endl;
        }
        displaySeparator();
    }
    
    // Test updating non-existent attendee
    std::cout << "4. Attempting to update non-existent attendee (ID 9999):" << std::endl;
    bool nonExistentUpdate = module.updateAttendee(9999, "This Should Fail");
    std::cout << "Update result: " << (nonExistentUpdate ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
}

// Test DELETE operations
void testDeleteOperations(AttendeeModule& module, std::vector<std::shared_ptr<Model::Attendee>>& testAttendees) {
    displayHeader("DELETE OPERATIONS TEST");
    
    if (testAttendees.empty()) {
        std::cout << "No test attendees available for delete testing." << std::endl;
        return;
    }
    
    // Delete the last created attendee
    int idToDelete = testAttendees.back()->getId();
    std::cout << "1. Deleting attendee with ID " << idToDelete << ":" << std::endl;
    displayAttendee(module.getAttendeeById(idToDelete));
    
    if (module.deleteAttendee(idToDelete)) {
        std::cout << "\nAttendee successfully deleted." << std::endl;
        
        // Verify deletion
        auto verifyDelete = module.getAttendeeById(idToDelete);
        if (!verifyDelete) {
            std::cout << "Verification successful: Attendee no longer exists." << std::endl;
        } else {
            std::cout << "Verification failed: Attendee still exists!" << std::endl;
        }
        
        // Remove from our test list
        testAttendees.pop_back();
    } else {
        std::cout << "Deletion failed." << std::endl;
    }
    displaySeparator();
    
    // Show remaining attendees
    std::cout << "2. Remaining attendees after deletion:" << std::endl;
    displayAttendeeList(module.getAllAttendees());
    
    // Try to delete non-existent attendee
    std::cout << "\n3. Attempting to delete non-existent attendee (ID 9999):" << std::endl;
    bool nonExistentDelete = module.deleteAttendee(9999);
    std::cout << "Delete result: " << (nonExistentDelete ? "Succeeded (unexpected)" : "Failed (expected)") << std::endl;
}

// Main testing function
int main() {
    std::cout << "\n\n";
    displayHeader("ATTENDEE MODULE COMPREHENSIVE TEST");
    std::cout << "Date: " << Model::DateTime::now().iso8601String << "\n\n";
    
    // Create module instance
    AttendeeModule module;
    
    // Vector to keep track of created attendees for testing
    std::vector<std::shared_ptr<Model::Attendee>> testAttendees;
    
    try {
        // Test CREATE operations
        testAttendees = testCreateOperation(module);
        std::cout << "\n\n";
        
        // Test READ operations
        testReadOperations(module, testAttendees);
        std::cout << "\n\n";
        
        // Test UPDATE operations
        testUpdateOperations(module, testAttendees);
        std::cout << "\n\n";
        
        // Test DELETE operations
        testDeleteOperations(module, testAttendees);
        
        displayHeader("TEST COMPLETED SUCCESSFULLY");
    }
    catch (const std::exception& e) {
        std::cerr << "\n\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
