#pragma once

#include "models.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <sstream>

class AttendeeModule {
private:
    std::vector<std::shared_ptr<Model::Attendee>> attendees;
    std::string dataFilePath = "../../data/attendees.dat";

public:
    // Constructor that loads attendees from file
    AttendeeModule() {
        loadAttendees();
    }
    
    // Destructor to ensure data is saved
    ~AttendeeModule() {
        saveAttendees();
    }

    // Create a new attendee (C in CRUD)
    std::shared_ptr<Model::Attendee> createAttendee(
        const std::string& name, 
        const std::string& email, 
        const std::string& phone, 
        Model::AttendeeType type = Model::AttendeeType::REGULAR,
        const std::string& username = "",
        const std::string& password = "",
        bool isStaff = false
    ) {
        // Generate a new ID (simple implementation - in production would use better strategy)
        int newId = generateNewId();
        
        // Create the attendee object
        auto regDate = Model::DateTime::now();
        std::string passwordHash = password; // In production, this would be hashed
        
        auto attendee = std::make_shared<Model::Attendee>(
            newId, name, email, phone, type, regDate, username, passwordHash, isStaff
        );
        
        // Add to the collection
        attendees.push_back(attendee);
        
        // Save to file
        saveAttendees();
        
        return attendee;
    }

    // Read an attendee by ID (R in CRUD)
    std::shared_ptr<Model::Attendee> getAttendeeById(int id) {
        for (const auto& attendee : attendees) {
            if (attendee->getId() == id) {
                return attendee;
            }
        }
        
        std::cerr << "Attendee with ID " << id << " not found." << std::endl;
        return nullptr;
    }

    // Read all attendees (R in CRUD)
    const std::vector<std::shared_ptr<Model::Attendee>>& getAllAttendees() const {
        return attendees;
    }

    // Find attendees by name (R in CRUD - search functionality)
    std::vector<std::shared_ptr<Model::Attendee>> findAttendeesByName(const std::string& nameQuery) {
        std::vector<std::shared_ptr<Model::Attendee>> results;
        
        for (const auto& attendee : attendees) {
            // Case insensitive search
            std::string attendeeName = attendee->name;
            std::string query = nameQuery;
            
            // Convert both to lowercase for comparison
            std::transform(attendeeName.begin(), attendeeName.end(), attendeeName.begin(), 
                       [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                       [](unsigned char c){ return std::tolower(c); });
            
            if (attendeeName.find(query) != std::string::npos) {
                results.push_back(attendee);
            }
        }
        
        return results;
    }

    // Find attendee by email (R in CRUD - search functionality)
    std::shared_ptr<Model::Attendee> findAttendeeByEmail(const std::string& email) {
        for (const auto& attendee : attendees) {
            if (attendee->email == email) {
                return attendee;
            }
        }
        
        return nullptr;
    }

    // Update an attendee (U in CRUD)
    bool updateAttendee(int id, 
                       const std::string& name = "", 
                       const std::string& email = "", 
                       const std::string& phone = "",
                       Model::AttendeeType type = Model::AttendeeType::REGULAR) {
        auto attendee = getAttendeeById(id);
        if (!attendee) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) attendee->name = name;
        if (!email.empty()) attendee->email = email;
        if (!phone.empty()) attendee->phone_number = phone;
        attendee->attendee_type = type;
        
        // Save changes
        return saveAttendees();
    }

    // Delete an attendee (D in CRUD)
    bool deleteAttendee(int id) {
        auto it = std::find_if(attendees.begin(), attendees.end(),
            [id](const std::shared_ptr<Model::Attendee>& a) { return a->getId() == id; });
        
        if (it == attendees.end()) {
            // Attendee not found
            return false;
        }
        
        // Remove the attendee
        attendees.erase(it);
        
        // Save changes
        return saveAttendees();
    }

private:
    // Helper function to generate a new unique ID
    int generateNewId() {
        int maxId = 0;
        
        for (const auto& attendee : attendees) {
            if (attendee->getId() > maxId) {
                maxId = attendee->getId();
            }
        }
        
        return maxId + 1;
    }

    // Load attendees from file using binary storage
    void loadAttendees() {
        attendees.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int attendeeCount = 0;
        file.read(reinterpret_cast<char*>(&attendeeCount), sizeof(int));
        
        for (int i = 0; i < attendeeCount; i++) {
            int id;
            file.read(reinterpret_cast<char*>(&id), sizeof(int));
            
            // Read strings using size + data pattern
            auto readString = [&file]() {
                size_t len;
                file.read(reinterpret_cast<char*>(&len), sizeof(size_t));
                std::string str(len, '\0');
                if (len > 0) {
                    file.read(&str[0], len);
                }
                return str;
            };
            
            std::string name = readString();
            std::string email = readString();
            std::string phone = readString();
            
            int typeInt;
            file.read(reinterpret_cast<char*>(&typeInt), sizeof(int));
            Model::AttendeeType type = static_cast<Model::AttendeeType>(typeInt);
            
            Model::DateTime regDate;
            regDate.iso8601String = readString();
            
            std::string username = readString();
            std::string passwordHash = readString();
            
            bool isStaff;
            file.read(reinterpret_cast<char*>(&isStaff), sizeof(bool));
            
            auto attendee = std::make_shared<Model::Attendee>(
                id, name, email, phone, type, regDate, username, passwordHash, isStaff
            );
            
            attendees.push_back(attendee);
        }
        
        file.close();
    }
    
    // Save attendees to file using binary storage
    bool saveAttendees() {
        std::ofstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << dataFilePath << std::endl;
            return false;
        }
        
        int attendeeCount = static_cast<int>(attendees.size());
        file.write(reinterpret_cast<const char*>(&attendeeCount), sizeof(int));
        
        for (const auto& attendee : attendees) {
            // Write ID
            file.write(reinterpret_cast<const char*>(&attendee->id), sizeof(int));
            
            // Helper lambda for writing strings
            auto writeString = [&file](const std::string& str) {
                size_t len = str.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
                if (len > 0) {
                    file.write(str.c_str(), len);
                }
            };
            
            writeString(attendee->name);
            writeString(attendee->email);
            writeString(attendee->phone_number);
            
            int typeInt = static_cast<int>(attendee->attendee_type);
            file.write(reinterpret_cast<const char*>(&typeInt), sizeof(int));
            
            writeString(attendee->registration_date.iso8601String);
            writeString(attendee->username);
            writeString(attendee->password_hash);
            
            file.write(reinterpret_cast<const char*>(&attendee->staff_privileges), sizeof(bool));
        }
        
        file.close();
        return true;
    }
};
