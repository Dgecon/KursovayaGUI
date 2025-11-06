#ifndef BOOKING_H
#define BOOKING_H

#include "Date.h"
#include "BookingStatus.h"
#include <string>

class Booking
{
private:
    int id;
    int roomId;     // ID комнаты вместо указателя
    int clientId;   // ID клиента вместо указателя
    Date checkInDate;
    Date checkOutDate;
    BookingStatus status;
    double totalPrice;

public:
    Booking(int bookingId, int roomId, int clientId, const Date& checkIn, const Date& checkOut);

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
};

#endif // BOOKING_H