#ifndef ATTENDEEMODULE_H
#define ATTENDEEMODULE_H

#include <string>
#include <vector>
#include <optional>

namespace Attendee {

    struct AttendeeInfo {
        int id;
        std::string name;
        std::string email;
        std::string phone_number;
        std::string attendee_type;
        std::string registration_date;
        std::string check_in_time;
        std::string check_out_time;
    };

    //for managing attendees need to access by phone number
    // Storage for attendees
    inline std::vector<AttendeeInfo> attendees;

    // Register a new attendee
    inline void registerAttendee(const AttendeeInfo& attendee) {
        attendees.push_back(attendee);
    }

    // Record check-in time by attendee id
    inline bool recordCheckIn(int id, const std::string& check_in_time) {
        for (auto& attendee : attendees) {
            if (attendee.id == id) {
                attendee.check_in_time = check_in_time;
                return true;
            }
        }
        return false;
    }

    // Record check-out time by attendee id
    inline bool recordCheckOut(int id, const std::string& check_out_time) {
        for (auto& attendee : attendees) {
            if (attendee.id == id) {
                attendee.check_out_time = check_out_time;
                return true;
            }
        }
        return false;
    }

    // Update attendee information (except id) by id
    inline bool updateAttendeeInfo(int id, const std::string& name, const std::string& email,
                                   const std::string& phone_number, const std::string& attendee_type,
                                   const std::string& registration_date) {
        for (auto& attendee : attendees) {
            if (attendee.id == id) {
                attendee.name = name;
                attendee.email = email;
                attendee.phone_number = phone_number;
                attendee.attendee_type = attendee_type;
                attendee.registration_date = registration_date;
                return true;
            }
        }
        return false;
    }

    // Update attendee information (except id) by phone number
    inline bool updateAttendeeInfoByPhone(const std::string& phone_number, const std::string& name, const std::string& email,
                                          const std::string& attendee_type, const std::string& registration_date) {
        for (auto& attendee : attendees) {
            if (attendee.phone_number == phone_number) {
                attendee.name = name;
                attendee.email = email;
                attendee.attendee_type = attendee_type;
                attendee.registration_date = registration_date;
                return true;
            }
        }
        return false;
    }

    // Optional: Find attendee by id
    inline std::optional<AttendeeInfo> getAttendee(int id) {
        for (const auto& attendee : attendees) {
            if (attendee.id == id) {
                return attendee;
            }
        }
        return std::nullopt;
    }

    // Optional: Find attendee by phone number
    inline std::optional<AttendeeInfo> getAttendeeByPhone(const std::string& phone_number) {
        for (const auto& attendee : attendees) {
            if (attendee.phone_number == phone_number) {
                return attendee;
            }
        }
        return std::nullopt;
    }

}

#endif 
