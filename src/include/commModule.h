#ifndef COMM_MODULE_H
#define COMM_MODULE_H

#include <string>

namespace comm {

    struct CommunicationLog {
        int comm_id;                
        int concert_id;             
        std::string message_content;
        std::string sent_at;
    };

    // Optionally, you can add function declarations for handling CommunicationLog
    // For example:
    // void printCommunicationLog(const CommunicationLog& log);
    // CommunicationLog createCommunicationLog(int comm_id, int concert_id, const std::string& message_content, const std::string& sent_at);

}

#endif
