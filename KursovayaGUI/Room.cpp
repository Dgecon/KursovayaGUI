#include "Room.h"
#include <sstream>

Room::Room(int id, int roomNumber, const std::string& category, double pricePerNight,
    RoomStatus status, const std::vector<std::string>& amenities, bool active)
    : id(id), roomNumber(roomNumber), category(category), pricePerNight(pricePerNight),
    status(status), amenities(amenities), active(active)
{
}

int Room::getRoomNumber() const { return roomNumber; }
std::string Room::getCategory() const { return category; }
double Room::getPricePerNight() const { return pricePerNight; }
RoomStatus Room::getStatus() const { return status; }
std::vector<std::string> Room::getAmenities() const { return amenities; }

void Room::setPricePerNight(double price) {
    if (price > 0) {
        pricePerNight = price;
    }
}

void Room::setStatus(RoomStatus newStatus) {
    status = newStatus;
}

void Room::addAmenity(const std::string& amenity) {
    amenities.push_back(amenity);
}

bool Room::isAvailable() const {
    return status == RoomStatus::AVAILABLE;
}

std::string Room::toString() const {
    std::stringstream ss;
    ss << "ID:" << id << ", Номер:" << roomNumber << ", Категория:" << category
        << ", Цена:" << pricePerNight << ", Статус:" << RoomStatusToString(status);
    return ss.str();
}