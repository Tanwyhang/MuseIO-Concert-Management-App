#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <map>
#include "models.hpp"
#include "baseModule.hpp"

namespace TicketManager {

    /**
     * @brief Ticket Module for handling all ticket-related operations
     * 
     * This module manages ticket creation, sales, validation, QR code generation,
     * and ticket status management for the MuseIO Concert Management System.
     */
    class TicketModule : public BaseModule<Model::Ticket, int> {
    public:
        /**
         * @brief Constructor
         * @param filePath Path to the ticket data file
         */
        TicketModule(const std::string& filePath = "data/tickets.dat");

        /**
         * @brief Destructor
         */
        ~TicketModule() = default;

        // Core Ticket Operations

        /**
         * @brief Create a new ticket for an attendee
         * @param attendee_id ID of the attendee purchasing the ticket
         * @param concert_id ID of the concert
         * @param ticket_type Type of ticket (VIP, Regular, etc.)
         * @return Ticket ID if successful, -1 if failed
         */
        int createTicket(int attendee_id, int concert_id, const std::string& ticket_type);

        /**
         * @brief Create multiple tickets for bulk purchase
         * @param attendee_id ID of the attendee purchasing tickets
         * @param concert_id ID of the concert
         * @param quantity Number of tickets to create
         * @param ticket_type Type of tickets
         * @return Vector of ticket IDs created
         */
        std::vector<int> createMultipleTickets(int attendee_id, int concert_id, 
                                             int quantity, const std::string& ticket_type);

        /**
         * @brief Update ticket status
         * @param ticket_id Ticket ID to update
         * @param status New ticket status
         * @return true if successful, false otherwise
         */
        bool updateTicketStatus(int ticket_id, Model::TicketStatus status);

        /**
         * @brief Cancel a ticket
         * @param ticket_id Ticket ID to cancel
         * @param reason Reason for cancellation
         * @return true if successful, false otherwise
         */
        bool cancelTicket(int ticket_id, const std::string& reason = "");

        // QR Code Management

        /**
         * @brief Generate QR code for a ticket
         * @param ticket_id Ticket ID
         * @return QR code string if successful, empty string if failed
         */
        std::string generateQRCode(int ticket_id);

        /**
         * @brief Validate a ticket using QR code
         * @param qr_code QR code to validate
         * @return Ticket ID if valid, -1 if invalid
         */
        int validateQRCode(const std::string& qr_code);

        /**
         * @brief Check-in attendee using ticket QR code
         * @param qr_code QR code from ticket
         * @return true if successful check-in, false otherwise
         */
        bool checkInWithQRCode(const std::string& qr_code);

        // Query Operations

        /**
         * @brief Get ticket by ID
         * @param ticket_id Ticket ID to retrieve
         * @return Shared pointer to ticket, nullptr if not found
         */
        std::shared_ptr<Model::Ticket> getTicketById(int ticket_id);

        /**
         * @brief Get all tickets for a specific attendee
         * @param attendee_id Attendee ID
         * @return Vector of tickets for the attendee
         */
        std::vector<std::shared_ptr<Model::Ticket>> getTicketsByAttendee(int attendee_id);

        /**
         * @brief Get active tickets for an attendee (not expired or cancelled)
         * @param attendee_id Attendee ID
         * @return Vector of active tickets
         */
        std::vector<std::shared_ptr<Model::Ticket>> getActiveTicketsByAttendee(int attendee_id);

        /**
         * @brief Get expired tickets for an attendee
         * @param attendee_id Attendee ID
         * @return Vector of expired tickets
         */
        std::vector<std::shared_ptr<Model::Ticket>> getExpiredTicketsByAttendee(int attendee_id);

        /**
         * @brief Get tickets by status
         * @param status Ticket status to filter by
         * @return Vector of tickets with the specified status
         */
        std::vector<std::shared_ptr<Model::Ticket>> getTicketsByStatus(Model::TicketStatus status);

        /**
         * @brief Get all tickets for a specific concert
         * @param concert_id Concert ID
         * @return Vector of tickets for the concert
         */
        std::vector<std::shared_ptr<Model::Ticket>> getTicketsByConcert(int concert_id);

        // Ticket Sales and Availability

        /**
         * @brief Check ticket availability for a concert
         * @param concert_id Concert ID
         * @return Number of available tickets
         */
        int getAvailableTicketCount(int concert_id);

        /**
         * @brief Check if tickets are still on sale for a concert
         * @param concert_id Concert ID
         * @return true if tickets are on sale, false otherwise
         */
        bool areTicketsOnSale(int concert_id);

        /**
         * @brief Reserve tickets for purchase (temporary hold)
         * @param concert_id Concert ID
         * @param quantity Number of tickets to reserve
         * @param reservation_minutes Minutes to hold reservation (default: 15)
         * @return Reservation ID if successful, empty string if failed
         */
        std::string reserveTickets(int concert_id, int quantity, int reservation_minutes = 15);

        /**
         * @brief Confirm ticket reservation (convert to sold)
         * @param reservation_id Reservation ID
         * @param attendee_id Attendee ID who will own the tickets
         * @return Vector of confirmed ticket IDs
         */
        std::vector<int> confirmReservation(const std::string& reservation_id, int attendee_id);

        /**
         * @brief Release ticket reservation
         * @param reservation_id Reservation ID to release
         * @return true if successful, false otherwise
         */
        bool releaseReservation(const std::string& reservation_id);

        // Analytics and Statistics

        /**
         * @brief Get ticket sales statistics for a concert
         * @param concert_id Concert ID
         * @return Ticket statistics struct
         */
        struct TicketStats {
            int total_tickets;
            int sold_tickets;
            int available_tickets;
            int checked_in_tickets;
            int cancelled_tickets;
            double sales_percentage;
            double check_in_percentage;
        };
        TicketStats getTicketStatistics(int concert_id);

        /**
         * @brief Get ticket sales report for a date range
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Ticket sales report string
         */
        std::string generateTicketSalesReport(const std::string& start_date, 
                                            const std::string& end_date);

        // Ticket Transfer and Management

        /**
         * @brief Transfer ticket ownership
         * @param ticket_id Ticket ID to transfer
         * @param new_attendee_id New owner's attendee ID
         * @param transfer_reason Reason for transfer
         * @return true if successful, false otherwise
         */
        bool transferTicket(int ticket_id, int new_attendee_id, 
                          const std::string& transfer_reason = "");

        /**
         * @brief Upgrade ticket to higher tier
         * @param ticket_id Ticket ID to upgrade
         * @param new_ticket_type New ticket type
         * @param additional_payment Additional payment required
         * @return true if successful, false otherwise
         */
        bool upgradeTicket(int ticket_id, const std::string& new_ticket_type, 
                         double additional_payment = 0.0);

    protected:
        // BaseModule implementation
        int getEntityId(const std::shared_ptr<Model::Ticket>& entity) const override;
        void loadEntities() override;
        bool saveEntities() override;

    private:
        // Ticket reservations (temporary holds)
        struct TicketReservation {
            std::string reservation_id;
            int concert_id;
            int quantity;
            Model::DateTime created_at;
            Model::DateTime expires_at;
            bool is_active;
        };
        std::vector<TicketReservation> reservations;

        /**
         * @brief Generate a unique QR code for a ticket
         * @param ticket_id Ticket ID
         * @param concert_id Concert ID
         * @param attendee_id Attendee ID
         * @return Generated QR code string
         */
        std::string generateUniqueQRCode(int ticket_id, int concert_id, int attendee_id);

        /**
         * @brief Generate a unique reservation ID
         * @return Unique reservation ID
         */
        std::string generateReservationId();

        /**
         * @brief Clean up expired reservations
         */
        void cleanupExpiredReservations();

        /**
         * @brief Validate ticket creation parameters
         * @param attendee_id Attendee ID
         * @param concert_id Concert ID
         * @param ticket_type Ticket type
         * @return true if valid, false otherwise
         */
        bool validateTicketCreation(int attendee_id, int concert_id, 
                                  const std::string& ticket_type);

        /**
         * @brief Log ticket transaction for audit trail
         * @param ticket Ticket to log
         * @param action Action performed (e.g., "CREATED", "SOLD", "CHECKED_IN")
         */
        void logTicketTransaction(const Model::Ticket& ticket, const std::string& action);
    };

}
