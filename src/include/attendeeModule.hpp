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
 * @brief Module for managing Attendee entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for attendee management including CRUD operations
 * and attendee-specific search capabilities.
 */
class AttendeeModule : public BaseModule<Model::Attendee> {

public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing attendees
     * @param filePath Path to the attendees data file
     */
    AttendeeModule(const std::string& filePath = "data/attendees.dat") : BaseModule<Model::Attendee>(filePath) {
        loadEntities();
    }
    
    /**
     * @brief Destructor
     * Ensures attendees are saved before destruction
     */
    ~AttendeeModule() override {
        saveEntities();
    }

    /**
     * @brief Create a new attendee
     * 
     * @param name Attendee name
     * @param email Email address
     * @param phone Phone number
     * @param type Attendee type (default: REGULAR)
     * @param username Optional username for authentication
     * @param password Optional password (would be hashed in production)
     * @param isStaff Whether the attendee has staff privileges
     * @return std::shared_ptr<Model::Attendee> Pointer to created attendee
     */
    std::shared_ptr<Model::Attendee> createAttendee(
        const std::string& name, 
        const std::string& email, 
        const std::string& phone, 
        Model::AttendeeType type = Model::AttendeeType::REGULAR,
        const std::string& username = "",
        const std::string& password = "",
        bool isStaff = false
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the attendee object
        auto regDate = Model::DateTime::now();
        std::string passwordHash = password; // In production, this would be hashed
        
        auto attendee = std::make_shared<Model::Attendee>(
            newId, name, email, phone, type, regDate, username, passwordHash, isStaff
        );
        
        // Add to the collection
        entities.push_back(attendee);
        
        // Save to file
        saveEntities();
        
        return attendee;
    }

    /**
     * @brief Get an attendee by ID
     * @param id Attendee ID to find
     * @return std::shared_ptr<Model::Attendee> Pointer to attendee or nullptr
     */
    std::shared_ptr<Model::Attendee> getAttendeeById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all attendees
     * @return const std::vector<std::shared_ptr<Model::Attendee>>& Reference to attendee vector
     */
    const std::vector<std::shared_ptr<Model::Attendee>>& getAllAttendees() const {
        return getAll();
    }

    /**
     * @brief Find attendees by partial name match (case insensitive)
     * @param nameQuery Partial name to search for
     * @return std::vector<std::shared_ptr<Model::Attendee>> Vector of matching attendees
     */
    std::vector<std::shared_ptr<Model::Attendee>> findAttendeesByName(const std::string& nameQuery) {
        return findByPredicate([&nameQuery](const std::shared_ptr<Model::Attendee>& attendee) {
            // Case insensitive search
            std::string attendeeName = attendee->name;
            std::string query = nameQuery;
            
            // Convert both to lowercase for comparison
            std::transform(attendeeName.begin(), attendeeName.end(), attendeeName.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return attendeeName.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Find attendee by exact email match
     * @param email Email to search for
     * @return std::shared_ptr<Model::Attendee> Matching attendee or nullptr
     */
    std::shared_ptr<Model::Attendee> findAttendeeByEmail(const std::string& email) {
        auto results = findByPredicate([&email](const std::shared_ptr<Model::Attendee>& attendee) {
            return attendee->email == email;
        });
        
        return results.empty() ? nullptr : results[0];
    }

    /**
     * @brief Update an attendee's information
     * 
     * @param id Attendee ID to update
     * @param name New name (if empty, no change)
     * @param email New email (if empty, no change)
     * @param phone New phone (if empty, no change)
     * @param type New attendee type
     * @return true if update successful
     * @return false if attendee not found or save failed
     */
    bool updateAttendee(int id, 
                        const std::string& name = "", 
                        const std::string& email = "", 
                        const std::string& phone = "",
                        Model::AttendeeType type = Model::AttendeeType::REGULAR) {
        auto attendee = getById(id);
        if (!attendee) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) attendee->name = name;
        if (!email.empty()) attendee->email = email;
        if (!phone.empty()) attendee->phone_number = phone;
        attendee->attendee_type = type;
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Delete an attendee
     * @param id Attendee ID to delete
     * @return true if deletion successful
     * @return false if attendee not found or save failed
     */
    bool deleteAttendee(int id) {
        return deleteEntity(id);
    }

protected:
    /**
     * @brief Get the ID of an attendee
     * Required by BaseModule
     * 
     * @param attendee Pointer to attendee
     * @return int Attendee ID
     */
    int getEntityId(const std::shared_ptr<Model::Attendee>& attendee) const override {
        return attendee->getId();
    }
    
    /**
     * @brief Load attendees from binary file
     * Required by BaseModule
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int attendeeCount = 0;
        readBinary(file, attendeeCount);
        
        for (int i = 0; i < attendeeCount; i++) {
            int id;
            readBinary(file, id);
            
            std::string name = readString(file);
            std::string email = readString(file);
            std::string phone = readString(file);
            
            int typeInt;
            readBinary(file, typeInt);
            Model::AttendeeType type = static_cast<Model::AttendeeType>(typeInt);
            
            Model::DateTime regDate;
            regDate.iso8601String = readString(file);
            
            std::string username = readString(file);
            std::string passwordHash = readString(file);
            
            bool isStaff;
            readBinary(file, isStaff);
            
            auto attendee = std::make_shared<Model::Attendee>(
                id, name, email, phone, type, regDate, username, passwordHash, isStaff
            );
            
            entities.push_back(attendee);
        }
        
        file.close();
    }
    
    /**
     * @brief Save attendees to binary file
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
        
        int attendeeCount = static_cast<int>(entities.size());
        writeBinary(file, attendeeCount);
        
        for (const auto& attendee : entities) {
            // Write ID
            writeBinary(file, attendee->id);
            
            writeString(file, attendee->name);
            writeString(file, attendee->email);
            writeString(file, attendee->phone_number);
            
            int typeInt = static_cast<int>(attendee->attendee_type);
            writeBinary(file, typeInt);
            
            writeString(file, attendee->registration_date.iso8601String);
            writeString(file, attendee->username);
            writeString(file, attendee->password_hash);
            
            writeBinary(file, attendee->staff_privileges);
        }
        
        file.close();
        return true;
    }
};
