#ifndef BOOKING_H
#define BOOKING_H

#include "Date.h"
#include "BookingStatus.h"
#include <string>
#include <vector>

class Booking
{
private:
    int id;
    int roomId;                   // ID комнаты для бронирования
    std::vector<int> clientIds;   // IDs клиентов для бронирования (группа)
    Date checkInDate;
    Date checkOutDate;
    BookingStatus status;
    double totalPrice;
    bool active = true;

public:
    Booking(int bookingId, int roomId, const std::vector<int>& clientIds, const Date& checkIn, const Date& checkOut, bool active = true);

    int getId() const;
    int getRoomId() const;
    std::vector<int> getClientIds() const;
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