#include <iostream>
#include <memory>
#include "../include/venueModule.hpp"

/**
 * @brief Focused test for 2D seating plan visualization
 * This test creates a venue and demonstrates the 2D array-based seating functionality
 */
int main() {
    std::cout << "=====================================================================" << std::endl;
    std::cout << "           2D SEATING PLAN VISUALIZATION DEMONSTRATION" << std::endl;
    std::cout << "=====================================================================" << std::endl;
    std::cout << std::endl;
    
    try {
        // Create a test venue
        std::cout << "🏢 Creating test venue..." << std::endl;
        auto venue = VenueManager::createVenue(
            "Demo Concert Hall",
            "123 Music Avenue",
            "Concert City",
            "CC",
            "12345",
            "USA",
            100,
            "Demonstration venue for 2D seating functionality",
            "info@democoncerthall.com"
        );
        
        if (!venue) {
            std::cerr << "❌ Failed to create venue!" << std::endl;
            return 1;
        }
        
        std::cout << "✅ Created venue: " << venue->name << " (ID: " << venue->id << ")" << std::endl;
        std::cout << std::endl;
        
        // Initialize 2D seating plan
        std::cout << "🎭 Initializing 2D seating plan (6 rows × 8 seats)..." << std::endl;
        bool initSuccess = VenueManager::initializeVenueSeatingPlan(venue->id, 6, 8);
        if (!initSuccess) {
            std::cerr << "❌ Failed to initialize seating plan!" << std::endl;
            return 1;
        }
        std::cout << "✅ 2D seating plan initialized" << std::endl;
        std::cout << std::endl;
        
        // Create standard seating arrangement
        std::cout << "🪑 Creating standard seating arrangement..." << std::endl;
        bool createSuccess = VenueManager::createStandardSeatingPlan(venue->id, 6, 8, "Regular");
        if (!createSuccess) {
            std::cerr << "❌ Failed to create seating arrangement!" << std::endl;
            return 1;
        }
        std::cout << "✅ Created 48 seats in 6 rows" << std::endl;
        std::cout << std::endl;
        
        // Display initial seating plan
        std::cout << "📋 INITIAL SEATING PLAN:" << std::endl;
        std::cout << VenueManager::getSeatingPlanVisualization(venue->id) << std::endl;
        
        // Display initial statistics
        std::cout << "📊 INITIAL STATISTICS:" << std::endl;
        std::cout << VenueManager::getVenueSeatingStats(venue->id) << std::endl;
        
        // Demonstrate 2D operations
        std::cout << "🎯 Demonstrating 2D array operations..." << std::endl;
        std::cout << std::endl;
        
        // 1. Get specific seat using 2D coordinates
        auto seatB4 = VenueManager::getSeatAt(venue->id, 1, 3); // Row B, Seat 4
        if (seatB4) {
            std::cout << "✅ Found seat at Row B, Column 4: " << seatB4->row_number << seatB4->col_number 
                      << " (ID: " << seatB4->seat_id << ")" << std::endl;
        }
        
        // 2. Get all seats in row C
        auto rowCSeats = VenueManager::getSeatsInRow(venue->id, 2);
        std::cout << "✅ Row C contains " << rowCSeats.size() << " seats" << std::endl;
        
        // 3. Find adjacent seats for group booking
        auto adjacentGroups = VenueManager::findAdjacentSeats(venue->id, 4);
        std::cout << "✅ Found " << adjacentGroups.size() << " groups of 4 adjacent seats" << std::endl;
        std::cout << std::endl;
        
        // Make some reservations to show the visualization changes
        std::cout << "🎫 Making sample reservations..." << std::endl;
        
        // Reserve a block of 4 seats
        if (!adjacentGroups.empty()) {
            bool reserved = VenueManager::reserveSeatBlock(venue->id, adjacentGroups[0]);
            if (reserved) {
                std::cout << "✅ Reserved 4 adjacent seats: ";
                for (const auto& seat : adjacentGroups[0]) {
                    std::cout << seat->row_number << seat->col_number << " ";
                }
                std::cout << std::endl;
            }
        }
        
        // Check in some individual seats
        auto seatA2 = VenueManager::getSeatAt(venue->id, 0, 1);
        if (seatA2) {
            VenueManager::updateSeatStatus(venue->id, seatA2->seat_id, Model::TicketStatus::CHECKED_IN);
            std::cout << "✅ Checked in seat " << seatA2->row_number << seatA2->col_number << std::endl;
        }
        
        auto seatF7 = VenueManager::getSeatAt(venue->id, 5, 6);
        if (seatF7) {
            VenueManager::updateSeatStatus(venue->id, seatF7->seat_id, Model::TicketStatus::CHECKED_IN);
            std::cout << "✅ Checked in seat " << seatF7->row_number << seatF7->col_number << std::endl;
        }
        
        // Cancel a seat
        auto seatD5 = VenueManager::getSeatAt(venue->id, 3, 4);
        if (seatD5) {
            VenueManager::updateSeatStatus(venue->id, seatD5->seat_id, Model::TicketStatus::CANCELLED);
            std::cout << "✅ Cancelled seat " << seatD5->row_number << seatD5->col_number << std::endl;
        }
        
        std::cout << std::endl;
        
        // Display updated seating plan
        std::cout << "📋 UPDATED SEATING PLAN (after reservations):" << std::endl;
        std::cout << VenueManager::getSeatingPlanVisualization(venue->id) << std::endl;
        
        // Display updated statistics
        std::cout << "📊 UPDATED STATISTICS:" << std::endl;
        std::cout << VenueManager::getVenueSeatingStats(venue->id) << std::endl;
        
        // Demonstrate finding adjacent seats after reservations
        auto remainingAdjacent = VenueManager::findAdjacentSeats(venue->id, 3);
        std::cout << "🔍 After reservations, found " << remainingAdjacent.size() 
                  << " groups of 3 adjacent seats remaining" << std::endl;
        
        if (!remainingAdjacent.empty()) {
            std::cout << "   First available group: ";
            for (const auto& seat : remainingAdjacent[0]) {
                std::cout << seat->row_number << seat->col_number << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "🎉 2D seating plan demonstration completed successfully!" << std::endl;
        std::cout << "=====================================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error during demonstration: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
