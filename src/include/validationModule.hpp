#pragma once

#include <string>
#include <regex>
#include <vector>
#include <cctype>

namespace InputValidator {
    
    /**
     * @brief Validation result structure
     */
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        
        ValidationResult(bool valid = true, const std::string& message = "") 
            : isValid(valid), errorMessage(message) {}
    };

    /**
     * @brief Email validation using regex
     * Pattern: local@domain.tld
     */
    ValidationResult validateEmail(const std::string& email) {
        if (email.empty()) {
            return ValidationResult(false, "Email cannot be empty");
        }
        
        // RFC 5322 compliant email regex (simplified)
        std::regex emailPattern(
            R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
        );
        
        if (!std::regex_match(email, emailPattern)) {
            return ValidationResult(false, "Invalid email format. Use format: example@domain.com");
        }
        
        if (email.length() > 254) {
            return ValidationResult(false, "Email address too long (max 254 characters)");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Password validation with strength requirements
     * Requirements: 8+ chars, uppercase, lowercase, digit, special char
     */
    ValidationResult validatePassword(const std::string& password) {
        if (password.empty()) {
            return ValidationResult(false, "Password cannot be empty");
        }
        
        if (password.length() < 8) {
            return ValidationResult(false, "Password must be at least 8 characters long");
        }
        
        if (password.length() > 128) {
            return ValidationResult(false, "Password too long (max 128 characters)");
        }
        
        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        
        for (char c : password) {
            if (std::isupper(c)) hasUpper = true;
            else if (std::islower(c)) hasLower = true;
            else if (std::isdigit(c)) hasDigit = true;
            else if (std::ispunct(c) || c == ' ') hasSpecial = true;
        }
        
        std::vector<std::string> missing;
        if (!hasUpper) missing.push_back("uppercase letter");
        if (!hasLower) missing.push_back("lowercase letter");
        if (!hasDigit) missing.push_back("digit");
        if (!hasSpecial) missing.push_back("special character");
        
        if (!missing.empty()) {
            std::string error = "Password must contain: ";
            for (size_t i = 0; i < missing.size(); ++i) {
                error += missing[i];
                if (i < missing.size() - 1) error += ", ";
            }
            return ValidationResult(false, error);
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Username validation
     * Requirements: 3-30 chars, alphanumeric + underscore, no spaces
     */
    ValidationResult validateUsername(const std::string& username) {
        if (username.empty()) {
            return ValidationResult(false, "Username cannot be empty");
        }
        
        if (username.length() < 3) {
            return ValidationResult(false, "Username must be at least 3 characters long");
        }
        
        if (username.length() > 30) {
            return ValidationResult(false, "Username too long (max 30 characters)");
        }
        
        // Username pattern: alphanumeric + underscore, must start with letter
        std::regex usernamePattern(R"(^[a-zA-Z][a-zA-Z0-9_]*$)");
        
        if (!std::regex_match(username, usernamePattern)) {
            return ValidationResult(false, "Username must start with letter and contain only letters, numbers, and underscores");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Phone number validation
     * Supports various formats: +1234567890, (123) 456-7890, 123-456-7890, etc.
     */
    ValidationResult validatePhoneNumber(const std::string& phone) {
        if (phone.empty()) {
            return ValidationResult(false, "Phone number cannot be empty");
        }
        
        // Remove all non-digit characters except +
        std::string cleanPhone = phone;
        cleanPhone.erase(std::remove_if(cleanPhone.begin(), cleanPhone.end(), 
            [](char c) { return !std::isdigit(c) && c != '+'; }), cleanPhone.end());
        
        // Check various phone number patterns
        std::vector<std::regex> phonePatterns = {
            std::regex(R"(^\+?[1-9]\d{7,14}$)"),          // International format
            std::regex(R"(^\d{10}$)"),                     // US format (10 digits)
            std::regex(R"(^\+1\d{10}$)")                   // US format with country code
        };
        
        bool isValid = false;
        for (const auto& pattern : phonePatterns) {
            if (std::regex_match(cleanPhone, pattern)) {
                isValid = true;
                break;
            }
        }
        
        if (!isValid) {
            return ValidationResult(false, "Invalid phone number format. Use formats like: +1234567890, (123) 456-7890, or 123-456-7890");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Name validation (first name, last name)
     * Requirements: 1-50 chars, letters and spaces only, no leading/trailing spaces
     */
    ValidationResult validateName(const std::string& name, const std::string& fieldName = "Name") {
        if (name.empty()) {
            return ValidationResult(false, fieldName + " cannot be empty");
        }
        
        if (name.length() > 50) {
            return ValidationResult(false, fieldName + " too long (max 50 characters)");
        }
        
        // Check for leading/trailing spaces
        if (name.front() == ' ' || name.back() == ' ') {
            return ValidationResult(false, fieldName + " cannot start or end with spaces");
        }
        
        // Name pattern: letters, spaces, hyphens, apostrophes
        std::regex namePattern(R"(^[a-zA-Z\s'-]+$)");
        
        if (!std::regex_match(name, namePattern)) {
            return ValidationResult(false, fieldName + " can only contain letters, spaces, hyphens, and apostrophes");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief URL validation
     * Validates HTTP/HTTPS URLs
     */
    ValidationResult validateURL(const std::string& url) {
        if (url.empty()) {
            return ValidationResult(false, "URL cannot be empty");
        }
        
        std::regex urlPattern(
            R"(^https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)$)"
        );
        
        if (!std::regex_match(url, urlPattern)) {
            return ValidationResult(false, "Invalid URL format. Use format: http://example.com or https://example.com");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Credit card number validation (basic Luhn algorithm)
     */
    ValidationResult validateCreditCard(const std::string& cardNumber) {
        if (cardNumber.empty()) {
            return ValidationResult(false, "Credit card number cannot be empty");
        }
        
        // Remove spaces and hyphens
        std::string cleanCard = cardNumber;
        cleanCard.erase(std::remove_if(cleanCard.begin(), cleanCard.end(), 
            [](char c) { return c == ' ' || c == '-'; }), cleanCard.end());
        
        // Check if all digits
        if (!std::all_of(cleanCard.begin(), cleanCard.end(), ::isdigit)) {
            return ValidationResult(false, "Credit card number must contain only digits");
        }
        
        // Check length (13-19 digits for most cards)
        if (cleanCard.length() < 13 || cleanCard.length() > 19) {
            return ValidationResult(false, "Credit card number must be 13-19 digits long");
        }
        
        // Luhn algorithm validation
        int sum = 0;
        bool doubleDigit = false;
        
        for (int i = cleanCard.length() - 1; i >= 0; i--) {
            int digit = cleanCard[i] - '0';
            
            if (doubleDigit) {
                digit *= 2;
                if (digit > 9) digit = digit / 10 + digit % 10;
            }
            
            sum += digit;
            doubleDigit = !doubleDigit;
        }
        
        if (sum % 10 != 0) {
            return ValidationResult(false, "Invalid credit card number");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Date validation (YYYY-MM-DD format)
     */
    ValidationResult validateDate(const std::string& date) {
        if (date.empty()) {
            return ValidationResult(false, "Date cannot be empty");
        }
        
        std::regex datePattern(R"(^\d{4}-\d{2}-\d{2}$)");
        
        if (!std::regex_match(date, datePattern)) {
            return ValidationResult(false, "Invalid date format. Use YYYY-MM-DD format");
        }
        
        // Extract year, month, day
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));
        
        // Basic range validation
        if (year < 1900 || year > 2100) {
            return ValidationResult(false, "Year must be between 1900 and 2100");
        }
        
        if (month < 1 || month > 12) {
            return ValidationResult(false, "Month must be between 01 and 12");
        }
        
        if (day < 1 || day > 31) {
            return ValidationResult(false, "Day must be between 01 and 31");
        }
        
        // Days in month validation (simplified)
        std::vector<int> daysInMonth = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        
        // Leap year check
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            daysInMonth[1] = 29;
        }
        
        if (day > daysInMonth[month - 1]) {
            return ValidationResult(false, "Invalid day for the given month");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Time validation (HH:MM format)
     */
    ValidationResult validateTime(const std::string& time) {
        if (time.empty()) {
            return ValidationResult(false, "Time cannot be empty");
        }
        
        std::regex timePattern(R"(^([01]?[0-9]|2[0-3]):[0-5][0-9]$)");
        
        if (!std::regex_match(time, timePattern)) {
            return ValidationResult(false, "Invalid time format. Use HH:MM format (24-hour)");
        }
        
        return ValidationResult(true);
    }

    /**
     * @brief Postal code validation (supports various formats)
     */
    ValidationResult validatePostalCode(const std::string& postalCode, const std::string& country = "US") {
        if (postalCode.empty()) {
            return ValidationResult(false, "Postal code cannot be empty");
        }
        
        std::regex pattern;
        std::string formatMsg;
        
        if (country == "US") {
            pattern = std::regex(R"(^\d{5}(-\d{4})?$)");
            formatMsg = "US format: 12345 or 12345-6789";
        } else if (country == "CA") {
            pattern = std::regex(R"(^[A-Za-z]\d[A-Za-z] \d[A-Za-z]\d$)");
            formatMsg = "Canadian format: A1A 1A1";
        } else if (country == "UK") {
            pattern = std::regex(R"(^[A-Z]{1,2}\d[A-Z\d]? \d[A-Z]{2}$)");
            formatMsg = "UK format: SW1A 1AA";
        } else {
            // Generic pattern for other countries
            pattern = std::regex(R"(^[A-Za-z0-9\s-]{3,10}$)");
            formatMsg = "3-10 characters, letters, numbers, spaces, hyphens";
        }
        
        if (!std::regex_match(postalCode, pattern)) {
            return ValidationResult(false, "Invalid postal code format. Expected " + formatMsg);
        }
        
        return ValidationResult(true);
    }
}
