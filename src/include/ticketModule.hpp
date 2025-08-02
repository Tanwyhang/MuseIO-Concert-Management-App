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
#include <random>

/**
 * @brief Module for managing Ticket entities
 * 
 * This class extends the BaseModule template to provide specific
 * functionality for ticket management including CRUD operations,
 * ticket generation, validation, and status management.
 */
class TicketModule : public BaseModule<Model::Ticket> {
public:
    /**
     * @brief Constructor
     * Initializes the module and loads existing tickets
     */
    TicketModule() : BaseModule<Model::Ticket>("../../data/tickets.dat") {
        loadEntities();
    }

    /**
     * @brief Destructor
     * Ensures tickets are saved before destruction
     */
    ~TicketModule() override {
        saveEntities();
    }

    /**
     * @brief Create a new ticket
     * 
     * @param concertTicket Pointer to associated ConcertTicket
     * @param attendee Pointer to ticket owner (can be null for unassigned tickets)
     * @param payment Pointer to payment record (can be null for unpaid tickets)
     * @param status Ticket status (default: AVAILABLE)
     * @return std::shared_ptr<Model::Ticket> Pointer to created ticket
     */
    std::shared_ptr<Model::Ticket> createTicket(
        std::weak_ptr<Model::ConcertTicket> concertTicket,
        std::weak_ptr<Model::Attendee> attendee = {},
        std::weak_ptr<Model::Payment> payment = {},
        Model::TicketStatus status = Model::TicketStatus::AVAILABLE
    ) {
        // Generate a new ID
        int newId = generateNewId();
        
        // Create the ticket object
        auto ticket = std::make_shared<Model::Ticket>();
        ticket->ticket_id = newId;
        ticket->status = status;
        ticket->qr_code = generateQRCode(newId);
        ticket->created_at = Model::DateTime::now();
        ticket->updated_at = Model::DateTime::now();
        ticket->attendee = attendee;
        ticket->payment = payment;
        ticket->concert_ticket = concertTicket;
        
        // Add to the collection
        entities.push_back(ticket);
        
        // Save to file
        saveEntities();
        
        return ticket;
    }

    /**
     * @brief Generate multiple tickets for a concert
     * 
     * @param concertTicket Pointer to the concert ticket configuration
     * @param quantity Number of tickets to generate
     * @return std::vector<std::shared_ptr<Model::Ticket>> Vector of created tickets
     */
    std::vector<std::shared_ptr<Model::Ticket>> generateTicketsForConcert(
        std::weak_ptr<Model::ConcertTicket> concertTicket,
        int quantity
    ) {
        std::vector<std::shared_ptr<Model::Ticket>> generatedTickets;
        
        for (int i = 0; i < quantity; i++) {
            auto ticket = createTicket(concertTicket);
            generatedTickets.push_back(ticket);
        }
        
        return generatedTickets;
    }

    /**
     * @brief Get a ticket by ID
     * @param id Ticket ID to find
     * @return std::shared_ptr<Model::Ticket> Pointer to ticket or nullptr
     */
    std::shared_ptr<Model::Ticket> getTicketById(int id) {
        return getById(id);
    }

    /**
     * @brief Get all tickets
     * @return const std::vector<std::shared_ptr<Model::Ticket>>& Reference to ticket vector
     */
    const std::vector<std::shared_ptr<Model::Ticket>>& getAllTickets() const {
        return getAll();
    }

    /**
     * @brief Find tickets by attendee ID
     * @param attendeeId Attendee ID to search for
     * @return std::vector<std::shared_ptr<Model::Ticket>> Vector of tickets owned by attendee
     */
    std::vector<std::shared_ptr<Model::Ticket>> findTicketsByAttendee(int attendeeId) {
        return findByPredicate([attendeeId](const std::shared_ptr<Model::Ticket>& ticket) {
            if (auto attendee = ticket->attendee.lock()) {
                return attendee->id == attendeeId;
            }
            return false;
        });
    }

    /**
     * @brief Find tickets by concert
     * @param concertId Concert ID to search for
     * @return std::vector<std::shared_ptr<Model::Ticket>> Vector of tickets for the concert
     */
    std::vector<std::shared_ptr<Model::Ticket>> findTicketsByConcert(int concertId) {
        return findByPredicate([concertId](const std::shared_ptr<Model::Ticket>& ticket) {
            if (auto concertTicket = ticket->concert_ticket.lock()) {
                if (auto concert = concertTicket->concert.lock()) {
                    return concert->id == concertId;
                }
            }
            return false;
        });
    }

    /**
     * @brief Find tickets by status
     * @param status Ticket status to filter by
     * @return std::vector<std::shared_ptr<Model::Ticket>> Vector of tickets with matching status
     */
    std::vector<std::shared_ptr<Model::Ticket>> findTicketsByStatus(Model::TicketStatus status) {
        return findByPredicate([status](const std::shared_ptr<Model::Ticket>& ticket) {
            return ticket->status == status;
        });
    }

    /**
     * @brief Assign a ticket to an attendee
     * 
     * @param ticketId Ticket ID to assign
     * @param attendee Attendee to assign ticket to
     * @param payment Optional payment record
     * @return true if successful
     * @return false if ticket not found or already assigned
     */
    bool assignTicketToAttendee(
        int ticketId,
        std::shared_ptr<Model::Attendee> attendee,
        std::weak_ptr<Model::Payment> payment = {}
    ) {
        auto ticket = getById(ticketId);
        if (!ticket) {
            return false;
        }
        
        if (ticket->status != Model::TicketStatus::AVAILABLE) {
            return false; // Ticket is not available
        }
        
        ticket->attendee = attendee;
        ticket->payment = payment;
        ticket->status = Model::TicketStatus::SOLD; // Mark as sold
        ticket->updated_at = Model::DateTime::now();
        
        // Add ticket to attendee's collection
        attendee->tickets.push_back(ticket);
        
        return saveEntities();
    }

    /**
     * @brief Change ticket status
     * @param ticketId Ticket ID
     * @param newStatus New status to set
     * @return true if successful
     * @return false if ticket not found
     */
    bool changeTicketStatus(int ticketId, Model::TicketStatus newStatus) {
        auto ticket = getById(ticketId);
        if (!ticket) {
            return false;
        }
        
        ticket->status = newStatus;
        ticket->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Check in a ticket (mark as used)
     * @param ticketId Ticket ID to check in
     * @return true if successful
     * @return false if ticket not found or not in valid state
     */
    bool checkInTicket(int ticketId) {
        auto ticket = getById(ticketId);
        if (!ticket) {
            return false;
        }
        
        if (ticket->status != Model::TicketStatus::SOLD) {
            return false; // Can only check in sold tickets
        }
        
        ticket->status = Model::TicketStatus::CHECKED_IN;
        ticket->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Validate a ticket by QR code
     * @param qrCode QR code to validate
     * @return std::shared_ptr<Model::Ticket> Valid ticket or nullptr
     */
    std::shared_ptr<Model::Ticket> validateTicketByQRCode(const std::string& qrCode) {
        for (auto& ticket : entities) {
            if (ticket->qr_code == qrCode && 
                (ticket->status == Model::TicketStatus::SOLD || 
                 ticket->status == Model::TicketStatus::RESERVED)) {
                return ticket;
            }
        }
        
        return nullptr; // No valid ticket found with this QR code
    }

    /**
     * @brief Cancel a ticket
     * @param ticketId Ticket ID to cancel
     * @return true if successful
     * @return false if ticket not found or not in valid state
     */
    bool cancelTicket(int ticketId) {
        auto ticket = getById(ticketId);
        if (!ticket) {
            return false;
        }
        
        if (ticket->status == Model::TicketStatus::CHECKED_IN) {
            return false; // Cannot cancel checked-in tickets
        }
        
        ticket->status = Model::TicketStatus::CANCELLED;
        ticket->updated_at = Model::DateTime::now();
        
        return saveEntities();
    }

    /**
     * @brief Update ticket price
     * @param ticketId Ticket ID
     * @param newPrice New ticket price
     * @return true if successful
     * @return false if ticket not found
     */
    bool updateTicketPrice(int ticketId, double newPrice) {
        auto ticket = getById(ticketId);
        if (!ticket || ticket->status != Model::TicketStatus::AVAILABLE) {
            return false;
        }
        
        // Update price in concert ticket if available
        if (auto concertTicket = ticket->concert_ticket.lock()) {
            concertTicket->base_price = newPrice;
        }
        
        ticket->updated_at = Model::DateTime::now();
        return saveEntities();
    }

    /**
     * @brief Delete a ticket
     * @param id Ticket ID to delete
     * @return true if deletion successful
     * @return false if ticket not found or save failed
     */
    bool deleteTicket(int id) {
        return deleteEntity(id);
    }

protected:
    /**
     * @brief Get the ID of a ticket
     * Required by BaseModule
     * 
     * @param ticket Pointer to ticket
     * @return int Ticket ID
     */
    int getEntityId(const std::shared_ptr<Model::Ticket>& ticket) const override {
        return ticket->ticket_id;
    }

    /**
     * @brief Load tickets from binary file
     * Required by BaseModule
     */
    void loadEntities() override {
        entities.clear();
        std::ifstream file(dataFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << dataFilePath << std::endl;
            return;
        }
        
        int ticketCount = 0;
        readBinary(file, ticketCount);
        
        for (int i = 0; i < ticketCount; i++) {
            auto ticket = std::make_shared<Model::Ticket>();
            
            // Read ticket info
            readBinary(file, ticket->ticket_id);
            
            int statusInt;
            readBinary(file, statusInt);
            ticket->status = static_cast<Model::TicketStatus>(statusInt);
            
            ticket->qr_code = readString(file);
            ticket->created_at.iso8601String = readString(file);
            ticket->updated_at.iso8601String = readString(file);
            
            // Note: weak_ptr references to attendee, payment, and concert_ticket
            // need to be resolved after all entities are loaded
            // (requires coordination with other modules)
            
            entities.push_back(ticket);
        }
        
        file.close();
    }
    
    /**
     * @brief Save tickets to binary file
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
        
        int ticketCount = static_cast<int>(entities.size());
        writeBinary(file, ticketCount);
        
        for (const auto& ticket : entities) {
            // Write ticket info
            writeBinary(file, ticket->ticket_id);
            
            int statusInt = static_cast<int>(ticket->status);
            writeBinary(file, statusInt);
            
            writeString(file, ticket->qr_code);
            writeString(file, ticket->created_at.iso8601String);
            writeString(file, ticket->updated_at.iso8601String);
            
            // Note: weak_ptr references to attendee, payment, and concert_ticket
            // are handled by the respective modules for their entities
            // Only the actual ticket data is saved here
        }
        
        file.close();
        return true;
    }
    
    /**
     * @brief Generate a QR code string for a ticket
     * @param ticketId Ticket ID
     * @return std::string Encoded QR data
     */
    std::string generateQRCode(int ticketId) {
        // In a real implementation, this would generate a proper QR code
        // For this example, we'll create a unique string that could be encoded as QR
        std::stringstream qrData;
        
        // Current timestamp for uniqueness
        auto now = std::chrono::system_clock::now();
        auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = nowMs.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
        
        // Generate a random component
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(10000, 99999);
        int randomNum = distrib(gen);
        
        // Format: TICKET-[ID]-[TIMESTAMP]-[RANDOM]
        qrData << "TICKET-" << ticketId << "-" << value << "-" << randomNum;
        
        return qrData.str();
    }
};

// Namespace for simplified access
namespace TicketManager {
    // Static instance of the module for function implementations
    static TicketModule& getModule() {
        static TicketModule module;
        return module;
    }

    // Function to create a ticket
    std::shared_ptr<Model::Ticket> createTicket(
        std::weak_ptr<Model::ConcertTicket> concertTicket,
        std::weak_ptr<Model::Attendee> attendee = {},
        std::weak_ptr<Model::Payment> payment = {},
        Model::TicketStatus status = Model::TicketStatus::AVAILABLE
    ) {
        return getModule().createTicket(concertTicket, attendee, payment, status);
    }

    // Function to generate multiple tickets
    std::vector<std::shared_ptr<Model::Ticket>> generateTicketsForConcert(
        std::weak_ptr<Model::ConcertTicket> concertTicket,
        int quantity
    ) {
        return getModule().generateTicketsForConcert(concertTicket, quantity);
    }

    // Function to find tickets by attendee
    std::vector<std::shared_ptr<Model::Ticket>> findTicketsByAttendee(int attendeeId) {
        return getModule().findTicketsByAttendee(attendeeId);
    }

    // Function to validate ticket by QR code
    std::shared_ptr<Model::Ticket> validateTicketByQRCode(const std::string& qrCode) {
        return getModule().validateTicketByQRCode(qrCode);
    }

    // Function to change ticket status
    bool changeTicketStatus(int ticketId, Model::TicketStatus newStatus) {
        return getModule().changeTicketStatus(ticketId, newStatus);
    }

    // Function to check in a ticket
    bool checkInTicket(int ticketId) {
        return getModule().checkInTicket(ticketId);
    }

    // Function to cancel a ticket
    bool cancelTicket(int ticketId) {
        return getModule().cancelTicket(ticketId);
    }
}
