#pragma once

#include <string>

namespace ConcertManager {

    // Functions to create and edit concerts
    void createConcert(const std::string& name, const std::string& description,
                       const std::string& startDateTime, const std::string& endDateTime);

    void editConcert(int concertId, const std::string& name, const std::string& description,
                     const std::string& startDateTime, const std::string& endDateTime);

    // Functions to manage shows
    void addShowToConcert(int concertId, const std::string& showName, const std::string& showTime);
    void removeShowFromConcert(int concertId, int showId);

    // Functions to handle concert operations
    void startConcert(int concertId);
    void endConcert(int concertId);
    void cancelConcert(int concertId);

}
