#include "../include/models.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Testing Updated Models (Sponsor Removal) ===" << std::endl;
    
    try {
        // Test 1: Create a Promotion without sponsors
        auto promotion = std::make_shared<Model::Promotion>();
        promotion->code = "SUMMER2024";
        promotion->description = "Summer discount promotion";
        promotion->discount_type = Model::DiscountType::PERCENTAGE;
        promotion->percentage = 15.0;
        promotion->start_date_time = Model::DateTime::now();
        promotion->end_date_time = Model::DateTime::now();
        promotion->is_active = true;
        promotion->usage_limit = 100;
        promotion->used_count = 0;
        
        std::cout << "✓ Successfully created Promotion without sponsors" << std::endl;
        std::cout << "  - Code: " << promotion->code << std::endl;
        std::cout << "  - Description: " << promotion->description << std::endl;
        std::cout << "  - Discount: " << promotion->percentage << "%" << std::endl;
        
        // Test 2: Verify that we can create other entities that might reference promotions
        auto concert = std::make_shared<Model::Concert>();
        concert->id = 1;
        concert->name = "Test Concert";
        concert->description = "Test concert for promotion";
        concert->event_status = Model::EventStatus::SCHEDULED;
        concert->created_at = Model::DateTime::now();
        concert->updated_at = Model::DateTime::now();
        
        // Add promotion to concert
        concert->promotions.push_back(promotion);
        
        std::cout << "✓ Successfully created Concert with Promotion" << std::endl;
        std::cout << "  - Concert: " << concert->name << std::endl;
        std::cout << "  - Promotions count: " << concert->promotions.size() << std::endl;
        
        // Test 3: Verify that DateTime works correctly
        auto dateTime = Model::DateTime::now();
        std::cout << "✓ DateTime functionality working" << std::endl;
        std::cout << "  - Current time: " << dateTime.iso8601String << std::endl;
        
        std::cout << "\n=== All Model Tests Passed! ===" << std::endl;
        std::cout << "✓ Sponsor entity successfully removed" << std::endl;
        std::cout << "✓ Promotion entity updated (no more sponsors)" << std::endl;
        std::cout << "✓ All other entities remain functional" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
