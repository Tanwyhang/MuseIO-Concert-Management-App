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
#include <unordered_map>

/**
 * @brief Module for managing Crew entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for crew management including CRUD operations,
 * task assignments, job management, and crew-specific search capabilities.
 */
class CrewModule : public BaseModule<Model::Crew> {

private:
    // Map to store crew ID to job title mapping
    std::unordered_map<int, std::string> crewJobs;

public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing crew members
     */
    CrewModule() : BaseModule<Model::Crew>("data/crews.dat") {
        loadEntities();
    }
    
    /**
     * @brief Destructor
     * Ensures crew data is saved before destruction
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
     * @param job Job title/role (default: "General Staff")
     * @return std::shared_ptr<Model::Crew> Pointer to created crew member
     */
    std::shared_ptr<Model::Crew> createCrewMember(
        const std::string& name, 
        const std::string& email, 
        const std::string& phone,
        const std::string& job = "General Staff"
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the crew member object
        auto crew = std::make_shared<Model::Crew>();
        crew->id = newId;
        crew->name = name;
        crew->email = email;
        crew->phone_number = phone;
        
        // Store job mapping
        crewJobs[newId] = job;
        
        // Add to the collection
        entities.push_back(crew);
        
        // Save to file
        saveEntities();
        
        return crew;
    }

    /**
     * @brief Get a crew member by ID
     * @param id Crew ID to find
     * @return std::shared_ptr<Model::Crew> Pointer to crew member or nullptr
     */
    std::shared_ptr<Model::Crew> getCrewById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all crew members
     * @return const std::vector<std::shared_ptr<Model::Crew>>& Reference to crew vector
     */
    const std::vector<std::shared_ptr<Model::Crew>>& getAllCrew() const {
        return getAll();
    }

    /**
     * @brief Find crew members by partial name match (case insensitive)
     * @param nameQuery Partial name to search for
     * @return std::vector<std::shared_ptr<Model::Crew>> Vector of matching crew members
     */
    std::vector<std::shared_ptr<Model::Crew>> findCrewByName(const std::string& nameQuery) {
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
    std::shared_ptr<Model::Crew> findCrewByEmail(const std::string& email) {
        auto results = findByPredicate([&email](const std::shared_ptr<Model::Crew>& crew) {
            return crew->email == email;
        });
        
        return results.empty() ? nullptr : results[0];
    }

    /**
     * @brief Find crew members by job title (case insensitive)
     * @param jobQuery Job title to search for
     * @return std::vector<std::shared_ptr<Model::Crew>> Vector of matching crew members
     */
    std::vector<std::shared_ptr<Model::Crew>> findCrewByJob(const std::string& jobQuery) {
        return findByPredicate([&jobQuery, this](const std::shared_ptr<Model::Crew>& crew) {
            auto it = crewJobs.find(crew->id);
            if (it == crewJobs.end()) return false;
            
            // Case insensitive search
            std::string crewJob = it->second;
            std::string query = jobQuery;
            
            // Convert both to lowercase for comparison
            std::transform(crewJob.begin(), crewJob.end(), crewJob.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return crewJob.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Get job title for a crew member
     * @param crewId ID of the crew member
     * @return std::string Job title, or "Unknown" if not found
     */
    std::string getCrewJob(int crewId) {
        auto it = crewJobs.find(crewId);
        return (it != crewJobs.end()) ? it->second : "Unknown";
    }

    /**
     * @brief Set job title for a crew member
     * @param crewId ID of the crew member
     * @param job New job title
     * @return true if successful, false if crew not found
     */
    bool setCrewJob(int crewId, const std::string& job) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        crewJobs[crewId] = job;
        return saveEntities(); // Save to persist the job mapping
    }

    /**
     * @brief Update a crew member's information
     * 
     * @param id Crew ID to update
     * @param name New name (if empty, no change)
     * @param email New email (if empty, no change)
     * @param phone New phone (if empty, no change)
     * @param job New job title (if empty, no change)
     * @return true if update successful
     * @return false if crew member not found or save failed
     */
    bool updateCrewMember(int id, 
                         const std::string& name = "", 
                         const std::string& email = "", 
                         const std::string& phone = "",
                         const std::string& job = "") {
        auto crew = getById(id);
        if (!crew) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) crew->name = name;
        if (!email.empty()) crew->email = email;
        if (!phone.empty()) crew->phone_number = phone;
        if (!job.empty()) crewJobs[id] = job;
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Assign a task to a crew member
     * 
     * @param crewId ID of the crew member
     * @param taskName Name of the task
     * @param description Task description
     * @param priority Task priority (default: MEDIUM)
     * @return true if assignment successful
     * @return false if crew member not found or save failed
     */
    bool assignTaskToCrew(int crewId, 
                         const std::string& taskName,
                         const std::string& description,
                         Model::TaskPriority priority = Model::TaskPriority::MEDIUM) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        // Generate new task ID
        int taskId = generateNewTaskId(crew);
        
        // Create new task
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
     * 
     * @param crewId ID of the crew member
     * @param taskId ID of the task
     * @param status New task status
     * @return true if update successful
     * @return false if crew member or task not found or save failed
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
     * 
     * @param crewId ID of the crew member
     * @return true if check-in successful
     * @return false if crew member not found or save failed
     */
    bool checkInCrew(int crewId) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        crew->check_in_time = Model::DateTime::now();
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Check out a crew member
     * 
     * @param crewId ID of the crew member
     * @return true if check-out successful
     * @return false if crew member not found or save failed
     */
    bool checkOutCrew(int crewId) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        crew->check_out_time = Model::DateTime::now();
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Get all tasks for a specific crew member
     * 
     * @param crewId ID of the crew member
     * @return std::vector<std::shared_ptr<Model::Task>> Vector of tasks (empty if crew not found)
     */
    std::vector<std::shared_ptr<Model::Task>> getCrewTasks(int crewId) {
        auto crew = getById(crewId);
        if (!crew) {
            return {};
        }
        
        return crew->tasks;
    }

    /**
     * @brief Get crew members by task status
     * 
     * @param status Task status to filter by
     * @return std::vector<std::shared_ptr<Model::Crew>> Vector of crew members with tasks of specified status
     */
    std::vector<std::shared_ptr<Model::Crew>> getCrewByTaskStatus(Model::TaskStatus status) {
        return findByPredicate([status](const std::shared_ptr<Model::Crew>& crew) {
            for (const auto& task : crew->tasks) {
                if (task->status == status) {
                    return true;
                }
            }
            return false;
        });
    }

    /**
     * @brief Delete a crew member
     * @param id Crew ID to delete
     * @return true if deletion successful
     * @return false if crew member not found or save failed
     */
    bool deleteCrewMember(int id) {
        // Remove job mapping when deleting crew member
        crewJobs.erase(id);
        return deleteEntity(id);
    }

    /**
     * @brief Remove a specific task from a crew member
     * 
     * @param crewId ID of the crew member
     * @param taskId ID of the task to remove
     * @return true if removal successful
     * @return false if crew member or task not found or save failed
     */
    bool removeTaskFromCrew(int crewId, int taskId) {
        auto crew = getById(crewId);
        if (!crew) {
            return false;
        }
        
        // Find and remove the task
        auto it = std::find_if(crew->tasks.begin(), crew->tasks.end(),
            [taskId](const std::shared_ptr<Model::Task>& task) {
                return task->task_id == taskId;
            });
        
        if (it != crew->tasks.end()) {
            crew->tasks.erase(it);
            return saveEntities();
        }
        
        return false; // Task not found
    }

protected:
    /**
     * @brief Get the ID of a crew member
     * Required by BaseModule
     * 
     * @param crew Pointer to crew member
     * @return int Crew ID
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
            
            // Read basic crew info
            readBinary(file, crew->id);
            crew->name = readString(file);
            crew->email = readString(file);
            crew->phone_number = readString(file);
            
            // Read check-in time
            bool hasCheckIn;
            readBinary(file, hasCheckIn);
            if (hasCheckIn) {
                Model::DateTime checkInTime;
                checkInTime.iso8601String = readString(file);
                crew->check_in_time = checkInTime;
            }
            
            // Read check-out time
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
                
                int statusInt;
                readBinary(file, statusInt);
                task->status = static_cast<Model::TaskStatus>(statusInt);
                
                int priorityInt;
                readBinary(file, priorityInt);
                task->priority = static_cast<Model::TaskPriority>(priorityInt);
                
                crew->tasks.push_back(task);
            }
            
            entities.push_back(crew);
        }
        
        // Read job mappings
        int jobCount = 0;
        readBinary(file, jobCount);
        crewJobs.clear();
        
        for (int i = 0; i < jobCount; i++) {
            int crewId;
            readBinary(file, crewId);
            std::string job = readString(file);
            crewJobs[crewId] = job;
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
            // Write basic crew info
            writeBinary(file, crew->id);
            writeString(file, crew->name);
            writeString(file, crew->email);
            writeString(file, crew->phone_number);
            
            // Write check-in time
            bool hasCheckIn = crew->check_in_time.has_value();
            writeBinary(file, hasCheckIn);
            if (hasCheckIn) {
                writeString(file, crew->check_in_time->iso8601String);
            }
            
            // Write check-out time
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
                writeBinary(file, statusInt);
                
                int priorityInt = static_cast<int>(task->priority);
                writeBinary(file, priorityInt);
            }
        }
        
        // Write job mappings
        int jobCount = static_cast<int>(crewJobs.size());
        writeBinary(file, jobCount);
        
        for (const auto& jobPair : crewJobs) {
            writeBinary(file, jobPair.first);  // crew ID
            writeString(file, jobPair.second); // job title
        }
        
        file.close();
        return true;
    }

private:
    /**
     * @brief Generate a new unique task ID for a crew member
     * @param crew The crew member to generate task ID for
     * @return int New unique task ID
     */
    int generateNewTaskId(const std::shared_ptr<Model::Crew>& crew) {
        int maxTaskId = 0;
        
        for (const auto& task : crew->tasks) {
            if (task->task_id > maxTaskId) {
                maxTaskId = task->task_id;
            }
        }
        
        return maxTaskId + 1;
    }
};

/**
 * @brief Namespace wrapper for CrewModule providing simplified access
 * 
 * This namespace provides static functions that wrap a singleton CrewModule instance,
 * following the dual-layer API pattern used throughout the system.
 */
namespace CrewManager {
    
    // Singleton instance
    static CrewModule& getInstance() {
        static CrewModule instance;
        return instance;
    }
    
    /**
     * @brief Create a new crew member
     */
    inline std::shared_ptr<Model::Crew> createCrewMember(const std::string& name, const std::string& email, const std::string& phone, const std::string& job = "General Staff") {
        return getInstance().createCrewMember(name, email, phone, job);
    }
    
    /**
     * @brief Get crew member by ID
     */
    inline std::shared_ptr<Model::Crew> getCrewById(int id) {
        return getInstance().getCrewById(id);
    }
    
    /**
     * @brief Get all crew members
     */
    inline const std::vector<std::shared_ptr<Model::Crew>>& getAllCrew() {
        return getInstance().getAllCrew();
    }
    
    /**
     * @brief Find crew members by name
     */
    inline std::vector<std::shared_ptr<Model::Crew>> findCrewByName(const std::string& nameQuery) {
        return getInstance().findCrewByName(nameQuery);
    }
    
    /**
     * @brief Find crew member by email
     */
    inline std::shared_ptr<Model::Crew> findCrewByEmail(const std::string& email) {
        return getInstance().findCrewByEmail(email);
    }
    
    /**
     * @brief Update crew member information
     */
    inline bool updateCrewMember(int id, const std::string& name = "", const std::string& email = "", const std::string& phone = "", const std::string& job = "") {
        return getInstance().updateCrewMember(id, name, email, phone, job);
    }
    
    /**
     * @brief Find crew members by job title
     */
    inline std::vector<std::shared_ptr<Model::Crew>> findCrewByJob(const std::string& jobQuery) {
        return getInstance().findCrewByJob(jobQuery);
    }
    
    /**
     * @brief Get job title for a crew member
     */
    inline std::string getCrewJob(int crewId) {
        return getInstance().getCrewJob(crewId);
    }
    
    /**
     * @brief Set job title for a crew member
     */
    inline bool setCrewJob(int crewId, const std::string& job) {
        return getInstance().setCrewJob(crewId, job);
    }
    
    /**
     * @brief Assign task to crew member
     */
    inline bool assignTaskToCrew(int crewId, const std::string& taskName, const std::string& description, Model::TaskPriority priority = Model::TaskPriority::MEDIUM) {
        return getInstance().assignTaskToCrew(crewId, taskName, description, priority);
    }
    
    /**
     * @brief Update task status
     */
    inline bool updateTaskStatus(int crewId, int taskId, Model::TaskStatus status) {
        return getInstance().updateTaskStatus(crewId, taskId, status);
    }
    
    /**
     * @brief Check in crew member
     */
    inline bool checkInCrew(int crewId) {
        return getInstance().checkInCrew(crewId);
    }
    
    /**
     * @brief Check out crew member
     */
    inline bool checkOutCrew(int crewId) {
        return getInstance().checkOutCrew(crewId);
    }
    
    /**
     * @brief Get crew tasks
     */
    inline std::vector<std::shared_ptr<Model::Task>> getCrewTasks(int crewId) {
        return getInstance().getCrewTasks(crewId);
    }
    
    /**
     * @brief Get crew by task status
     */
    inline std::vector<std::shared_ptr<Model::Crew>> getCrewByTaskStatus(Model::TaskStatus status) {
        return getInstance().getCrewByTaskStatus(status);
    }
    
    /**
     * @brief Delete crew member
     */
    inline bool deleteCrewMember(int id) {
        return getInstance().deleteCrewMember(id);
    }
    
    /**
     * @brief Remove task from crew
     */
    inline bool removeTaskFromCrew(int crewId, int taskId) {
        return getInstance().removeTaskFromCrew(crewId, taskId);
    }
}