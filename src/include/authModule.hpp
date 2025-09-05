#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include <unordered_map>
#include "models.hpp"

/**
 * @brief Module for handling authentication and authorization
 * 
 * This class provides secure credential storage and verification using
 * direct memory access and pointer manipulation for enhanced security.
 * It uses manual memory management with raw pointers for sensitive operations.
 */
class AuthModule {
private:
    // Memory block for storing credential data
    char* secureMemory;
    size_t memorySize;
    
    // Encryption key (used for XOR operations)
    unsigned char* encryptionKey;
    size_t keySize;
    
    // In-memory credential mapping (username -> offset in secureMemory)
    std::unordered_map<std::string, size_t> credentialMap;
    
    // Constants for memory layout
    static constexpr size_t CREDENTIAL_BLOCK_SIZE = 256;
    static constexpr size_t USERNAME_MAX_LENGTH = 64;
    static constexpr size_t PASSWORD_HASH_LENGTH = 64;
    static constexpr size_t SALT_LENGTH = 16;
    static constexpr size_t USER_TYPE_SIZE = 4;
    
    /**
     * @brief Generates a secure random salt for password hashing
     * @param salt Pointer to buffer where salt will be stored
     * @param length Length of salt to generate
     */
    void generateSalt(unsigned char* salt, size_t length) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 255);
        
        for (size_t i = 0; i < length; ++i) {
            *(salt + i) = static_cast<unsigned char>(dist(gen));
        }
    }
    
    /**
     * @brief Compute a simple hash of password with salt
     * @param password The password to hash
     * @param salt The salt to use
     * @param saltLength The length of the salt
     * @param output Pointer to output buffer for hash
     * @param outputLength Length of output buffer
     */
    void hashPassword(const std::string& password, 
                      const unsigned char* salt, 
                      size_t saltLength,
                      unsigned char* output, 
                      size_t outputLength) {
        // Initialize output with zeros
        std::memset(output, 0, outputLength);
        
        // Simple hash function (for demonstration - in production use a secure hash function)
        for (size_t i = 0; i < password.length(); ++i) {
            size_t pos = i % outputLength;
            *(output + pos) ^= password[i];
            
            if (i < saltLength) {
                *(output + pos) ^= *(salt + i);
            }
            
            // Additional mixing
            for (size_t j = 0; j < outputLength; ++j) {
                size_t nextPos = (pos + j) % outputLength;
                *(output + nextPos) = (*(output + nextPos) << 1) | (*(output + nextPos) >> 7);
            }
        }
    }
    
    /**
     * @brief XOR encrypt/decrypt data with the encryption key
     * @param data Pointer to data to encrypt/decrypt
     * @param length Length of data
     */
    void xorCrypt(unsigned char* data, size_t length) {
        for (size_t i = 0; i < length; ++i) {
            *(data + i) ^= *(encryptionKey + (i % keySize));
        }
    }
    
    /**
     * @brief Securely store credentials in memory
     * @param username The username
     * @param passwordHash The hashed password
     * @param salt The salt used for hashing
     * @param userType The type of user
     * @return Offset in memory where credentials are stored
     */
    size_t storeCredential(const std::string& username, 
                          const unsigned char* passwordHash,
                          const unsigned char* salt,
                          int userType) {
        // Find next available memory block
        size_t offset = credentialMap.size() * CREDENTIAL_BLOCK_SIZE;
        
        // Check if we have enough memory
        if (offset + CREDENTIAL_BLOCK_SIZE > memorySize) {
            resizeSecureMemory(memorySize * 2);
        }
        
        // Get pointer to the memory block
        char* block = secureMemory + offset;
        
        // Layout: [username(64)][passwordHash(64)][salt(16)][userType(4)][reserved(108)]
        
        // Clear the block
        std::memset(block, 0, CREDENTIAL_BLOCK_SIZE);
        
        // Store username (truncate if necessary)
        size_t usernameLen = std::min(username.length(), USERNAME_MAX_LENGTH - 1);
        std::memcpy(block, username.c_str(), usernameLen);
        
        // Store password hash
        std::memcpy(block + USERNAME_MAX_LENGTH, passwordHash, PASSWORD_HASH_LENGTH);
        
        // Store salt
        std::memcpy(block + USERNAME_MAX_LENGTH + PASSWORD_HASH_LENGTH, salt, SALT_LENGTH);
        
        // Store user type
        int* userTypePtr = reinterpret_cast<int*>(block + USERNAME_MAX_LENGTH + PASSWORD_HASH_LENGTH + SALT_LENGTH);
        *userTypePtr = userType;
        
        // Encrypt the block
        xorCrypt(reinterpret_cast<unsigned char*>(block), CREDENTIAL_BLOCK_SIZE);
        
        // Store mapping
        credentialMap[username] = offset;
        
        return offset;
    }
    
    /**
     * @brief Resize the secure memory buffer
     * @param newSize New size for the memory buffer
     */
    void resizeSecureMemory(size_t newSize) {
        if (newSize <= memorySize) {
            return;
        }
        
        // Allocate new memory
        char* newMemory = new char[newSize];
        std::memset(newMemory, 0, newSize);
        
        // Copy existing data
        if (secureMemory) {
            std::memcpy(newMemory, secureMemory, memorySize);
            
            // Securely wipe old memory
            for (size_t i = 0; i < memorySize; ++i) {
                *(secureMemory + i) = 0;
            }
            
            delete[] secureMemory;
        }
        
        secureMemory = newMemory;
        memorySize = newSize;
    }
    
    /**
     * @brief Generate encryption key
     */
    void generateEncryptionKey() {
        keySize = 32; // 256-bit key
        encryptionKey = new unsigned char[keySize];
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 255);
        
        for (size_t i = 0; i < keySize; ++i) {
            *(encryptionKey + i) = static_cast<unsigned char>(dist(gen));
        }
    }

public:
    /**
     * @brief Constructor
     * @param initialMemorySize Initial size of secure memory buffer
     */
    AuthModule(size_t initialMemorySize = 4096) {
        // Allocate secure memory
        secureMemory = new char[initialMemorySize];
        memorySize = initialMemorySize;
        
        // Initialize memory with zeros
        std::memset(secureMemory, 0, memorySize);
        
        // Generate encryption key
        generateEncryptionKey();
    }
    
    /**
     * @brief Destructor - securely wipes memory before deallocation
     */
    ~AuthModule() {
        // Securely wipe encryption key
        if (encryptionKey) {
            for (size_t i = 0; i < keySize; ++i) {
                *(encryptionKey + i) = 0;
            }
            delete[] encryptionKey;
        }
        
        // Securely wipe credential memory
        if (secureMemory) {
            for (size_t i = 0; i < memorySize; ++i) {
                *(secureMemory + i) = 0;
            }
            delete[] secureMemory;
        }
    }
    
    /**
     * @brief Register a new user
     * @param username Username
     * @param password Password
     * @param password Password
     * @return True if registration successful
     */
    bool registerUser(const std::string& username, const std::string& password) {
        // Check if username already exists
        if (credentialMap.find(username) != credentialMap.end()) {
            return false;
        }
        
        // Generate salt
        unsigned char salt[SALT_LENGTH];
        generateSalt(salt, SALT_LENGTH);
        
        // Hash password
        unsigned char passwordHash[PASSWORD_HASH_LENGTH];
        hashPassword(password, salt, SALT_LENGTH, passwordHash, PASSWORD_HASH_LENGTH);
        
        // Store in secure memory (userType = 0 for all users)
        storeCredential(username, passwordHash, salt, 0);
        
        return true;
    }
    
    /**
     * @brief Authenticate a user
     * @param username Username
     * @param password Password
     * @return true if authentication successful, false otherwise
     */
    bool authenticateUser(const std::string& username, const std::string& password) {
        // Check if username exists
        auto it = credentialMap.find(username);
        if (it == credentialMap.end()) {
            return false;
        }
        
        // Get offset in secure memory
        size_t offset = it->second;
        
        // Make a copy of the encrypted block
        char block[CREDENTIAL_BLOCK_SIZE];
        std::memcpy(block, secureMemory + offset, CREDENTIAL_BLOCK_SIZE);
        
        // Decrypt the block
        xorCrypt(reinterpret_cast<unsigned char*>(block), CREDENTIAL_BLOCK_SIZE);
        
        // Extract salt
        const unsigned char* salt = reinterpret_cast<const unsigned char*>(
            block + USERNAME_MAX_LENGTH + PASSWORD_HASH_LENGTH
        );
        
        // Hash the provided password
        unsigned char inputHash[PASSWORD_HASH_LENGTH];
        hashPassword(password, salt, SALT_LENGTH, inputHash, PASSWORD_HASH_LENGTH);
        
        // Get stored hash
        const unsigned char* storedHash = reinterpret_cast<const unsigned char*>(
            block + USERNAME_MAX_LENGTH
        );
        
        // Compare hashes
        bool passwordMatch = true;
        for (size_t i = 0; i < PASSWORD_HASH_LENGTH; ++i) {
            if (*(inputHash + i) != *(storedHash + i)) {
                passwordMatch = false;
                break;
            }
        }
        
        return passwordMatch;
    }
    
    /**
     * @brief Change user password
     * @param username Username
     * @param oldPassword Old password
     * @param newPassword New password
     * @return True if password change successful
     */
    bool changePassword(const std::string& username, 
                       const std::string& oldPassword,
                       const std::string& newPassword) {
        // First authenticate the user
        bool isAuthenticated = authenticateUser(username, oldPassword);
        if (!isAuthenticated) {
            return false;
        }
        
        // Get offset in secure memory
        size_t offset = credentialMap[username];
        
        // Make a copy of the encrypted block
        char block[CREDENTIAL_BLOCK_SIZE];
        std::memcpy(block, secureMemory + offset, CREDENTIAL_BLOCK_SIZE);
        
        // Decrypt the block
        xorCrypt(reinterpret_cast<unsigned char*>(block), CREDENTIAL_BLOCK_SIZE);
        
        // Generate new salt
        unsigned char* salt = reinterpret_cast<unsigned char*>(
            block + USERNAME_MAX_LENGTH + PASSWORD_HASH_LENGTH
        );
        generateSalt(salt, SALT_LENGTH);
        
        // Hash the new password
        unsigned char* passwordHash = reinterpret_cast<unsigned char*>(
            block + USERNAME_MAX_LENGTH
        );
        hashPassword(newPassword, salt, SALT_LENGTH, passwordHash, PASSWORD_HASH_LENGTH);
        
        // Re-encrypt the block
        xorCrypt(reinterpret_cast<unsigned char*>(block), CREDENTIAL_BLOCK_SIZE);
        
        // Store back in secure memory
        std::memcpy(secureMemory + offset, block, CREDENTIAL_BLOCK_SIZE);
        
        return true;
    }
    
    /**
     * @brief Delete a user
     * @param username Username
     * @return True if deletion successful
     */
    bool deleteUser(const std::string& username) {
        auto it = credentialMap.find(username);
        if (it == credentialMap.end()) {
            return false;
        }
        
        size_t offset = it->second;
        
        // Securely wipe the memory block
        char* block = secureMemory + offset;
        for (size_t i = 0; i < CREDENTIAL_BLOCK_SIZE; ++i) {
            *(block + i) = 0;
        }
        
        // Remove from map
        credentialMap.erase(it);
        
        return true;
    }
    
    /**
     * @brief Get user type
     * @param username Username
     * @return User type if found, -1 otherwise
     */
    int getUserType(const std::string& username) {
        auto it = credentialMap.find(username);
        if (it == credentialMap.end()) {
            return -1;
        }
        
        size_t offset = it->second;
        
        // Make a copy of the encrypted block
        char block[CREDENTIAL_BLOCK_SIZE];
        std::memcpy(block, secureMemory + offset, CREDENTIAL_BLOCK_SIZE);
        
        // Decrypt the block
        xorCrypt(reinterpret_cast<unsigned char*>(block), CREDENTIAL_BLOCK_SIZE);
        
        // Extract user type
        const int* userTypePtr = reinterpret_cast<const int*>(
            block + USERNAME_MAX_LENGTH + PASSWORD_HASH_LENGTH + SALT_LENGTH
        );
        
        return *userTypePtr;
    }
    
    /**
     * @brief Check if a username exists
     * @param username Username to check
     * @return True if username exists
     */
    bool userExists(const std::string& username) {
        return credentialMap.find(username) != credentialMap.end();
    }
    
    /**
     * @brief Get number of registered users
     * @return Number of registered users
     */
    size_t getUserCount() const {
        return credentialMap.size();
    }

    /**
     * @brief Get all admin users (for demo/testing purposes)
     * @return Vector of pairs containing username and user type for admin users
     */
    std::vector<std::pair<std::string, int>> getAdminUsers() {
        std::vector<std::pair<std::string, int>> adminUsers;
        
        for (const auto& entry : credentialMap) {
            int userType = getUserType(entry.first);
            if (userType == 1) { // Admin user type
                adminUsers.emplace_back(entry.first, userType);
            }
        }
        
        return adminUsers;
    }

    /**
     * @brief Get all staff users (for demo/testing purposes)
     * @return Vector of pairs containing username and user type for staff users
     */
    std::vector<std::pair<std::string, int>> getStaffUsers() {
        std::vector<std::pair<std::string, int>> staffUsers;
        
        for (const auto& entry : credentialMap) {
            int userType = getUserType(entry.first);
            if (userType == 2) { // Staff user type
                staffUsers.emplace_back(entry.first, userType);
            }
        }
        
        return staffUsers;
    }

    /**
     * @brief Get all regular users (for demo/testing purposes)
     * @return Vector of pairs containing username and user type for regular users
     */
    std::vector<std::pair<std::string, int>> getRegularUsers() {
        std::vector<std::pair<std::string, int>> regularUsers;
        
        for (const auto& entry : credentialMap) {
            int userType = getUserType(entry.first);
            if (userType == 0) { // Regular user type
                regularUsers.emplace_back(entry.first, userType);
            }
        }
        
        return regularUsers;
    }

    /**
     * @brief Save credentials to a file
     * @param filePath Path to save file
     * @return True if save successful
     */
    bool saveCredentials(const std::string& filePath) {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Write memory size
        file.write(reinterpret_cast<const char*>(&memorySize), sizeof(memorySize));
        
        // Write key size
        file.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
        
        // Write encryption key
        file.write(reinterpret_cast<const char*>(encryptionKey), keySize);
        
        // Write secure memory
        file.write(secureMemory, memorySize);
        
        // Write map size
        size_t mapSize = credentialMap.size();
        file.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        
        // Write map entries
        for (const auto& entry : credentialMap) {
            size_t usernameLength = entry.first.length();
            file.write(reinterpret_cast<const char*>(&usernameLength), sizeof(usernameLength));
            file.write(entry.first.c_str(), usernameLength);
            file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
        }
        
        return true;
    }
    
    /**
     * @brief Load credentials from a file
     * @param filePath Path to load file
     * @return True if load successful
     */
    bool loadCredentials(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Clean up existing data
        if (secureMemory) {
            for (size_t i = 0; i < memorySize; ++i) {
                *(secureMemory + i) = 0;
            }
            delete[] secureMemory;
            secureMemory = nullptr;
        }
        
        if (encryptionKey) {
            for (size_t i = 0; i < keySize; ++i) {
                *(encryptionKey + i) = 0;
            }
            delete[] encryptionKey;
            encryptionKey = nullptr;
        }
        
        credentialMap.clear();
        
        // Read memory size
        file.read(reinterpret_cast<char*>(&memorySize), sizeof(memorySize));
        
        // Read key size
        file.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        
        // Read encryption key
        encryptionKey = new unsigned char[keySize];
        file.read(reinterpret_cast<char*>(encryptionKey), keySize);
        
        // Read secure memory
        secureMemory = new char[memorySize];
        file.read(secureMemory, memorySize);
        
        // Read map size
        size_t mapSize;
        file.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
        
        // Read map entries
        for (size_t i = 0; i < mapSize; ++i) {
            size_t usernameLength;
            file.read(reinterpret_cast<char*>(&usernameLength), sizeof(usernameLength));
            
            std::string username(usernameLength, '\0');
            file.read(&username[0], usernameLength);
            
            size_t offset;
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            
            credentialMap[username] = offset;
        }
        
        return true;
    }
};
