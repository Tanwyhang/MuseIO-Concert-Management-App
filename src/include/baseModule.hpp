#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <functional>

/**
 * @brief Base template class for entity modules with common CRUD operations
 * 
 * This template class provides a foundation for all entity modules (Attendee, Performer, Venue, etc.)
 * with common CRUD functionality and binary serialization support.
 * 
 * @tparam EntityType The entity class type (e.g., Model::Attendee)
 * @tparam IdType The type used for entity IDs (default: int)
 */
template <typename EntityType, typename IdType = int>
class BaseModule {
protected:
    std::vector<std::shared_ptr<EntityType>> entities;
    std::string dataFilePath;

    // Pure virtual methods that derived classes must implement
    
    /**
     * @brief Get the ID of an entity
     * @param entity Pointer to the entity
     * @return The entity's ID
     */
    virtual IdType getEntityId(const std::shared_ptr<EntityType>& entity) const = 0;
    
    /**
     * @brief Load entities from storage
     */
    virtual void loadEntities() = 0;
    
    /**
     * @brief Save entities to storage
     * @return true if successful, false otherwise
     */
    virtual bool saveEntities() = 0;

public:
    /**
     * @brief Constructor
     * @param filePath Path to the data file for this entity type
     */
    BaseModule(const std::string& filePath) : dataFilePath(filePath) {}
    
    /**
     * @brief Virtual destructor
     */
    virtual ~BaseModule() = default;

    /**
     * @brief Get an entity by its ID
     * @param id The ID to search for
     * @return Shared pointer to the entity, or nullptr if not found
     */
    virtual std::shared_ptr<EntityType> getById(IdType id) {
        for (const auto& entity : entities) {
            if (getEntityId(entity) == id) {
                return entity;
            }
        }
        
        std::cerr << "Entity with ID " << id << " not found." << std::endl;
        return nullptr;
    }

    /**
     * @brief Get all entities
     * @return Constant reference to the vector of entities
     */
    virtual const std::vector<std::shared_ptr<EntityType>>& getAll() const {
        return entities;
    }

    /**
     * @brief Delete an entity by ID
     * @param id The ID of the entity to delete
     * @return true if successful, false if entity not found or save failed
     */
    virtual bool deleteEntity(IdType id) {
        auto it = std::find_if(entities.begin(), entities.end(),
            [this, id](const std::shared_ptr<EntityType>& entity) {
                return getEntityId(entity) == id;
            });
        
        if (it == entities.end()) {
            // Entity not found
            return false;
        }
        
        // Remove the entity
        entities.erase(it);
        
        // Save changes
        return saveEntities();
    }
    
    /**
     * @brief Find entities by a custom predicate
     * @param predicate Function that takes an entity and returns bool
     * @return Vector of entities that match the predicate
     */
    std::vector<std::shared_ptr<EntityType>> findByPredicate(std::function<bool(const std::shared_ptr<EntityType>&)> predicate) const {
        std::vector<std::shared_ptr<EntityType>> results;
        
        for (const auto& entity : entities) {
            if (predicate(entity)) {
                results.push_back(entity);
            }
        }
        
        return results;
    }

    /**
     * @brief Helper to generate a new unique ID
     * @return A new ID that is one higher than the current maximum
     */
    virtual IdType generateNewId() {
        IdType maxId = 0;
        
        for (const auto& entity : entities) {
            IdType id = getEntityId(entity);
            if (id > maxId) {
                maxId = id;
            }
        }
        
        return maxId + 1;
    }

protected:
    /**
     * @brief Helper template for reading binary data
     * @tparam T Data type to read
     * @param file Input file stream
     * @param value Reference to store the read value
     */
    template<typename T>
    void readBinary(std::ifstream& file, T& value) {
        file.read(reinterpret_cast<char*>(&value), sizeof(T));
    }
    
    /**
     * @brief Helper template for writing binary data
     * @tparam T Data type to write
     * @param file Output file stream
     * @param value The value to write
     */
    template<typename T>
    void writeBinary(std::ofstream& file, const T& value) {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }
    
    /**
     * @brief Read a string from binary file using size + data pattern
     * @param file Input file stream
     * @return The read string
     */
    std::string readString(std::ifstream& file) {
        size_t len;
        readBinary(file, len);
        
        std::string str(len, '\0');
        if (len > 0) {
            file.read(&str[0], len);
        }
        return str;
    }
    
    /**
     * @brief Write a string to binary file using size + data pattern
     * @param file Output file stream
     * @param str The string to write
     */
    void writeString(std::ofstream& file, const std::string& str) {
        size_t len = str.length();
        writeBinary(file, len);
        if (len > 0) {
            file.write(str.c_str(), len);
        }
    }
};


