#pragma once
#include <string>
#include <vector>

namespace FeedbackManager {

struct Feedback {
    int rating;
    std::string comments;
    std::string submitted_at;
};

// Collect feedback from a user
void collectFeedback(int rating, const std::string& comments);

// View all feedback ratings
std::vector<Feedback> viewRatings();

// Analyze comments for insights (e.g., count positive/negative words)
void analyzeComments(const std::vector<Feedback>& feedbackList);

} // namespace feedback