#include "attendeeModule.h"
#include <iostream>
#include <string>
#include <limits>

void printAttendee(const Attendee::AttendeeInfo& attendee) {
    std::cout << "ID: " << attendee.id
              << ", Name: " << attendee.name
              << ", Email: " << attendee.email
              << ", Phone: " << attendee.phone_number
              << ", Type: " << attendee.attendee_type
              << ", Registered: " << attendee.registration_date
              << ", Check-in: " << attendee.check_in_time
              << ", Check-out: " << attendee.check_out_time
              << std::endl;
}

int main() {
    int n;
    std::cout << "Enter number of attendees to register: ";
    std::cin >> n;
    std::cin.ignore();

    for (int i = 0; i < n; ++i) {
        Attendee::AttendeeInfo info;
        std::cout << "ID: ";
        while (true) {
            std::string id_input;
            std::getline(std::cin, id_input);
            try {
                info.id = std::stoi(id_input);
                break;
            } catch (const std::exception&) {
                std::cout << "Invalid input. Please enter a valid ID: ";
            }
        }
        std::cout << "Name: ";
        std::getline(std::cin, info.name);
        std::cout << "Email: ";
        std::getline(std::cin, info.email);
        std::cout << "Phone: ";
        std::getline(std::cin, info.phone_number);
        std::cout << "Type (VIP/Regular): ";
        std::getline(std::cin, info.attendee_type);
        std::cout << "Registration date (YYYY-MM-DD): ";
        std::getline(std::cin, info.registration_date);
        info.check_in_time = "";
        info.check_out_time = "";
        Attendee::registerAttendee(info);
    }

    // Print all attendees
    std::cout << "\nAll attendees:\n";
    for (const auto& attendee : Attendee::attendees) {
        printAttendee(attendee);
    }

    std::cout << "\nAttendee module tester executed successfully." << std::endl;
    return 0;
}
