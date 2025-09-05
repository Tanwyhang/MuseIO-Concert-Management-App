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
#include <iomanip>

/**
 * @brief Module for managing Venue entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for venue management including CRUD operations
 * and venue-specific search capabilities.
 */
class VenueModule : public BaseModule<Model::Venue> {
public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing venues
     * @param filePath Path to the venues data file
     */
    VenueModule(const std::string& filePath = "data/venues.dat") : BaseModule<Model::Venue>(filePath) {
        loadEntities();
    }

    /**
     * @brief Destructor
     * Ensures venues are saved before destruction
     */
    ~VenueModule() override {
        saveEntities();
    }

    /**
     * @brief Create a new venue
     * 
     * @param name Venue name
     * @param address Street address
     * @param city City name
     * @param state State/province name
     * @param zipCode Postal/ZIP code
     * @param country Country name
     * @param capacity Maximum capacity
     * @param description Venue description
     * @param contactInfo Contact information
     * @param seatmap Optional seatmap reference (URL or file path)
     * @return std::shared_ptr<Model::Venue> Pointer to created venue
     */
    std::shared_ptr<Model::Venue> createVenue(
        const std::string& name,
        const std::string& address,
        const std::string& city,
        const std::string& state,
        const std::string& zipCode,
        const std::string& country,
        int capacity,
        const std::string& description = "",
        const std::string& contactInfo = "",
        const std::string& seatmap = ""
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the venue object
        auto venue = std::make_shared<Model::Venue>();
        venue->id = newId;
        venue->name = name;
        venue->address = address;
        venue->city = city;
        venue->state = state;
        venue->zip_code = zipCode;
        venue->country = country;
        venue->capacity = capacity;
        venue->description = description;
        venue->contact_info = contactInfo;
        venue->seatmap = seatmap;
        venue->seats = {}; // Initialize empty seat vector
        
        // Add to collection
        entities.push_back(venue);
        
        // Save to file
        saveEntities();
        
        return venue;
    }

    /**
     * @brief Get a venue by ID
     * @param id Venue ID to find
     * @return std::shared_ptr<Model::Venue> Pointer to venue or nullptr
     */
    std::shared_ptr<Model::Venue> getVenueById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all venues
     * @return const std::vector<std::shared_ptr<Model::Venue>>& Reference to venue vector
     */
    const std::vector<std::shared_ptr<Model::Venue>>& getAllVenues() const {
        return getAll();
    }

    /**
     * @brief Find venues by partial name match (case insensitive)
     * @param nameQuery Partial name to search for
     * @return std::vector<std::shared_ptr<Model::Venue>> Vector of matching venues
     */
    std::vector<std::shared_ptr<Model::Venue>> findVenuesByName(const std::string& nameQuery) {
        return findByPredicate([&nameQuery](const std::shared_ptr<Model::Venue>& venue) {
            // Case insensitive search
            std::string venueName = venue->name;
            std::string query = nameQuery;
            
            // Convert both to lowercase for comparison
            std::transform(venueName.begin(), venueName.end(), venueName.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return venueName.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Find venues by city
     * @param city City name to search for
     * @return std::vector<std::shared_ptr<Model::Venue>> Venues in the specified city
     */
    std::vector<std::shared_ptr<Model::Venue>> findVenuesByCity(const std::string& city) {
        return findByPredicate([&city](const std::shared_ptr<Model::Venue>& venue) {
            std::string venueCity = venue->city;
            std::string searchCity = city;
            
            // Convert both to lowercase for case-insensitive comparison
            std::transform(venueCity.begin(), venueCity.end(), venueCity.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(searchCity.begin(), searchCity.end(), searchCity.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return venueCity == searchCity;
        });
    }

    /**
     * @brief Find venues by minimum capacity
     * @param minCapacity Minimum capacity required
     * @return std::vector<std::shared_ptr<Model::Venue>> Venues with sufficient capacity
     */
    std::vector<std::shared_ptr<Model::Venue>> findVenuesByCapacity(int minCapacity) {
        return findByPredicate([minCapacity](const std::shared_ptr<Model::Venue>& venue) {
            return venue->capacity >= minCapacity;
        });
    }

    /**
     * @brief Update a venue's information
     * 
     * @param id Venue ID to update
     * @param name New name (if empty, no change)
     * @param address New address (if empty, no change)
     * @param city New city (if empty, no change)
     * @param state New state (if empty, no change)
     * @param zipCode New ZIP code (if empty, no change)
     * @param country New country (if empty, no change)
     * @param capacity New capacity (if <= 0, no change)
     * @param description New description (if empty, no change)
     * @param contactInfo New contact info (if empty, no change)
     * @param seatmap New seatmap (if empty, no change)
     * @return true if update successful
     * @return false if venue not found or save failed
     */
    bool updateVenue(
        int id,
        const std::string& name = "",
        const std::string& address = "",
        const std::string& city = "",
        const std::string& state = "",
        const std::string& zipCode = "",
        const std::string& country = "",
        int capacity = 0,
        const std::string& description = "",
        const std::string& contactInfo = "",
        const std::string& seatmap = ""
    ) {
        auto venue = getById(id);
        if (!venue) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) venue->name = name;
        if (!address.empty()) venue->address = address;
        if (!city.empty()) venue->city = city;
        if (!state.empty()) venue->state = state;
        if (!zipCode.empty()) venue->zip_code = zipCode;
        if (!country.empty()) venue->country = country;
        if (capacity > 0) venue->capacity = capacity;
        if (!description.empty()) venue->description = description;
        if (!contactInfo.empty()) venue->contact_info = contactInfo;
        if (!seatmap.empty()) venue->seatmap = seatmap;
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Initialize the 2D seating plan for a venue
     * @param venueId Venue ID
     * @param numRows Number of rows in the seating plan
     * @param numCols Number of columns in the seating plan
     * @return true if successful
     */
    bool initializeVenueSeatingPlan(int venueId, int numRows, int numCols) {
        auto venue = getById(venueId);
        if (!venue) {
            return false;
        }
        
        venue->initializeSeatingPlan(numRows, numCols);
        return saveEntities();
    }

    /**
     * @brief Add a seat to a venue with 2D mapping
     * 
     * @param venueId Venue ID
     * @param seatType Seat type (e.g., "VIP", "Regular")
     * @param row Row identifier
     * @param column Column identifier
     * @return std::shared_ptr<Model::Seat> Pointer to created seat or nullptr on failure
     */
    std::shared_ptr<Model::Seat> addSeat(
        int venueId,
        const std::string& seatType,
        const std::string& row,
        const std::string& column
    ) {
        auto venue = getById(venueId);
        if (!venue) {
            return nullptr;
        }
        
        // Generate seat ID
        int seatId = 1;
        if (!venue->seats.empty()) {
            seatId = venue->seats.back()->seat_id + 1;
        }
        
        // Create seat
        auto seat = std::make_shared<Model::Seat>();
        seat->seat_id = seatId;
        seat->seat_type = seatType;
        seat->row_number = row;
        seat->col_number = column;
        seat->status = Model::TicketStatus::AVAILABLE;
        
        // Use venue's addSeat method which handles both linear and 2D mapping
        venue->addSeat(seat);
        
        // Save changes
        saveEntities();
        
        return seat;
    }

    /**
     * @brief Enhanced addSeat method with explicit 2D mapping
     * @param venueId Venue ID
     * @param seatType Seat type (e.g., "VIP", "Regular")
     * @param row Row identifier
     * @param column Column identifier
     * @return std::shared_ptr<Model::Seat> Pointer to created seat or nullptr on failure
     */
    std::shared_ptr<Model::Seat> addSeatWithMapping(
        int venueId,
        const std::string& seatType,
        const std::string& row,
        const std::string& column
    ) {
        return addSeat(venueId, seatType, row, column);
    }

    /**
     * @brief Remove a seat from a venue
     * @param venueId Venue ID
     * @param seatId Seat ID
     * @return true if successful
     * @return false if venue or seat not found
     */
    bool removeSeat(int venueId, int seatId) {
        auto venue = getById(venueId);
        if (!venue) {
            return false;
        }
        
        auto it = std::find_if(venue->seats.begin(), venue->seats.end(),
            [seatId](const std::shared_ptr<Model::Seat>& seat) {
                return seat->seat_id == seatId;
            });
        
        if (it == venue->seats.end()) {
            return false; // Seat not found
        }
        
        venue->seats.erase(it);
        return saveEntities();
    }

    /**
     * @brief Update a seat's status
     * @param venueId Venue ID
     * @param seatId Seat ID
     * @param status New status
     * @return true if successful
     * @return false if venue or seat not found
     */
    bool updateSeatStatus(int venueId, int seatId, Model::TicketStatus status) {
        auto venue = getById(venueId);
        if (!venue) {
            return false;
        }
        
        for (auto& seat : venue->seats) {
            if (seat->seat_id == seatId) {
                seat->status = status;
                return saveEntities();
            }
        }
        
        return false; // Seat not found
    }

    /**
     * @brief Get all seats for a venue
     * @param venueId Venue ID
     * @return std::vector<std::shared_ptr<Model::Seat>> Vector of seats or empty vector
     */
    std::vector<std::shared_ptr<Model::Seat>> getSeatsForVenue(int venueId) {
        auto venue = getById(venueId);
        if (!venue) {
            return {};
        }
        
        return venue->seats;
    }

    /**
     * @brief Get available seats for a venue
     * @param venueId Venue ID
     * @return std::vector<std::shared_ptr<Model::Seat>> Vector of available seats
     */
    std::vector<std::shared_ptr<Model::Seat>> getAvailableSeats(int venueId) {
        auto venue = getById(venueId);
        if (!venue) {
            return {};
        }
        
        std::vector<std::shared_ptr<Model::Seat>> availableSeats;
        for (auto& seat : venue->seats) {
            if (seat->status == Model::TicketStatus::AVAILABLE) {
                availableSeats.push_back(seat);
            }
        }
        
        return availableSeats;
    }

    /**
     * @brief Get seats in a specific row using 2D array access
     * @param venueId Venue ID
     * @param rowIndex Zero-based row index
     * @return std::vector<std::shared_ptr<Model::Seat>> Seats in the row
     */
    std::vector<std::shared_ptr<Model::Seat>> getSeatsInRow(int venueId, int rowIndex) {
        auto venue = getById(venueId);
        if (!venue || rowIndex < 0 || rowIndex >= venue->rows) {
            return {};
        }
        
        std::vector<std::shared_ptr<Model::Seat>> rowSeats;
        for (int col = 0; col < venue->columns; col++) {
            auto seat = venue->getSeatAt(rowIndex, col);
            if (seat) {
                rowSeats.push_back(seat);
            }
        }
        
        return rowSeats;
    }

    /**
     * @brief Get a specific seat using 2D coordinates
     * @param venueId Venue ID
     * @param row Zero-based row index
     * @param col Zero-based column index
     * @return std::shared_ptr<Model::Seat> Seat at position or nullptr
     */
    std::shared_ptr<Model::Seat> getSeatAt(int venueId, int row, int col) {
        auto venue = getById(venueId);
        if (!venue) {
            return nullptr;
        }
        
        return venue->getSeatAt(row, col);
    }

    /**
     * @brief Find adjacent available seats for group booking
     * @param venueId Venue ID
     * @param numSeats Number of adjacent seats needed
     * @return std::vector<std::vector<std::shared_ptr<Model::Seat>>> Vector of possible seat groups
     */
    std::vector<std::vector<std::shared_ptr<Model::Seat>>> findAdjacentSeats(
        int venueId, 
        int numSeats
    ) {
        auto venue = getById(venueId);
        if (!venue || numSeats <= 0) {
            return {};
        }
        
        std::vector<std::vector<std::shared_ptr<Model::Seat>>> adjacentGroups;
        
        // Check each row for consecutive available seats
        for (int row = 0; row < venue->rows; row++) {
            for (int col = 0; col <= venue->columns - numSeats; col++) {
                std::vector<std::shared_ptr<Model::Seat>> group;
                bool validGroup = true;
                
                // Check if we can get numSeats consecutive seats
                for (int i = 0; i < numSeats; i++) {
                    auto seat = venue->getSeatAt(row, col + i);
                    if (!seat || seat->status != Model::TicketStatus::AVAILABLE) {
                        validGroup = false;
                        break;
                    }
                    group.push_back(seat);
                }
                
                if (validGroup && group.size() == static_cast<size_t>(numSeats)) {
                    adjacentGroups.push_back(group);
                }
            }
        }
        
        return adjacentGroups;
    }

    /**
     * @brief Get seating plan visualization as a 2D string representation
     * @param venueId Venue ID
     * @return std::string ASCII representation of the seating plan
     */
    std::string getSeatingPlanVisualization(int venueId) {
        auto venue = getById(venueId);
        if (!venue) {
            return "Venue not found";
        }
        
        if (venue->seatingPlan.empty()) {
            return "Seating plan not initialized. Use initializeVenueSeatingPlan() first.";
        }
        
        std::ostringstream visualization;
        visualization << "Seating Plan for " << venue->name << ":\n";
        visualization << "Legend: [A]=Available, [S]=Sold, [C]=Checked In, [X]=Unavailable\n\n";
        
        // Column headers
        visualization << "   ";
        for (int col = 0; col < venue->columns; col++) {
            visualization << std::setw(3) << (col + 1);
        }
        visualization << "\n";
        
        // Rows with seats
        for (int row = 0; row < venue->rows; row++) {
            char rowLabel = (row < 26) ? ('A' + row) : ('A' + (row / 26 - 1)) + ('A' + (row % 26));
            visualization << std::setw(2) << rowLabel << " ";
            
            for (int col = 0; col < venue->columns; col++) {
                auto seat = venue->getSeatAt(row, col);
                if (!seat) {
                    visualization << " --";
                } else {
                    switch (seat->status) {
                        case Model::TicketStatus::AVAILABLE:
                            visualization << " [A]";
                            break;
                        case Model::TicketStatus::SOLD:
                            visualization << " [S]";
                            break;
                        case Model::TicketStatus::CHECKED_IN:
                            visualization << " [C]";
                            break;
                        default:
                            visualization << " [X]";
                            break;
                    }
                }
            }
            visualization << "\n";
        }
        
        return visualization.str();
    }

    /**
     * @brief Bulk seat creation for standard rectangular venues
     * @param venueId Venue ID
     * @param numRows Number of rows
     * @param seatsPerRow Number of seats per row
     * @param defaultSeatType Default seat type (e.g., "Regular")
     * @return true if successful
     */
    bool createStandardSeatingPlan(
        int venueId,
        int numRows,
        int seatsPerRow,
        const std::string& defaultSeatType = "Regular"
    ) {
        auto venue = getById(venueId);
        if (!venue) {
            return false;
        }
        
        // Initialize the 2D seating plan
        venue->initializeSeatingPlan(numRows, seatsPerRow);
        
        // Create seats for each position
        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < seatsPerRow; col++) {
                char rowChar = (row < 26) ? ('A' + row) : ('A' + (row / 26 - 1)) + ('A' + (row % 26));
                std::string rowName(1, rowChar);
                std::string colName = std::to_string(col + 1);
                
                addSeat(venueId, defaultSeatType, rowName, colName);
            }
        }
        
        return saveEntities();
    }

    /**
     * @brief Reserve a block of seats for group booking
     * @param venueId Venue ID
     * @param seats Vector of seats to reserve
     * @return true if all seats were successfully reserved
     */
    bool reserveSeatBlock(int venueId, const std::vector<std::shared_ptr<Model::Seat>>& seats) {
        auto venue = getById(venueId);
        if (!venue) {
            return false;
        }
        
        // Check if seats vector is empty
        if (seats.empty()) {
            return false; // Cannot reserve an empty block of seats
        }
        
        // First check if all seats are available
        for (const auto& seat : seats) {
            if (!seat || seat->status != Model::TicketStatus::AVAILABLE) {
                return false; // Cannot reserve block if any seat is unavailable
            }
        }
        
        // Reserve all seats
        for (const auto& seat : seats) {
            seat->status = Model::TicketStatus::SOLD;
        }
        
        return saveEntities();
    }

    /**
     * @brief Get venue seating statistics
     * @param venueId Venue ID
     * @return std::string Statistics summary
     */
    std::string getVenueSeatingStats(int venueId) {
        auto venue = getById(venueId);
        if (!venue) {
            return "Venue not found";
        }
        
        int totalSeats = static_cast<int>(venue->seats.size());
        int availableSeats = 0;
        int soldSeats = 0;
        int checkedInSeats = 0;
        
        for (const auto& seat : venue->seats) {
            switch (seat->status) {
                case Model::TicketStatus::AVAILABLE:
                    availableSeats++;
                    break;
                case Model::TicketStatus::SOLD:
                    soldSeats++;
                    break;
                case Model::TicketStatus::CHECKED_IN:
                    checkedInSeats++;
                    break;
                default:
                    break;
            }
        }
        
        std::ostringstream stats;
        stats << "Venue: " << venue->name << "\n";
        stats << "Total Seats: " << totalSeats << "\n";
        stats << "Available: " << availableSeats << " (" << 
                 (totalSeats > 0 ? (availableSeats * 100 / totalSeats) : 0) << "%)\n";
        stats << "Sold: " << soldSeats << " (" << 
                 (totalSeats > 0 ? (soldSeats * 100 / totalSeats) : 0) << "%)\n";
        stats << "Checked In: " << checkedInSeats << " (" << 
                 (totalSeats > 0 ? (checkedInSeats * 100 / totalSeats) : 0) << "%)\n";
        
        if (venue->rows > 0 && venue->columns > 0) {
            stats << "Layout: " << venue->rows << " rows × " << venue->columns << " columns\n";
        }
        
        return stats.str();
    }

    /**
     * @brief Delete a venue
     * @param id Venue ID to delete
     * @return true if deletion successful
     * @return false if venue not found or save failed
     */
    bool deleteVenue(int id) {
        return deleteEntity(id);
    }

protected:
    /**
     * @brief Get the ID of a venue
     * Required by BaseModule
     * 
     * @param venue Pointer to venue
     * @return int Venue ID
     */
    int getEntityId(const std::shared_ptr<Model::Venue>& venue) const override {
        return venue->id;
    }

    /**
     * @brief Load venues from binary file
     * Required by BaseModule
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int venueCount = 0;
        readBinary(file, venueCount);
        
        for (int i = 0; i < venueCount; i++) {
            auto venue = std::make_shared<Model::Venue>();
            
            // Read venue info
            readBinary(file, venue->id);
            venue->name = readString(file);
            venue->address = readString(file);
            venue->city = readString(file);
            venue->state = readString(file);
            venue->zip_code = readString(file);
            venue->country = readString(file);
            readBinary(file, venue->capacity);
            venue->description = readString(file);
            venue->contact_info = readString(file);
            venue->seatmap = readString(file);
            
            // Read 2D seating plan dimensions (new format)
            readBinary(file, venue->rows);
            readBinary(file, venue->columns);
            
            // Read seats
            int seatCount = 0;
            readBinary(file, seatCount);
            
            // Initialize 2D seating plan if dimensions are set
            if (venue->rows > 0 && venue->columns > 0) {
                venue->initializeSeatingPlan(venue->rows, venue->columns);
            }
            
            for (int j = 0; j < seatCount; j++) {
                auto seat = std::make_shared<Model::Seat>();
                readBinary(file, seat->seat_id);
                seat->seat_type = readString(file);
                seat->row_number = readString(file);
                seat->col_number = readString(file);
                
                int statusInt;
                readBinary(file, statusInt);
                seat->status = static_cast<Model::TicketStatus>(statusInt);
                
                // Use venue's addSeat method to handle both linear and 2D mapping
                venue->addSeat(seat);
            }
            
            entities.push_back(venue);
        }
        
        file.close();
    }

    /**
     * @brief Save venues to binary file
     * Required by BaseModule
     * 
     * @return true if save successful
     * @return false if file could not be opened
     */
    bool saveEntities() override {
        std::ofstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << dataFilePath << std::endl;
            return false;
        }
        
        int venueCount = static_cast<int>(entities.size());
        writeBinary(file, venueCount);
        
        for (const auto& venue : entities) {
            // Write venue info
            writeBinary(file, venue->id);
            writeString(file, venue->name);
            writeString(file, venue->address);
            writeString(file, venue->city);
            writeString(file, venue->state);
            writeString(file, venue->zip_code);
            writeString(file, venue->country);
            writeBinary(file, venue->capacity);
            writeString(file, venue->description);
            writeString(file, venue->contact_info);
            writeString(file, venue->seatmap);
            
            // Write 2D seating plan dimensions (new format)
            writeBinary(file, venue->rows);
            writeBinary(file, venue->columns);
            
            // Write seats
            int seatCount = static_cast<int>(venue->seats.size());
            writeBinary(file, seatCount);
            
            for (const auto& seat : venue->seats) {
                writeBinary(file, seat->seat_id);
                writeString(file, seat->seat_type);
                writeString(file, seat->row_number);
                writeString(file, seat->col_number);
                
                int statusInt = static_cast<int>(seat->status);
                writeBinary(file, statusInt);
            }
        }
        
        file.close();
        return true;
    }
};

// Namespace for simplified access
namespace VenueManager {
    // Static instance of the module for function implementations
    static VenueModule& getModule() {
        static VenueModule module;
        return module;
    }

    // Function to create a venue
    std::shared_ptr<Model::Venue> createVenue(
        const std::string& name,
        const std::string& address,
        const std::string& city,
        const std::string& state,
        const std::string& zipCode,
        const std::string& country,
        int capacity,
        const std::string& description = "",
        const std::string& contactInfo = "",
        const std::string& seatmap = ""
    ) {
        return getModule().createVenue(name, address, city, state, zipCode, country, 
                                       capacity, description, contactInfo, seatmap);
    }

    // Function to get a venue by ID
    std::shared_ptr<Model::Venue> getVenueById(int id) {
        return getModule().getVenueById(id);
    }

    // Function to find venues by name
    std::vector<std::shared_ptr<Model::Venue>> findVenuesByName(const std::string& nameQuery) {
        return getModule().findVenuesByName(nameQuery);
    }

    // Function to find venues by city
    std::vector<std::shared_ptr<Model::Venue>> findVenuesByCity(const std::string& city) {
        return getModule().findVenuesByCity(city);
    }

    // Function to find venues by minimum capacity
    std::vector<std::shared_ptr<Model::Venue>> findVenuesByCapacity(int minCapacity) {
        return getModule().findVenuesByCapacity(minCapacity);
    }

    // Function to get all venues
    const std::vector<std::shared_ptr<Model::Venue>>& getAllVenues() {
        return getModule().getAllVenues();
    }

    // Function to update venue information
    bool updateVenue(int id, const std::string& name = "", const std::string& address = "",
                     const std::string& city = "", const std::string& state = "",
                     const std::string& zipCode = "", const std::string& country = "",
                     int capacity = 0, const std::string& description = "",
                     const std::string& contactInfo = "", const std::string& seatmap = "") {
        return getModule().updateVenue(id, name, address, city, state, zipCode, country,
                                      capacity, description, contactInfo, seatmap);
    }

    // Function to delete a venue
    bool deleteVenue(int id) {
        return getModule().deleteVenue(id);
    }

    // Function to add a seat to a venue
    std::shared_ptr<Model::Seat> addSeat(int venueId, const std::string& seatType,
                                         const std::string& row, const std::string& column) {
        return getModule().addSeat(venueId, seatType, row, column);
    }

    // Function to get available seats
    std::vector<std::shared_ptr<Model::Seat>> getAvailableSeats(int venueId) {
        return getModule().getAvailableSeats(venueId);
    }

    // NEW 2D Array Functions

    // Initialize venue seating plan
    bool initializeVenueSeatingPlan(int venueId, int numRows, int numCols) {
        return getModule().initializeVenueSeatingPlan(venueId, numRows, numCols);
    }

    // Get seats in a specific row
    std::vector<std::shared_ptr<Model::Seat>> getSeatsInRow(int venueId, int rowIndex) {
        return getModule().getSeatsInRow(venueId, rowIndex);
    }

    // Get seat at specific coordinates
    std::shared_ptr<Model::Seat> getSeatAt(int venueId, int row, int col) {
        return getModule().getSeatAt(venueId, row, col);
    }

    // Find adjacent seats for group booking
    std::vector<std::vector<std::shared_ptr<Model::Seat>>> findAdjacentSeats(int venueId, int numSeats) {
        return getModule().findAdjacentSeats(venueId, numSeats);
    }

    // Get visual representation of seating plan
    std::string getSeatingPlanVisualization(int venueId) {
        return getModule().getSeatingPlanVisualization(venueId);
    }

    // Create standard rectangular seating plan
    bool createStandardSeatingPlan(int venueId, int numRows, int seatsPerRow, 
                                   const std::string& defaultSeatType = "Regular") {
        return getModule().createStandardSeatingPlan(venueId, numRows, seatsPerRow, defaultSeatType);
    }

    // Reserve a block of seats
    bool reserveSeatBlock(int venueId, const std::vector<std::shared_ptr<Model::Seat>>& seats) {
        return getModule().reserveSeatBlock(venueId, seats);
    }

    // Get venue seating statistics
    std::string getVenueSeatingStats(int venueId) {
        return getModule().getVenueSeatingStats(venueId);
    }

    // Remove a seat
    bool removeSeat(int venueId, int seatId) {
        return getModule().removeSeat(venueId, seatId);
    }

    // Update seat status
    bool updateSeatStatus(int venueId, int seatId, Model::TicketStatus status) {
        return getModule().updateSeatStatus(venueId, seatId, status);
    }

    // Get all seats for a venue
    std::vector<std::shared_ptr<Model::Seat>> getSeatsForVenue(int venueId) {
        return getModule().getSeatsForVenue(venueId);
    }
}
