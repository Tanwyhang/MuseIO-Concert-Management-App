#ifndef ATTENDEE_MODULE_H
#define ATTENDEE_MODULE_H

#include <string>

namespace attendee {

    struct Attendee {
        int id;                       
        std::string name;
        std::string email;
        std::string phone_number;
        std::string attendee_type;
        std::string registration_date;
        std::string check_in_time;
        std::string check_out_time;
    };


    void printAttendee(const Attendee& a); //for declaration
    Attendee createAttendee(int id, const std::string& name,
                            const std::string& email,
                            const std::string& phone,
                            const std::string& type,
                            const std::string& reg_date);

} 

#endif 
