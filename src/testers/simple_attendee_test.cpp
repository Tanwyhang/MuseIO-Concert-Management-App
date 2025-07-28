#include <iostream>
#include <memory>
#include "../include/attendeeModule.hpp"
#include "../include/models.hpp"

int main() {
    std::cout << "Simple Attendee Module Test\n";
    std::cout << "=========================\n";
    
    // Create an AttendeeModule instance
    AttendeeModule module;
    
    // Create a test attendee
    auto attendee = module.createAttendee(
        "Wyhang", 
        "test@example.com", 
        "123-456-7890",
        Model::AttendeeType::REGULAR,
        "",
        "",
        false
    );
    
    std::cout << "Created attendee: " << attendee->name << " (ID: " << attendee->getId() << ")\n";
    
    // Find the attendee
    auto found = module.getAttendeeById(attendee->getId());
    if (found) {
        std::cout << "Found attendee by ID: " << found->name << "\n";
    }
    
    // Update the attendee
    if (module.updateAttendee(attendee->getId(), "Updated Name")) {
        std::cout << "Updated attendee name successfully\n";
        
        auto updated = module.getAttendeeById(attendee->getId());
        std::cout << "New name: " << updated->name << "\n";
    }
    
    // Test complete
    std::cout << "\nTest completed successfully!\n";
    return 0;
}
