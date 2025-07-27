#ifndef SERIALIZATION_UTILS_H
#define SERIALIZATION_UTILS_H

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>
#include <type_traits>

namespace MuseIO {

// Forward declarations
class DateTime;

// Helper class for string serialization
class StringSerializer {
public:
    static void serialize(std::ofstream& out, const std::string& str) {
        size_t length = str.length();
        out.write(reinterpret_cast<const char*>(&length), sizeof(length));
        out.write(str.c_str(), length);
    }
    
    static std::string deserialize(std::ifstream& in) {
        size_t length;
        in.read(reinterpret_cast<char*>(&length), sizeof(length));
        
        char* buffer = new char[length + 1];
        in.read(buffer, length);
        buffer[length] = '\0';
        
        std::string result(buffer);
        delete[] buffer;
        
        return result;
    }
};

// Template for serializing any enum type
template<typename EnumType>
class EnumSerializer {
public:
    static void serialize(std::ofstream& out, EnumType value) {
        using UnderlyingType = typename std::underlying_type<EnumType>::type;
        UnderlyingType raw = static_cast<UnderlyingType>(value);
        out.write(reinterpret_cast<const char*>(&raw), sizeof(raw));
    }
    
    static EnumType deserialize(std::ifstream& in) {
        using UnderlyingType = typename std::underlying_type<EnumType>::type;
        UnderlyingType raw;
        in.read(reinterpret_cast<char*>(&raw), sizeof(raw));
        return static_cast<EnumType>(raw);
    }
};

// Forward declaration
struct DateTime;

// DateTime serialization helper
class DateTimeSerializer {
public:
    static void serialize(std::ofstream& out, const DateTime& dt);
    static void deserialize(std::ifstream& in, DateTime& dt);
};

// Collection serialization helper
template<typename T>
class CollectionSerializer {
public:
    // Serialize a collection of serializable objects
    static void serialize(std::ofstream& out, const std::vector<std::shared_ptr<T>>& items) {
        size_t count = items.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        
        for (const auto& item : items) {
            if (item) {
                item->serialize(out);
            }
        }
    }
    
    // Deserialize a collection of serializable objects
    static void deserialize(std::ifstream& in, std::vector<std::shared_ptr<T>>& items) {
        items.clear();
        
        size_t count;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        
        for (size_t i = 0; i < count; i++) {
            auto item = std::make_shared<T>();
            item->deserialize(in);
            items.push_back(item);
        }
    }
};

// Main serialization manager
class SerializationManager {
private:
    std::string data_directory;
    
    // Helper method to ensure the data directory exists
    void ensureDataDirectoryExists() {
        if (!std::filesystem::exists(data_directory)) {
            std::filesystem::create_directories(data_directory);
        }
    }
    
public:
    // Get full path for a data file
    std::string getFilePath(const std::string& fileName) const {
        return data_directory + "/" + fileName;
    }
    
    // Constructor
    SerializationManager(const std::string& dataDir = "data") : data_directory(dataDir) {
        // Convert relative path to absolute if needed
        if (!std::filesystem::path(data_directory).is_absolute()) {
            data_directory = std::filesystem::current_path().string() + "/" + data_directory;
        }
        ensureDataDirectoryExists();
    }
    
    // Save a collection of serializable objects to a .dat file
    template<typename T>
    bool saveData(const std::vector<std::shared_ptr<T>>& items, const std::string& fileName) {
        std::string filePath = getFilePath(fileName);
        std::ofstream file(filePath, std::ios::binary);
        
        if (!file) {
            std::cerr << "Error opening file for writing: " << filePath << std::endl;
            return false;
        }
        
        try {
            CollectionSerializer<T>::serialize(file, items);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving data: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Load a collection of serializable objects from a .dat file
    template<typename T>
    bool loadData(std::vector<std::shared_ptr<T>>& items, const std::string& fileName) {
        std::string filePath = getFilePath(fileName);
        std::ifstream file(filePath, std::ios::binary);
        
        if (!file) {
            std::cerr << "Error opening file for reading: " << filePath << std::endl;
            return false;
        }
        
        try {
            CollectionSerializer<T>::deserialize(file, items);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading data: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Save a single object to a .dat file
    template<typename T>
    bool saveObject(const std::shared_ptr<T>& item, const std::string& fileName) {
        std::string filePath = getFilePath(fileName);
        std::ofstream file(filePath, std::ios::binary);
        
        if (!file) {
            std::cerr << "Error opening file for writing: " << filePath << std::endl;
            return false;
        }
        
        try {
            item->serialize(file);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving object: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Load a single object from a .dat file
    template<typename T>
    bool loadObject(std::shared_ptr<T>& item, const std::string& fileName) {
        std::string filePath = getFilePath(fileName);
        std::ifstream file(filePath, std::ios::binary);
        
        if (!file) {
            std::cerr << "Error opening file for reading: " << filePath << std::endl;
            return false;
        }
        
        try {
            item = std::make_shared<T>();
            item->deserialize(file);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading object: " << e.what() << std::endl;
            return false;
        }
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
    
    // Get all .dat files in the data directory
    std::vector<std::string> listDataFiles() {
        std::vector<std::string> files;
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(data_directory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".dat") {
                    files.push_back(entry.path().filename().string());
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error listing data files: " << e.what() << std::endl;
        }
        
        return files;
    }
};

// Macro to easily add serialization methods to model classes
#define SERIALIZABLE_CLASS_BEGIN(ClassName) \
public: \
    void serialize(std::ofstream& out) const { 

#define SERIALIZABLE_FIELD_INT(field) \
        out.write(reinterpret_cast<const char*>(&field), sizeof(field));

#define SERIALIZABLE_FIELD_DOUBLE(field) \
        out.write(reinterpret_cast<const char*>(&field), sizeof(field));

#define SERIALIZABLE_FIELD_BOOL(field) \
        out.write(reinterpret_cast<const char*>(&field), sizeof(field));

#define SERIALIZABLE_FIELD_STRING(field) \
        StringSerializer::serialize(out, field);

#define SERIALIZABLE_FIELD_ENUM(field) \
        EnumSerializer<decltype(field)>::serialize(out, field);

#define SERIALIZABLE_FIELD_DATETIME(field) \
        DateTimeSerializer::serialize(out, field);

#define SERIALIZABLE_CLASS_END \
    } \
    \
    bool deserialize(std::ifstream& in) { 

#define DESERIALIZABLE_FIELD_INT(field) \
        in.read(reinterpret_cast<char*>(&field), sizeof(field));

#define DESERIALIZABLE_FIELD_DOUBLE(field) \
        in.read(reinterpret_cast<char*>(&field), sizeof(field));

#define DESERIALIZABLE_FIELD_BOOL(field) \
        in.read(reinterpret_cast<char*>(&field), sizeof(field));

#define DESERIALIZABLE_FIELD_STRING(field) \
        field = StringSerializer::deserialize(in);

#define DESERIALIZABLE_FIELD_ENUM(field) \
        field = EnumSerializer<decltype(field)>::deserialize(in);

#define DESERIALIZABLE_FIELD_DATETIME(field) \
        DateTimeSerializer::deserialize(in, field);

#define DESERIALIZABLE_CLASS_END \
        return in.good(); \
    }

} // namespace MuseIO

#endif // SERIALIZATION_UTILS_H
