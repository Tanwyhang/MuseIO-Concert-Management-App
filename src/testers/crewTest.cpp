#include "../include/crewModule.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Crew Module Test ===" << std::endl;
    
    try {
        // Test 1: Create crew members using the module directly
        CrewModule crewModule;
        
        auto crew1 = crewModule.createCrewMember(
            "Alice Johnson", 
            "alice@events.com", 
            "+1-555-0101"
        );
        
        auto crew2 = crewModule.createCrewMember(
            "Bob Smith", 
            "bob@events.com", 
            "+1-555-0102"
        );
        
        std::cout << "✓ Created crew members successfully" << std::endl;
        
        // Test 2: Assign tasks
        bool taskResult1 = crewModule.assignTask(
            crew1->id, 
            "Setup Stage", 
            "Set up the main stage for the concert",
            Model::TaskPriority::HIGH
        );
        
        bool taskResult2 = crewModule.assignTask(
            crew1->id, 
            "Sound Check", 
            "Perform sound system check",
            Model::TaskPriority::MEDIUM
        );
        
        assert(taskResult1 == true);
        assert(taskResult2 == true);
        std::cout << "✓ Task assignment working correctly" << std::endl;
        
        // Test 3: Check tasks
        auto tasks = crewModule.getCrewTasks(crew1->id);
        assert(tasks.size() == 2);
        std::cout << "✓ Task retrieval working correctly" << std::endl;
        
        // Test 4: Update task status
        bool statusUpdate = crewModule.updateTaskStatus(
            crew1->id, 
            tasks[0]->task_id, 
            Model::TaskStatus::IN_PROGRESS
        );
        assert(statusUpdate == true);
        std::cout << "✓ Task status update working correctly" << std::endl;
        
        // Test 5: Check-in/Check-out
        bool checkInResult = crewModule.checkInCrewMember(crew1->id);
        assert(checkInResult == true);
        
        auto checkedInCrew = crewModule.getCrewMemberById(crew1->id);
        assert(checkedInCrew->check_in_time.has_value());
        std::cout << "✓ Check-in working correctly" << std::endl;
        
        bool checkOutResult = crewModule.checkOutCrewMember(crew1->id);
        assert(checkOutResult == true);
        
        auto checkedOutCrew = crewModule.getCrewMemberById(crew1->id);
        assert(checkedOutCrew->check_out_time.has_value());
        std::cout << "✓ Check-out working correctly" << std::endl;
        
        // Test 6: Search functionality
        auto searchResults = crewModule.findCrewMembersByName("alice");
        assert(searchResults.size() == 1);
        assert(searchResults[0]->name == "Alice Johnson");
        std::cout << "✓ Name search working correctly" << std::endl;
        
        auto emailResult = crewModule.findCrewMemberByEmail("bob@events.com");
        assert(emailResult != nullptr);
        assert(emailResult->name == "Bob Smith");
        std::cout << "✓ Email search working correctly" << std::endl;
        
        // Test 7: Using namespace wrapper
        std::cout << "\n--- Testing namespace wrapper ---" << std::endl;
        
        auto crew3 = CrewManager::addCrewMember(
            "Charlie Brown", 
            "charlie@events.com", 
            "+1-555-0103"
        );
        
        bool taskAssign = CrewManager::assignTaskToCrew(
            crew3->id, 
            "Security Check", 
            "Monitor venue security",
            Model::TaskPriority::CRITICAL
        );
        assert(taskAssign == true);
        
        bool checkIn = CrewManager::checkInCrew(crew3->id);
        assert(checkIn == true);
        
        auto crew3Tasks = CrewManager::listCrewTasks(crew3->id);
        assert(crew3Tasks.size() == 1);
        assert(crew3Tasks[0]->task_name == "Security Check");
        
        std::cout << "✓ Namespace wrapper working correctly" << std::endl;
        
        // Test 8: List all crew members
        std::cout << "\n--- All Crew Members ---" << std::endl;
        CrewManager::listCrewMembers();
        
        auto allCrew = CrewManager::getAllCrewMembers();
        std::cout << "Total crew members: " << allCrew.size() << std::endl;
        
        // Display tasks for each crew member
        for (const auto& crew : allCrew) {
            auto memberTasks = CrewManager::listCrewTasks(crew->id);
            std::cout << "Crew member " << crew->name << " has " << memberTasks.size() << " tasks:" << std::endl;
            for (const auto& task : memberTasks) {
                std::cout << "  - " << task->task_name << " (" 
                         << (task->status == Model::TaskStatus::TODO ? "TODO" : 
                             task->status == Model::TaskStatus::IN_PROGRESS ? "IN_PROGRESS" : "COMPLETED") 
                         << ")" << std::endl;
            }
        }
        
        std::cout << "\n=== All Crew Module Tests Passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
