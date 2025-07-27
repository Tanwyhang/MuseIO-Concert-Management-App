#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <ctime>
#include <optional>
#include <fstream>
#include "serializationUtils.h"
#include <dataModule.h>

namespace Model {

    // Forward declarations from models.h
    struct DateTime;

    // Forward declarations
    class Concert;
    class Venue;
    class ConcertTicket;
    class Feedback;
    class Promotion;
    class Crew;
    class Attendee;
    class Performer;
    class Payment;
    class CommunicationLog;
    class ConcertReport;
    class Task;
    class Seat;
    class Sponsor;
    class Ticket;

    // Utility struct for handling date and time
    struct DateTime {
        std::string iso8601String; // ISO 8601 format: "YYYY-MM-DDThh:mm:ssZ"
        
        // Constructor from string
        DateTime(const std::string& dateTimeStr = "") : iso8601String(dateTimeStr) {}
        
        // Constructor from system time
        static DateTime now() {
            auto now = std::chrono::system_clock::now();
            std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
            char buffer[30];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&nowTime));
            return DateTime(std::string(buffer));
        }
        
        // Comparison operators
        bool operator<(const DateTime& other) const { return iso8601String < other.iso8601String; }
        bool operator>(const DateTime& other) const { return iso8601String > other.iso8601String; }
        bool operator==(const DateTime& other) const { return iso8601String == other.iso8601String; }
        
        // Return the formatted string
        std::string toString() const { return iso8601String; }
        
        SERIALIZABLE_CLASS_BEGIN(DateTime)
            SERIALIZABLE_FIELD_STRING(iso8601String)
        SERIALIZABLE_CLASS_END
            DESERIALIZABLE_FIELD_STRING(iso8601String)
        DESERIALIZABLE_CLASS_END
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
        RESERVED,
        SOLD,
        CHECKED_IN,
        CANCELLED,
        EXPIRED
    };

    // Enum for task status
    enum class TaskStatus {
        TODO,
        IN_PROGRESS,
        COMPLETED,
        BLOCKED
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
        STAFF,
        PRESS,
        PERFORMER
    };

    // Enum for discount type
    enum class DiscountType {
        PERCENTAGE,
        FIXED_AMOUNT,
        BUY_X_GET_Y
    };

    // Seat class for venue seating
    class Seat {
    private:
        int seat_id;
        std::string seat_type;    // e.g., "VIP", "Regular", "Accessible"
        std::string row_number;
        std::string col_number;
        TicketStatus status;

    public:
        Seat(int id, const std::string& type, const std::string& row, const std::string& col)
            : seat_id(id), seat_type(type), row_number(row), col_number(col), status(TicketStatus::AVAILABLE) {}
        
        // Getters
        int getId() const { return seat_id; }
        const std::string& getType() const { return seat_type; }
        const std::string& getRow() const { return row_number; }
        const std::string& getCol() const { return col_number; }
        TicketStatus getStatus() const { return status; }
        
        // Setters
        void setStatus(TicketStatus newStatus) { status = newStatus; }
    };

    // Venue class for concert locations
    class Venue {
    private:
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
        std::vector<std::shared_ptr<Seat>> seats;

    public:
        Venue(int id, const std::string& name, const std::string& address, const std::string& city,
            const std::string& state, const std::string& zipCode, const std::string& country,
            int capacity, const std::string& description = "", const std::string& contactInfo = "", 
            const std::string& seatmap = "")
            : id(id), name(name), address(address), city(city), state(state), zip_code(zipCode), 
            country(country), capacity(capacity), description(description), contact_info(contactInfo),
            seatmap(seatmap) {}
        
        // Getters
        int getId() const { return id; }
        const std::string& getName() const { return name; }
        const std::string& getAddress() const { return address; }
        const std::string& getCity() const { return city; }
        const std::string& getState() const { return state; }
        const std::string& getZipCode() const { return zip_code; }
        const std::string& getCountry() const { return country; }
        int getCapacity() const { return capacity; }
        const std::string& getDescription() const { return description; }
        const std::string& getContactInfo() const { return contact_info; }
        const std::string& getSeatmap() const { return seatmap; }
        
        // Seat management
        void addSeat(const std::shared_ptr<Seat>& seat) { seats.push_back(seat); }
        const std::vector<std::shared_ptr<Seat>>& getSeats() const { return seats; }
        std::shared_ptr<Seat> findSeat(int seatId) const;
    };

    // Task class for crew assignments
    class Task {
    private:
        int task_id;
        std::string task_name;
        std::string description;
        TaskStatus status;
        TaskPriority priority;

    public:
        Task(int id, const std::string& name, const std::string& desc, 
            TaskStatus status = TaskStatus::TODO, TaskPriority priority = TaskPriority::MEDIUM)
            : task_id(id), task_name(name), description(desc), status(status), priority(priority) {}
        
        // Getters
        int getId() const { return task_id; }
        const std::string& getName() const { return task_name; }
        const std::string& getDescription() const { return description; }
        TaskStatus getStatus() const { return status; }
        TaskPriority getPriority() const { return priority; }
        
        // Setters
        void setStatus(TaskStatus newStatus) { status = newStatus; }
        void setPriority(TaskPriority newPriority) { priority = newPriority; }
        void setDescription(const std::string& desc) { description = desc; }
    };

    // Crew class for event staff
    class Crew {
    private:
        int id;
        std::string name;
        std::string email;
        std::string phone_number;
        std::vector<std::shared_ptr<Task>> tasks;
        std::optional<DateTime> check_in_time;
        std::optional<DateTime> check_out_time;

    public:
        Crew(int id, const std::string& name, const std::string& email, const std::string& phoneNumber)
            : id(id), name(name), email(email), phone_number(phoneNumber) {}
        
        // Getters
        int getId() const { return id; }
        const std::string& getName() const { return name; }
        const std::string& getEmail() const { return email; }
        const std::string& getPhoneNumber() const { return phone_number; }
        const std::optional<DateTime>& getCheckInTime() const { return check_in_time; }
        const std::optional<DateTime>& getCheckOutTime() const { return check_out_time; }
        
        // Task management
        void assignTask(const std::shared_ptr<Task>& task) { tasks.push_back(task); }
        const std::vector<std::shared_ptr<Task>>& getTasks() const { return tasks; }
        
        // Check in/out
        void checkIn() { check_in_time = DateTime::now(); }
        void checkOut() { check_out_time = DateTime::now(); }
    };

    // Feedback class for user comments
    class Feedback {
    private:
        int rating;               // typically 1-5 or 1-10
        std::string comments;
        DateTime submitted_at;
        std::weak_ptr<Attendee> attendee; // To avoid circular references

    public:
        Feedback(int rating, const std::string& comments, const DateTime& submittedAt = DateTime::now())
            : rating(rating), comments(comments), submitted_at(submittedAt) {}
        
        // Getters
        int getRating() const { return rating; }
        const std::string& getComments() const { return comments; }
        const DateTime& getSubmittedAt() const { return submitted_at; }
        
        // Attendee relationship
        void setAttendee(const std::shared_ptr<Attendee>& attendee);
        std::shared_ptr<Attendee> getAttendee() const { return attendee.lock(); }
    };

    // Sponsor class for event sponsors
    class Sponsor {
    private:
        std::string sponsor_id;
        std::string name;
        std::string contact;
        std::string email;

    public:
        Sponsor(const std::string& id, const std::string& name, const std::string& contact, const std::string& email)
            : sponsor_id(id), name(name), contact(contact), email(email) {}
        
        // Getters
        const std::string& getId() const { return sponsor_id; }
        const std::string& getName() const { return name; }
        const std::string& getContact() const { return contact; }
        const std::string& getEmail() const { return email; }
    };

    // Promotion class for discounts and special offers
    class Promotion {
    private:
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

    public:
        Promotion(const std::string& code, const std::string& description, DiscountType type,
                double percentage, const DateTime& startDate, const DateTime& endDate,
                int usageLimit = 0)
            : code(code), description(description), discount_type(type), percentage(percentage),
            start_date_time(startDate), end_date_time(endDate), is_active(true),
            usage_limit(usageLimit), used_count(0) {}
        
        // Getters
        const std::string& getCode() const { return code; }
        const std::string& getDescription() const { return description; }
        DiscountType getDiscountType() const { return discount_type; }
        double getPercentage() const { return percentage; }
        const DateTime& getStartDateTime() const { return start_date_time; }
        const DateTime& getEndDateTime() const { return end_date_time; }
        bool isActive() const { return is_active; }
        int getUsageLimit() const { return usage_limit; }
        int getUsedCount() const { return used_count; }
        
        // Sponsor management
        void addSponsor(const std::shared_ptr<Sponsor>& sponsor) { sponsors.push_back(sponsor); }
        const std::vector<std::shared_ptr<Sponsor>>& getSponsors() const { return sponsors; }
        
        // Promotion operations
        bool canUse() const {
            DateTime now = DateTime::now();
            return is_active && 
                now > start_date_time && 
                now < end_date_time && 
                (usage_limit == 0 || used_count < usage_limit);
        }
        
        void use() {
            if (canUse()) {
                used_count++;
                if (usage_limit > 0 && used_count >= usage_limit) {
                    is_active = false;
                }
            }
        }
        
        void deactivate() { is_active = false; }
    };

    // Payment class for transaction records
    class Payment {
    private:
        int payment_id;
        double amount;
        std::string currency;
        std::string payment_method; // e.g., "Credit Card", "PayPal", etc.
        std::string transaction_id; // External payment processor ID
        PaymentStatus status;
        DateTime payment_date_time;
        std::weak_ptr<Attendee> attendee; // To avoid circular references

    public:
        Payment(int id, double amount, const std::string& currency, 
                const std::string& method, const std::string& transactionId,
                PaymentStatus status = PaymentStatus::PENDING,
                const DateTime& dateTime = DateTime::now())
            : payment_id(id), amount(amount), currency(currency), payment_method(method),
            transaction_id(transactionId), status(status), payment_date_time(dateTime) {}
        
        // Getters
        int getId() const { return payment_id; }
        double getAmount() const { return amount; }
        const std::string& getCurrency() const { return currency; }
        const std::string& getPaymentMethod() const { return payment_method; }
        const std::string& getTransactionId() const { return transaction_id; }
        PaymentStatus getStatus() const { return status; }
        const DateTime& getDateTime() const { return payment_date_time; }
        
        // Status operations
        void setStatus(PaymentStatus newStatus) { status = newStatus; }
        bool isComplete() const { return status == PaymentStatus::COMPLETED; }
        
        // Attendee relationship
        void setAttendee(const std::shared_ptr<Attendee>& attendee);
        std::shared_ptr<Attendee> getAttendee() const { return attendee.lock(); }
    };

    // Performer class for artists, bands, etc.
    class Performer {
    private:
        int performer_id;
        std::string name;
        std::string type;        // e.g., "Solo Artist", "Band", "DJ", etc.
        std::string contact_info;
        std::string bio;
        std::string image_url;   // URL to performer's image

    public:
        Performer(int id, const std::string& name, const std::string& type,
                const std::string& contactInfo = "", const std::string& bio = "",
                const std::string& imageUrl = "")
            : performer_id(id), name(name), type(type), contact_info(contactInfo),
            bio(bio), image_url(imageUrl) {}
        
        // Getters
        int getId() const { return performer_id; }
        const std::string& getName() const { return name; }
        const std::string& getType() const { return type; }
        const std::string& getContactInfo() const { return contact_info; }
        const std::string& getBio() const { return bio; }
        const std::string& getImageUrl() const { return image_url; }
        
        // Setters
        void setContactInfo(const std::string& info) { contact_info = info; }
        void setBio(const std::string& newBio) { bio = newBio; }
        void setImageUrl(const std::string& url) { image_url = url; }
    };

    // Ticket class for attendee access
    class Ticket {
    private:
        int ticket_id;
        TicketStatus status;
        std::string qr_code;    // Encoded ticket info for validation
        DateTime created_at;
        DateTime updated_at;
        std::weak_ptr<Attendee> attendee; // To avoid circular references
        std::weak_ptr<Payment> payment;   // To avoid circular references
        std::weak_ptr<ConcertTicket> concert_ticket; // To avoid circular references

    public:
        Ticket(int id, const std::string& qrCode, 
            TicketStatus status = TicketStatus::AVAILABLE,
            const DateTime& createdAt = DateTime::now())
            : ticket_id(id), status(status), qr_code(qrCode),
            created_at(createdAt), updated_at(createdAt) {}
        
        // Getters
        int getId() const { return ticket_id; }
        TicketStatus getStatus() const { return status; }
        const std::string& getQrCode() const { return qr_code; }
        const DateTime& getCreatedAt() const { return created_at; }
        const DateTime& getUpdatedAt() const { return updated_at; }
        
        // Status operations
        void setStatus(TicketStatus newStatus) {
            status = newStatus;
            updated_at = DateTime::now();
        }
        
        // Relationship setters
        void setAttendee(const std::shared_ptr<Attendee>& attendee);
        void setPayment(const std::shared_ptr<Payment>& payment);
        void setConcertTicket(const std::shared_ptr<ConcertTicket>& concertTicket);
        
        // Relationship getters
        std::shared_ptr<Attendee> getAttendee() const { return attendee.lock(); }
        std::shared_ptr<Payment> getPayment() const { return payment.lock(); }
        std::shared_ptr<ConcertTicket> getConcertTicket() const { return concert_ticket.lock(); }
    };

    // Attendee class for event participants
    class Attendee : public std::enable_shared_from_this<Attendee> {
    private:
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

    public:
        Attendee(int id, const std::string& name, const std::string& email, 
                const std::string& phoneNumber = "", AttendeeType type = AttendeeType::REGULAR,
                const DateTime& registrationDate = DateTime::now(),
                const std::string& username = "", const std::string& passwordHash = "", bool isStaff = false)
            : id(id), name(name), email(email), phone_number(phoneNumber),
            username(username), password_hash(passwordHash), staff_privileges(isStaff),
            attendee_type(type), registration_date(registrationDate) {}
        
        // Getters
        int getId() const { return id; }
        const std::string& getName() const { return name; }
        const std::string& getEmail() const { return email; }
        const std::string& getPhoneNumber() const { return phone_number; }
        const std::string& getUsername() const { return username; }
        bool isStaff() const { return staff_privileges; }
        AttendeeType getAttendeeType() const { return attendee_type; }
        const DateTime& getRegistrationDate() const { return registration_date; }
        const std::optional<DateTime>& getCheckInTime() const { return check_in_time; }
        const std::optional<DateTime>& getCheckOutTime() const { return check_out_time; }
        
        // Authentication
        bool checkPassword(const std::string& password) const {
            // In a real app, this would hash the input password and compare
            // For now, we'll do a simple comparison with the stored hash
            std::size_t hash = std::hash<std::string>{}(password);
            return password_hash == std::to_string(hash);
        }
        
        void setPassword(const std::string& newPasswordHash) {
            password_hash = newPasswordHash;
        }
        
        void setStaffPrivileges(bool isStaff) {
            staff_privileges = isStaff;
        }
        
        // Check in/out
        void checkIn() { check_in_time = DateTime::now(); }
        void checkOut() { check_out_time = DateTime::now(); }
        
        // Ticket management
        void addTicket(const std::shared_ptr<Ticket>& ticket) { 
            tickets.push_back(ticket);
            ticket->setAttendee(shared_from_this());
        }
        const std::vector<std::shared_ptr<Ticket>>& getTickets() const { return tickets; }
        
        // Feedback management
        void addFeedback(const std::shared_ptr<Feedback>& feedback) { 
            feedbacks.push_back(feedback);
            feedback->setAttendee(shared_from_this());
        }
        const std::vector<std::shared_ptr<Feedback>>& getFeedbacks() const { return feedbacks; }
        
        // Payment management
        void addPayment(const std::shared_ptr<Payment>& payment) { 
            payments.push_back(payment);
            payment->setAttendee(shared_from_this());
        }
        const std::vector<std::shared_ptr<Payment>>& getPayments() const { return payments; }
    };

    // ConcertTicket class for ticket configuration
    class ConcertTicket : public std::enable_shared_from_this<ConcertTicket> {
    private:
        std::weak_ptr<Concert> concert; // To avoid circular references
        double base_price;
        int quantity_available;
        int quantity_sold;
        DateTime start_sale_date_time;
        DateTime end_sale_date_time;
        std::vector<std::shared_ptr<Ticket>> tickets;

    public:
        ConcertTicket(double basePrice, int quantityAvailable,
                    const DateTime& startSaleDateTime, const DateTime& endSaleDateTime)
            : base_price(basePrice), quantity_available(quantityAvailable), quantity_sold(0),
            start_sale_date_time(startSaleDateTime), end_sale_date_time(endSaleDateTime) {}
        
        // Getters
        double getBasePrice() const { return base_price; }
        int getQuantityAvailable() const { return quantity_available; }
        int getQuantitySold() const { return quantity_sold; }
        const DateTime& getStartSaleDateTime() const { return start_sale_date_time; }
        const DateTime& getEndSaleDateTime() const { return end_sale_date_time; }
        
        // Ticket operations
        bool canSellTickets() const {
            DateTime now = DateTime::now();
            return quantity_sold < quantity_available &&
                now > start_sale_date_time &&
                now < end_sale_date_time;
        }
        
        std::shared_ptr<Ticket> createTicket(int ticketId, const std::string& qrCode);
        
        void addTicket(const std::shared_ptr<Ticket>& ticket) {
            tickets.push_back(ticket);
            ticket->setConcertTicket(shared_from_this());
        }
        
        bool sellTicket() {
            if (canSellTickets()) {
                quantity_sold++;
                return true;
            }
            return false;
        }
        
        // Concert relationship
        void setConcert(const std::shared_ptr<Concert>& concertPtr);
        std::shared_ptr<Concert> getConcert() const { return concert.lock(); }
    };

    // CommunicationLog class for tracking messages
    class CommunicationLog {
    private:
        int comm_id;
        std::weak_ptr<Concert> concert; // To avoid circular references
        std::string message_content;
        DateTime sent_at;
        std::string comm_type; // Email, SMS, In-App, etc.
        int recipient_count;   // Number of recipients
        bool is_automated;     // Whether this was an automated notification

    public:
        CommunicationLog(int id, const std::string& message, const DateTime& sentAt = DateTime::now(),
                        const std::string& type = "General", int recipients = 0, bool automated = false)
            : comm_id(id), message_content(message), sent_at(sentAt),
            comm_type(type), recipient_count(recipients), is_automated(automated) {}
        
        // Getters
        int getId() const { return comm_id; }
        const std::string& getMessageContent() const { return message_content; }
        const DateTime& getSentAt() const { return sent_at; }
        const std::string& getCommType() const { return comm_type; }
        int getRecipientCount() const { return recipient_count; }
        bool isAutomated() const { return is_automated; }
        
        // Setters
        void setCommType(const std::string& type) { comm_type = type; }
        void setRecipientCount(int count) { recipient_count = count; }
        void setIsAutomated(bool automated) { is_automated = automated; }
        
        // Concert relationship
        void setConcert(const std::shared_ptr<Concert>& concertPtr);
        std::shared_ptr<Concert> getConcert() const { return concert.lock(); }
    };

    // ConcertReport class for analytics
    class ConcertReport {
    private:
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

    public:
        ConcertReport(int id, const DateTime& date,
                    int totalRegistrations = 0, int ticketsSold = 0,
                    double salesVolume = 0.0, double attendeeEngagementScore = 0.0,
                    double npsScore = 0.0, const DateTime& createdAt = DateTime::now())
            : id(id), date(date), total_registrations(totalRegistrations),
            tickets_sold(ticketsSold), sales_volume(salesVolume),
            attendee_engagement_score(attendeeEngagementScore), nps_score(npsScore),
            created_at(createdAt), updated_at(createdAt) {}
        
        // Getters
        int getId() const { return id; }
        const DateTime& getDate() const { return date; }
        int getTotalRegistrations() const { return total_registrations; }
        int getTicketsSold() const { return tickets_sold; }
        double getSalesVolume() const { return sales_volume; }
        double getAttendeeEngagementScore() const { return attendee_engagement_score; }
        double getNpsScore() const { return nps_score; }
        const DateTime& getCreatedAt() const { return created_at; }
        const DateTime& getUpdatedAt() const { return updated_at; }
        
        // Setters
        void setTotalRegistrations(int value) { 
            total_registrations = value; 
            updated_at = DateTime::now();
        }
        
        void setTicketsSold(int value) { 
            tickets_sold = value; 
            updated_at = DateTime::now();
        }
        
        void setSalesVolume(double value) { 
            sales_volume = value; 
            updated_at = DateTime::now();
        }
        
        void setAttendeeEngagementScore(double value) { 
            attendee_engagement_score = value; 
            updated_at = DateTime::now();
        }
        
        void setNpsScore(double value) { 
            nps_score = value; 
            updated_at = DateTime::now();
        }
        
        // Concert relationship
        void setConcert(const std::shared_ptr<Concert>& concertPtr);
        std::shared_ptr<Concert> getConcert() const { return concert.lock(); }
    };

    // Main Concert class for events
    class Concert : public std::enable_shared_from_this<Concert> {
    private:
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

    public:
        Concert(int id, const std::string& name, const std::string& description,
                const DateTime& startDateTime, const DateTime& endDateTime,
                std::shared_ptr<Venue> venue,
                EventStatus status = EventStatus::SCHEDULED,
                const DateTime& createdAt = DateTime::now())
            : id(id), name(name), description(description),
            start_date_time(startDateTime), end_date_time(endDateTime),
            event_status(status), venue(venue),
            created_at(createdAt), updated_at(createdAt) {}
        
        // Getters
        int getId() const { return id; }
        const std::string& getName() const { return name; }
        const std::string& getDescription() const { return description; }
        const DateTime& getStartDateTime() const { return start_date_time; }
        const DateTime& getEndDateTime() const { return end_date_time; }
        // Added getDateTime() method to return the start date/time for convenience
        const DateTime& getDateTime() const { return start_date_time; }
        EventStatus getEventStatus() const { return event_status; }
        const DateTime& getCreatedAt() const { return created_at; }
        const DateTime& getUpdatedAt() const { return updated_at; }
        std::shared_ptr<Venue> getVenue() const { return venue; }
        
        // Setters
        void setEventStatus(EventStatus status) { 
            event_status = status;
            updated_at = DateTime::now();
        }
        
        // Ticket management
        void setTicketInfo(std::shared_ptr<ConcertTicket> info) { 
            ticketInfo = info;
            info->setConcert(shared_from_this());
        }
        std::shared_ptr<ConcertTicket> getTicketInfo() const { return ticketInfo; }
        
        // Feedback management
        void addFeedback(const std::shared_ptr<Feedback>& feedback) { feedbacks.push_back(feedback); }
        const std::vector<std::shared_ptr<Feedback>>& getFeedbacks() const { return feedbacks; }
        
        // Promotion management
        void addPromotion(const std::shared_ptr<Promotion>& promotion) { promotions.push_back(promotion); }
        const std::vector<std::shared_ptr<Promotion>>& getPromotions() const { return promotions; }
        
        // Crew management
        void addCrew(const std::shared_ptr<Crew>& crew) { crews.push_back(crew); }
        const std::vector<std::shared_ptr<Crew>>& getCrews() const { return crews; }
        
        // Attendee management
        void addAttendee(const std::shared_ptr<Attendee>& attendee) { attendees.push_back(attendee); }
        const std::vector<std::shared_ptr<Attendee>>& getAttendees() const { return attendees; }
        
        // Performer management
        void addPerformer(const std::shared_ptr<Performer>& performer) { performers.push_back(performer); }
        const std::vector<std::shared_ptr<Performer>>& getPerformers() const { return performers; }
        
        // Payment management
        void addPayment(const std::shared_ptr<Payment>& payment) { payments.push_back(payment); }
        const std::vector<std::shared_ptr<Payment>>& getPayments() const { return payments; }
        
        // Communication logs
        void addCommunicationLog(const std::shared_ptr<CommunicationLog>& log) { 
            comm_logs.push_back(log);
            log->setConcert(shared_from_this());
        }
        const std::vector<std::shared_ptr<CommunicationLog>>& getCommunicationLogs() const { return comm_logs; }
        
        // Reports
        void addReport(const std::shared_ptr<ConcertReport>& report) { 
            reports.push_back(report);
            report->setConcert(shared_from_this());
        }
        const std::vector<std::shared_ptr<ConcertReport>>& getReports() const { return reports; }
        
        // Serialization methods using the templated utilities
        SERIALIZABLE_CLASS_BEGIN(Concert)
            SERIALIZABLE_FIELD_INT(id)
            SERIALIZABLE_FIELD_STRING(name)
            SERIALIZABLE_FIELD_STRING(description)
            SERIALIZABLE_FIELD_DATETIME(start_date_time)
            SERIALIZABLE_FIELD_DATETIME(end_date_time)
            SERIALIZABLE_FIELD_ENUM(event_status)
            SERIALIZABLE_FIELD_DATETIME(created_at)
            SERIALIZABLE_FIELD_DATETIME(updated_at)
            // Note: Complex object relationships require special handling
        SERIALIZABLE_CLASS_END
            // serialize/deserialize relationships

            DESERIALIZABLE_FIELD_INT(id)
            DESERIALIZABLE_FIELD_STRING(name)
            DESERIALIZABLE_FIELD_STRING(description)
            DESERIALIZABLE_FIELD_DATETIME(start_date_time)
            DESERIALIZABLE_FIELD_DATETIME(end_date_time)
            DESERIALIZABLE_FIELD_ENUM(event_status)
            DESERIALIZABLE_FIELD_DATETIME(created_at)
            DESERIALIZABLE_FIELD_DATETIME(updated_at)
            // Note: Complex object relationships require special handling
        DESERIALIZABLE_CLASS_END
    };

    // Implementation of methods that require the full class definitions

    // Implement methods that need complete class definitions
    inline void Feedback::setAttendee(const std::shared_ptr<Attendee>& attendeePtr) {
        attendee = attendeePtr;
    }

    inline void Payment::setAttendee(const std::shared_ptr<Attendee>& attendeePtr) {
        attendee = attendeePtr;
    }

    inline void Ticket::setAttendee(const std::shared_ptr<Attendee>& attendeePtr) {
        attendee = attendeePtr;
    }

    inline void Ticket::setPayment(const std::shared_ptr<Payment>& paymentPtr) {
        payment = paymentPtr;
    }

    inline void Ticket::setConcertTicket(const std::shared_ptr<ConcertTicket>& concertTicketPtr) {
        concert_ticket = concertTicketPtr;
    }

    inline std::shared_ptr<Seat> Venue::findSeat(int seatId) const {
        for (const auto& seat : seats) {
            if (seat->getId() == seatId) {
                return seat;
            }
        }
        return nullptr;
    }

    inline std::shared_ptr<Ticket> ConcertTicket::createTicket(int ticketId, const std::string& qrCode) {
        auto ticket = std::make_shared<Ticket>(ticketId, qrCode);
        addTicket(ticket);
        return ticket;
    }

    inline void ConcertTicket::setConcert(const std::shared_ptr<Concert>& concertPtr) {
        concert = concertPtr;
    }

    inline void CommunicationLog::setConcert(const std::shared_ptr<Concert>& concertPtr) {
        concert = concertPtr;
    }

    inline void ConcertReport::setConcert(const std::shared_ptr<Concert>& concertPtr) {
        concert = concertPtr;
    }
}
