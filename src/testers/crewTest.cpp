#include "../include/crewModule.hpp"
#include <iostream>
#include <cassert>
#include <string>

/**
 * @brief Test file for CrewModule functionality
 * 
 * This file tests all the CRUD operations, task management,
 * time tracking, and search functionality of the CrewModule.
 */

void testCrewCreation() {
    std::cout << "\n=== Testing Crew Creation ===" << std::endl;
    
    // Test creating crew members with different jobs using the namespace wrapper
    auto crew1 = CrewManager::createCrewMember("John Smith", "john.smith@example.com", "+1-555-0101", "Manager");
    auto crew2 = CrewManager::createCrewMember("Jane Doe", "jane.doe@example.com", "+1-555-0102", "Cashier");
    auto crew3 = CrewManager::createCrewMember("Mike Johnson", "mike.johnson@example.com", "+1-555-0103", "Security");
    auto crew4 = CrewManager::createCrewMember("Sarah Wilson", "sarah.wilson@example.com", "+1-555-0104", "Sound Technician");
    auto crew5 = CrewManager::createCrewMember("Bob Brown", "bob.brown@example.com", "+1-555-0105"); // Default job
    
    assert(crew1 != nullptr);
    assert(crew2 != nullptr);
    assert(crew3 != nullptr);
    assert(crew4 != nullptr);
    assert(crew5 != nullptr);
    
    assert(crew1->name == "John Smith");
    assert(crew1->email == "john.smith@example.com");
    assert(crew1->phone_number == "+1-555-0101");
    assert(CrewManager::getCrewJob(crew1->id) == "Manager");
    
    assert(crew2->name == "Jane Doe");
    assert(CrewManager::getCrewJob(crew2->id) == "Cashier");
    
    assert(crew3->name == "Mike Johnson");
    assert(CrewManager::getCrewJob(crew3->id) == "Security");
    
    assert(CrewManager::getCrewJob(crew4->id) == "Sound Technician");
    assert(CrewManager::getCrewJob(crew5->id) == "General Staff"); // Default job
    
    std::cout << "✓ Created crew member: " << crew1->name << " (ID: " << crew1->id << ", Job: " << CrewManager::getCrewJob(crew1->id) << ")" << std::endl;
    std::cout << "✓ Created crew member: " << crew2->name << " (ID: " << crew2->id << ", Job: " << CrewManager::getCrewJob(crew2->id) << ")" << std::endl;
    std::cout << "✓ Created crew member: " << crew3->name << " (ID: " << crew3->id << ", Job: " << CrewManager::getCrewJob(crew3->id) << ")" << std::endl;
    std::cout << "✓ Created crew member: " << crew4->name << " (ID: " << crew4->id << ", Job: " << CrewManager::getCrewJob(crew4->id) << ")" << std::endl;
    std::cout << "✓ Created crew member: " << crew5->name << " (ID: " << crew5->id << ", Job: " << CrewManager::getCrewJob(crew5->id) << ")" << std::endl;
    
    std::cout << "✓ Crew creation tests passed!" << std::endl;
}

void testCrewRetrieval() {
    std::cout << "\n=== Testing Crew Retrieval ===" << std::endl;
    
    // Get all crew members
    const auto& allCrew = CrewManager::getAllCrew();
    std::cout << "✓ Total crew members: " << allCrew.size() << std::endl;
    
    assert(allCrew.size() >= 3); // Should have at least the ones we created
    
    // Test retrieval by ID
    if (!allCrew.empty()) {
        int firstId = allCrew[0]->id;
        auto retrievedCrew = CrewManager::getCrewById(firstId);
        assert(retrievedCrew != nullptr);
        assert(retrievedCrew->id == firstId);
        std::cout << "✓ Successfully retrieved crew by ID: " << retrievedCrew->name << std::endl;
    }
    
    // Test retrieval of non-existent crew
    auto nonExistent = CrewManager::getCrewById(99999);
    assert(nonExistent == nullptr);
    std::cout << "✓ Correctly returned nullptr for non-existent crew" << std::endl;
    
    std::cout << "✓ Crew retrieval tests passed!" << std::endl;
}

void testCrewSearch() {
    std::cout << "\n=== Testing Crew Search ===" << std::endl;
    
    // Test search by name (case insensitive)
    auto johnResults = CrewManager::findCrewByName("john");
    assert(!johnResults.empty());
    std::cout << "✓ Found " << johnResults.size() << " crew member(s) with 'john' in name" << std::endl;
    
    auto smithResults = CrewManager::findCrewByName("Smith");
    assert(!smithResults.empty());
    std::cout << "✓ Found " << smithResults.size() << " crew member(s) with 'Smith' in name" << std::endl;
    
    // Test search by email
    auto emailResult = CrewManager::findCrewByEmail("jane.doe@example.com");
    assert(emailResult != nullptr);
    assert(emailResult->name == "Jane Doe");
    std::cout << "✓ Found crew member by email: " << emailResult->name << std::endl;
    
    // Test search for non-existent email
    auto noResult = CrewManager::findCrewByEmail("nonexistent@example.com");
    assert(noResult == nullptr);
    std::cout << "✓ Correctly returned nullptr for non-existent email" << std::endl;
    
    std::cout << "✓ Crew search tests passed!" << std::endl;
}

void testJobManagement() {
    std::cout << "\n=== Testing Job Management ===" << std::endl;
    
    // Test search by job (case insensitive)
    auto managerResults = CrewManager::findCrewByJob("manager");
    assert(!managerResults.empty());
    std::cout << "✓ Found " << managerResults.size() << " crew member(s) with 'manager' in job title" << std::endl;
    
    auto cashierResults = CrewManager::findCrewByJob("Cashier");
    assert(!cashierResults.empty());
    std::cout << "✓ Found " << cashierResults.size() << " crew member(s) with 'Cashier' in job title" << std::endl;
    
    auto securityResults = CrewManager::findCrewByJob("security");
    assert(!securityResults.empty());
    std::cout << "✓ Found " << securityResults.size() << " crew member(s) with 'security' in job title" << std::endl;
    
    auto techResults = CrewManager::findCrewByJob("technician");
    assert(!techResults.empty());
    std::cout << "✓ Found " << techResults.size() << " crew member(s) with 'technician' in job title" << std::endl;
    
    // Test job updates
    auto crew = CrewManager::findCrewByEmail("bob.brown@example.com");
    assert(crew != nullptr);
    int crewId = crew->id;
    
    // Verify initial job
    assert(CrewManager::getCrewJob(crewId) == "General Staff");
    std::cout << "✓ Initial job verified: " << CrewManager::getCrewJob(crewId) << std::endl;
    
    // Update job using setCrewJob
    bool jobUpdateResult = CrewManager::setCrewJob(crewId, "Stage Manager");
    assert(jobUpdateResult == true);
    assert(CrewManager::getCrewJob(crewId) == "Stage Manager");
    std::cout << "✓ Job updated using setCrewJob: " << CrewManager::getCrewJob(crewId) << std::endl;
    
    // Update job using updateCrewMember
    bool updateResult = CrewManager::updateCrewMember(crewId, "", "", "", "Event Coordinator");
    assert(updateResult == true);
    assert(CrewManager::getCrewJob(crewId) == "Event Coordinator");
    std::cout << "✓ Job updated using updateCrewMember: " << CrewManager::getCrewJob(crewId) << std::endl;
    
    // Test job update for non-existent crew
    bool failJobUpdate = CrewManager::setCrewJob(99999, "Ghost Job");
    assert(failJobUpdate == false);
    std::cout << "✓ Correctly failed to update job for non-existent crew" << std::endl;
    
    // Test getting job for non-existent crew
    std::string unknownJob = CrewManager::getCrewJob(99999);
    assert(unknownJob == "Unknown");
    std::cout << "✓ Correctly returned 'Unknown' for non-existent crew job" << std::endl;
    
    std::cout << "✓ Job management tests passed!" << std::endl;
}

void testCrewUpdate() {
    std::cout << "\n=== Testing Crew Updates ===" << std::endl;
    
    // Find a crew member to update
    auto crew = CrewManager::findCrewByEmail("john.smith@example.com");
    assert(crew != nullptr);
    
    int crewId = crew->id;
    std::string originalName = crew->name;
    
    // Test updating name only
    bool updateResult = CrewManager::updateCrewMember(crewId, "John Smith Jr.");
    assert(updateResult == true);
    
    // Verify the update
    auto updatedCrew = CrewManager::getCrewById(crewId);
    assert(updatedCrew != nullptr);
    assert(updatedCrew->name == "John Smith Jr.");
    assert(updatedCrew->email == "john.smith@example.com"); // Should remain unchanged
    std::cout << "✓ Successfully updated crew name: " << updatedCrew->name << std::endl;
    
    // Test updating multiple fields including job
    updateResult = CrewManager::updateCrewMember(crewId, "John Smith Sr.", "john.sr@example.com", "+1-555-9999", "Senior Manager");
    assert(updateResult == true);
    
    updatedCrew = CrewManager::getCrewById(crewId);
    assert(updatedCrew->name == "John Smith Sr.");
    assert(updatedCrew->email == "john.sr@example.com");
    assert(updatedCrew->phone_number == "+1-555-9999");
    assert(CrewManager::getCrewJob(crewId) == "Senior Manager");
    std::cout << "✓ Successfully updated multiple crew fields including job: " << CrewManager::getCrewJob(crewId) << std::endl;
    
    // Test updating non-existent crew
    bool failResult = CrewManager::updateCrewMember(99999, "Ghost Member");
    assert(failResult == false);
    std::cout << "✓ Correctly failed to update non-existent crew" << std::endl;
    
    std::cout << "✓ Crew update tests passed!" << std::endl;
}

void testTaskManagement() {
    std::cout << "\n=== Testing Task Management ===" << std::endl;
    
    // Find a crew member for task assignment
    auto crew = CrewManager::findCrewByEmail("jane.doe@example.com");
    assert(crew != nullptr);
    int crewId = crew->id;
    
    // Test task assignment
    bool assignResult1 = CrewManager::assignTaskToCrew(crewId, "Setup Sound System", "Install and test all audio equipment", Model::TaskPriority::HIGH);
    assert(assignResult1 == true);
    std::cout << "✓ Assigned high priority task: Setup Sound System" << std::endl;
    
    bool assignResult2 = CrewManager::assignTaskToCrew(crewId, "Arrange Seating", "Set up chairs and tables for attendees", Model::TaskPriority::MEDIUM);
    assert(assignResult2 == true);
    std::cout << "✓ Assigned medium priority task: Arrange Seating" << std::endl;
    
    bool assignResult3 = CrewManager::assignTaskToCrew(crewId, "Final Cleanup", "Clean venue after event", Model::TaskPriority::LOW);
    assert(assignResult3 == true);
    std::cout << "✓ Assigned low priority task: Final Cleanup" << std::endl;
    
    // Test getting crew tasks
    auto tasks = CrewManager::getCrewTasks(crewId);
    assert(tasks.size() >= 3);
    std::cout << "✓ Crew has " << tasks.size() << " assigned tasks" << std::endl;
    
    // Verify task details
    bool foundSoundTask = false;
    bool foundSeatingTask = false;
    bool foundCleanupTask = false;
    
    for (const auto& task : tasks) {
        if (task->task_name == "Setup Sound System") {
            foundSoundTask = true;
            assert(task->priority == Model::TaskPriority::HIGH);
            assert(task->status == Model::TaskStatus::TODO);
        } else if (task->task_name == "Arrange Seating") {
            foundSeatingTask = true;
            assert(task->priority == Model::TaskPriority::MEDIUM);
        } else if (task->task_name == "Final Cleanup") {
            foundCleanupTask = true;
            assert(task->priority == Model::TaskPriority::LOW);
        }
    }
    
    assert(foundSoundTask && foundSeatingTask && foundCleanupTask);
    std::cout << "✓ All assigned tasks found with correct priorities" << std::endl;
    
    // Test task status update
    int soundTaskId = -1;
    for (const auto& task : tasks) {
        if (task->task_name == "Setup Sound System") {
            soundTaskId = task->task_id;
            break;
        }
    }
    
    assert(soundTaskId != -1);
    bool statusUpdateResult = CrewManager::updateTaskStatus(crewId, soundTaskId, Model::TaskStatus::IN_PROGRESS);
    assert(statusUpdateResult == true);
    std::cout << "✓ Updated task status to IN_PROGRESS" << std::endl;
    
    // Verify status update
    auto updatedTasks = CrewManager::getCrewTasks(crewId);
    bool statusUpdated = false;
    for (const auto& task : updatedTasks) {
        if (task->task_id == soundTaskId) {
            assert(task->status == Model::TaskStatus::IN_PROGRESS);
            statusUpdated = true;
            break;
        }
    }
    assert(statusUpdated);
    std::cout << "✓ Task status successfully updated and verified" << std::endl;
    
    // Test task assignment to non-existent crew
    bool failAssign = CrewManager::assignTaskToCrew(99999, "Ghost Task", "This should fail");
    assert(failAssign == false);
    std::cout << "✓ Correctly failed to assign task to non-existent crew" << std::endl;
    
    std::cout << "✓ Task management tests passed!" << std::endl;
}

void testTimeTracking() {
    std::cout << "\n=== Testing Time Tracking ===" << std::endl;
    
    // Find a crew member for time tracking
    auto crew = CrewManager::findCrewByEmail("mike.johnson@example.com");
    assert(crew != nullptr);
    int crewId = crew->id;
    
    // Initially, crew should not be checked in
    assert(!crew->check_in_time.has_value());
    assert(!crew->check_out_time.has_value());
    std::cout << "✓ Crew member initially not checked in" << std::endl;
    
    // Test check-in
    bool checkInResult = CrewManager::checkInCrew(crewId);
    assert(checkInResult == true);
    std::cout << "✓ Successfully checked in crew member" << std::endl;
    
    // Verify check-in
    auto checkedInCrew = CrewManager::getCrewById(crewId);
    assert(checkedInCrew != nullptr);
    assert(checkedInCrew->check_in_time.has_value());
    assert(!checkedInCrew->check_out_time.has_value());
    std::cout << "✓ Check-in time recorded: " << checkedInCrew->check_in_time->iso8601String << std::endl;
    
    // Test check-out
    bool checkOutResult = CrewManager::checkOutCrew(crewId);
    assert(checkOutResult == true);
    std::cout << "✓ Successfully checked out crew member" << std::endl;
    
    // Verify check-out
    auto checkedOutCrew = CrewManager::getCrewById(crewId);
    assert(checkedOutCrew != nullptr);
    assert(checkedOutCrew->check_in_time.has_value());
    assert(checkedOutCrew->check_out_time.has_value());
    std::cout << "✓ Check-out time recorded: " << checkedOutCrew->check_out_time->iso8601String << std::endl;
    
    // Test checking in/out non-existent crew
    bool failCheckIn = CrewManager::checkInCrew(99999);
    assert(failCheckIn == false);
    bool failCheckOut = CrewManager::checkOutCrew(99999);
    assert(failCheckOut == false);
    std::cout << "✓ Correctly failed time tracking for non-existent crew" << std::endl;
    
    std::cout << "✓ Time tracking tests passed!" << std::endl;
}

void testAdvancedFeatures() {
    std::cout << "\n=== Testing Advanced Features ===" << std::endl;
    
    // Test getting crew by task status
    auto crewWithTodoTasks = CrewManager::getCrewByTaskStatus(Model::TaskStatus::TODO);
    std::cout << "✓ Found " << crewWithTodoTasks.size() << " crew member(s) with TODO tasks" << std::endl;
    
    auto crewWithInProgressTasks = CrewManager::getCrewByTaskStatus(Model::TaskStatus::IN_PROGRESS);
    std::cout << "✓ Found " << crewWithInProgressTasks.size() << " crew member(s) with IN_PROGRESS tasks" << std::endl;
    
    // Test task removal
    auto crew = CrewManager::findCrewByEmail("jane.doe@example.com");
    assert(crew != nullptr);
    int crewId = crew->id;
    
    auto tasks = CrewManager::getCrewTasks(crewId);
    if (!tasks.empty()) {
        int taskToRemove = tasks[0]->task_id;
        std::string taskName = tasks[0]->task_name;
        size_t originalTaskCount = tasks.size();
        
        bool removeResult = CrewManager::removeTaskFromCrew(crewId, taskToRemove);
        assert(removeResult == true);
        std::cout << "✓ Successfully removed task: " << taskName << std::endl;
        
        // Verify task removal
        auto updatedTasks = CrewManager::getCrewTasks(crewId);
        assert(updatedTasks.size() == originalTaskCount - 1);
        
        // Verify the specific task is gone
        bool taskStillExists = false;
        for (const auto& task : updatedTasks) {
            if (task->task_id == taskToRemove) {
                taskStillExists = true;
                break;
            }
        }
        assert(!taskStillExists);
        std::cout << "✓ Task successfully removed from crew" << std::endl;
    }
    
    // Test removing non-existent task
    bool failRemove = CrewManager::removeTaskFromCrew(crewId, 99999);
    assert(failRemove == false);
    std::cout << "✓ Correctly failed to remove non-existent task" << std::endl;
    
    std::cout << "✓ Advanced features tests passed!" << std::endl;
}

void testCrewDeletion() {
    std::cout << "\n=== Testing Crew Deletion ===" << std::endl;
    
    // Create a temporary crew member for deletion test
    auto tempCrew = CrewManager::createCrewMember("Temp Worker", "temp@example.com", "+1-555-TEMP");
    assert(tempCrew != nullptr);
    int tempId = tempCrew->id;
    std::cout << "✓ Created temporary crew member for deletion test (ID: " << tempId << ")" << std::endl;
    
    // Verify it exists
    auto foundCrew = CrewManager::getCrewById(tempId);
    assert(foundCrew != nullptr);
    
    // Delete the crew member
    bool deleteResult = CrewManager::deleteCrewMember(tempId);
    assert(deleteResult == true);
    std::cout << "✓ Successfully deleted crew member" << std::endl;
    
    // Verify deletion
    auto deletedCrew = CrewManager::getCrewById(tempId);
    assert(deletedCrew == nullptr);
    std::cout << "✓ Crew member no longer exists after deletion" << std::endl;
    
    // Test deleting non-existent crew
    bool failDelete = CrewManager::deleteCrewMember(99999);
    assert(failDelete == false);
    std::cout << "✓ Correctly failed to delete non-existent crew" << std::endl;
    
    std::cout << "✓ Crew deletion tests passed!" << std::endl;
}

void testDataPersistence() {
    std::cout << "\n=== Testing Data Persistence ===" << std::endl;
    
    // Get current crew count
    const auto& currentCrew = CrewManager::getAllCrew();
    size_t crewCount = currentCrew.size();
    std::cout << "✓ Current crew count: " << crewCount << std::endl;
    
    // Create a new module instance to test loading
    CrewModule testModule;
    const auto& loadedCrew = testModule.getAllCrew();
    
    assert(loadedCrew.size() == crewCount);
    std::cout << "✓ Data persistence verified - loaded " << loadedCrew.size() << " crew members" << std::endl;
    
    // Verify crew details are preserved
    if (!loadedCrew.empty()) {
        auto firstCrew = loadedCrew[0];
        assert(!firstCrew->name.empty());
        assert(!firstCrew->email.empty());
        assert(!firstCrew->phone_number.empty());
        std::cout << "✓ Crew details preserved: " << firstCrew->name << std::endl;
        
        // Check if tasks are preserved
        if (!firstCrew->tasks.empty()) {
            auto firstTask = firstCrew->tasks[0];
            assert(!firstTask->task_name.empty());
            assert(!firstTask->description.empty());
            std::cout << "✓ Task details preserved: " << firstTask->task_name << std::endl;
        }
    }
    
    std::cout << "✓ Data persistence tests passed!" << std::endl;
}

void displayTestSummary() {
    std::cout << "\n=== Test Summary ===" << std::endl;
    const auto& allCrew = CrewManager::getAllCrew();
    
    std::cout << "Total crew members: " << allCrew.size() << std::endl;
    
    for (const auto& crew : allCrew) {
        std::cout << "\nCrew Member: " << crew->name << " (ID: " << crew->id << ")" << std::endl;
        std::cout << "  Email: " << crew->email << std::endl;
        std::cout << "  Phone: " << crew->phone_number << std::endl;
        std::cout << "  Job: " << CrewManager::getCrewJob(crew->id) << std::endl;
        std::cout << "  Tasks: " << crew->tasks.size() << std::endl;
        
        if (crew->check_in_time.has_value()) {
            std::cout << "  Check-in: " << crew->check_in_time->iso8601String << std::endl;
        }
        if (crew->check_out_time.has_value()) {
            std::cout << "  Check-out: " << crew->check_out_time->iso8601String << std::endl;
        }
        
        for (const auto& task : crew->tasks) {
            std::cout << "    Task: " << task->task_name;
            std::cout << " [Status: ";
            switch (task->status) {
                case Model::TaskStatus::TODO: std::cout << "TODO"; break;
                case Model::TaskStatus::IN_PROGRESS: std::cout << "IN_PROGRESS"; break;
                case Model::TaskStatus::COMPLETED: std::cout << "COMPLETED"; break;
            }
            std::cout << ", Priority: ";
            switch (task->priority) {
                case Model::TaskPriority::LOW: std::cout << "LOW"; break;
                case Model::TaskPriority::MEDIUM: std::cout << "MEDIUM"; break;
                case Model::TaskPriority::HIGH: std::cout << "HIGH"; break;
                case Model::TaskPriority::CRITICAL: std::cout << "CRITICAL"; break;
            }
            std::cout << "]" << std::endl;
        }
    }
}

int main() {
    std::cout << "Starting CrewModule Tests..." << std::endl;
    
    try {
        testCrewCreation();
        testCrewRetrieval();
        testCrewSearch();
        testJobManagement();
        testCrewUpdate();
        testTaskManagement();
        testTimeTracking();
        testAdvancedFeatures();
        testCrewDeletion();
        testDataPersistence();
        
        displayTestSummary();
        
        std::cout << "\n🎉 All CrewModule tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception!" << std::endl;
        return 1;
    }
    
    return 0;
}
