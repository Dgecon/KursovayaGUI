#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include <vector>
#include "Date.h"
#include "BookingStatus.h"

class Booking {
private:
    int id;
    int roomId;
    std::vector<int> clientIds;
    Date checkInDate;
    Date checkOutDate;
    BookingStatus status;
    double totalPrice = 0.0;
    bool active = true;

public:
    Booking() = default;
    Booking(int bookingId, int roomId, const std::vector<int>& clientIds, const Date& checkIn, const Date& checkOut, bool active = true);

    int getId() const;
    int getRoomId() const;
    std::vector<int> getClientIds() const;
    Date getCheckInDate() const;
    Date getCheckOutDate() const;
    BookingStatus getStatus() const;
    double getTotalPrice() const;

    void setStatus(BookingStatus newStatus);
    void setTotalPrice(double p);
    bool isActive() const;
    void setActive(bool v);

    std::string toString() const;

    // compatibility helpers
    bool overlapsWith(const Booking& other) const;
    static bool datesOverlap(const Date& a1, const Date& a2, const Date& b1, const Date& b2);
};

#endif // BOOKING_H