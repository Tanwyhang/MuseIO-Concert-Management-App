#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include "models.h"
#include "serializationUtils.h"

// Class for handling data persistence operations
class DataManager {
private:
    SerializationManager serializer;
    
    // Object caches
    std::vector<std::shared_ptr<Model::Concert>> concerts;
    std::vector<std::shared_ptr<Model::Venue>> venues;
    std::vector<std::shared_ptr<Model::Performer>> performers;
    std::vector<std::shared_ptr<Model::Attendee>> attendees;
    // Add more collections as needed
    
    // File names for different object types
    const std::string CONCERTS_FILE = "concerts.dat";
    const std::string VENUES_FILE = "venues.dat";
    const std::string PERFORMERS_FILE = "performers.dat";
    const std::string ATTENDEES_FILE = "attendees.dat";
    // Add more file names as needed
    
    // Helper method to get the full file path
    std::string getFilePath(const std::string& fileName) const {
        return serializer.getFilePath(fileName);
    }
    
public:
    // Constructor
    DataManager(const std::string& dataDir = "data") : serializer(dataDir) {}
    
    // Load all data from files
    bool loadAllData() {
        bool success = true;
        
        // Load each data type
        if (!loadConcerts()) success = false;
        if (!loadVenues()) success = false;
        if (!loadPerformers()) success = false;
        if (!loadAttendees()) success = false;
        // Add more as needed
        
        return success;
    }
    
    // Save all data to files
    bool saveAllData() {
        bool success = true;
        
        // Save each data type
        if (!saveConcerts()) success = false;
        if (!saveVenues()) success = false;
        if (!savePerformers()) success = false;
        if (!saveAttendees()) success = false;
        // Add more as needed
        
        return success;
    }
    
    // Save a collection of objects to a .dat file
    template<typename T>
    bool saveData(const std::vector<std::shared_ptr<T>>& items, const std::string& fileName) {
        return serializer.saveData(items, fileName);
    }
    
    // Load a collection of objects from a .dat file
    template<typename T>
    bool loadData(std::vector<std::shared_ptr<T>>& items, const std::string& fileName) {
        return serializer.loadData(items, fileName);
    }
    
    // Concert operations
    bool loadConcerts() {
        return loadData(concerts, CONCERTS_FILE);
    }
    
    bool saveConcerts() {
        return saveData(concerts, CONCERTS_FILE);
    }
    
    std::shared_ptr<Model::Concert> getConcert(int id) {
        for (auto& concert : concerts) {
            if (concert->getId() == id) {
                return concert;
            }
        }
        return nullptr;
    }
    
    void addConcert(std::shared_ptr<Model::Concert> concert) {
        concerts.push_back(concert);
    }
    
    const std::vector<std::shared_ptr<Model::Concert>>& getAllConcerts() {
        return concerts;
    }
    
    // Venue operations
    bool loadVenues() {
        return loadData(venues, VENUES_FILE);
    }
    
    bool saveVenues() {
        return saveData(venues, VENUES_FILE);
    }
    
    std::shared_ptr<Model::Venue> getVenue(int id) {
        for (auto& venue : venues) {
            if (venue->getId() == id) {
                return venue;
            }
        }
        return nullptr;
    }
    
    void addVenue(std::shared_ptr<Model::Venue> venue) {
        venues.push_back(venue);
    }
    
    const std::vector<std::shared_ptr<Model::Venue>>& getAllVenues() {
        return venues;
    }
    
    // Performer operations
    bool loadPerformers() {
        return loadData(performers, PERFORMERS_FILE);
    }
    
    bool savePerformers() {
        return saveData(performers, PERFORMERS_FILE);
    }

    std::shared_ptr<Model::Performer> getPerformer(int id) {
        for (auto& performer : performers) {
            if (performer->getId() == id) {
                return performer;
            }
        }
        return nullptr;
    }
    
    void addPerformer(std::shared_ptr<Model::Performer> performer) {
        performers.push_back(performer);
    }
    
    const std::vector<std::shared_ptr<Model::Performer>>& getAllPerformers() {
        return performers;
    }
    
    // Attendee operations
    bool loadAttendees() {
        return loadData(attendees, ATTENDEES_FILE);
    }
    
    bool saveAttendees() {
        return saveData(attendees, ATTENDEES_FILE);
    }
    
    std::shared_ptr<Model::Attendee> getAttendee(int id) {
        for (auto& attendee : attendees) {
            if (attendee->getId() == id) {
                return attendee;
            }
        }
        return nullptr;
    }
    
    void addAttendee(std::shared_ptr<Model::Attendee> attendee) {
        attendees.push_back(attendee);
    }
    
    const std::vector<std::shared_ptr<Model::Attendee>>& getAllAttendees() {
        return attendees;
    }
    
    // Check if a data file exists
    bool fileExists(const std::string& fileName) {
        std::string filePath = getFilePath(fileName);
        return std::filesystem::exists(filePath);
    }
    
    // Delete a data file
    bool deleteFile(const std::string& fileName) {
        std::string filePath = getFilePath(fileName);
        try {
            return std::filesystem::remove(filePath);
        }
        catch (const std::exception& e) {
            std::cerr << "Error deleting file: " << e.what() << std::endl;
            return false;
        }
    }
};

#endif // DATA_MANAGER_H
