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
 * @brief Module for managing Performer entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for performer management including CRUD operations
 * and performer-specific search capabilities.
 */
class PerformerModule : public BaseModule<Model::Performer> {

public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing performers
     * @param filePath Path to the performers data file
     */
    PerformerModule(const std::string& filePath = "data/performers.dat") : BaseModule<Model::Performer>(filePath) {
        loadEntities();
    }
    
    /**
     * @brief Destructor
     * Ensures performers are saved before destruction
     */
    ~PerformerModule() override {
        saveEntities();
    }

    /**
     * @brief Create a new performer
     * 
     * @param name Performer name
     * @param type Performer type (e.g., "Solo Artist", "Band", "DJ", etc.)
     * @param contactInfo Contact information
     * @param bio Biography/description
     * @param imageUrl URL to performer's image (optional)
     * @return std::shared_ptr<Model::Performer> Pointer to created performer
     */
    std::shared_ptr<Model::Performer> createPerformer(
        const std::string& name, 
        const std::string& type, 
        const std::string& contactInfo, 
        const std::string& bio,
        const std::string& imageUrl = ""
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the performer object
        auto performer = std::make_shared<Model::Performer>();
        performer->performer_id = newId;
        performer->name = name;
        performer->type = type;
        performer->contact_info = contactInfo;
        performer->bio = bio;
        performer->image_url = imageUrl;
        
        // Add to the collection
        entities.push_back(performer);
        
        // Save to file
        saveEntities();
        
        return performer;
    }

    /**
     * @brief Get a performer by ID
     * @param id Performer ID to find
     * @return std::shared_ptr<Model::Performer> Pointer to performer or nullptr
     */
    std::shared_ptr<Model::Performer> getPerformerById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all performers
     * @return const std::vector<std::shared_ptr<Model::Performer>>& Reference to performer vector
     */
    const std::vector<std::shared_ptr<Model::Performer>>& getAllPerformers() const {
        return getAll();
    }

    /**
     * @brief Find performers by partial name match (case insensitive)
     * @param nameQuery Partial name to search for
     * @return std::vector<std::shared_ptr<Model::Performer>> Vector of matching performers
     */
    std::vector<std::shared_ptr<Model::Performer>> findPerformersByName(const std::string& nameQuery) {
        return findByPredicate([&nameQuery](const std::shared_ptr<Model::Performer>& performer) {
            // Case insensitive search
            std::string performerName = performer->name;
            std::string query = nameQuery;
            
            // Convert both to lowercase for comparison
            std::transform(performerName.begin(), performerName.end(), performerName.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return performerName.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Find performers by type (case insensitive)
     * @param typeQuery Type to search for (e.g., "Band", "Solo Artist", "DJ")
     * @return std::vector<std::shared_ptr<Model::Performer>> Vector of matching performers
     */
    std::vector<std::shared_ptr<Model::Performer>> findPerformersByType(const std::string& typeQuery) {
        return findByPredicate([&typeQuery](const std::shared_ptr<Model::Performer>& performer) {
            // Case insensitive search
            std::string performerType = performer->type;
            std::string query = typeQuery;
            
            // Convert both to lowercase for comparison
            std::transform(performerType.begin(), performerType.end(), performerType.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return performerType.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Update a performer's information
     * 
     * @param id Performer ID to update
     * @param name New name (if empty, no change)
     * @param type New type (if empty, no change)
     * @param contactInfo New contact info (if empty, no change)
     * @param bio New bio (if empty, no change)
     * @param imageUrl New image URL (if empty, no change)
     * @return true if update successful
     * @return false if performer not found or save failed
     */
    bool updatePerformer(int id, 
                        const std::string& name = "", 
                        const std::string& type = "", 
                        const std::string& contactInfo = "",
                        const std::string& bio = "",
                        const std::string& imageUrl = "") {
        auto performer = getById(id);
        if (!performer) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) performer->name = name;
        if (!type.empty()) performer->type = type;
        if (!contactInfo.empty()) performer->contact_info = contactInfo;
        if (!bio.empty()) performer->bio = bio;
        if (!imageUrl.empty()) performer->image_url = imageUrl;
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Delete a performer
     * @param id Performer ID to delete
     * @return true if deletion successful
     * @return false if performer not found or save failed
     */
    bool deletePerformer(int id) {
        return deleteEntity(id);
    }

protected:
    /**
     * @brief Get the ID of a performer
     * Required by BaseModule
     * 
     * @param performer Pointer to performer
     * @return int Performer ID
     */
    int getEntityId(const std::shared_ptr<Model::Performer>& performer) const override {
        return performer->performer_id;
    }
    
    /**
     * @brief Load performers from binary file
     * Required by BaseModule
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int performerCount = 0;
        readBinary(file, performerCount);
        
        for (int i = 0; i < performerCount; i++) {
            auto performer = std::make_shared<Model::Performer>();
            
            readBinary(file, performer->performer_id);
            performer->name = readString(file);
            performer->type = readString(file);
            performer->contact_info = readString(file);
            performer->bio = readString(file);
            performer->image_url = readString(file);
            
            entities.push_back(performer);
        }
        
        file.close();
    }
    
    /**
     * @brief Save performers to binary file
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
        
        int performerCount = static_cast<int>(entities.size());
        writeBinary(file, performerCount);
        
        for (const auto& performer : entities) {
            writeBinary(file, performer->performer_id);
            writeString(file, performer->name);
            writeString(file, performer->type);
            writeString(file, performer->contact_info);
            writeString(file, performer->bio);
            writeString(file, performer->image_url);
        }
        
        file.close();
        return true;
    }
};

/**
 * @brief Namespace wrapper for PerformerModule providing static functions
 * 
 * This provides the dual-layer API pattern used throughout the system.
 * Functions wrap a singleton instance of PerformerModule.
 */
namespace PerformerManager {
    
    // Singleton instance
    static PerformerModule& getInstance() {
        static PerformerModule instance;
        return instance;
    }
    
    /**
     * @brief Create a new performer
     */
    inline std::shared_ptr<Model::Performer> createPerformer(
        const std::string& name, 
        const std::string& type, 
        const std::string& contactInfo, 
        const std::string& bio,
        const std::string& imageUrl = ""
    ) {
        return getInstance().createPerformer(name, type, contactInfo, bio, imageUrl);
    }
    
    /**
     * @brief Get a performer by ID
     */
    inline std::shared_ptr<Model::Performer> getPerformerById(int id) {
        return getInstance().getPerformerById(id);
    }
    
    /**
     * @brief Get all performers
     */
    inline const std::vector<std::shared_ptr<Model::Performer>>& getAllPerformers() {
        return getInstance().getAllPerformers();
    }
    
    /**
     * @brief Find performers by name
     */
    inline std::vector<std::shared_ptr<Model::Performer>> findPerformersByName(const std::string& nameQuery) {
        return getInstance().findPerformersByName(nameQuery);
    }
    
    /**
     * @brief Find performers by type
     */
    inline std::vector<std::shared_ptr<Model::Performer>> findPerformersByType(const std::string& typeQuery) {
        return getInstance().findPerformersByType(typeQuery);
    }
    
    /**
     * @brief Update a performer
     */
    inline bool updatePerformer(int id, 
                               const std::string& name = "", 
                               const std::string& type = "", 
                               const std::string& contactInfo = "",
                               const std::string& bio = "",
                               const std::string& imageUrl = "") {
        return getInstance().updatePerformer(id, name, type, contactInfo, bio, imageUrl);
    }
    
    /**
     * @brief Delete a performer
     */
    inline bool deletePerformer(int id) {
        return getInstance().deletePerformer(id);
    }
}
