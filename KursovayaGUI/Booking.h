#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include <vector>
#include "Date.h"
#include "BookingStatus.h"
#include "Money.h"

class Booking {
private:
    int id;
    int roomId;
    std::vector<int> clientIds;
    Date checkInDate;
    Date checkOutDate;
    BookingStatus status;
    Money totalPrice = Money::FromKopeks(0);
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

    Money getTotalPrice() const;
    double getTotalPriceDouble() const; // only for UI formatting/back-compat

    // Try to change booking status. Returns true if transition is allowed and applied.
    // Allowed transitions (typical):
    // CONFIRMED -> CHECKED_IN | CANCELLED
    // CHECKED_IN -> COMPLETED | CANCELLED
    // COMPLETED -> (final)
    // CANCELLED -> (final)
    bool setStatus(BookingStatus newStatus);
    bool canTransition(BookingStatus newStatus) const;

    void setTotalPrice(Money p);
    void setTotalPrice(double p);

    bool isActive() const;
    void setActive(bool v);

    std::string toString() const;

    // compatibility helpers
    bool overlapsWith(const Booking& other) const;
    static bool datesOverlap(const Date& a1, const Date& a2, const Date& b1, const Date& b2);
};

#endif // BOOKING_H