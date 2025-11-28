#include "Date.h"
#include "Booking.h"
#include <string>
#include <sstream>

Booking::Booking(int bookingId, int rId, const std::vector<int>& cIds, const Date& checkIn, const Date& checkOut, bool active)
    : id(bookingId), roomId(rId), clientIds(cIds), checkInDate(checkIn),
    checkOutDate(checkOut), status(BookingStatus::CONFIRMED), totalPrice(0.0), active(active)
{
}

int Booking::getId() const {
    return id;
}
int Booking::getRoomId() const {
    return roomId;
}
std::vector<int> Booking::getClientIds() const {
    return clientIds;
}
Date Booking::getCheckInDate() const {
    return checkInDate;
}
Date Booking::getCheckOutDate() const {
    return checkOutDate;
}
BookingStatus Booking::getStatus() const {
    return status;
}
double Booking::getTotalPrice() const {
    return totalPrice;
}
void Booking::setStatus(BookingStatus newStatus) {
    status = newStatus;
}

std::string Booking::toString() const {
    std::stringstream ss;
    ss << "Бронирование #" << id << ": комната(" << roomId << "), клиенты(";
    for (size_t i =0; i < clientIds.size(); ++i) {
        ss << clientIds[i];
        if (i +1 < clientIds.size()) ss << ",";
    }
    ss << "), " << checkInDate.toString() << " - " << checkOutDate.toString();
    return ss.str();
}