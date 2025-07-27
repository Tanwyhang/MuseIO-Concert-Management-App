#include <iostream>
#include "include/models.h"
#include "include/dataManager.h"

using namespace MuseIO;

// Sample function to demonstrate data persistence
void demonstrateDataPersistence() {
    // Create a data manager instance
    DataManager dataManager("data");
    
    // Create some test concerts
    std::vector<std::shared_ptr<Concert>> concerts;
    
    // Create a venue for the concerts
    auto venue = std::make_shared<Venue>(1, "Madison Square Garden", "4 Pennsylvania Plaza", 
                                         "New York", "NY", "10001", "USA", 20000,
                                         "World's most famous arena");
    
    // Create a few concerts
    auto now = DateTime::now();
    auto tomorrow = DateTime(now.toString()); // For simplicity, same date
    auto nextWeek = DateTime(now.toString()); // For simplicity, same date
    
    auto concert1 = std::make_shared<Concert>(1, "Rock Festival", "Annual rock music festival",
                                             now, tomorrow, venue, EventStatus::SCHEDULED);
                                             
    auto concert2 = std::make_shared<Concert>(2, "Jazz Night", "Smooth jazz evening",
                                             tomorrow, nextWeek, venue, EventStatus::SCHEDULED);
                                             
    concerts.push_back(concert1);
    concerts.push_back(concert2);
    
    // Save the concerts to a .dat file
    std::cout << "Saving concerts to data file..." << std::endl;
    if (dataManager.saveData(concerts, "concerts.dat")) {
        std::cout << "Concerts saved successfully!" << std::endl;
    } else {
        std::cout << "Failed to save concerts." << std::endl;
    }
    
    // Clear the concerts vector to demonstrate loading
    concerts.clear();
    
    // Load the concerts from the .dat file
    std::cout << "\nLoading concerts from data file..." << std::endl;
    if (dataManager.loadData(concerts, "concerts.dat")) {
        std::cout << "Concerts loaded successfully!" << std::endl;
        std::cout << "Loaded " << concerts.size() << " concerts:" << std::endl;
        
        // Display the loaded concerts
        for (const auto& concert : concerts) {
            std::cout << "- ID: " << concert->getId() 
                      << ", Name: " << concert->getName()
                      << ", Status: " << static_cast<int>(concert->getEventStatus())
                      << std::endl;
        }
    } else {
        std::cout << "Failed to load concerts." << std::endl;
    }
}

// You can call this function from your main.cpp to test
// demonstrateDataPersistence();
