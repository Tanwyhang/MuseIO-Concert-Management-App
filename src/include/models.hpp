#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>
#include <ctime>

namespace Model {
    // Forward declarations
    struct DateTime;
    struct Seat;
    struct Venue;
    struct Task;
    struct Crew;
    struct Feedback;
    struct Sponsor;
    struct Promotion;
    struct Payment;
    struct Performer;
    struct Ticket;
    struct Attendee;
    struct ConcertTicket;
    struct CommunicationLog;
    struct ConcertReport;
    struct Concert;

    // Utility struct for handling date and time
    struct DateTime {
        std::string iso8601String; // ISO 8601 format: "YYYY-MM-DDThh:mm:ssZ"
        
        // Static helper to get current date/time
        static DateTime now() {
            auto now = std::chrono::system_clock::now();
            auto nowTime = std::chrono::system_clock::to_time_t(now);
            
            std::tm localTime;
            #ifdef _WIN32
                localtime_s(&localTime, &nowTime);
            #else
                localtime_r(&nowTime, &localTime);
            #endif
            
            char buffer[30];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &localTime);
            
            DateTime dt;
            dt.iso8601String = buffer;
            return dt;
        }
    };

    // Enum for event status
    enum class EventStatus {
        SCHEDULED,
        CANCELLED,
        POSTPONED,
        COMPLETED,
        SOLDOUT
    };

    // Enum for payment status
    enum class PaymentStatus {
        PENDING,
        COMPLETED,
        FAILED,
        REFUNDED
    };

    // Enum for ticket status
    enum class TicketStatus {
        AVAILABLE,
        SOLD,
        CHECKED_IN,
        CANCELLED,
        EXPIRED
    };

    // Enum for task status
    enum class TaskStatus {
        TODO,
        IN_PROGRESS,
        COMPLETED
    };

    // Enum for task priority
    enum class TaskPriority {
        LOW,
        MEDIUM,
        HIGH,
        CRITICAL
    };

    // Enum for attendee type
    enum class AttendeeType {
        REGULAR,
        VIP,
    };

    // Enum for discount type
    enum class DiscountType {
        PERCENTAGE,
        FIXED_AMOUNT,
        BUY_X_GET_Y
    };

    // Seat struct for venue seating
    struct Seat {
        int seat_id;
        std::string seat_type;    // e.g., "VIP", "Regular", "Accessible"
        std::string row_number;
        std::string col_number;
        TicketStatus status;
    };

    // Venue struct for concert locations
    struct Venue {
        int id;
        std::string name;
        std::string address;
        std::string city;
        std::string state;
        std::string zip_code;
        std::string country;
        int capacity;
        std::string description;
        std::string contact_info;
        std::string seatmap;           // URL or reference to seatmap file
        
        // Linear collection of all seats
        std::vector<std::shared_ptr<Seat>> seats;
        
        // 2D array representation of seating plan
        std::vector<std::vector<std::shared_ptr<Seat>>> seatingPlan;
        
        // Dimensions of the seating plan
        int rows;
        int columns;
        
        // Method to initialize 2D seating plan
        void initializeSeatingPlan(int numRows, int numCols) {
            rows = numRows;
            columns = numCols;
            seatingPlan.resize(rows);
            for (int i = 0; i < rows; i++) {
                seatingPlan[i].resize(columns, nullptr);
            }
        }
        
        // Method to map a seat to the 2D seating plan
        void mapSeatToSeatingPlan(const std::shared_ptr<Seat>& seat) {
            if (!seat || seat->row_number.empty() || seat->col_number.empty()) {
                return;
            }
            
            // Convert seat row/col to array indices
            // Assuming row_number could be like "A", "B", etc. or "1", "2", etc.
            int rowIdx;
            if (seat->row_number[0] >= 'A' && seat->row_number[0] <= 'Z') {
                rowIdx = seat->row_number[0] - 'A';
            } else {
                try {
                    rowIdx = std::stoi(seat->row_number) - 1;
                } catch (...) {
                    return; // Invalid format
                }
            }
            
            int colIdx;
            try {
                colIdx = std::stoi(seat->col_number) - 1;
            } catch (...) {
                return; // Invalid format
            }
            
            // Check bounds
            if (rowIdx >= 0 && rowIdx < rows && colIdx >= 0 && colIdx < columns) {
                seatingPlan[rowIdx][colIdx] = seat;
            }
        }
        
        // Method to add a seat and update the seating plan
        void addSeat(const std::shared_ptr<Seat>& seat) {
            seats.push_back(seat);
            mapSeatToSeatingPlan(seat);
        }
        
        // Get seat at specific row/column in the seating plan
        std::shared_ptr<Seat> getSeatAt(int row, int col) const {
            if (row >= 0 && row < rows && col >= 0 && col < columns) {
                return seatingPlan[row][col];
            }
            return nullptr;
        }
    };

    // Task struct for crew assignments
    struct Task {
        int task_id;
        std::string task_name;
        std::string description;
        TaskStatus status;
        TaskPriority priority;
    };

    // Crew struct for event staff
    struct Crew {
        int id;
        std::string name;
        std::string email;
        std::string phone_number;
        std::vector<std::shared_ptr<Task>> tasks;
        std::optional<DateTime> check_in_time;
        std::optional<DateTime> check_out_time;
    };

    // Feedback struct for user comments
    struct Feedback {
        int rating;               // typically 1-5 or 1-10
        std::string comments;
        DateTime submitted_at;
        std::weak_ptr<Attendee> attendee; // To avoid circular references
    };

    // Sponsor struct for event sponsors
    struct Sponsor {
        std::string sponsor_id;
        std::string name;
        std::string contact;
        std::string email;
    };

    // Promotion struct for discounts and special offers
    struct Promotion {
        std::string code;
        std::string description;
        std::vector<std::shared_ptr<Sponsor>> sponsors;
        DiscountType discount_type;
        double percentage;           // For PERCENTAGE type
        DateTime start_date_time;
        DateTime end_date_time;
        bool is_active;
        int usage_limit;
        int used_count;
    };

    // Payment struct for transaction records
    struct Payment {
        int payment_id;
        double amount;
        std::string currency;
        std::string payment_method; // e.g., "Credit Card", "PayPal", etc.
        std::string transaction_id; // External payment processor ID
        PaymentStatus status;
        DateTime payment_date_time;
        std::weak_ptr<Attendee> attendee; // To avoid circular references
    };

    // Performer struct for artists, bands, etc.
    struct Performer {
        int performer_id;
        std::string name;
        std::string type;        // e.g., "Solo Artist", "Band", "DJ", etc.
        std::string contact_info;
        std::string bio;
        std::string image_url;   // URL to performer's image
    };

    // Ticket struct for attendee access
    struct Ticket {
        int ticket_id;
        TicketStatus status;
        std::string qr_code;    // Encoded ticket info for validation
        DateTime created_at;
        DateTime updated_at;
        std::weak_ptr<Attendee> attendee; // To avoid circular references
        std::weak_ptr<Payment> payment;   // To avoid circular references
        std::weak_ptr<ConcertTicket> concert_ticket; // To avoid circular references
    };

    // Attendee struct for event participants
    struct Attendee {
        int id;
        std::string name;
        std::string email;
        std::string phone_number;
        std::string username;     // Added for authentication
        std::string password_hash; // Added for authentication
        bool staff_privileges;    // Added for authentication
        AttendeeType attendee_type;
        DateTime registration_date;
        std::optional<DateTime> check_in_time;
        std::optional<DateTime> check_out_time;
        std::vector<std::shared_ptr<Ticket>> tickets;
        std::vector<std::shared_ptr<Feedback>> feedbacks;
        std::vector<std::shared_ptr<Payment>> payments;
        
        // Constructor needed by attendeeModule
        Attendee(int id, const std::string& name, const std::string& email, 
                const std::string& phone, AttendeeType type, 
                const DateTime& regDate, const std::string& username = "",
                const std::string& passwordHash = "", bool isStaff = false)
            : id(id), name(name), email(email), phone_number(phone), 
              username(username), password_hash(passwordHash), staff_privileges(isStaff),
              attendee_type(type), registration_date(regDate) {}
        
        // Accessor methods needed by attendeeModule
        int getId() const { return id; }
        const std::string& getEmail() const { return email; }
        const std::string& getUsername() const { return username; }
        AttendeeType getAttendeeType() const { return attendee_type; }
    };

    // ConcertTicket struct for ticket configuration
    struct ConcertTicket {
        std::weak_ptr<Concert> concert; // To avoid circular references
        double base_price;
        int quantity_available;
        int quantity_sold;
        DateTime start_sale_date_time;
        DateTime end_sale_date_time;
        std::vector<std::shared_ptr<Ticket>> tickets;
    };

    // CommunicationLog struct for tracking messages
    struct CommunicationLog {
        int comm_id;
        std::weak_ptr<Concert> concert; // To avoid circular references
        std::string message_content;
        DateTime sent_at;
        std::string comm_type; // Email, SMS, In-App, etc.
        int recipient_count;   // Number of recipients
        bool is_automated;     // Whether this was an automated notification
    };

    // ConcertReport struct for analytics
    struct ConcertReport {
        int id;
        DateTime date;
        int total_registrations;
        int tickets_sold;
        double sales_volume;
        double attendee_engagement_score;
        double nps_score; // Net Promoter Score
        DateTime created_at;
        DateTime updated_at;
        std::weak_ptr<Concert> concert; // To avoid circular references
    };

    // Main Concert struct for events
    struct Concert {
        int id;
        std::string name;
        std::string description;
        DateTime start_date_time;
        DateTime end_date_time;
        EventStatus event_status;
        DateTime created_at;
        DateTime updated_at;
        std::shared_ptr<ConcertTicket> ticketInfo;
        std::shared_ptr<Venue> venue;
        std::vector<std::shared_ptr<Feedback>> feedbacks;
        std::vector<std::shared_ptr<Promotion>> promotions;
        std::vector<std::shared_ptr<Crew>> crews;
        std::vector<std::shared_ptr<Attendee>> attendees;
        std::vector<std::shared_ptr<Performer>> performers;
        std::vector<std::shared_ptr<Payment>> payments;
        std::vector<std::shared_ptr<CommunicationLog>> comm_logs;
        std::vector<std::shared_ptr<ConcertReport>> reports;
    };
}
