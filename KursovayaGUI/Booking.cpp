#include "Date.h"
#include "Booking.h"
#include <string>
#include <sstream>

Booking::Booking(int bookingId, int rId, int cId, const Date& checkIn, const Date& checkOut)
    : id(bookingId), roomId(rId), clientId(cId), checkInDate(checkIn),
    checkOutDate(checkOut), status(BookingStatus::CONFIRMED), totalPrice(0.0)
{
    // totalPrice будет установлен позже, когда будем знать цену комнаты
}

int Booking::getId() const {
    return id;
}
int Booking::getRoomId() const {
    return roomId;
}
int Booking::getClientId() const {
    return clientId;
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
    ss << "Бронирование #" << id << ": комната(" << roomId << "), клиент("
        << clientId << "), " << checkInDate.toString() << " - " << checkOutDate.toString();
    return ss.str();
}