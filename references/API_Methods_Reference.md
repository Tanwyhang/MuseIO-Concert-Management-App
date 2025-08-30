# MuseIO Concert Management System - Complete API Methods Reference

## Table of Contents
1. [AttendeeModule](#attendeemodule)
2. [AuthModule](#authmodule)
3. [BaseModule Template](#basemodule-template)
4. [CommunicationModule](#communicationmodule)
5. [ConcertModule](#concertmodule)
6. [CrewModule](#crewmodule)
7. [FeedbackModule](#feedbackmodule)
8. [PaymentModule](#paymentmodule)
9. [PerformerModule](#performermodule)
10. [ReportModule](#reportmodule)
11. [TicketModule](#ticketmodule)
12. [UIManager](#uimanager)
13. [VenueModule](#venuemodule)
14. [Models](#models)

---

## AttendeeModule

**File**: `attendeeModule.hpp`  
**Inherits from**: `BaseModule<Model::Attendee>`  
**Data File**: `../../data/attendees.dat`

### Constructor & Destructor
```cpp
AttendeeModule()
~AttendeeModule()
```

### Core Methods
```cpp
std::shared_ptr<Model::Attendee> createAttendee(
    const std::string& name, 
    const std::string& email, 
    const std::string& phone, 
    Model::AttendeeType type = Model::AttendeeType::REGULAR,
    const std::string& username = "",
    const std::string& password = "",
    bool isStaff = false
)

std::shared_ptr<Model::Attendee> getAttendeeById(int id)

const std::vector<std::shared_ptr<Model::Attendee>>& getAllAttendees() const

std::vector<std::shared_ptr<Model::Attendee>> findAttendeesByName(const std::string& nameQuery)

std::shared_ptr<Model::Attendee> findAttendeeByEmail(const std::string& email)

bool updateAttendee(
    int id, 
    const std::string& name = "", 
    const std::string& email = "", 
    const std::string& phone = "",
    Model::AttendeeType type = Model::AttendeeType::REGULAR
)

bool deleteAttendee(int id)
```

### Protected Methods
```cpp
int getEntityId(const std::shared_ptr<Model::Attendee>& attendee) const override

void loadEntities() override

bool saveEntities() override
```

---

## AuthModule

**File**: `authModule.hpp`  
**Security Features**: Raw pointer memory management, XOR encryption, secure memory wiping

### Constructor & Destructor
```cpp
AuthModule(size_t initialMemorySize = 4096)
~AuthModule()
```

### Core Authentication Methods
```cpp
bool registerUser(const std::string& username, const std::string& password, int userType = 0)

int authenticateUser(const std::string& username, const std::string& password)

bool changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword)

bool deleteUser(const std::string& username)

int getUserType(const std::string& username)

bool userExists(const std::string& username)

size_t getUserCount() const
```

### File Operations
```cpp
bool saveCredentials(const std::string& filePath)

bool loadCredentials(const std::string& filePath)
```

### Private Security Methods
```cpp
void generateSalt(unsigned char* salt, size_t length)

void hashPassword(const std::string& password, const unsigned char* salt, size_t saltLength, unsigned char* output, size_t outputLength)

void xorCrypt(unsigned char* data, size_t length)

size_t storeCredential(const std::string& username, const unsigned char* passwordHash, const unsigned char* salt, int userType)

void resizeSecureMemory(size_t newSize)

void generateEncryptionKey()
```

---

## BaseModule Template

**File**: `baseModule.hpp`  
**Template Parameters**: `<typename EntityType, typename IdType = int>`

### Constructor & Destructor
```cpp
BaseModule(const std::string& filePath)
virtual ~BaseModule()
```

### Virtual Methods (must be implemented by derived classes)
```cpp
virtual IdType getEntityId(const std::shared_ptr<EntityType>& entity) const = 0
virtual void loadEntities() = 0
virtual bool saveEntities() = 0
```

### Core Template Methods
```cpp
virtual std::shared_ptr<EntityType> getById(IdType id)

virtual const std::vector<std::shared_ptr<EntityType>>& getAll() const

virtual bool deleteEntity(IdType id)

std::vector<std::shared_ptr<EntityType>> findByPredicate(std::function<bool(const std::shared_ptr<EntityType>&)> predicate) const

virtual IdType generateNewId()
```

### Binary I/O Helper Methods
```cpp
template<typename T> void readBinary(std::ifstream& file, T& value)

template<typename T> void writeBinary(std::ofstream& file, const T& value)

std::string readString(std::ifstream& file)

void writeString(std::ofstream& file, const std::string& str)
```

---

## CommunicationModule

**File**: `commModule.hpp`  
**Inherits from**: `BaseModule<Model::CommunicationLog>`  
**Data File**: `../../data/communications.dat`

### Enums
```cpp
enum class UserRole { ATTENDEE, ADMIN, MODERATOR }
enum class MessageType { REGULAR, ANNOUNCEMENT, PINNED, SYSTEM, REACTION }
```

### Structures
```cpp
struct ChatMessage {
    int message_id;
    int concert_id;
    int sender_id;
    UserRole sender_role;
    std::string sender_name;
    std::string message_content;
    MessageType message_type;
    Model::DateTime sent_at;
    bool is_pinned;
    bool is_moderated;
    std::vector<std::string> reactions;
    int reply_to_id;
}

struct ChatSubscriber {
    int user_id;
    std::string username;
    bool has_unread;
    Model::DateTime last_seen;
    UserRole role;
}
```

### Constructor & Destructor
```cpp
CommunicationModule()
~CommunicationModule()
```

### Core Chat Methods
```cpp
bool createChatroom(int concertId, const std::string& concertName)

bool subscribeToChat(int concertId, int userId, const std::string& username, UserRole role)

ChatMessage* sendMessage(int concertId, int senderId, const std::string& senderName, UserRole senderRole, const std::string& content, MessageType messageType = MessageType::REGULAR, int replyToId = -1)

std::vector<ChatMessage*> getMessages(int concertId, int limit = 0, const std::string& afterTimestamp = "")

std::vector<ChatMessage*> getPinnedMessages(int concertId)

bool togglePinMessage(int concertId, int messageId, int userId, bool pin)

std::vector<ChatMessage*> searchMessages(int concertId, const std::string& keyword, bool caseSensitive = false)

bool addReaction(int concertId, int messageId, int userId, const std::string& reaction)
```

### Notification Methods
```cpp
int getUnreadCount(int concertId, int userId)

void markAsRead(int concertId, int userId)
```

### Statistics
```cpp
std::string getChatStatistics(int concertId)
```

### Protected Methods
```cpp
int getEntityId(const std::shared_ptr<Model::CommunicationLog>& commLog) const override

void loadEntities() override

bool saveEntities() override
```

### Private Helper Methods
```cpp
void notifyNewMessage(int concertId, int messageId)

bool isUserAdmin(int concertId, int userId)

ChatSubscriber* findSubscriber(int concertId, int userId)

void loadChatData()

void saveChatData()
```

### CommunicationManager Namespace Functions
```cpp
bool createEventChatroom(int concertId, const std::string& concertName)

bool joinChat(int concertId, int userId, const std::string& username, UserRole role)

ChatMessage* sendMessage(int concertId, int senderId, const std::string& senderName, UserRole senderRole, const std::string& content, MessageType messageType = MessageType::REGULAR)

std::vector<ChatMessage*> getRecentMessages(int concertId, int limit = 50)

std::vector<ChatMessage*> searchChat(int concertId, const std::string& keyword)

std::vector<ChatMessage*> getPinnedAnnouncements(int concertId)

std::string getChatStats(int concertId)

void markAsRead(int concertId, int userId)

int getUnreadCount(int concertId, int userId)
```

---

## ConcertModule

**File**: `concertModule.hpp`  
**Inherits from**: `BaseModule<Model::Concert>`  
**Data File**: `../../data/concerts.dat`

### Constructor & Destructor
```cpp
ConcertModule()
~ConcertModule()
```

### Core Concert Management
```cpp
std::shared_ptr<Model::Concert> createConcert(const std::string& name, const std::string& description, const std::string& startDateTime, const std::string& endDateTime)

bool editConcert(int concertId, const std::string& name = "", const std::string& description = "", const std::string& startDateTime = "", const std::string& endDateTime = "")

std::shared_ptr<Model::Concert> getConcertById(int id)

const std::vector<std::shared_ptr<Model::Concert>>& getAllConcerts() const
```

### Search Methods
```cpp
std::vector<std::shared_ptr<Model::Concert>> findConcertsByName(const std::string& nameQuery)

std::vector<std::shared_ptr<Model::Concert>> findConcertsByDateRange(const std::string& startDate, const std::string& endDate)

std::vector<std::shared_ptr<Model::Concert>> findConcertsByStatus(Model::EventStatus status)
```

### Concert Configuration
```cpp
bool setVenueForConcert(int concertId, std::shared_ptr<Model::Venue> venue)

bool setupTicketInfo(int concertId, double basePrice, int quantity, const std::string& saleStartDateTime, const std::string& saleEndDateTime)

bool addPerformerToConcert(int concertId, std::shared_ptr<Model::Performer> performer)

bool addPromotionToConcert(int concertId, std::shared_ptr<Model::Promotion> promotion)
```

### Concert Status Management
```cpp
bool changeConcertStatus(int concertId, Model::EventStatus newStatus)

bool startConcert(int concertId)

bool endConcert(int concertId)

bool cancelConcert(int concertId)
```

### Show Management
```cpp
bool addShowToConcert(int concertId, const std::string& showName, const std::string& showTime)

bool removeShowFromConcert(int concertId, int showId)
```

### Reporting
```cpp
std::shared_ptr<Model::ConcertReport> generateConcertReport(int concertId)
```

---

## CrewModule

**File**: `crewModule.hpp`  
**Inherits from**: `BaseModule<Model::Crew>`  
**Data File**: `../../data/crews.dat`

### Constructor & Destructor
```cpp
CrewModule()
~CrewModule()
```

### Core Crew Management
```cpp
std::shared_ptr<Model::Crew> createCrewMember(const std::string& name, const std::string& email, const std::string& phone, const std::string& job = "General Staff")

std::shared_ptr<Model::Crew> getCrewById(int id)

const std::vector<std::shared_ptr<Model::Crew>>& getAllCrew() const
```

### Search Methods
```cpp
std::vector<std::shared_ptr<Model::Crew>> findCrewByName(const std::string& nameQuery)

std::shared_ptr<Model::Crew> findCrewByEmail(const std::string& email)

std::vector<std::shared_ptr<Model::Crew>> findCrewByJob(const std::string& jobQuery)
```

### Job Management
```cpp
std::string getCrewJob(int crewId)

bool setCrewJob(int crewId, const std::string& job)
```

### Task Management
```cpp
bool assignTaskToCrew(int crewId, const std::string& taskName, const std::string& description, Model::TaskPriority priority = Model::TaskPriority::MEDIUM)

bool updateTaskStatus(int crewId, int taskId, Model::TaskStatus status)

std::vector<std::shared_ptr<Model::Task>> getCrewTasks(int crewId)

std::vector<std::shared_ptr<Model::Task>> getTasksByStatus(Model::TaskStatus status)

std::vector<std::shared_ptr<Model::Task>> getTasksByPriority(Model::TaskPriority priority)
```

### Check-in/Check-out
```cpp
bool checkInCrew(int crewId)

bool checkOutCrew(int crewId)

bool isCrewCheckedIn(int crewId)

std::vector<std::shared_ptr<Model::Crew>> getCheckedInCrew()
```

### Update Methods
```cpp
bool updateCrewMember(int id, const std::string& name = "", const std::string& email = "", const std::string& phone = "")

bool deleteCrewMember(int id)
```

### CrewManager Namespace Functions
```cpp
std::shared_ptr<Model::Crew> createCrewMember(const std::string& name, const std::string& email, const std::string& phone, const std::string& job = "General Staff")

std::shared_ptr<Model::Crew> getCrewById(int id)

const std::vector<std::shared_ptr<Model::Crew>>& getAllCrew()

std::vector<std::shared_ptr<Model::Crew>> findCrewByName(const std::string& nameQuery)

std::shared_ptr<Model::Crew> findCrewByEmail(const std::string& email)

std::vector<std::shared_ptr<Model::Crew>> findCrewByJob(const std::string& jobQuery)

bool assignTaskToCrew(int crewId, const std::string& taskName, const std::string& description, Model::TaskPriority priority = Model::TaskPriority::MEDIUM)

bool checkInCrew(int crewId)

bool checkOutCrew(int crewId)

bool setCrewJob(int crewId, const std::string& job)
```

---

## FeedbackModule

**File**: `feedbackModule.hpp`  
**Inherits from**: `BaseModule<Model::Feedback>`  
**Data File**: `../../data/feedback.dat`

### Enums & Structures
```cpp
enum class SentimentType { POSITIVE, NEUTRAL, NEGATIVE, CRITICAL }
enum class FeedbackCategory { SOUND, VENUE, PRICING, PERFORMERS, ORGANIZATION, GENERAL }

struct ExtendedFeedback {
    std::shared_ptr<Model::Feedback> baseFeedback;
    int concert_id;
    FeedbackCategory category;
    SentimentType sentiment;
    bool requires_escalation;
    std::string escalation_reason;
}
```

### Constructor & Destructor
```cpp
FeedbackModule()
~FeedbackModule()
```

### Core Feedback Methods
```cpp
std::shared_ptr<Model::Feedback> createFeedback(int concertId, int attendeeId, int rating, const std::string& comments, FeedbackCategory category = FeedbackCategory::GENERAL)

std::vector<ExtendedFeedback*> getFeedbackForEvent(int concertId)

double getEventAverageRating(int concertId)

std::vector<ExtendedFeedback*> getUrgentFeedback()

std::vector<int> getEventsWithLowRatings()
```

### Analysis Methods
```cpp
std::string analyzeSentiment(int concertId)

std::vector<ExtendedFeedback*> getFeedbackByCategory(int concertId, FeedbackCategory category)

std::vector<ExtendedFeedback*> getFeedbackBySentiment(SentimentType sentiment)
```

### FeedbackManager Namespace Functions
```cpp
std::shared_ptr<Model::Feedback> collectFeedback(int concertId, int attendeeId, int rating, const std::string& comments, FeedbackCategory category = FeedbackCategory::GENERAL)

double getEventRating(int concertId)

std::string analyzeSentiment(int concertId)

std::vector<ExtendedFeedback*> getUrgentFeedback()

std::vector<ExtendedFeedback*> getFeedbackForEvent(int concertId)
```

---

## PaymentModule

**File**: `paymentModule.hpp`  
**Namespace**: `PaymentManager`  
**Inherits from**: `BaseModule<Model::Payment, int>`

### Constructor & Destructor
```cpp
PaymentModule(const std::string& filePath)
~PaymentModule()
```

### Core Payment Operations
```cpp
std::string processPayment(int attendee_id, double amount, const std::string& currency, const std::string& payment_method)

int createPayment(int attendee_id, double amount, const std::string& currency, const std::string& payment_method, const std::string& transaction_id)

int createPayment(const Model::Payment& payment)

bool updatePaymentStatus(int payment_id, Model::PaymentStatus status)

std::string processRefund(int payment_id, double refund_amount = 0.0, const std::string& reason = "")
```

### Query Operations
```cpp
std::shared_ptr<Model::Payment> getPaymentById(int payment_id)

std::vector<std::shared_ptr<Model::Payment>> getPaymentsByAttendee(int attendee_id)

std::vector<std::shared_ptr<Model::Payment>> getPaymentsByStatus(Model::PaymentStatus status)

std::vector<std::shared_ptr<Model::Payment>> getPaymentsByDateRange(const std::string& start_date, const std::string& end_date)

std::vector<std::shared_ptr<Model::Payment>> getPaymentsByMethod(const std::string& payment_method)
```

### Analytics & Reporting
```cpp
struct PaymentStatistics {
    int total_payments;
    double total_amount;
    double average_payment;
    int successful_payments;
    int failed_payments;
    int refunded_payments;
    std::map<std::string, int> payments_by_method;
    std::map<std::string, double> revenue_by_currency;
}

PaymentStatistics getPaymentStatistics()

PaymentStatistics getPaymentStatistics(const std::string& start_date, const std::string& end_date)

double getTotalRevenue()

double getTotalRevenue(const std::string& start_date, const std::string& end_date)

std::map<std::string, double> getRevenueByMethod()

std::map<Model::PaymentStatus, int> getPaymentStatusDistribution()
```

---

## PerformerModule

**File**: `performerModule.hpp`  
**Inherits from**: `BaseModule<Model::Performer>`  
**Data File**: `../../data/performers.dat`

### Constructor & Destructor
```cpp
PerformerModule()
~PerformerModule()
```

### Core Performer Management
```cpp
std::shared_ptr<Model::Performer> createPerformer(const std::string& name, const std::string& type, const std::string& contactInfo, const std::string& bio, const std::string& imageUrl = "")

std::shared_ptr<Model::Performer> getPerformerById(int id)

const std::vector<std::shared_ptr<Model::Performer>>& getAllPerformers() const
```

### Search Methods
```cpp
std::vector<std::shared_ptr<Model::Performer>> findPerformersByName(const std::string& nameQuery)

std::vector<std::shared_ptr<Model::Performer>> findPerformersByType(const std::string& typeQuery)
```

### Update Methods
```cpp
bool updatePerformer(int id, const std::string& name = "", const std::string& type = "", const std::string& contactInfo = "", const std::string& bio = "", const std::string& imageUrl = "")

bool deletePerformer(int id)
```

### PerformerManager Namespace Functions
```cpp
std::shared_ptr<Model::Performer> createPerformer(const std::string& name, const std::string& type, const std::string& contactInfo, const std::string& bio, const std::string& imageUrl = "")

std::shared_ptr<Model::Performer> getPerformerById(int id)

const std::vector<std::shared_ptr<Model::Performer>>& getAllPerformers()

std::vector<std::shared_ptr<Model::Performer>> findPerformersByName(const std::string& nameQuery)

std::vector<std::shared_ptr<Model::Performer>> findPerformersByType(const std::string& typeQuery)

bool updatePerformer(int id, const std::string& name = "", const std::string& type = "", const std::string& contactInfo = "", const std::string& bio = "", const std::string& imageUrl = "")

bool deletePerformer(int id)
```

---

## ReportModule

**File**: `reportModule.hpp`  
**Namespace**: `ReportManager`  
**Inherits from**: `BaseModule<Model::ConcertReport, int>`

### Constructor & Destructor
```cpp
ReportModule(const std::string& filePath = "data/reports.dat")
~ReportModule()
```

### Core Report Operations
```cpp
int generateConcertReport(int concert_id)

bool updateConcertReport(int report_id)

std::string generateSalesAnalyticsReport(const std::string& start_date, const std::string& end_date, const std::string& format = "JSON")

std::string generatePayrollReport(const std::string& start_date, const std::string& end_date, const std::string& format = "JSON")

std::string generateProfitLossStatement(const std::string& start_date, const std::string& end_date, const std::string& format = "JSON")
```

### Analytics & Metrics
```cpp
struct SummaryMetrics {
    int total_concerts;
    int active_concerts;
    int completed_concerts;
    int cancelled_concerts;
    int total_attendees;
    int total_tickets_sold;
    double total_revenue;
    double average_ticket_price;
    double overall_satisfaction_score;
    double nps_score;
    std::string most_popular_concert;
    std::string top_performing_venue;
}

SummaryMetrics calculateSummaryMetrics()

struct ConcertMetrics {
    int concert_id;
    int total_attendees;
    int tickets_sold;
    double revenue;
    double average_rating;
    int feedback_count;
    double occupancy_rate;
}

ConcertMetrics getConcertMetrics(int concert_id)
```

### Query Operations
```cpp
std::shared_ptr<Model::ConcertReport> getReportById(int report_id)

std::vector<std::shared_ptr<Model::ConcertReport>> getReportsByConcert(int concert_id)

std::vector<std::shared_ptr<Model::ConcertReport>> getReportsByDateRange(const std::string& start_date, const std::string& end_date)
```

### Scheduling
```cpp
std::string scheduleAutomaticReport(const std::string& report_type, const std::string& frequency, const std::vector<std::string>& recipients)

bool cancelScheduledReport(const std::string& schedule_id)
```

---

## TicketModule

**File**: `ticketModule.hpp`  
**Namespace**: `TicketManager`  
**Inherits from**: `BaseModule<Model::Ticket, int>`

### Constructor & Destructor
```cpp
TicketModule(const std::string& filePath)
~TicketModule()
```

### Core Ticket Operations
```cpp
int createTicket(int attendee_id, int concert_id, const std::string& ticket_type)

std::vector<int> createMultipleTickets(int attendee_id, int concert_id, int quantity, const std::string& ticket_type)

bool updateTicketStatus(int ticket_id, Model::TicketStatus status)

bool cancelTicket(int ticket_id, const std::string& reason = "")
```

### QR Code Management
```cpp
std::string generateQRCode(int ticket_id)

int validateQRCode(const std::string& qr_code)

bool checkInWithQRCode(const std::string& qr_code)
```

### Query Operations
```cpp
std::shared_ptr<Model::Ticket> getTicketById(int ticket_id)

std::vector<std::shared_ptr<Model::Ticket>> getTicketsByAttendee(int attendee_id)

std::vector<std::shared_ptr<Model::Ticket>> getTicketsByConcert(int concert_id)

std::vector<std::shared_ptr<Model::Ticket>> getTicketsByStatus(Model::TicketStatus status)
```

### Reservation System
```cpp
struct TicketReservation {
    std::string reservation_id;
    int concert_id;
    int quantity;
    Model::DateTime reserved_at;
    Model::DateTime expires_at;
    bool is_confirmed;
}

std::string reserveTickets(int concert_id, int quantity, int minutes_to_hold)

std::vector<int> confirmReservation(const std::string& reservation_id, int attendee_id)

bool cancelReservation(const std::string& reservation_id)

std::vector<TicketReservation> getActiveReservations()
```

### Analytics
```cpp
struct TicketStatistics {
    int total_tickets;
    int sold_tickets;
    int available_tickets;
    int checked_in_tickets;
    int cancelled_tickets;
    double sales_rate;
    double check_in_rate;
}

TicketStatistics getTicketStatistics()

TicketStatistics getTicketStatistics(int concert_id)

int getAvailableTicketCount(int concert_id)

double getTicketSalesRate(int concert_id)
```

---

## UIManager

**File**: `uiModule.hpp`  
**Namespace**: `UIManager`

### Menu Constants
```cpp
const std::string MAIN_MENU

const std::string APP_BANNER

const std::string MANAGEMENT_MENU

const std::string USER_MENU

const std::string MAIN_MENU_CHOICES

const std::string MANAGEMENT_PORTAL_CHOICES

const std::string USER_PORTAL_CHOICES
```

---

## VenueModule

**File**: `venueModule.hpp`  
**Inherits from**: `BaseModule<Model::Venue>`  
**Data File**: `../../data/venues.dat`

### Constructor & Destructor
```cpp
VenueModule()
~VenueModule()
```

### Core Venue Management
```cpp
std::shared_ptr<Model::Venue> createVenue(const std::string& name, const std::string& address, const std::string& city, const std::string& state, const std::string& zipCode, const std::string& country, int capacity, const std::string& description = "", const std::string& contactInfo = "", const std::string& seatmap = "")

std::shared_ptr<Model::Venue> getVenueById(int id)

const std::vector<std::shared_ptr<Model::Venue>>& getAllVenues() const
```

### Search Methods
```cpp
std::vector<std::shared_ptr<Model::Venue>> findVenuesByName(const std::string& nameQuery)

std::vector<std::shared_ptr<Model::Venue>> findVenuesByCity(const std::string& city)

std::vector<std::shared_ptr<Model::Venue>> findVenuesByCapacity(int minCapacity)
```

### Seating Management
```cpp
bool initializeVenueSeatingPlan(int venueId, int rows, int columns)

bool addSeat(int venueId, const std::string& seatType, const std::string& row, const std::string& column)

std::vector<std::shared_ptr<Model::Seat>> getSeatsForVenue(int venueId)

std::shared_ptr<Model::Seat> getSeatById(int venueId, int seatId)

bool updateSeatStatus(int venueId, int seatId, Model::TicketStatus status)

std::vector<std::shared_ptr<Model::Seat>> getAvailableSeats(int venueId)

std::vector<std::shared_ptr<Model::Seat>> getSeatsByStatus(int venueId, Model::TicketStatus status)

std::string displaySeatingPlan(int venueId)
```

### Update Methods
```cpp
bool updateVenue(int id, const std::string& name = "", const std::string& address = "", const std::string& city = "", const std::string& state = "", const std::string& zipCode = "", const std::string& country = "", int capacity = 0, const std::string& description = "", const std::string& contactInfo = "")

bool deleteVenue(int id)
```

### VenueManager Namespace Functions
```cpp
std::shared_ptr<Model::Venue> createVenue(const std::string& name, const std::string& address, const std::string& city, const std::string& state, const std::string& zipCode, const std::string& country, int capacity, const std::string& description = "", const std::string& contactInfo = "", const std::string& seatmap = "")

std::shared_ptr<Model::Venue> getVenueById(int id)

const std::vector<std::shared_ptr<Model::Venue>>& getAllVenues()

std::vector<std::shared_ptr<Model::Venue>> findVenuesByName(const std::string& nameQuery)

std::vector<std::shared_ptr<Model::Venue>> findVenuesByCity(const std::string& city)

std::vector<std::shared_ptr<Model::Venue>> findVenuesByCapacity(int minCapacity)

bool initializeVenueSeatingPlan(int venueId, int rows, int columns)

bool addSeat(int venueId, const std::string& seatType, const std::string& row, const std::string& column)

std::vector<std::shared_ptr<Model::Seat>> getSeatsForVenue(int venueId)

bool updateSeatStatus(int venueId, int seatId, Model::TicketStatus status)

std::string displaySeatingPlan(int venueId)
```

---

## Models

**File**: `models.hpp`  
**Namespace**: `Model`

### Core Structures
```cpp
struct DateTime {
    std::string iso8601String;
    static DateTime now();
}

struct Seat {
    int seat_id;
    std::string seat_type;
    std::string row_number;
    std::string col_number;
    TicketStatus status;
}

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
    std::string seatmap;
    std::vector<std::shared_ptr<Seat>> seats;
    std::vector<std::vector<std::shared_ptr<Seat>>> seatingPlan;
    int rows;
    int columns;
}

struct Task {
    int task_id;
    std::string task_name;
    std::string description;
    TaskStatus status;
    TaskPriority priority;
}

struct Crew {
    int id;
    std::string name;
    std::string email;
    std::string phone_number;
    std::vector<std::shared_ptr<Task>> tasks;
    std::optional<DateTime> check_in_time;
    std::optional<DateTime> check_out_time;
}

struct Feedback {
    int rating;
    std::string comments;
    DateTime submitted_at;
    std::weak_ptr<Attendee> attendee;
}

struct Promotion {
    std::string code;
    std::string description;
    DiscountType discount_type;
    double percentage;
    DateTime start_date_time;
    DateTime end_date_time;
    bool is_active;
    int usage_limit;
    int used_count;
}

struct Payment {
    int payment_id;
    double amount;
    std::string currency;
    std::string payment_method;
    std::string transaction_id;
    PaymentStatus status;
    DateTime payment_date_time;
    std::weak_ptr<Attendee> attendee;
}

struct Performer {
    int performer_id;
    std::string name;
    std::string performer_type;
    std::string contact_info;
    std::string bio;
    std::string image_url;
}

struct Ticket {
    int ticket_id;
    std::string ticket_type;
    double price;
    TicketStatus status;
    DateTime purchased_at;
    std::string qr_code;
    std::weak_ptr<Attendee> attendee;
    std::weak_ptr<Concert> concert;
}

struct Attendee {
    int id;
    std::string name;
    std::string email;
    std::string phone_number;
    AttendeeType attendee_type;
    DateTime registration_date;
    std::string username;
    std::string password_hash;
    bool staff_privileges;
}

struct ConcertTicket {
    double base_price;
    int quantity_available;
    int quantity_sold;
    DateTime start_sale_date_time;
    DateTime end_sale_date_time;
    std::weak_ptr<Concert> concert;
}

struct CommunicationLog {
    int comm_id;
    std::string message_content;
    DateTime sent_at;
    std::string comm_type;
    int recipient_count;
    bool is_automated;
    std::weak_ptr<Concert> concert;
}

struct ConcertReport {
    int id;
    int total_attendees;
    double total_revenue;
    double total_expenses;
    double profit_loss;
    DateTime generated_at;
    std::string report_content;
    std::weak_ptr<Concert> concert;
}

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
}
```

### Enums
```cpp
enum class EventStatus { SCHEDULED, CANCELLED, POSTPONED, COMPLETED, SOLDOUT }

enum class PaymentStatus { PENDING, COMPLETED, FAILED, REFUNDED }

enum class TicketStatus { AVAILABLE, SOLD, CHECKED_IN, CANCELLED, EXPIRED }

enum class TaskStatus { TODO, IN_PROGRESS, COMPLETED }

enum class TaskPriority { LOW, MEDIUM, HIGH, CRITICAL }

enum class AttendeeType { REGULAR, VIP }

enum class DiscountType { PERCENTAGE, FIXED_AMOUNT, BUY_X_GET_Y }
```

---

## Usage Notes

1. **File Paths**: All modules use relative paths like `../../data/*.dat` for data persistence
2. **Smart Pointers**: System uses `std::shared_ptr<T>` for ownership and `std::weak_ptr<T>` for relationships  
3. **Binary Serialization**: All modules implement binary I/O using `readBinary()`, `writeBinary()`, `readString()`, `writeString()`
4. **Dual-Layer API**: Most modules provide both class-based access and namespace wrapper functions
5. **Error Handling**: Methods typically return `nullptr` for not found, `false` for operation failure
6. **Case Sensitivity**: Search functions are generally case-insensitive using `std::transform` + `std::tolower`
7. **ID Generation**: Uses `generateNewId()` method (max existing ID + 1)
8. **Template Inheritance**: Most entity modules extend `BaseModule<EntityType>` template
