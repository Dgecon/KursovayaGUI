#ifndef BOOKING_H
#define BOOKING_H

#include "Date.h"
#include "BookingStatus.h"
#include <string>

class Booking
{
private:
    int id;
    int roomId;     // ID комнаты для бронирования
    int clientId;   // ID клиента для бронирования
    Date checkInDate;
    Date checkOutDate;
    BookingStatus status;
    double totalPrice;
    bool active = true;

public:
    Booking(int bookingId, int roomId, int clientId, const Date& checkIn, const Date& checkOut, bool active = true);

    int getId() const;
    int getRoomId() const;
    int getClientId() const;
    Date getCheckInDate() const;
    Date getCheckOutDate() const;
    BookingStatus getStatus() const;
    double getTotalPrice() const;

    void setStatus(BookingStatus newStatus);
    void setTotalPrice(double price) { totalPrice = price; }
    std::string toString() const;
    bool isActive() const { return active; }
    void setActive(bool val) { active = val; }
};

#endif // BOOKING_H