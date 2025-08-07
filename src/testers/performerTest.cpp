#include "../include/performerModule.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

void testPerformerCreation() {
    std::cout << "\n=== Testing Performer Creation ===" << std::endl;
    
    PerformerModule performerModule;
    
    // Test creating performers with different types
    auto performer1 = performerModule.createPerformer(
        "The Beatles", 
        "Band", 
        "contact@beatles.com", 
        "Legendary British rock band formed in Liverpool in 1960.",
        "https://example.com/beatles.jpg"
    );
    
    auto performer2 = performerModule.createPerformer(
        "John Williams", 
        "Solo Artist", 
        "john@example.com", 
        "Renowned film composer known for Star Wars, Jaws, and Indiana Jones soundtracks."
    );
    
    auto performer3 = performerModule.createPerformer(
        "DJ Snake", 
        "DJ", 
        "dj@snakemusic.com", 
        "French DJ and record producer known for electronic dance music.",
        "https://example.com/djsnake.jpg"
    );
    
    // Verify creation
    assert(performer1 != nullptr);
    assert(performer2 != nullptr);
    assert(performer3 != nullptr);
    
    assert(performer1->name == "The Beatles");
    assert(performer1->type == "Band");
    assert(performer1->contact_info == "contact@beatles.com");
    assert(performer1->image_url == "https://example.com/beatles.jpg");
    
    assert(performer2->name == "John Williams");
    assert(performer2->type == "Solo Artist");
    assert(performer2->image_url == ""); // No image URL provided
    
    assert(performer3->name == "DJ Snake");
    assert(performer3->type == "DJ");
    
    // Verify unique IDs
    assert(performer1->performer_id != performer2->performer_id);
    assert(performer2->performer_id != performer3->performer_id);
    assert(performer1->performer_id != performer3->performer_id);
    
    std::cout << "Created performer 1: " << performer1->name << " (ID: " << performer1->performer_id << ")" << std::endl;
    std::cout << "Created performer 2: " << performer2->name << " (ID: " << performer2->performer_id << ")" << std::endl;
    std::cout << "Created performer 3: " << performer3->name << " (ID: " << performer3->performer_id << ")" << std::endl;
    
    std::cout << "✅ Performer creation tests passed!" << std::endl;
}

void testPerformerRetrieval() {
    std::cout << "\n=== Testing Performer Retrieval ===" << std::endl;
    
    PerformerModule performerModule;
    
    // Create test performers
    auto performer1 = performerModule.createPerformer("Adele", "Solo Artist", "adele@music.com", "British singer-songwriter");
    auto performer2 = performerModule.createPerformer("Coldplay", "Band", "coldplay@band.com", "British rock band");
    
    // Test retrieval by ID
    auto retrieved1 = performerModule.getPerformerById(performer1->performer_id);
    auto retrieved2 = performerModule.getPerformerById(performer2->performer_id);
    auto retrievedNonExistent = performerModule.getPerformerById(99999);
    
    assert(retrieved1 != nullptr);
    assert(retrieved2 != nullptr);
    assert(retrievedNonExistent == nullptr);
    
    assert(retrieved1->name == "Adele");
    assert(retrieved2->name == "Coldplay");
    
    // Test get all performers
    auto allPerformers = performerModule.getAllPerformers();
    assert(allPerformers.size() >= 2);
    
    std::cout << "Retrieved performer by ID: " << retrieved1->name << std::endl;
    std::cout << "Total performers in system: " << allPerformers.size() << std::endl;
    
    std::cout << "✅ Performer retrieval tests passed!" << std::endl;
}

void testPerformerSearch() {
    std::cout << "\n=== Testing Performer Search ===" << std::endl;
    
    PerformerModule performerModule;
    
    // Create test performers with various names and types
    performerModule.createPerformer("Taylor Swift", "Solo Artist", "taylor@music.com", "American singer-songwriter");
    performerModule.createPerformer("Imagine Dragons", "Band", "dragons@rock.com", "American pop rock band");
    performerModule.createPerformer("Calvin Harris", "DJ", "calvin@edm.com", "Scottish DJ and producer");
    performerModule.createPerformer("Swift Justice", "Band", "swift@justice.com", "Rock band");
    performerModule.createPerformer("The Weeknd", "Solo Artist", "weeknd@music.com", "Canadian singer");
    
    // Test search by name (case insensitive)
    auto swiftResults = performerModule.findPerformersByName("swift");
    assert(swiftResults.size() == 2); // Should find "Taylor Swift" and "Swift Justice"
    
    auto dragonsResults = performerModule.findPerformersByName("DRAGONS");
    assert(dragonsResults.size() == 1);
    assert(dragonsResults[0]->name == "Imagine Dragons");
    
    auto partialResults = performerModule.findPerformersByName("week");
    assert(partialResults.size() == 1);
    assert(partialResults[0]->name == "The Weeknd");
    
    // Test search by type (case insensitive)
    auto bandResults = performerModule.findPerformersByType("band");
    assert(bandResults.size() >= 2); // At least "Imagine Dragons" and "Swift Justice"
    
    auto soloResults = performerModule.findPerformersByType("SOLO artist");
    assert(soloResults.size() >= 2); // At least "Taylor Swift" and "The Weeknd"
    
    auto djResults = performerModule.findPerformersByType("dj");
    assert(djResults.size() >= 1); // At least "Calvin Harris"
    
    // Test no results
    auto noResults = performerModule.findPerformersByName("NonExistentPerformer");
    assert(noResults.empty());
    
    std::cout << "Found " << swiftResults.size() << " performers with 'swift' in name" << std::endl;
    std::cout << "Found " << bandResults.size() << " band performers" << std::endl;
    std::cout << "Found " << soloResults.size() << " solo artist performers" << std::endl;
    
    std::cout << "✅ Performer search tests passed!" << std::endl;
}

void testPerformerUpdate() {
    std::cout << "\n=== Testing Performer Update ===" << std::endl;
    
    PerformerModule performerModule;
    
    // Create a test performer
    auto performer = performerModule.createPerformer(
        "Original Name", 
        "Original Type", 
        "original@contact.com", 
        "Original bio"
    );
    
    int performerId = performer->performer_id;
    
    // Test updating individual fields
    bool result1 = performerModule.updatePerformer(performerId, "Updated Name");
    assert(result1 == true);
    
    auto updated1 = performerModule.getPerformerById(performerId);
    assert(updated1->name == "Updated Name");
    assert(updated1->type == "Original Type"); // Should remain unchanged
    
    // Test updating multiple fields
    bool result2 = performerModule.updatePerformer(
        performerId, 
        "Final Name", 
        "New Type", 
        "new@contact.com", 
        "Updated biography",
        "https://newimage.com/photo.jpg"
    );
    assert(result2 == true);
    
    auto updated2 = performerModule.getPerformerById(performerId);
    assert(updated2->name == "Final Name");
    assert(updated2->type == "New Type");
    assert(updated2->contact_info == "new@contact.com");
    assert(updated2->bio == "Updated biography");
    assert(updated2->image_url == "https://newimage.com/photo.jpg");
    
    // Test updating with empty strings (should not change)
    bool result3 = performerModule.updatePerformer(performerId, "", "Newer Type", "");
    assert(result3 == true);
    
    auto updated3 = performerModule.getPerformerById(performerId);
    assert(updated3->name == "Final Name"); // Should remain unchanged
    assert(updated3->type == "Newer Type"); // Should be updated
    assert(updated3->contact_info == "new@contact.com"); // Should remain unchanged
    
    // Test updating non-existent performer
    bool result4 = performerModule.updatePerformer(99999, "Non-existent");
    assert(result4 == false);
    
    std::cout << "Updated performer name: " << updated3->name << std::endl;
    std::cout << "Updated performer type: " << updated3->type << std::endl;
    
    std::cout << "✅ Performer update tests passed!" << std::endl;
}

void testPerformerDeletion() {
    std::cout << "\n=== Testing Performer Deletion ===" << std::endl;
    
    PerformerModule performerModule;
    
    // Create test performers
    auto performer1 = performerModule.createPerformer("To Be Deleted", "Band", "delete@me.com", "Will be removed");
    auto performer2 = performerModule.createPerformer("To Keep", "Solo Artist", "keep@me.com", "Will remain");
    
    int deleteId = performer1->performer_id;
    int keepId = performer2->performer_id;
    
    // Verify they exist
    assert(performerModule.getPerformerById(deleteId) != nullptr);
    assert(performerModule.getPerformerById(keepId) != nullptr);
    
    // Delete one performer
    bool deleteResult = performerModule.deletePerformer(deleteId);
    assert(deleteResult == true);
    
    // Verify deletion
    assert(performerModule.getPerformerById(deleteId) == nullptr);
    assert(performerModule.getPerformerById(keepId) != nullptr);
    
    // Try to delete non-existent performer
    bool deleteNonExistent = performerModule.deletePerformer(99999);
    assert(deleteNonExistent == false);
    
    // Try to delete already deleted performer
    bool deleteAgain = performerModule.deletePerformer(deleteId);
    assert(deleteAgain == false);
    
    std::cout << "Successfully deleted performer with ID: " << deleteId << std::endl;
    std::cout << "Kept performer: " << performerModule.getPerformerById(keepId)->name << std::endl;
    
    std::cout << "✅ Performer deletion tests passed!" << std::endl;
}

void testPerformerManager() {
    std::cout << "\n=== Testing PerformerManager Namespace ===" << std::endl;
    
    // Test the namespace wrapper functions
    auto performer1 = PerformerManager::createPerformer(
        "Namespace Test Band", 
        "Rock Band", 
        "namespace@test.com", 
        "Testing namespace functionality"
    );
    
    assert(performer1 != nullptr);
    assert(performer1->name == "Namespace Test Band");
    
    // Test retrieval through namespace
    auto retrieved = PerformerManager::getPerformerById(performer1->performer_id);
    assert(retrieved != nullptr);
    assert(retrieved->name == "Namespace Test Band");
    
    // Test update through namespace
    bool updateResult = PerformerManager::updatePerformer(
        performer1->performer_id, 
        "Updated Namespace Band"
    );
    assert(updateResult == true);
    
    auto updatedPerformer = PerformerManager::getPerformerById(performer1->performer_id);
    assert(updatedPerformer->name == "Updated Namespace Band");
    
    // Test search through namespace
    auto searchResults = PerformerManager::findPerformersByName("Namespace");
    assert(searchResults.size() >= 1);
    
    auto typeResults = PerformerManager::findPerformersByType("Rock");
    assert(typeResults.size() >= 1);
    
    // Test get all through namespace
    auto allPerformers = PerformerManager::getAllPerformers();
    assert(allPerformers.size() >= 1);
    
    // Test deletion through namespace
    bool deleteResult = PerformerManager::deletePerformer(performer1->performer_id);
    assert(deleteResult == true);
    
    auto deletedCheck = PerformerManager::getPerformerById(performer1->performer_id);
    assert(deletedCheck == nullptr);
    
    std::cout << "Created and managed performer through namespace: " << performer1->name << std::endl;
    std::cout << "Total performers via namespace: " << allPerformers.size() << std::endl;
    
    std::cout << "✅ PerformerManager namespace tests passed!" << std::endl;
}

void testDataPersistence() {
    std::cout << "\n=== Testing Data Persistence ===" << std::endl;
    
    // Create performers in one module instance
    {
        PerformerModule performerModule;
        performerModule.createPerformer("Persistence Test 1", "Band", "test1@persist.com", "First test performer");
        performerModule.createPerformer("Persistence Test 2", "Solo Artist", "test2@persist.com", "Second test performer");
        // Module destructor will save data
    }
    
    // Load data in a new module instance
    {
        PerformerModule newPerformerModule;
        auto allPerformers = newPerformerModule.getAllPerformers();
        
        // Find our test performers
        bool found1 = false, found2 = false;
        for (const auto& performer : allPerformers) {
            if (performer->name == "Persistence Test 1") {
                found1 = true;
                assert(performer->type == "Band");
                assert(performer->contact_info == "test1@persist.com");
            }
            if (performer->name == "Persistence Test 2") {
                found2 = true;
                assert(performer->type == "Solo Artist");
                assert(performer->contact_info == "test2@persist.com");
            }
        }
        
        assert(found1 && found2);
        std::cout << "Successfully persisted and loaded " << allPerformers.size() << " performers" << std::endl;
    }
    
    std::cout << "✅ Data persistence tests passed!" << std::endl;
}

int main() {
    std::cout << "🎵 Starting PerformerModule Tests 🎵" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testPerformerCreation();
        testPerformerRetrieval();
        testPerformerSearch();
        testPerformerUpdate();
        testPerformerDeletion();
        testPerformerManager();
        testDataPersistence();
        
        std::cout << "\n🎉 All PerformerModule tests passed successfully! 🎉" << std::endl;
        std::cout << "The PerformerModule is working correctly and ready for integration." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
