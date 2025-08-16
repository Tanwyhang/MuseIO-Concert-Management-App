#include "../include/performerModule.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Performer Module Test ===" << std::endl;
    
    try {
        // Test 1: Create performers using the module directly
        PerformerModule performerModule;
        
        auto performer1 = performerModule.createPerformer(
            "The Beatles", 
            "Band", 
            "contact@beatles.com", 
            "Legendary British rock band from Liverpool.",
            "https://example.com/beatles.jpg"
        );
        
        auto performer2 = performerModule.createPerformer(
            "John Doe", 
            "Solo Artist", 
            "john@example.com", 
            "Singer-songwriter with folk influences."
        );
        
        std::cout << "✓ Created performers successfully" << std::endl;
        
        // Test 2: Search by name
        auto searchResults = performerModule.findPerformersByName("beatles");
        assert(searchResults.size() == 1);
        assert(searchResults[0]->name == "The Beatles");
        std::cout << "✓ Name search working correctly" << std::endl;
        
        // Test 3: Search by type
        auto bandResults = performerModule.findPerformersByType("band");
        assert(bandResults.size() == 1);
        assert(bandResults[0]->type == "Band");
        std::cout << "✓ Type search working correctly" << std::endl;
        
        // Test 4: Update performer
        bool updateResult = performerModule.updatePerformer(
            performer1->performer_id, 
            "", // Don't change name
            "", // Don't change type
            "newcontact@beatles.com" // Update contact
        );
        assert(updateResult == true);
        
        auto updatedPerformer = performerModule.getPerformerById(performer1->performer_id);
        assert(updatedPerformer->contact_info == "newcontact@beatles.com");
        std::cout << "✓ Update working correctly" << std::endl;
        
        // Test 5: Using namespace wrapper
        std::cout << "\n--- Testing namespace wrapper ---" << std::endl;
        
        auto performer3 = PerformerManager::createPerformer(
            "Mozart", 
            "Classical Composer", 
            "wolfgang@classical.com", 
            "Austrian classical composer"
        );
        
        auto allPerformers = PerformerManager::getAllPerformers();
        std::cout << "Total performers: " << allPerformers.size() << std::endl;
        
        for (const auto& performer : allPerformers) {
            std::cout << "- ID: " << performer->performer_id 
                     << ", Name: " << performer->name 
                     << ", Type: " << performer->type << std::endl;
        }
        
        std::cout << "✓ Namespace wrapper working correctly" << std::endl;
        
        std::cout << "\n=== All Performer Module Tests Passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
