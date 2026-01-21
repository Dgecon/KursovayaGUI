#include "Booking.h"
#include <sstream>

Booking::Booking(int bookingId, int roomId, const std::vector<int>& clientIds, const Date& checkIn, const Date& checkOut, bool active)
 : id(bookingId), roomId(roomId), clientIds(clientIds), checkInDate(checkIn), checkOutDate(checkOut),
 status(BookingStatus::CONFIRMED), totalPrice(Money::FromKopeks(0)), active(active)
{
}

int Booking::getId() const { return id; }
int Booking::getRoomId() const { return roomId; }
std::vector<int> Booking::getClientIds() const { return clientIds; }
Date Booking::getCheckInDate() const { return checkInDate; }
Date Booking::getCheckOutDate() const { return checkOutDate; }
BookingStatus Booking::getStatus() const { return status; }
Money Booking::getTotalPrice() const { return totalPrice; }
double Booking::getTotalPriceDouble() const { return totalPrice.ToDouble(); }

bool Booking::canTransition(BookingStatus newStatus) const {
 if (status == newStatus) return true;
 switch (status) {
 case BookingStatus::CONFIRMED:
 return newStatus == BookingStatus::CHECKED_IN || newStatus == BookingStatus::CANCELLED;
 case BookingStatus::CHECKED_IN:
 return newStatus == BookingStatus::COMPLETED || newStatus == BookingStatus::CANCELLED;
 case BookingStatus::COMPLETED:
 case BookingStatus::CANCELLED:
 return false;
 default:
 return false;
 }
}

bool Booking::setStatus(BookingStatus newStatus) {
 if (!canTransition(newStatus)) return false;
 status = newStatus;
 return true;
}

void Booking::setTotalPrice(Money p) { totalPrice = p; }
void Booking::setTotalPrice(double p)
{
 const auto kopeks = static_cast<Money::storage_type>(p *100.0 + (p >=0 ?0.5 : -0.5));
 totalPrice = Money::FromKopeks(kopeks);
}

bool Booking::isActive() const { return active; }
void Booking::setActive(bool v) { active = v; }

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

bool Booking::overlapsWith(const Booking& other) const {
 if (roomId != other.roomId) return false;
 return datesOverlap(checkInDate, checkOutDate, other.checkInDate, other.checkOutDate);
}

bool Booking::datesOverlap(const Date& a1, const Date& a2, const Date& b1, const Date& b2) {
 if (!a1.isValid() || !a2.isValid() || !b1.isValid() || !b2.isValid()) return false;
 if (!a1.isBefore(a2) && !a1.equals(a2)) return false;
 if (!b1.isBefore(b2) && !b1.equals(b2)) return false;
 return a1.isBefore(b2) && b1.isBefore(a2);
}