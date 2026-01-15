#pragma once
#include <string>
#include <vector>
#include "RoomStatus.h"

class Room
{
private:
    int id; // уникальный ID комнаты
    int roomNumber; // номер комнаты
    std::string category;
    double pricePerNight;
    RoomStatus status;
    std::vector<std::string> amenities;
    bool active = true;
public:
    Room(int id, int roomNumber, const std::string& category, double pricePerNight,
        RoomStatus status, const std::vector<std::string>& amenities, bool active = true);
    int getId() const { return id; }
    int getRoomNumber() const;
    std::string getCategory() const;
    double getPricePerNight() const;
    RoomStatus getStatus() const;
    std::vector<std::string> getAmenities() const;
    void setPricePerNight(double price);

    // Try to change room status. Returns true if transition is allowed and applied.
    // Allowed transitions (typical hotel logic):
    // AVAILABLE -> BOOKED | MAINTENANCE
    // BOOKED -> OCCUPIED | AVAILABLE (cancellation) | MAINTENANCE
    // OCCUPIED -> CLEANING | MAINTENANCE
    // CLEANING -> AVAILABLE | MAINTENANCE
    // MAINTENANCE -> AVAILABLE
    bool setStatus(RoomStatus newStatus);

    // Check if a transition is allowed without applying it
    bool canTransition(RoomStatus newStatus) const;

    void addAmenity(const std::string& amenity);
    bool isAvailable() const;
    std::string toString() const;
    bool isActive() const { return active; }
    void setActive(bool val) { active = val; }

    // setters for editing
    void setRoomNumber(int num) { roomNumber = num; }
    void setCategory(const std::string& cat) { category = cat; }
};