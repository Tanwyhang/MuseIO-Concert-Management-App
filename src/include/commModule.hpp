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
#include <unordered_set>
#include <queue>
#include <regex>

/**
 * @brief User role in chat system
 */
enum class UserRole {
    ATTENDEE,
    ADMIN,
    MODERATOR
};

/**
 * @brief Message type for different communication styles
 */
enum class MessageType {
    REGULAR,
    ANNOUNCEMENT,
    PINNED,
    SYSTEM,
    REACTION
};

/**
 * @brief Enhanced message structure for chat functionality
 */
struct ChatMessage {
    int message_id;
    int concert_id;
    int sender_id;
    UserRole sender_role;
    std::string sender_name;
    std::string message_content;
    MessageType message_type;
    Model::DateTime sent_at;
    bool is_pinned;
    bool is_moderated;
    std::vector<std::string> reactions; // Emoji reactions
    int reply_to_id; // For threading
    
    ChatMessage() : message_id(0), concert_id(0), sender_id(0), 
                   sender_role(UserRole::ATTENDEE), message_type(MessageType::REGULAR),
                   is_pinned(false), is_moderated(false), reply_to_id(-1) {}
};

/**
 * @brief Subscriber for notification system
 */
struct ChatSubscriber {
    int user_id;
    std::string username;
    bool has_unread;
    Model::DateTime last_seen;
    UserRole role;
    
    ChatSubscriber(int id, const std::string& name, UserRole userRole) 
        : user_id(id), username(name), has_unread(false), role(userRole) {
        last_seen = Model::DateTime::now();
    }
};

/**
 * @brief Module for managing Communication/Chat entities
 * 
 * This class extends the BaseModule template to provide real-time
 * chat functionality with role-based access, notifications, and
 * persistent storage.
 */
class CommunicationModule : public BaseModule<Model::CommunicationLog> {
private:
    // Memory-efficient storage (pointer-optimized)
    std::unordered_map<int, std::vector<ChatMessage*>> chatrooms; // ConcertID → Messages
    std::unordered_map<int, std::vector<ChatSubscriber*>> subscribers; // ConcertID → Users
    std::unordered_map<int, std::unordered_set<int>> pinned_messages; // ConcertID → MessageIDs
    
    // Live notification system
    std::unordered_map<int, std::queue<int>> pending_notifications; // UserID → MessageIDs
    
    // Message ID counter
    int next_message_id;

public:
    /**
     * @brief Constructor
     * @param filePath Path to the communications data file
     */
    CommunicationModule(const std::string& filePath = "data/communications.dat") : BaseModule<Model::CommunicationLog>(filePath),
                            next_message_id(1) {
        loadEntities();
        loadChatData();
    }
    
    /**
     * @brief Destructor
     */
    ~CommunicationModule() override {
        saveEntities();
        saveChatData();
        
        // Clean up memory
        for (auto& pair : chatrooms) {
            for (auto* msg : pair.second) {
                delete msg;
            }
        }
        for (auto& pair : subscribers) {
            for (auto* sub : pair.second) {
                delete sub;
            }
        }
    }

    /**
     * @brief Create or get chatroom for a concert
     * @param concertId Concert ID
     * @param concertName Concert name for system message
     * @return true if chatroom was created, false if it already existed
     */
    bool createChatroom(int concertId, const std::string& concertName) {
        if (chatrooms.find(concertId) != chatrooms.end()) {
            return false; // Already exists
        }
        
        // Initialize empty chatroom
        chatrooms[concertId] = std::vector<ChatMessage*>();
        subscribers[concertId] = std::vector<ChatSubscriber*>();
        pinned_messages[concertId] = std::unordered_set<int>();
        
        // Add system message
        auto* systemMsg = new ChatMessage();
        systemMsg->message_id = next_message_id++;
        systemMsg->concert_id = concertId;
        systemMsg->sender_id = 0; // System user
        systemMsg->sender_role = UserRole::ADMIN;
        systemMsg->sender_name = "System";
        systemMsg->message_content = "Welcome to the chatroom for " + concertName + "! 🎵";
        systemMsg->message_type = MessageType::SYSTEM;
        systemMsg->sent_at = Model::DateTime::now();
        
        chatrooms[concertId].push_back(systemMsg);
        
        // Create corresponding CommunicationLog entry
        auto commLog = std::make_shared<Model::CommunicationLog>();
        commLog->comm_id = next_message_id - 1;
        commLog->message_content = systemMsg->message_content;
        commLog->sent_at = systemMsg->sent_at;
        commLog->comm_type = "SYSTEM";
        commLog->recipient_count = 0;
        commLog->is_automated = true;
        
        entities.push_back(commLog);
        saveEntities();
        
        return true;
    }

    /**
     * @brief Subscribe user to a chatroom
     * @param concertId Concert ID
     * @param userId User ID
     * @param username Username
     * @param role User role
     * @return true if subscription successful
     */
    bool subscribeToChat(int concertId, int userId, const std::string& username, UserRole role) {
        if (chatrooms.find(concertId) == chatrooms.end()) {
            return false; // Chatroom doesn't exist
        }
        
        // Check if already subscribed
        auto& subs = subscribers[concertId];
        for (auto* sub : subs) {
            if (sub->user_id == userId) {
                return true; // Already subscribed
            }
        }
        
        // Add new subscriber
        auto* newSub = new ChatSubscriber(userId, username, role);
        subs.push_back(newSub);
        
        return true;
    }

    /**
     * @brief Send message to chatroom
     * @param concertId Concert ID
     * @param senderId Sender user ID
     * @param senderName Sender username
     * @param senderRole Sender role
     * @param content Message content
     * @param messageType Type of message
     * @param replyToId ID of message being replied to (-1 for none)
     * @return Pointer to created message
     */
    ChatMessage* sendMessage(int concertId, int senderId, const std::string& senderName,
                            UserRole senderRole, const std::string& content,
                            MessageType messageType = MessageType::REGULAR, int replyToId = -1) {
        
        if (chatrooms.find(concertId) == chatrooms.end()) {
            return nullptr; // Chatroom doesn't exist
        }
        
        // Create new message
        auto* newMsg = new ChatMessage();
        newMsg->message_id = next_message_id++;
        newMsg->concert_id = concertId;
        newMsg->sender_id = senderId;
        newMsg->sender_role = senderRole;
        newMsg->sender_name = senderName;
        newMsg->message_content = content;
        newMsg->message_type = messageType;
        newMsg->sent_at = Model::DateTime::now();
        newMsg->reply_to_id = replyToId;
        
        // Auto-pin announcements from admins
        if (messageType == MessageType::ANNOUNCEMENT && senderRole == UserRole::ADMIN) {
            newMsg->is_pinned = true;
            pinned_messages[concertId].insert(newMsg->message_id);
        }
        
        // Add to chatroom
        chatrooms[concertId].push_back(newMsg);
        
        // Notify subscribers (live notification system)
        notifyNewMessage(concertId, newMsg->message_id);
        
        // Create CommunicationLog entry
        auto commLog = std::make_shared<Model::CommunicationLog>();
        commLog->comm_id = newMsg->message_id;
        commLog->message_content = content;
        commLog->sent_at = newMsg->sent_at;
        commLog->comm_type = (messageType == MessageType::ANNOUNCEMENT) ? "ANNOUNCEMENT" : "CHAT";
        commLog->recipient_count = static_cast<int>(subscribers[concertId].size());
        commLog->is_automated = false;
        
        entities.push_back(commLog);
        saveEntities();
        
        return newMsg;
    }

    /**
     * @brief Get messages for a chatroom
     * @param concertId Concert ID
     * @param limit Maximum number of messages (0 = all)
     * @param afterTimestamp Only get messages after this time (empty = all)
     * @return Vector of messages
     */
    std::vector<ChatMessage*> getMessages(int concertId, int limit = 0, 
                                         const std::string& afterTimestamp = "") {
        if (chatrooms.find(concertId) == chatrooms.end()) {
            return {};
        }
        
        auto& messages = chatrooms[concertId];
        std::vector<ChatMessage*> result;
        
        for (auto* msg : messages) {
            if (!afterTimestamp.empty() && msg->sent_at.iso8601String <= afterTimestamp) {
                continue;
            }
            result.push_back(msg);
        }
        
        // Apply limit if specified
        if (limit > 0 && result.size() > static_cast<size_t>(limit)) {
            result.erase(result.begin(), result.end() - limit);
        }
        
        return result;
    }

    /**
     * @brief Get pinned messages for a chatroom
     * @param concertId Concert ID
     * @return Vector of pinned messages
     */
    std::vector<ChatMessage*> getPinnedMessages(int concertId) {
        std::vector<ChatMessage*> pinned;
        
        if (chatrooms.find(concertId) == chatrooms.end()) {
            return pinned;
        }
        
        auto& pinnedIds = pinned_messages[concertId];
        auto& messages = chatrooms[concertId];
        
        for (auto* msg : messages) {
            if (pinnedIds.find(msg->message_id) != pinnedIds.end()) {
                pinned.push_back(msg);
            }
        }
        
        return pinned;
    }

    /**
     * @brief Pin/unpin a message (admin only)
     * @param concertId Concert ID
     * @param messageId Message ID
     * @param userId User requesting the action
     * @param pin true to pin, false to unpin
     * @return true if successful
     */
    bool togglePinMessage(int concertId, int messageId, int userId, bool pin) {
        // Check if user has admin privileges
        if (!isUserAdmin(concertId, userId)) {
            return false;
        }
        
        if (pin) {
            pinned_messages[concertId].insert(messageId);
            // Find and update the message
            for (auto* msg : chatrooms[concertId]) {
                if (msg->message_id == messageId) {
                    msg->is_pinned = true;
                    break;
                }
            }
        } else {
            pinned_messages[concertId].erase(messageId);
            // Find and update the message
            for (auto* msg : chatrooms[concertId]) {
                if (msg->message_id == messageId) {
                    msg->is_pinned = false;
                    break;
                }
            }
        }
        
        return true;
    }

    /**
     * @brief Search messages by keyword
     * @param concertId Concert ID
     * @param keyword Search term
     * @param caseSensitive Whether search is case sensitive
     * @return Vector of matching messages
     */
    std::vector<ChatMessage*> searchMessages(int concertId, const std::string& keyword, 
                                            bool caseSensitive = false) {
        std::vector<ChatMessage*> results;
        
        if (chatrooms.find(concertId) == chatrooms.end()) {
            return results;
        }
        
        std::string searchTerm = keyword;
        if (!caseSensitive) {
            std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
        }
        
        for (auto* msg : chatrooms[concertId]) {
            std::string content = msg->message_content;
            if (!caseSensitive) {
                std::transform(content.begin(), content.end(), content.begin(), ::tolower);
            }
            
            if (content.find(searchTerm) != std::string::npos) {
                results.push_back(msg);
            }
        }
        
        return results;
    }

    /**
     * @brief Add reaction to a message
     * @param concertId Concert ID
     * @param messageId Message ID
     * @param userId User adding reaction
     * @param reaction Reaction emoji/text
     * @return true if successful
     */
    bool addReaction(int concertId, int messageId, int userId, const std::string& reaction) {
        for (auto* msg : chatrooms[concertId]) {
            if (msg->message_id == messageId) {
                // Simple implementation - just add reaction
                // In production, you'd track who reacted to prevent duplicates
                msg->reactions.push_back(reaction);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Get unread message count for a user
     * @param concertId Concert ID
     * @param userId User ID
     * @return Number of unread messages
     */
    int getUnreadCount(int concertId, int userId) {
        auto* subscriber = findSubscriber(concertId, userId);
        if (!subscriber) return 0;
        
        auto notifications = pending_notifications.find(userId);
        return (notifications != pending_notifications.end()) ? 
               static_cast<int>(notifications->second.size()) : 0;
    }

    /**
     * @brief Mark messages as read for a user
     * @param concertId Concert ID
     * @param userId User ID
     */
    void markAsRead(int concertId, int userId) {
        auto* subscriber = findSubscriber(concertId, userId);
        if (subscriber) {
            subscriber->has_unread = false;
            subscriber->last_seen = Model::DateTime::now();
        }
        
        // Clear pending notifications
        pending_notifications[userId] = std::queue<int>();
    }

    /**
     * @brief Get chat statistics for an event
     * @param concertId Concert ID
     * @return Formatted statistics string
     */
    std::string getChatStatistics(int concertId) {
        if (chatrooms.find(concertId) == chatrooms.end()) {
            return "Chatroom not found for event " + std::to_string(concertId);
        }
        
        auto& messages = chatrooms[concertId];
        auto& subs = subscribers[concertId];
        
        int announcements = 0, regular = 0, pinned = 0;
        for (auto* msg : messages) {
            if (msg->message_type == MessageType::ANNOUNCEMENT) announcements++;
            else if (msg->message_type == MessageType::REGULAR) regular++;
            if (msg->is_pinned) pinned++;
        }
        
        std::stringstream stats;
        stats << "=== Chat Statistics for Event " << concertId << " ===\n";
        stats << "Total Messages: " << messages.size() << "\n";
        stats << "Regular Messages: " << regular << "\n";
        stats << "Announcements: " << announcements << "\n";
        stats << "Pinned Messages: " << pinned << "\n";
        stats << "Active Subscribers: " << subs.size() << "\n";
        
        return stats.str();
    }

protected:
    /**
     * @brief Get entity ID from CommunicationLog
     */
    int getEntityId(const std::shared_ptr<Model::CommunicationLog>& commLog) const override {
        return commLog->comm_id;
    }

    /**
     * @brief Load communication logs from file
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int logCount = 0;
        readBinary(file, logCount);
        
        for (int i = 0; i < logCount; i++) {
            auto commLog = std::make_shared<Model::CommunicationLog>();
            
            readBinary(file, commLog->comm_id);
            commLog->message_content = readString(file);
            commLog->sent_at.iso8601String = readString(file);
            commLog->comm_type = readString(file);
            readBinary(file, commLog->recipient_count);
            readBinary(file, commLog->is_automated);
            
            entities.push_back(commLog);
        }
        
        file.close();
    }

    /**
     * @brief Save communication logs to file
     */
    bool saveEntities() override {
        std::ofstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << dataFilePath << std::endl;
            return false;
        }
        
        int logCount = static_cast<int>(entities.size());
        writeBinary(file, logCount);
        
        for (const auto& commLog : entities) {
            writeBinary(file, commLog->comm_id);
            writeString(file, commLog->message_content);
            writeString(file, commLog->sent_at.iso8601String);
            writeString(file, commLog->comm_type);
            writeBinary(file, commLog->recipient_count);
            writeBinary(file, commLog->is_automated);
        }
        
        file.close();
        return true;
    }

private:
    /**
     * @brief Live notification system (no polling)
     */
    void notifyNewMessage(int concertId, int messageId) {
        for (auto* subscriber : subscribers[concertId]) {
            subscriber->has_unread = true;
            pending_notifications[subscriber->user_id].push(messageId);
        }
    }

    /**
     * @brief Check if user has admin privileges
     */
    bool isUserAdmin(int concertId, int userId) {
        auto* subscriber = findSubscriber(concertId, userId);
        return subscriber && (subscriber->role == UserRole::ADMIN || 
                             subscriber->role == UserRole::MODERATOR);
    }

    /**
     * @brief Find subscriber by user ID
     */
    ChatSubscriber* findSubscriber(int concertId, int userId) {
        if (subscribers.find(concertId) == subscribers.end()) {
            return nullptr;
        }
        
        for (auto* sub : subscribers[concertId]) {
            if (sub->user_id == userId) {
                return sub;
            }
        }
        return nullptr;
    }

    /**
     * @brief Load chat-specific data
     */
    void loadChatData() {
        // Load chat data from separate files
        // This is a placeholder for the concept
        std::ifstream file("data/chat_data.bin", std::ios::binary);
        if (file.is_open()) {
            readBinary(file, next_message_id);
            file.close();
        }
    }

    /**
     * @brief Save chat-specific data
     */
    void saveChatData() {
        // Save chat data to separate files
        std::ofstream file("data/chat_data.bin", std::ios::binary);
        if (file.is_open()) {
            writeBinary(file, next_message_id);
            file.close();
        }
    }
};

// Namespace wrapper for simplified access
namespace CommunicationManager {
    static CommunicationModule& getInstance() {
        static CommunicationModule instance;
        return instance;
    }

    /**
     * @brief Create chatroom for event
     */
    inline bool createEventChatroom(int concertId, const std::string& concertName) {
        return getInstance().createChatroom(concertId, concertName);
    }

    /**
     * @brief Join chatroom
     */
    inline bool joinChat(int concertId, int userId, const std::string& username, UserRole role) {
        return getInstance().subscribeToChat(concertId, userId, username, role);
    }

    /**
     * @brief Send message
     */
    inline ChatMessage* sendMessage(int concertId, int senderId, const std::string& senderName,
                                   UserRole senderRole, const std::string& content,
                                   MessageType messageType = MessageType::REGULAR) {
        return getInstance().sendMessage(concertId, senderId, senderName, senderRole, content, messageType);
    }

    /**
     * @brief Get recent messages
     */
    inline std::vector<ChatMessage*> getRecentMessages(int concertId, int limit = 50) {
        return getInstance().getMessages(concertId, limit);
    }

    /**
     * @brief Search chat history
     */
    inline std::vector<ChatMessage*> searchChat(int concertId, const std::string& keyword) {
        return getInstance().searchMessages(concertId, keyword);
    }

    /**
     * @brief Get pinned announcements
     */
    inline std::vector<ChatMessage*> getPinnedAnnouncements(int concertId) {
        return getInstance().getPinnedMessages(concertId);
    }

    /**
     * @brief Get chat statistics
     */
    inline std::string getChatStats(int concertId) {
        return getInstance().getChatStatistics(concertId);
    }

    /**
     * @brief Mark messages as read
     */
    inline void markAsRead(int concertId, int userId) {
        getInstance().markAsRead(concertId, userId);
    }

    /**
     * @brief Get unread message count
     */
    inline int getUnreadCount(int concertId, int userId) {
        return getInstance().getUnreadCount(concertId, userId);
    }
}
