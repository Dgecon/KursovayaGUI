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
    void setStatus(RoomStatus newStatus);
    void addAmenity(const std::string& amenity);
    bool isAvailable() const;
    std::string toString() const;
    bool isActive() const { return active; }
    void setActive(bool val) { active = val; }

    // setters for editing
    void setRoomNumber(int num) { roomNumber = num; }
    void setCategory(const std::string& cat) { category = cat; }
};