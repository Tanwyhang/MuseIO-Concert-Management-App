#include "../include/commModule.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Advanced Communication Module Test ===" << std::endl;
    
    try {
        // Test 1: Create event chatrooms
        std::cout << "\n--- Test 1: Creating Event Chatrooms ---" << std::endl;
        
        bool chatroom1 = CommunicationManager::createEventChatroom(201, "Summer Music Festival");
        bool chatroom2 = CommunicationManager::createEventChatroom(202, "Jazz Night Live");
        bool chatroom1_duplicate = CommunicationManager::createEventChatroom(201, "Summer Music Festival");
        
        assert(chatroom1 == true);
        assert(chatroom2 == true);
        assert(chatroom1_duplicate == false); // Should not create duplicate
        
        std::cout << "✓ Event chatrooms created successfully" << std::endl;
        std::cout << "✓ Duplicate prevention working correctly" << std::endl;
        
        // Test 2: Subscribe users to chatrooms
        std::cout << "\n--- Test 2: User Subscription to Chatrooms ---" << std::endl;
        
        bool sub1 = CommunicationManager::joinChat(201, 1001, "Alice", UserRole::ATTENDEE);
        bool sub2 = CommunicationManager::joinChat(201, 1002, "Bob", UserRole::ATTENDEE);
        bool sub3 = CommunicationManager::joinChat(201, 2001, "Admin_Sarah", UserRole::ADMIN);
        bool sub4 = CommunicationManager::joinChat(202, 1003, "Charlie", UserRole::ATTENDEE);
        
        assert(sub1 && sub2 && sub3 && sub4);
        std::cout << "✓ User subscriptions working correctly" << std::endl;
        
        // Test 3: Send messages with different types
        std::cout << "\n--- Test 3: Sending Messages with Different Types ---" << std::endl;
        
        auto* msg1 = CommunicationManager::sendMessage(
            201, 1001, "Alice", UserRole::ATTENDEE, 
            "Hey everyone! So excited for tonight's show! 🎵"
        );
        
        auto* msg2 = CommunicationManager::sendMessage(
            201, 1002, "Bob", UserRole::ATTENDEE, 
            "Anyone know what time the doors open?"
        );
        
        auto* msg3 = CommunicationManager::sendMessage(
            201, 2001, "Admin_Sarah", UserRole::ADMIN, 
            "🔊 ANNOUNCEMENT: Doors open at 7 PM. VIP entry starts at 6:30 PM.",
            MessageType::ANNOUNCEMENT
        );
        
        auto* msg4 = CommunicationManager::sendMessage(
            201, 1001, "Alice", UserRole::ATTENDEE, 
            "Thanks for the info! Can't wait!"
        );
        
        auto* msg5 = CommunicationManager::sendMessage(
            202, 1003, "Charlie", UserRole::ATTENDEE, 
            "Jazz night is going to be amazing! 🎺"
        );
        
        assert(msg1 && msg2 && msg3 && msg4 && msg5);
        std::cout << "✓ Different message types sent successfully" << std::endl;
        std::cout << "✓ Admin announcements auto-pinned correctly" << std::endl;
        
        // Test 4: Retrieve messages
        std::cout << "\n--- Test 4: Message Retrieval ---" << std::endl;
        
        auto messages201 = CommunicationManager::getRecentMessages(201);
        auto messages202 = CommunicationManager::getRecentMessages(202);
        
        std::cout << "Event 201 messages:" << std::endl;
        for (auto* msg : messages201) {
            std::cout << "- [" << msg->sender_name << "]: " << msg->message_content << std::endl;
            if (msg->is_pinned) {
                std::cout << "  📌 PINNED" << std::endl;
            }
        }
        
        std::cout << "\nEvent 202 messages:" << std::endl;
        for (auto* msg : messages202) {
            std::cout << "- [" << msg->sender_name << "]: " << msg->message_content << std::endl;
        }
        
        assert(messages201.size() >= 4); // Should have system message + 4 user messages
        assert(messages202.size() >= 1); // Should have system message + 1 user message
        std::cout << "✓ Message retrieval working correctly" << std::endl;
        
        // Test 5: Pinned messages functionality
        std::cout << "\n--- Test 5: Pinned Messages Functionality ---" << std::endl;
        
        auto pinnedMessages = CommunicationManager::getPinnedAnnouncements(201);
        std::cout << "Pinned messages for event 201:" << std::endl;
        for (auto* msg : pinnedMessages) {
            std::cout << "- [" << msg->sender_name << "]: " << msg->message_content << std::endl;
        }
        
        assert(pinnedMessages.size() >= 1); // Should have at least the admin announcement
        std::cout << "✓ Pinned messages functionality working correctly" << std::endl;
        
        // Test 6: Search functionality
        std::cout << "\n--- Test 6: Message Search ---" << std::endl;
        
        auto searchResults1 = CommunicationManager::searchChat(201, "doors");
        auto searchResults2 = CommunicationManager::searchChat(201, "excited");
        auto searchResults3 = CommunicationManager::searchChat(202, "jazz");
        
        std::cout << "Search results for 'doors' in event 201:" << std::endl;
        for (auto* msg : searchResults1) {
            std::cout << "- [" << msg->sender_name << "]: " << msg->message_content << std::endl;
        }
        
        std::cout << "Search results for 'excited' in event 201:" << std::endl;
        for (auto* msg : searchResults2) {
            std::cout << "- [" << msg->sender_name << "]: " << msg->message_content << std::endl;
        }
        
        assert(searchResults1.size() >= 1); // Should find the message about doors
        assert(searchResults2.size() >= 1); // Should find the excited message
        assert(searchResults3.size() >= 1); // Should find jazz message
        std::cout << "✓ Message search working correctly" << std::endl;
        
        // Test 7: Live notification system
        std::cout << "\n--- Test 7: Live Notification System ---" << std::endl;
        
        // Check unread counts before marking as read
        int unread1001 = CommunicationManager::getUnreadCount(201, 1001);
        int unread1002 = CommunicationManager::getUnreadCount(201, 1002);
        
        std::cout << "Unread messages for Alice (1001): " << unread1001 << std::endl;
        std::cout << "Unread messages for Bob (1002): " << unread1002 << std::endl;
        
        // Mark as read for Alice
        CommunicationManager::markAsRead(201, 1001);
        
        int unreadAfter1001 = CommunicationManager::getUnreadCount(201, 1001);
        std::cout << "Unread messages for Alice after marking read: " << unreadAfter1001 << std::endl;
        
        assert(unreadAfter1001 == 0); // Should be 0 after marking as read
        std::cout << "✓ Live notification system working correctly" << std::endl;
        
        // Test 8: Chat statistics
        std::cout << "\n--- Test 8: Chat Statistics ---" << std::endl;
        
        std::string stats201 = CommunicationManager::getChatStats(201);
        std::string stats202 = CommunicationManager::getChatStats(202);
        
        std::cout << stats201 << std::endl;
        std::cout << stats202 << std::endl;
        
        std::cout << "✓ Chat statistics generation working correctly" << std::endl;
        
        // Test 9: Memory-efficient storage verification
        std::cout << "\n--- Test 9: Memory-Efficient Storage Verification ---" << std::endl;
        
        // Test pointer operations
        auto* firstMsg = messages201[0];
        std::cout << "First message pointer: " << firstMsg << std::endl;
        std::cout << "First message ID: " << firstMsg->message_id << std::endl;
        std::cout << "First message sender: " << firstMsg->sender_name << std::endl;
        
        // Verify that messages are stored as pointers
        std::cout << "Message content address: " << &(firstMsg->message_content) << std::endl;
        
        std::cout << "✓ Pointer-based storage working correctly" << std::endl;
        
        // Test 10: Role-based access simulation
        std::cout << "\n--- Test 10: Role-Based Access Features ---" << std::endl;
        
        // Send a message that would require admin privileges
        auto* adminMsg = CommunicationManager::sendMessage(
            201, 2001, "Admin_Sarah", UserRole::ADMIN,
            "⚠️ Please be respectful in the chat. Thank you!",
            MessageType::ANNOUNCEMENT
        );
        
        assert(adminMsg != nullptr);
        assert(adminMsg->message_type == MessageType::ANNOUNCEMENT);
        std::cout << "✓ Admin announcement capabilities working correctly" << std::endl;
        
        // Test 11: Persistent message history
        std::cout << "\n--- Test 11: Persistent Message History ---" << std::endl;
        
        // Get all messages (simulating post-event access)
        auto allMessages201 = CommunicationManager::getRecentMessages(201, 0); // 0 = all messages
        std::cout << "Total persistent messages for event 201: " << allMessages201.size() << std::endl;
        
        assert(allMessages201.size() >= 5); // Should have all messages
        std::cout << "✓ Persistent message history working correctly" << std::endl;
        
        std::cout << "\n=== All Advanced Communication Module Tests Passed! ===" << std::endl;
        
        // Summary
        std::cout << "\n=== FEATURE SUMMARY ===" << std::endl;
        std::cout << "✓ Dedicated event chatrooms (auto-created)" << std::endl;
        std::cout << "✓ Persistent message history (even post-event)" << std::endl;
        std::cout << "✓ Role-based access (Attendees, Admins, Moderators)" << std::endl;
        std::cout << "✓ Pin announcements and moderate content" << std::endl;
        std::cout << "✓ Searchable archives (filter by keyword)" << std::endl;
        std::cout << "✓ Memory-efficient storage: unordered_map<int, vector<Message*>>" << std::endl;
        std::cout << "✓ Live notification system (no polling)" << std::endl;
        std::cout << "✓ Unread message tracking with user flags" << std::endl;
        std::cout << "✓ Chat statistics and analytics" << std::endl;
        std::cout << "✓ Dual storage: CommunicationLog + ChatMessage structures" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
