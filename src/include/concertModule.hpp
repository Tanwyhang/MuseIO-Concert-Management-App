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

/**
 * @brief Module for managing Concert entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for concert management including CRUD operations
 * and concert-specific search capabilities.
 */
class ConcertModule : public BaseModule<Model::Concert> {
public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing concerts
     * @param filePath Path to the concerts data file
     */
    ConcertModule(const std::string& filePath = "data/concerts.dat") : BaseModule<Model::Concert>(filePath) {
        loadEntities();
    }

    /**
     * @brief Destructor
     * Ensures concerts are saved before destruction
     */
    ~ConcertModule() override {
        saveEntities();
    }

    /**
     * @brief Create a new concert
     * 
     * @param name Concert name
     * @param description Concert description
     * @param startDateTime Start date and time (ISO 8601 format)
     * @param endDateTime End date and time (ISO 8601 format)
     * @return std::shared_ptr<Model::Concert> Pointer to created concert
     */
    std::shared_ptr<Model::Concert> createConcert(
        const std::string& name,
        const std::string& description,
        const std::string& startDateTime,
        const std::string& endDateTime
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the concert object
        auto now = Model::DateTime::now();
        
        Model::DateTime start;
        start.iso8601String = startDateTime;
        
        Model::DateTime end;
        end.iso8601String = endDateTime;
        
        auto concert = std::make_shared<Model::Concert>();
        concert->id = newId;
        concert->name = name;
        concert->description = description;
        concert->start_date_time = start;
        concert->end_date_time = end;
        concert->event_status = Model::EventStatus::SCHEDULED; // Default status
        concert->created_at = now;
        concert->updated_at = now;
        
        // Initialize vectors
        concert->ticketInfo = nullptr; // Will be set when ticket info is added
        concert->venue = nullptr;      // Will be set when venue is assigned
        concert->feedbacks = {};
        concert->promotions = {};
        concert->crews = {};
        concert->attendees = {};
        concert->performers = {};
        concert->payments = {};
        concert->comm_logs = {};
        concert->reports = {};
        
        // Add to collection
        entities.push_back(concert);
        
        // Save to file
        saveEntities();
        
        return concert;
    }

    /**
     * @brief Edit an existing concert
     * 
     * @param concertId Concert ID to edit
     * @param name New name (if empty, no change)
     * @param description New description (if empty, no change)
     * @param startDateTime New start date/time (if empty, no change)
     * @param endDateTime New end date/time (if empty, no change)
     * @return true if update successful
     * @return false if concert not found or save failed
     */
    bool editConcert(
        int concertId,
        const std::string& name = "",
        const std::string& description = "",
        const std::string& startDateTime = "",
        const std::string& endDateTime = ""
    ) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        // Update fields if provided (non-empty)
        if (!name.empty()) concert->name = name;
        if (!description.empty()) concert->description = description;
        
        if (!startDateTime.empty()) {
            Model::DateTime start;
            start.iso8601String = startDateTime;
            concert->start_date_time = start;
        }
        
        if (!endDateTime.empty()) {
            Model::DateTime end;
            end.iso8601String = endDateTime;
            concert->end_date_time = end;
        }
        
        // Update the modified timestamp
        concert->updated_at = Model::DateTime::now();
        
        // Save changes
        return saveEntities();
    }

    /**
     * @brief Get a concert by ID
     * @param id Concert ID to find
     * @return std::shared_ptr<Model::Concert> Pointer to concert or nullptr
     */
    std::shared_ptr<Model::Concert> getConcertById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all concerts
     * @return const std::vector<std::shared_ptr<Model::Concert>>& Reference to concert vector
     */
    const std::vector<std::shared_ptr<Model::Concert>>& getAllConcerts() const {
        return getAll();
    }

    /**
     * @brief Find concerts by partial name match (case insensitive)
     * @param nameQuery Partial name to search for
     * @return std::vector<std::shared_ptr<Model::Concert>> Vector of matching concerts
     */
    std::vector<std::shared_ptr<Model::Concert>> findConcertsByName(const std::string& nameQuery) {
        return findByPredicate([&nameQuery](const std::shared_ptr<Model::Concert>& concert) {
            // Case insensitive search
            std::string concertName = concert->name;
            std::string query = nameQuery;
            
            // Convert both to lowercase for comparison
            std::transform(concertName.begin(), concertName.end(), concertName.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            std::transform(query.begin(), query.end(), query.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            return concertName.find(query) != std::string::npos;
        });
    }

    /**
     * @brief Find concerts by date range
     * @param startDate Start date in ISO 8601 format (e.g., "2025-08-01T00:00:00Z")
     * @param endDate End date in ISO 8601 format (e.g., "2025-08-31T23:59:59Z")
     * @return std::vector<std::shared_ptr<Model::Concert>> Concerts within date range
     */
    std::vector<std::shared_ptr<Model::Concert>> findConcertsByDateRange(const std::string& startDate, const std::string& endDate) {
        return findByPredicate([&startDate, &endDate](const std::shared_ptr<Model::Concert>& concert) {
            // Simple string comparison of ISO dates
            // Note: This assumes ISO 8601 format which sorts lexicographically
            return concert->start_date_time.iso8601String >= startDate &&
                   concert->start_date_time.iso8601String <= endDate;
        });
    }

    /**
     * @brief Find concerts by status
     * @param status Event status to filter by
     * @return std::vector<std::shared_ptr<Model::Concert>> Concerts with matching status
     */
    std::vector<std::shared_ptr<Model::Concert>> findConcertsByStatus(Model::EventStatus status) {
        return findByPredicate([status](const std::shared_ptr<Model::Concert>& concert) {
            return concert->event_status == status;
        });
    }

    /**
     * @brief Find concerts by venue ID
     * @param venueId Venue ID to search for
     * @return std::vector<std::shared_ptr<Model::Concert>> Concerts at the specified venue
     */
    std::vector<std::shared_ptr<Model::Concert>> findConcertsByVenue(int venueId) {
        return findByPredicate([venueId](const std::shared_ptr<Model::Concert>& concert) {
            return concert->venue && concert->venue->id == venueId;
        });
    }

    /**
     * @brief Find concerts by performer type (genre-like search)
     * @param performerType Performer type to search for (case insensitive)
     * @return std::vector<std::shared_ptr<Model::Concert>> Concerts with performers of the specified type
     */
    std::vector<std::shared_ptr<Model::Concert>> findConcertsByPerformerType(const std::string& performerType) {
        return findByPredicate([&performerType](const std::shared_ptr<Model::Concert>& concert) {
            // Case insensitive search through all performers
            std::string searchType = performerType;
            std::transform(searchType.begin(), searchType.end(), searchType.begin(), 
                        [](unsigned char c){ return std::tolower(c); });
            
            for (const auto& performer : concert->performers) {
                if (performer) {
                    std::string perfType = performer->type;
                    std::transform(perfType.begin(), perfType.end(), perfType.begin(), 
                                [](unsigned char c){ return std::tolower(c); });
                    
                    if (perfType.find(searchType) != std::string::npos) {
                        return true;
                    }
                }
            }
            return false;
        });
    }

    /**
     * @brief Set venue for a concert
     * @param concertId Concert ID
     * @param venue Venue object to assign
     * @return true if successful
     * @return false if concert not found
     */
    bool setVenueForConcert(int concertId, std::shared_ptr<Model::Venue> venue) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        concert->venue = venue;
        concert->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Set up ticket info for a concert
     * @param concertId Concert ID
     * @param basePrice Base ticket price
     * @param quantity Total available tickets
     * @param saleStartDateTime Sale start date/time in ISO 8601
     * @param saleEndDateTime Sale end date/time in ISO 8601
     * @return true if successful
     * @return false if concert not found
     */
    bool setupTicketInfo(int concertId, double basePrice, int quantity, 
                         const std::string& saleStartDateTime, const std::string& saleEndDateTime) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        // Create ticket info if it doesn't exist
        if (!concert->ticketInfo) {
            concert->ticketInfo = std::make_shared<Model::ConcertTicket>();
        }
        
        // Set ticket info fields
        concert->ticketInfo->concert = concert; // Weak pointer to parent concert
        concert->ticketInfo->base_price = basePrice;
        concert->ticketInfo->quantity_available = quantity;
        concert->ticketInfo->quantity_sold = 0; // Reset sales count
        
        // Set sale dates
        Model::DateTime startSale, endSale;
        startSale.iso8601String = saleStartDateTime;
        endSale.iso8601String = saleEndDateTime;
        
        concert->ticketInfo->start_sale_date_time = startSale;
        concert->ticketInfo->end_sale_date_time = endSale;
        
        // Update the concert
        concert->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Add a performer to a concert
     * @param concertId Concert ID
     * @param performer Performer to add
     * @return true if successful
     * @return false if concert not found
     */
    bool addPerformerToConcert(int concertId, std::shared_ptr<Model::Performer> performer) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        // Check if performer already exists in the concert
        auto it = std::find_if(concert->performers.begin(), concert->performers.end(),
            [&performer](const std::shared_ptr<Model::Performer>& p) {
                return p->performer_id == performer->performer_id;
            });
            
        if (it == concert->performers.end()) {
            // Add performer if not already in the list
            concert->performers.push_back(performer);
            concert->updated_at = Model::DateTime::now();
            return saveEntities();
        }
        
        return true; // Performer was already added
    }

    /**
     * @brief Add a promotion to a concert
     * @param concertId Concert ID
     * @param promotion Promotion to add
     * @return true if successful
     * @return false if concert not found
     */
    bool addPromotionToConcert(int concertId, std::shared_ptr<Model::Promotion> promotion) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        // Check if promotion code already exists
        auto it = std::find_if(concert->promotions.begin(), concert->promotions.end(),
            [&promotion](const std::shared_ptr<Model::Promotion>& p) {
                return p->code == promotion->code;
            });
            
        if (it == concert->promotions.end()) {
            // Add promotion if not already in the list
            concert->promotions.push_back(promotion);
            concert->updated_at = Model::DateTime::now();
            return saveEntities();
        }
        
        return false; // Promotion code already exists
    }

    /**
     * @brief Change the status of a concert
     * @param concertId Concert ID
     * @param newStatus New event status
     * @return true if successful
     * @return false if concert not found
     */
    bool changeConcertStatus(int concertId, Model::EventStatus newStatus) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        concert->event_status = newStatus;
        concert->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Start a concert (change status to COMPLETED)
     * @param concertId Concert ID
     * @return true if successful
     * @return false if concert not found or not in SCHEDULED status
     */
    bool startConcert(int concertId) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        if (concert->event_status != Model::EventStatus::SCHEDULED) {
            return false; // Can only start concerts that are scheduled
        }
        
        return changeConcertStatus(concertId, Model::EventStatus::COMPLETED);
    }

    /**
     * @brief End a concert (if already started)
     * @param concertId Concert ID
     * @return true if successful
     * @return false if concert not found or not in progress
     */
    bool endConcert(int concertId) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        if (concert->event_status != Model::EventStatus::COMPLETED) {
            return false; // Can only end concerts that are in progress
        }
        
        // Generate a basic report for the concert
        generateConcertReport(concertId);
        
        return true;
    }

    /**
     * @brief Cancel a concert
     * @param concertId Concert ID
     * @return true if successful
     * @return false if concert not found or already completed
     */
    bool cancelConcert(int concertId) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        if (concert->event_status == Model::EventStatus::COMPLETED) {
            return false; // Can't cancel completed concerts
        }
        
        return changeConcertStatus(concertId, Model::EventStatus::CANCELLED);
    }

    /**
     * @brief Add a show to a concert (implemented as a communication log)
     * @param concertId Concert ID
     * @param showName Show name
     * @param showTime Show time in ISO 8601 format
     * @return true if successful
     * @return false if concert not found
     */
    bool addShowToConcert(int concertId, const std::string& showName, const std::string& showTime) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        // Create a communication log for the show
        auto showLog = std::make_shared<Model::CommunicationLog>();
        
        // Generate a new ID
        int maxId = 0;
        for (const auto& log : concert->comm_logs) {
            if (log->comm_id > maxId) {
                maxId = log->comm_id;
            }
        }
        
        showLog->comm_id = maxId + 1;
        showLog->concert = concert; // Weak pointer to parent concert
        
        // Format the message to include show details
        std::ostringstream message;
        message << "Show: " << showName << "\n";
        message << "Time: " << showTime << "\n";
        showLog->message_content = message.str();
        
        // Set additional fields
        Model::DateTime now = Model::DateTime::now();
        showLog->sent_at = now;
        showLog->comm_type = "Show";
        showLog->recipient_count = 0;
        showLog->is_automated = false;
        
        // Add to concert logs
        concert->comm_logs.push_back(showLog);
        concert->updated_at = now;
        
        return saveEntities();
    }

    /**
     * @brief Remove a show from a concert
     * @param concertId Concert ID
     * @param showId Show ID (communication log ID)
     * @return true if successful
     * @return false if concert or show not found
     */
    bool removeShowFromConcert(int concertId, int showId) {
        auto concert = getConcertById(concertId);
        if (!concert) {
            return false;
        }
        
        // Find and remove the show from communication logs
        auto it = std::find_if(concert->comm_logs.begin(), concert->comm_logs.end(),
            [showId](const std::shared_ptr<Model::CommunicationLog>& log) {
                return log->comm_id == showId && log->comm_type == "Show";
            });
        
        if (it == concert->comm_logs.end()) {
            return false; // Show not found
        }
        
        concert->comm_logs.erase(it);
        concert->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Generate a basic report for a concert
     * @param concertId Concert ID
     * @return std::shared_ptr<Model::ConcertReport> Generated report or nullptr
     */
    std::shared_ptr<Model::ConcertReport> generateConcertReport(int concertId) {
        auto concert = getConcertById(concertId);
        if (!concert || !concert->ticketInfo) {
            return nullptr;
        }
        
        // Create a new report
        auto report = std::make_shared<Model::ConcertReport>();
        
        // Generate a new report ID
        int maxId = 0;
        for (const auto& r : concert->reports) {
            if (r->id > maxId) {
                maxId = r->id;
            }
        }
        
        report->id = maxId + 1;
        report->date = Model::DateTime::now();
        report->concert = concert; // Weak pointer to parent concert
        
        // Set basic metrics
        report->total_registrations = concert->attendees.size();
        report->tickets_sold = concert->ticketInfo->quantity_sold;
        
        // Calculate sales volume
        report->sales_volume = concert->ticketInfo->base_price * report->tickets_sold;
        
        // Calculate average feedback score if available
        double totalRating = 0;
        int ratingCount = 0;
        
        for (const auto& feedback : concert->feedbacks) {
            totalRating += feedback->rating;
            ratingCount++;
        }
        
        report->attendee_engagement_score = ratingCount > 0 ? totalRating / ratingCount : 0.0;
        
        // Calculate NPS (simplified)
        int promoters = 0, detractors = 0;
        
        for (const auto& feedback : concert->feedbacks) {
            if (feedback->rating >= 9) { // Promoters (9-10)
                promoters++;
            } else if (feedback->rating <= 6) { // Detractors (0-6)
                detractors++;
            }
        }
        
        report->nps_score = ratingCount > 0 ? (promoters - detractors) * 100.0 / ratingCount : 0.0;
        
        // Set timestamps
        report->created_at = Model::DateTime::now();
        report->updated_at = Model::DateTime::now();
        
        // Add to concert reports
        concert->reports.push_back(report);
        concert->updated_at = Model::DateTime::now();
        
        saveEntities();
        
        return report;
    }

    /**
     * @brief Delete a concert
     * @param id Concert ID to delete
     * @return true if deletion successful
     * @return false if concert not found or save failed
     */
    bool deleteConcert(int id) {
        return deleteEntity(id);
    }

protected:
    /**
     * @brief Get the ID of a concert
     * Required by BaseModule
     * 
     * @param concert Pointer to concert
     * @return int Concert ID
     */
    int getEntityId(const std::shared_ptr<Model::Concert>& concert) const override {
        return concert->id;
    }

    /**
     * @brief Load concerts from binary file
     * Required by BaseModule
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int concertCount = 0;
        readBinary(file, concertCount);
        
        for (int i = 0; i < concertCount; i++) {
            auto concert = std::make_shared<Model::Concert>();
            
            // Read basic concert info
            readBinary(file, concert->id);
            
            concert->name = readString(file);
            concert->description = readString(file);
            
            concert->start_date_time.iso8601String = readString(file);
            concert->end_date_time.iso8601String = readString(file);
            
            int statusInt;
            readBinary(file, statusInt);
            concert->event_status = static_cast<Model::EventStatus>(statusInt);
            
            concert->created_at.iso8601String = readString(file);
            concert->updated_at.iso8601String = readString(file);
            
            // Read ticket info
            bool hasTicketInfo;
            readBinary(file, hasTicketInfo);
            
            if (hasTicketInfo) {
                concert->ticketInfo = std::make_shared<Model::ConcertTicket>();
                concert->ticketInfo->concert = concert; // Weak pointer to parent
                readBinary(file, concert->ticketInfo->base_price);
                readBinary(file, concert->ticketInfo->quantity_available);
                readBinary(file, concert->ticketInfo->quantity_sold);
                concert->ticketInfo->start_sale_date_time.iso8601String = readString(file);
                concert->ticketInfo->end_sale_date_time.iso8601String = readString(file);
            }
            
            // Read venue
            bool hasVenue;
            readBinary(file, hasVenue);
            
            if (hasVenue) {
                concert->venue = std::make_shared<Model::Venue>();
                readBinary(file, concert->venue->id);
                concert->venue->name = readString(file);
                concert->venue->address = readString(file);
                concert->venue->city = readString(file);
                concert->venue->state = readString(file);
                concert->venue->zip_code = readString(file);
                concert->venue->country = readString(file);
                readBinary(file, concert->venue->capacity);
                concert->venue->description = readString(file);
                concert->venue->contact_info = readString(file);
                concert->venue->seatmap = readString(file);
            }
            
            // Read performers
            int performerCount;
            readBinary(file, performerCount);
            
            for (int j = 0; j < performerCount; j++) {
                auto performer = std::make_shared<Model::Performer>();
                readBinary(file, performer->performer_id);
                performer->name = readString(file);
                performer->type = readString(file);
                performer->contact_info = readString(file);
                performer->bio = readString(file);
                performer->image_url = readString(file);
                
                concert->performers.push_back(performer);
            }
            
            // Read promotions
            int promoCount;
            readBinary(file, promoCount);
            
            for (int j = 0; j < promoCount; j++) {
                auto promo = std::make_shared<Model::Promotion>();
                promo->code = readString(file);
                promo->description = readString(file);
                
                int discountTypeInt;
                readBinary(file, discountTypeInt);
                promo->discount_type = static_cast<Model::DiscountType>(discountTypeInt);
                
                readBinary(file, promo->percentage);
                promo->start_date_time.iso8601String = readString(file);
                promo->end_date_time.iso8601String = readString(file);
                readBinary(file, promo->is_active);
                readBinary(file, promo->usage_limit);
                readBinary(file, promo->used_count);
                
                concert->promotions.push_back(promo);
            }
            
            // Read communication logs (including shows)
            int logCount;
            readBinary(file, logCount);
            
            for (int j = 0; j < logCount; j++) {
                auto log = std::make_shared<Model::CommunicationLog>();
                log->concert = concert; // Weak pointer to parent
                
                readBinary(file, log->comm_id);
                log->message_content = readString(file);
                log->sent_at.iso8601String = readString(file);
                log->comm_type = readString(file);
                readBinary(file, log->recipient_count);
                readBinary(file, log->is_automated);
                
                concert->comm_logs.push_back(log);
            }
            
            // Store the concert
            entities.push_back(concert);
        }
        
        file.close();
    }
    
    /**
     * @brief Save concerts to binary file
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
        
        int concertCount = static_cast<int>(entities.size());
        writeBinary(file, concertCount);
        
        for (const auto& concert : entities) {
            // Write basic concert info
            writeBinary(file, concert->id);
            
            writeString(file, concert->name);
            writeString(file, concert->description);
            
            writeString(file, concert->start_date_time.iso8601String);
            writeString(file, concert->end_date_time.iso8601String);
            
            int statusInt = static_cast<int>(concert->event_status);
            writeBinary(file, statusInt);
            
            writeString(file, concert->created_at.iso8601String);
            writeString(file, concert->updated_at.iso8601String);
            
            // Write ticket info
            bool hasTicketInfo = (concert->ticketInfo != nullptr);
            writeBinary(file, hasTicketInfo);
            
            if (hasTicketInfo) {
                writeBinary(file, concert->ticketInfo->base_price);
                writeBinary(file, concert->ticketInfo->quantity_available);
                writeBinary(file, concert->ticketInfo->quantity_sold);
                writeString(file, concert->ticketInfo->start_sale_date_time.iso8601String);
                writeString(file, concert->ticketInfo->end_sale_date_time.iso8601String);
            }
            
            // Write venue
            bool hasVenue = (concert->venue != nullptr);
            writeBinary(file, hasVenue);
            
            if (hasVenue) {
                writeBinary(file, concert->venue->id);
                writeString(file, concert->venue->name);
                writeString(file, concert->venue->address);
                writeString(file, concert->venue->city);
                writeString(file, concert->venue->state);
                writeString(file, concert->venue->zip_code);
                writeString(file, concert->venue->country);
                writeBinary(file, concert->venue->capacity);
                writeString(file, concert->venue->description);
                writeString(file, concert->venue->contact_info);
                writeString(file, concert->venue->seatmap);
            }
            
            // Write performers
            int performerCount = static_cast<int>(concert->performers.size());
            writeBinary(file, performerCount);
            
            for (const auto& performer : concert->performers) {
                writeBinary(file, performer->performer_id);
                writeString(file, performer->name);
                writeString(file, performer->type);
                writeString(file, performer->contact_info);
                writeString(file, performer->bio);
                writeString(file, performer->image_url);
            }
            
            // Write promotions
            int promoCount = static_cast<int>(concert->promotions.size());
            writeBinary(file, promoCount);
            
            for (const auto& promo : concert->promotions) {
                writeString(file, promo->code);
                writeString(file, promo->description);
                
                int discountTypeInt = static_cast<int>(promo->discount_type);
                writeBinary(file, discountTypeInt);
                
                writeBinary(file, promo->percentage);
                writeString(file, promo->start_date_time.iso8601String);
                writeString(file, promo->end_date_time.iso8601String);
                writeBinary(file, promo->is_active);
                writeBinary(file, promo->usage_limit);
                writeBinary(file, promo->used_count);
            }
            
            // Write communication logs (including shows)
            int logCount = static_cast<int>(concert->comm_logs.size());
            writeBinary(file, logCount);
            
            for (const auto& log : concert->comm_logs) {
                writeBinary(file, log->comm_id);
                writeString(file, log->message_content);
                writeString(file, log->sent_at.iso8601String);
                writeString(file, log->comm_type);
                writeBinary(file, log->recipient_count);
                writeBinary(file, log->is_automated);
            }
        }
        
        file.close();
        return true;
    }
};

// Namespace implementation for the original function declarations
namespace ConcertManager {
    // Static instance of the module for function implementations
    static ConcertModule& getModule() {
        static ConcertModule module;
        return module;
    }

    // Functions to create and edit concerts
    void createConcert(const std::string& name, const std::string& description,
                       const std::string& startDateTime, const std::string& endDateTime) {
        getModule().createConcert(name, description, startDateTime, endDateTime);
    }

    void editConcert(int concertId, const std::string& name, const std::string& description,
                     const std::string& startDateTime, const std::string& endDateTime) {
        getModule().editConcert(concertId, name, description, startDateTime, endDateTime);
    }

    // Functions to manage shows
    void addShowToConcert(int concertId, const std::string& showName, const std::string& showTime) {
        getModule().addShowToConcert(concertId, showName, showTime);
    }
    
    void removeShowFromConcert(int concertId, int showId) {
        getModule().removeShowFromConcert(concertId, showId);
    }

    // Functions to handle concert operations
    void startConcert(int concertId) {
        getModule().startConcert(concertId);
    }
    
    void endConcert(int concertId) {
        getModule().endConcert(concertId);
    }
    
    void cancelConcert(int concertId) {
        getModule().cancelConcert(concertId);
    }
}
