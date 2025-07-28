#pragma once
#include <string>
#include <vector>

namespace CrewManager {

// Forward declaration of Task and TaskArray
struct Task;
using TaskArray = std::vector<Task>;

// Crew management functions
void addCrewMember(int id, const std::string& name, const std::string& email, const std::string& phone_number);
void assignTaskToCrew(int crewId, const Task& task);
void checkInCrew(int crewId, const std::string& check_in_time);
void checkOutCrew(int crewId, const std::string& check_out_time);
void removeCrewMember(int crewId);
void listCrewMembers();
void listCrewTasks(int crewId);

} // namespace crew