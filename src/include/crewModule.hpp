#pragma once

#include "models.hpp"
#include "baseModule.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <sstream>

/**
 * @brief Module for managing Crew entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for crew management including CRUD operations,
 * task assignments, and check-in/check-out capabilities.
 */
class CrewModule : public BaseModule<Model::Crew> {

public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing crew members
     */
    CrewModule() : BaseModule<Model::Crew>("../../data/crews.dat") {
        loadEntities();
    }
    
    /**
     * @brief Destructor
     * Ensures crew members are saved before destruction
     */
    ~CrewModule() override {
        saveEntities();
    }

    /**
     * @brief Create a new crew member
     * 
     * @param name Crew member name
     * @param email Email address
     * @param phone Phone number
     * @return std::shared_ptr<Model::Crew> Pointer to created crew member
     */
    std::shared_ptr<Model::Crew> createCrewMember(
        const std::string& name, 
        const std::string& email, 
        const std::string& phone
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the crew member object
        auto crew = std::make_shared<Model::Crew>();
        crew->id = newId;
        crew->name = name;
        crew->email = email;
        crew->phone_number = phone;
        
        // Add to the collection
        entities.push_back(crew);
        
        // Save to file
        saveEntities();
        
        return crew;
    }

    /**
     * @brief Get a crew member by ID
     * @param id Crew member ID to find
     * @return std::shared_ptr<Model::Crew> Pointer to crew member or nullptr
     */
    std::shared_ptr<Model::Crew> getCrewMemberById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all crew members
     * @return const std::vector<std::shared_ptr<Model::Crew>>& Reference to crew vector
     */
    const std::vector<std::shared_ptr<Model::Crew>>& getAllCrewMembers() const {
        return getAll();
    }

    /**
     * @brief Find crew members by partial name match (case insensitive)
     * @param nameQuery Partial name to search for
     * @return std::vector<std::shared_ptr<Model::Crew>> Vector of matching crew members
     */
    std::vector<std::shared_ptr<Model::Crew>> findCrewMembersByName(const std::string& nameQuery) {
        return findByPredicate([&nameQuery](const std::shared_ptr<Model::Crew>& crew) {
            // Case insensitive search
            std::string crewName = crew->name;
            std::string query = nameQuery;
            
            // Convert both to lowercase for comparison
            std::transform(crewName.begin(), crewName.end(), crewName.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return crewName.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Find crew member by exact email match
     * @param email Email to search for
     * @return std::shared_ptr<Model::Crew> Matching crew member or nullptr
     */
    std::shared_ptr<Model::Crew> findCrewMemberByEmail(const std::string& email) {
        auto results = findByPredicate([&email](const std::shared_ptr<Model::Crew>& crew) {
            return crew->email == email;
        });
        
        return results.empty() ? nullptr : results[0];
    }

    /**
     * @brief Update a crew member's information
     * 
     * @param id Crew member ID to update
     * @param name New name (if empty, no change)
     * @param email New email (if empty, no change)
     * @param phone New phone (if empty, no change)
     * @return true if update successful
     * @return false if crew member not found or save failed
     */
    bool updateCrewMember(int id, 
                         const std::string& name = "", 
                         const std::string& email = "", 
                         const std::string& phone = "") {
        auto crew = getById(id);
        if (!crew) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) crew->name = name;
        if (!email.empty()) crew->email = email;
        if (!phone.empty()) crew->phone_number = phone;
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Delete a crew member
     * @param id Crew member ID to delete
     * @return true if deletion successful
     * @return false if crew member not found or save failed
     */
    bool deleteCrewMember(int id) {
        return deleteEntity(id);
    }

    /**
     * @brief Assign a task to a crew member
     * @param crewId ID of the crew member
     * @param taskName Name of the task
     * @param description Task description
     * @param priority Task priority
     * @return true if task assignment successful
     */
    bool assignTask(int crewId, const std::string& taskName, 
                   const std::string& description, 
                   Model::TaskPriority priority = Model::TaskPriority::MEDIUM) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        // Generate a new task ID
        int taskId = 1;
        for (const auto& task : crew->tasks) {
            if (task->task_id >= taskId) {
                taskId = task->task_id + 1;
            }
        }
        
        // Create the task
        auto task = std::make_shared<Model::Task>();
        task->task_id = taskId;
        task->task_name = taskName;
        task->description = description;
        task->status = Model::TaskStatus::TODO;
        task->priority = priority;
        
        // Add task to crew member
        crew->tasks.push_back(task);
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Update task status for a crew member
     * @param crewId ID of the crew member
     * @param taskId ID of the task
     * @param status New task status
     * @return true if update successful
     */
    bool updateTaskStatus(int crewId, int taskId, Model::TaskStatus status) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        // Find the task
        for (auto& task : crew->tasks) {
            if (task->task_id == taskId) {
                task->status = status;
                return saveEntities();
            }
        }
        
        return false; // Task not found
    }

    /**
     * @brief Check in a crew member
     * @param crewId ID of the crew member
     * @return true if check-in successful
     */
    bool checkInCrewMember(int crewId) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        crew->check_in_time = Model::DateTime::now();
        return saveEntities();
    }

    /**
     * @brief Check out a crew member
     * @param crewId ID of the crew member
     * @return true if check-out successful
     */
    bool checkOutCrewMember(int crewId) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        crew->check_out_time = Model::DateTime::now();
        return saveEntities();
    }

    /**
     * @brief Get all tasks for a crew member
     * @param crewId ID of the crew member
     * @return Vector of tasks for the crew member
     */
    std::vector<std::shared_ptr<Model::Task>> getCrewTasks(int crewId) {
        auto crew = getById(crewId);
        if (!crew) {
            return {};
        }
        
        return crew->tasks;
    }

protected:
    /**
     * @brief Get the ID of a crew member
     * Required by BaseModule
     * 
     * @param crew Pointer to crew member
     * @return int Crew member ID
     */
    int getEntityId(const std::shared_ptr<Model::Crew>& crew) const override {
        return crew->id;
    }
    
    /**
     * @brief Load crew members from binary file
     * Required by BaseModule
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int crewCount = 0;
        readBinary(file, crewCount);
        
        for (int i = 0; i < crewCount; i++) {
            auto crew = std::make_shared<Model::Crew>();
            
            readBinary(file, crew->id);
            crew->name = readString(file);
            crew->email = readString(file);
            crew->phone_number = readString(file);
            
            // Read check-in time (optional)
            bool hasCheckIn;
            readBinary(file, hasCheckIn);
            if (hasCheckIn) {
                Model::DateTime checkInTime;
                checkInTime.iso8601String = readString(file);
                crew->check_in_time = checkInTime;
            }
            
            // Read check-out time (optional)
            bool hasCheckOut;
            readBinary(file, hasCheckOut);
            if (hasCheckOut) {
                Model::DateTime checkOutTime;
                checkOutTime.iso8601String = readString(file);
                crew->check_out_time = checkOutTime;
            }
            
            // Read tasks
            int taskCount;
            readBinary(file, taskCount);
            for (int j = 0; j < taskCount; j++) {
                auto task = std::make_shared<Model::Task>();
                readBinary(file, task->task_id);
                task->task_name = readString(file);
                task->description = readString(file);
                
                int statusInt, priorityInt;
                readBinary(file, statusInt);
                readBinary(file, priorityInt);
                task->status = static_cast<Model::TaskStatus>(statusInt);
                task->priority = static_cast<Model::TaskPriority>(priorityInt);
                
                crew->tasks.push_back(task);
            }
            
            entities.push_back(crew);
        }
        
        file.close();
    }
    
    /**
     * @brief Save crew members to binary file
     * Required by BaseModule
     * 
     * @return true if save successful
     * @return false if file could not be opened
     */
    bool saveEntities() override {
        std::ofstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << dataFilePath << std::endl;
            return false;
        }
        
        int crewCount = static_cast<int>(entities.size());
        writeBinary(file, crewCount);
        
        for (const auto& crew : entities) {
            writeBinary(file, crew->id);
            writeString(file, crew->name);
            writeString(file, crew->email);
            writeString(file, crew->phone_number);
            
            // Write check-in time (optional)
            bool hasCheckIn = crew->check_in_time.has_value();
            writeBinary(file, hasCheckIn);
            if (hasCheckIn) {
                writeString(file, crew->check_in_time->iso8601String);
            }
            
            // Write check-out time (optional)
            bool hasCheckOut = crew->check_out_time.has_value();
            writeBinary(file, hasCheckOut);
            if (hasCheckOut) {
                writeString(file, crew->check_out_time->iso8601String);
            }
            
            // Write tasks
            int taskCount = static_cast<int>(crew->tasks.size());
            writeBinary(file, taskCount);
            for (const auto& task : crew->tasks) {
                writeBinary(file, task->task_id);
                writeString(file, task->task_name);
                writeString(file, task->description);
                
                int statusInt = static_cast<int>(task->status);
                int priorityInt = static_cast<int>(task->priority);
                writeBinary(file, statusInt);
                writeBinary(file, priorityInt);
            }
        }
        
        file.close();
        return true;
    }
};

// Namespace wrapper for simplified access
namespace CrewManager {
    // Static instance for singleton pattern
    static CrewModule& getInstance() {
        static CrewModule instance;
        return instance;
    }

    /**
     * @brief Create a new crew member
     */
    inline std::shared_ptr<Model::Crew> addCrewMember(
        const std::string& name, 
        const std::string& email, 
        const std::string& phone
    ) {
        return getInstance().createCrewMember(name, email, phone);
    }

    /**
     * @brief Get a crew member by ID
     */
    inline std::shared_ptr<Model::Crew> getCrewMemberById(int id) {
        return getInstance().getCrewMemberById(id);
    }

    /**
     * @brief Get all crew members
     */
    inline const std::vector<std::shared_ptr<Model::Crew>>& getAllCrewMembers() {
        return getInstance().getAllCrewMembers();
    }

    /**
     * @brief Find crew members by name
     */
    inline std::vector<std::shared_ptr<Model::Crew>> findCrewMembersByName(const std::string& nameQuery) {
        return getInstance().findCrewMembersByName(nameQuery);
    }

    /**
     * @brief Find crew member by email
     */
    inline std::shared_ptr<Model::Crew> findCrewMemberByEmail(const std::string& email) {
        return getInstance().findCrewMemberByEmail(email);
    }

    /**
     * @brief Update a crew member
     */
    inline bool updateCrewMember(int id, 
                                const std::string& name = "", 
                                const std::string& email = "", 
                                const std::string& phone = "") {
        return getInstance().updateCrewMember(id, name, email, phone);
    }

    /**
     * @brief Delete a crew member
     */
    inline bool removeCrewMember(int id) {
        return getInstance().deleteCrewMember(id);
    }

    /**
     * @brief Assign a task to a crew member
     */
    inline bool assignTaskToCrew(int crewId, const std::string& taskName, 
                                const std::string& description, 
                                Model::TaskPriority priority = Model::TaskPriority::MEDIUM) {
        return getInstance().assignTask(crewId, taskName, description, priority);
    }

    /**
     * @brief Update task status
     */
    inline bool updateTaskStatus(int crewId, int taskId, Model::TaskStatus status) {
        return getInstance().updateTaskStatus(crewId, taskId, status);
    }

    /**
     * @brief Check in a crew member
     */
    inline bool checkInCrew(int crewId) {
        return getInstance().checkInCrewMember(crewId);
    }

    /**
     * @brief Check out a crew member
     */
    inline bool checkOutCrew(int crewId) {
        return getInstance().checkOutCrewMember(crewId);
    }

    /**
     * @brief Get tasks for a crew member
     */
    inline std::vector<std::shared_ptr<Model::Task>> listCrewTasks(int crewId) {
        return getInstance().getCrewTasks(crewId);
    }

    /**
     * @brief List all crew members (for compatibility)
     */
    inline void listCrewMembers() {
        const auto& crews = getInstance().getAllCrewMembers();
        std::cout << "=== Crew Members ===" << std::endl;
        for (const auto& crew : crews) {
            std::cout << "ID: " << crew->id << ", Name: " << crew->name 
                     << ", Email: " << crew->email << ", Phone: " << crew->phone_number << std::endl;
        }
    }
}