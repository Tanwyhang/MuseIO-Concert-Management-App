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
#include <queue>
#include <regex>

/**
 * @brief Sentiment analysis result
 */
enum class SentimentType {
    POSITIVE,
    NEUTRAL,
    NEGATIVE,
    CRITICAL
};

/**
 * @brief Feedback category for structured analysis
 */
enum class FeedbackCategory {
    SOUND,
    VENUE,
    PRICING,
    PERFORMERS,
    ORGANIZATION,
    GENERAL
};

/**
 * @brief Extended feedback structure for enhanced analysis
 */
struct ExtendedFeedback {
    std::shared_ptr<Model::Feedback> baseFeedback;
    int concert_id;
    FeedbackCategory category;
    SentimentType sentiment;
    bool requires_escalation;
    std::string escalation_reason;
    
    ExtendedFeedback(std::shared_ptr<Model::Feedback> feedback, int concertId) 
        : baseFeedback(feedback), concert_id(concertId), 
          category(FeedbackCategory::GENERAL), sentiment(SentimentType::NEUTRAL),
          requires_escalation(false) {}
};

/**
 * @brief Module for managing Feedback entities with advanced analytics
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for feedback management including sentiment analysis,
 * categorization, and escalation systems.
 */
class FeedbackModule : public BaseModule<Model::Feedback> {
private:
    // Priority queue for urgent feedback (pointer-optimized)
    std::priority_queue<ExtendedFeedback*, std::vector<ExtendedFeedback*>, 
                       std::function<bool(ExtendedFeedback*, ExtendedFeedback*)>> urgentQueue;
    
    // In-memory analysis storage (dual-layer design)
    std::unordered_map<int, std::vector<ExtendedFeedback*>> feedbackByEvent;
    std::unordered_map<int, double> eventAverageRatings;
    
    // Sentiment analysis keywords
    std::vector<std::string> positiveKeywords;
    std::vector<std::string> negativeKeywords;
    std::vector<std::string> criticalKeywords;

public:
    /**
     * @brief Constructor with priority queue initialization
     * @param filePath Path to the feedback data file
     */
    FeedbackModule(const std::string& filePath = "data/feedback.dat") : BaseModule<Model::Feedback>(filePath),
                       urgentQueue([](ExtendedFeedback* a, ExtendedFeedback* b) {
                           // Higher priority for lower ratings and critical sentiment
                           if (a->baseFeedback->rating != b->baseFeedback->rating) {
                               return a->baseFeedback->rating > b->baseFeedback->rating;
                           }
                           return a->sentiment > b->sentiment;
                       }) {
        loadEntities();
        initializeSentimentKeywords();
        reprocessAllFeedback();
    }
    
    /**
     * @brief Destructor
     */
    ~FeedbackModule() override {
        saveEntities();
        // Clean up extended feedback objects
        for (auto& pair : feedbackByEvent) {
            for (auto* extFeedback : pair.second) {
                delete extFeedback;
            }
        }
    }

    /**
     * @brief Create new feedback with automatic analysis
     * 
     * @param concertId Concert ID this feedback is for
     * @param attendeeId ID of attendee providing feedback
     * @param rating Rating (1-5)
     * @param comments Text comments
     * @param category Feedback category
     * @return std::shared_ptr<Model::Feedback> Created feedback
     */
    std::shared_ptr<Model::Feedback> createFeedback(
        int concertId,
        int attendeeId,
        int rating,
        const std::string& comments,
        FeedbackCategory category = FeedbackCategory::GENERAL
    ) {
        // Validate rating
        if (rating < 1 || rating > 5) {
            throw std::invalid_argument("Rating must be between 1 and 5");
        }
        
        // Create base feedback
        auto feedback = std::make_shared<Model::Feedback>();
        feedback->rating = rating;
        feedback->comments = comments;
        feedback->submitted_at = Model::DateTime::now();
        
        // Add to base collection
        entities.push_back(feedback);
        
        // Create extended feedback for analysis
        auto* extFeedback = new ExtendedFeedback(feedback, concertId);
        extFeedback->category = category;
        
        // Perform sentiment analysis
        analyzeSentiment(extFeedback);
        
        // Add to event-specific storage
        feedbackByEvent[concertId].push_back(extFeedback);
        
        // Update event average rating
        updateEventAverageRating(concertId);
        
        // Check for escalation
        if (extFeedback->requires_escalation) {
            urgentQueue.push(extFeedback);
        }
        
        // Save to file
        saveEntities();
        saveEventSpecificFeedback(concertId);
        
        return feedback;
    }

    /**
     * @brief Get feedback for a specific event
     * @param concertId Concert ID
     * @return Vector of extended feedback for the event
     */
    std::vector<ExtendedFeedback*> getFeedbackForEvent(int concertId) {
        auto it = feedbackByEvent.find(concertId);
        return (it != feedbackByEvent.end()) ? it->second : std::vector<ExtendedFeedback*>();
    }

    /**
     * @brief Get average rating for an event
     * @param concertId Concert ID
     * @return Average rating (0.0 if no feedback)
     */
    double getEventAverageRating(int concertId) {
        auto it = eventAverageRatings.find(concertId);
        return (it != eventAverageRatings.end()) ? it->second : 0.0;
    }

    /**
     * @brief Get urgent feedback requiring escalation
     * @return Vector of urgent feedback (pointer-optimized)
     */
    std::vector<ExtendedFeedback*> getUrgentFeedback() {
        std::vector<ExtendedFeedback*> urgent;
        auto tempQueue = urgentQueue; // Copy to preserve original
        
        while (!tempQueue.empty()) {
            urgent.push_back(tempQueue.top());
            tempQueue.pop();
        }
        
        return urgent;
    }

    /**
     * @brief Get events with low ratings (< 2.5 average)
     * @return Vector of concert IDs with low ratings
     */
    std::vector<int> getLowRatedEvents() {
        std::vector<int> lowRated;
        for (const auto& pair : eventAverageRatings) {
            if (pair.second < 2.5) {
                lowRated.push_back(pair.first);
            }
        }
        return lowRated;
    }

    /**
     * @brief Generate sentiment analysis report for an event
     * @param concertId Concert ID
     * @return Formatted report string
     */
    std::string generateSentimentReport(int concertId) {
        auto feedback = getFeedbackForEvent(concertId);
        if (feedback.empty()) {
            return "No feedback available for event " + std::to_string(concertId);
        }
        
        int positive = 0, neutral = 0, negative = 0, critical = 0;
        for (auto* fb : feedback) {
            switch (fb->sentiment) {
                case SentimentType::POSITIVE: positive++; break;
                case SentimentType::NEUTRAL: neutral++; break;
                case SentimentType::NEGATIVE: negative++; break;
                case SentimentType::CRITICAL: critical++; break;
            }
        }
        
        std::stringstream report;
        report << "=== Sentiment Analysis Report for Event " << concertId << " ===\n";
        report << "Total Feedback: " << feedback.size() << "\n";
        report << "Average Rating: " << getEventAverageRating(concertId) << "/5.0\n";
        report << "Sentiment Breakdown:\n";
        report << "  Positive: " << positive << " (" << (positive*100/feedback.size()) << "%)\n";
        report << "  Neutral:  " << neutral << " (" << (neutral*100/feedback.size()) << "%)\n";
        report << "  Negative: " << negative << " (" << (negative*100/feedback.size()) << "%)\n";
        report << "  Critical: " << critical << " (" << (critical*100/feedback.size()) << "%)\n";
        
        if (getEventAverageRating(concertId) < 2.5) {
            report << "\n⚠️  WARNING: Event flagged for low rating (<2.5)\n";
        }
        
        return report.str();
    }

    /**
     * @brief Mark urgent feedback as resolved
     * @param feedback Pointer to feedback to resolve
     */
    void resolveUrgentFeedback(ExtendedFeedback* feedback) {
        feedback->requires_escalation = false;
        // Note: For simplicity, we don't remove from priority queue
        // In production, you'd use a more sophisticated data structure
    }

protected:
    /**
     * @brief Get the ID of a feedback (using submitted timestamp as pseudo-ID)
     */
    int getEntityId(const std::shared_ptr<Model::Feedback>& feedback) const override {
        // Since Feedback doesn't have an ID in the model, we'll use a hash of the timestamp
        return std::hash<std::string>{}(feedback->submitted_at.iso8601String) % 1000000;
    }

    /**
     * @brief Load feedback from binary file
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int feedbackCount = 0;
        readBinary(file, feedbackCount);
        
        for (int i = 0; i < feedbackCount; i++) {
            auto feedback = std::make_shared<Model::Feedback>();
            
            readBinary(file, feedback->rating);
            feedback->comments = readString(file);
            feedback->submitted_at.iso8601String = readString(file);
            
            entities.push_back(feedback);
        }
        
        file.close();
    }

    /**
     * @brief Save feedback to binary file
     */
    bool saveEntities() override {
        std::ofstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << dataFilePath << std::endl;
            return false;
        }
        
        int feedbackCount = static_cast<int>(entities.size());
        writeBinary(file, feedbackCount);
        
        for (const auto& feedback : entities) {
            writeBinary(file, feedback->rating);
            writeString(file, feedback->comments);
            writeString(file, feedback->submitted_at.iso8601String);
        }
        
        file.close();
        return true;
    }

private:
    /**
     * @brief Initialize sentiment analysis keywords
     */
    void initializeSentimentKeywords() {
        positiveKeywords = {"excellent", "amazing", "fantastic", "great", "wonderful", 
                           "awesome", "perfect", "loved", "brilliant", "outstanding"};
        negativeKeywords = {"terrible", "awful", "bad", "horrible", "disappointing", 
                           "poor", "worst", "hate", "boring", "overpriced"};
        criticalKeywords = {"unsafe", "emergency", "injury", "dangerous", "fire", 
                           "violence", "theft", "medical", "security"};
    }

    /**
     * @brief Perform sentiment analysis on feedback
     */
    void analyzeSentiment(ExtendedFeedback* feedback) {
        std::string comments = feedback->baseFeedback->comments;
        std::transform(comments.begin(), comments.end(), comments.begin(), ::tolower);
        
        int positiveScore = 0, negativeScore = 0, criticalScore = 0;
        
        // Check for critical keywords first
        for (const auto& keyword : criticalKeywords) {
            if (comments.find(keyword) != std::string::npos) {
                criticalScore += 3;
            }
        }
        
        // Check sentiment keywords
        for (const auto& keyword : positiveKeywords) {
            if (comments.find(keyword) != std::string::npos) {
                positiveScore++;
            }
        }
        
        for (const auto& keyword : negativeKeywords) {
            if (comments.find(keyword) != std::string::npos) {
                negativeScore++;
            }
        }
        
        // Determine sentiment
        if (criticalScore > 0) {
            feedback->sentiment = SentimentType::CRITICAL;
            feedback->requires_escalation = true;
            feedback->escalation_reason = "Critical keywords detected";
        } else if (feedback->baseFeedback->rating <= 2) {
            feedback->sentiment = SentimentType::NEGATIVE;
            if (feedback->baseFeedback->rating == 1) {
                feedback->requires_escalation = true;
                feedback->escalation_reason = "1-star rating";
            }
        } else if (feedback->baseFeedback->rating >= 4 || positiveScore > negativeScore) {
            feedback->sentiment = SentimentType::POSITIVE;
        } else {
            feedback->sentiment = SentimentType::NEUTRAL;
        }
    }

    /**
     * @brief Update average rating for an event
     */
    void updateEventAverageRating(int concertId) {
        auto feedback = getFeedbackForEvent(concertId);
        if (feedback.empty()) return;
        
        double sum = 0.0;
        for (auto* fb : feedback) {
            sum += fb->baseFeedback->rating;
        }
        
        eventAverageRatings[concertId] = sum / feedback.size();
    }

    /**
     * @brief Reprocess all existing feedback for analysis
     */
    void reprocessAllFeedback() {
        // This would be called when loading existing data
        // For now, it's a placeholder for the concept
    }

    /**
     * @brief Save event-specific feedback to separate file
     */
    void saveEventSpecificFeedback(int concertId) {
        std::string filename = "data/feedback_" + std::to_string(concertId) + ".txt";
        std::ofstream file(filename);
        
        if (!file.is_open()) return;
        
        auto feedback = getFeedbackForEvent(concertId);
        for (auto* fb : feedback) {
            file << "Rating: " << fb->baseFeedback->rating << "\n";
            file << "Comments: " << fb->baseFeedback->comments << "\n";
            file << "Sentiment: " << static_cast<int>(fb->sentiment) << "\n";
            file << "Escalation: " << (fb->requires_escalation ? "YES" : "NO") << "\n";
            file << "---\n";
        }
        
        file.close();
    }
};

// Namespace wrapper for simplified access
namespace FeedbackManager {
    static FeedbackModule& getInstance() {
        static FeedbackModule instance;
        return instance;
    }

    /**
     * @brief Create feedback for an event
     */
    inline std::shared_ptr<Model::Feedback> collectFeedback(
        int concertId, int attendeeId, int rating, const std::string& comments,
        FeedbackCategory category = FeedbackCategory::GENERAL
    ) {
        return getInstance().createFeedback(concertId, attendeeId, rating, comments, category);
    }

    /**
     * @brief Get all feedback for an event
     */
    inline std::vector<ExtendedFeedback*> viewEventFeedback(int concertId) {
        return getInstance().getFeedbackForEvent(concertId);
    }

    // Added for integration test detailed retrieval (alias of viewEventFeedback)
    inline std::vector<ExtendedFeedback*> getFeedbackDetails(int concertId) {
        return getInstance().getFeedbackForEvent(concertId);
    }

    /**
     * @brief Analyze sentiment for an event
     */
    inline std::string analyzeSentiment(int concertId) {
        return getInstance().generateSentimentReport(concertId);
    }

    /**
     * @brief Get urgent feedback requiring escalation
     */
    inline std::vector<ExtendedFeedback*> getUrgentFeedback() {
        return getInstance().getUrgentFeedback();
    }

    /**
     * @brief Get events with low ratings
     */
    inline std::vector<int> getLowRatedEvents() {
        return getInstance().getLowRatedEvents();
    }

    /**
     * @brief Get average rating for an event
     */
    inline double getEventRating(int concertId) {
        return getInstance().getEventAverageRating(concertId);
    }
}