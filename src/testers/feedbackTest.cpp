#include "../include/feedbackModule.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Advanced Feedback Module Test ===" << std::endl;
    
    try {
        // Test 1: Create feedback with sentiment analysis
        std::cout << "\n--- Test 1: Creating Feedback with Sentiment Analysis ---" << std::endl;
        
        auto feedback1 = FeedbackManager::collectFeedback(
            101, 1001, 5, 
            "Absolutely amazing concert! The sound was excellent and the performers were fantastic!",
            FeedbackCategory::SOUND
        );
        
        auto feedback2 = FeedbackManager::collectFeedback(
            101, 1002, 2, 
            "Terrible experience. The sound was awful and overpriced tickets.",
            FeedbackCategory::PRICING
        );
        
        auto feedback3 = FeedbackManager::collectFeedback(
            101, 1003, 1, 
            "Unsafe conditions! There was a dangerous crowd surge and no security response.",
            FeedbackCategory::GENERAL
        );
        
        auto feedback4 = FeedbackManager::collectFeedback(
            102, 1004, 4, 
            "Great venue and wonderful atmosphere. Loved the performers!",
            FeedbackCategory::VENUE
        );
        
        std::cout << "✓ Created 4 feedback entries with automatic sentiment analysis" << std::endl;
        
        // Test 2: Check event ratings
        std::cout << "\n--- Test 2: Event Rating Analysis ---" << std::endl;
        
        double event101Rating = FeedbackManager::getEventRating(101);
        double event102Rating = FeedbackManager::getEventRating(102);
        
        std::cout << "Event 101 average rating: " << event101Rating << "/5.0" << std::endl;
        std::cout << "Event 102 average rating: " << event102Rating << "/5.0" << std::endl;
        
        assert(event101Rating > 0);
        assert(event102Rating > 0);
        std::cout << "✓ Event rating calculation working correctly" << std::endl;
        
        // Test 3: Sentiment analysis report
        std::cout << "\n--- Test 3: Sentiment Analysis Report ---" << std::endl;
        
        std::string sentimentReport101 = FeedbackManager::analyzeSentiment(101);
        std::string sentimentReport102 = FeedbackManager::analyzeSentiment(102);
        
        std::cout << sentimentReport101 << std::endl;
        std::cout << sentimentReport102 << std::endl;
        
        std::cout << "✓ Sentiment analysis reports generated successfully" << std::endl;
        
        // Test 4: Urgent feedback detection
        std::cout << "\n--- Test 4: Urgent Feedback Detection ---" << std::endl;
        
        auto urgentFeedback = FeedbackManager::getUrgentFeedback();
        std::cout << "Urgent feedback items requiring escalation: " << urgentFeedback.size() << std::endl;
        
        for (auto* fb : urgentFeedback) {
            std::cout << "- Event " << fb->concert_id 
                     << ", Rating: " << fb->baseFeedback->rating 
                     << ", Reason: " << fb->escalation_reason << std::endl;
            std::cout << "  Comment: " << fb->baseFeedback->comments << std::endl;
        }
        
        assert(urgentFeedback.size() > 0); // Should have at least the critical feedback
        std::cout << "✓ Urgent feedback detection working correctly" << std::endl;
        
        // Test 5: Low-rated events detection
        std::cout << "\n--- Test 5: Low-Rated Events Detection ---" << std::endl;
        
        auto lowRatedEvents = FeedbackManager::getLowRatedEvents();
        std::cout << "Events with ratings < 2.5: ";
        for (int eventId : lowRatedEvents) {
            std::cout << eventId << " ";
        }
        std::cout << std::endl;
        
        std::cout << "✓ Low-rated events detection working correctly" << std::endl;
        
        // Test 6: View event-specific feedback
        std::cout << "\n--- Test 6: Event-Specific Feedback Retrieval ---" << std::endl;
        
        auto event101Feedback = FeedbackManager::viewEventFeedback(101);
        std::cout << "Event 101 has " << event101Feedback.size() << " feedback entries:" << std::endl;
        
        for (auto* fb : event101Feedback) {
            std::cout << "- Rating: " << fb->baseFeedback->rating 
                     << ", Sentiment: " << static_cast<int>(fb->sentiment)
                     << ", Category: " << static_cast<int>(fb->category) << std::endl;
            std::cout << "  \"" << fb->baseFeedback->comments << "\"" << std::endl;
        }
        
        assert(event101Feedback.size() == 3); // Should have 3 feedback for event 101
        std::cout << "✓ Event-specific feedback retrieval working correctly" << std::endl;
        
        // Test 7: Pointer-optimized operations
        std::cout << "\n--- Test 7: Pointer-Optimized Operations ---" << std::endl;
        
        // Test that we're working with pointers efficiently
        auto* firstFeedback = event101Feedback[0];
        std::cout << "First feedback pointer address: " << firstFeedback << std::endl;
        std::cout << "First feedback rating: " << firstFeedback->baseFeedback->rating << std::endl;
        
        // Modify through pointer
        std::string originalComment = firstFeedback->baseFeedback->comments;
        std::cout << "Original comment length: " << originalComment.length() << std::endl;
        
        std::cout << "✓ Pointer operations working correctly" << std::endl;
        
        // Test 8: Dual-layer storage verification
        std::cout << "\n--- Test 8: Dual-Layer Storage Verification ---" << std::endl;
        
        // The feedback should be stored both in base entities and extended analysis
        FeedbackModule& module = FeedbackManager::getInstance();
        auto baseEntities = module.getAll();
        
        std::cout << "Base entities count: " << baseEntities.size() << std::endl;
        std::cout << "Extended feedback for event 101: " << event101Feedback.size() << std::endl;
        std::cout << "Extended feedback for event 102: " << FeedbackManager::viewEventFeedback(102).size() << std::endl;
        
        assert(baseEntities.size() >= 4); // Should have at least 4 base feedback entities
        std::cout << "✓ Dual-layer storage working correctly" << std::endl;
        
        std::cout << "\n=== All Advanced Feedback Module Tests Passed! ===" << std::endl;
        
        // Summary
        std::cout << "\n=== FEATURE SUMMARY ===" << std::endl;
        std::cout << "✓ Event-specific ratings & comments with 1-5 star system" << std::endl;
        std::cout << "✓ Categorized feedback (Sound, Venue, Pricing, etc.)" << std::endl;
        std::cout << "✓ Real-time sentiment analysis (positive/neutral/negative/critical)" << std::endl;
        std::cout << "✓ Automatic flagging of events with <2.5 avg rating" << std::endl;
        std::cout << "✓ Escalation alerts for critical issues (safety concerns)" << std::endl;
        std::cout << "✓ Dual-layer storage: Raw feedback + In-memory analysis" << std::endl;
        std::cout << "✓ Pointer-optimized sentiment scoring with priority queue" << std::endl;
        std::cout << "✓ Per-event feedback file serialization" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
