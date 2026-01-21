#include "Room.h"
#include <sstream>

static Money MoneyFromDoubleRounded(double v)
{
 const auto kopeks = static_cast<Money::storage_type>(v *100.0 + (v >=0 ?0.5 : -0.5));
 return Money::FromKopeks(kopeks);
}

Room::Room(int id, int roomNumber, const std::string& category, Money pricePerNight,
 RoomStatus status, const std::vector<std::string>& amenities, bool active)
 : id(id), roomNumber(roomNumber), category(category), pricePerNight(pricePerNight),
 status(status), amenities(amenities), active(active)
{
}

Room::Room(int id, int roomNumber, const std::string& category, double pricePerNight,
 RoomStatus status, const std::vector<std::string>& amenities, bool active)
 : Room(id, roomNumber, category, MoneyFromDoubleRounded(pricePerNight), status, amenities, active)
{
}

int Room::getRoomNumber() const { return roomNumber; }
std::string Room::getCategory() const { return category; }
Money Room::getPricePerNight() const { return pricePerNight; }
double Room::getPricePerNightDouble() const { return pricePerNight.ToDouble(); }
RoomStatus Room::getStatus() const { return status; }
std::vector<std::string> Room::getAmenities() const { return amenities; }

void Room::setPricePerNight(Money price)
{
 if (price.ToKopeks() >0) {
 pricePerNight = price;
 }
}

void Room::setPricePerNight(double price)
{
 setPricePerNight(MoneyFromDoubleRounded(price));
}

bool Room::canTransition(RoomStatus newStatus) const {
 if (status == newStatus) return true;
 switch (status) {
 case RoomStatus::AVAILABLE:
 return newStatus == RoomStatus::BOOKED || newStatus == RoomStatus::MAINTENANCE;
 case RoomStatus::BOOKED:
 return newStatus == RoomStatus::OCCUPIED || newStatus == RoomStatus::AVAILABLE || newStatus == RoomStatus::MAINTENANCE;
 case RoomStatus::OCCUPIED:
 return newStatus == RoomStatus::CLEANING || newStatus == RoomStatus::MAINTENANCE;
 case RoomStatus::CLEANING:
 return newStatus == RoomStatus::AVAILABLE || newStatus == RoomStatus::MAINTENANCE;
 case RoomStatus::MAINTENANCE:
 return newStatus == RoomStatus::AVAILABLE;
 default:
 return false;
 }
}

bool Room::setStatus(RoomStatus newStatus) {
 if (!canTransition(newStatus)) return false;
 status = newStatus;
 return true;
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
 << ", Цена:" << pricePerNight.ToString() << ", Статус:" << RoomStatusToString(status);
 return ss.str();
}