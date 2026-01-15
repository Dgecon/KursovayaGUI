#include "BookingStatus.h"
#include <algorithm>

std::string to_string(BookingStatus s) {
    switch (s) {
        case BookingStatus::UNKNOWN: return "UNKNOWN";
        case BookingStatus::CONFIRMED: return "CONFIRMED";
        case BookingStatus::CHECKED_IN: return "CHECKED_IN";
        case BookingStatus::COMPLETED: return "COMPLETED";
        case BookingStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

BookingStatus booking_status_from_string(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "CONFIRMED") return BookingStatus::CONFIRMED;
    if (s == "CHECKED_IN" || s == "CHECKEDIN" || s == "CHECKED IN") return BookingStatus::CHECKED_IN;
    if (s == "COMPLETED") return BookingStatus::COMPLETED;
    if (s == "CANCELLED" || s == "CANCELED") return BookingStatus::CANCELLED;
    return BookingStatus::UNKNOWN;
}

bool is_finalized(BookingStatus s) {
    return s == BookingStatus::COMPLETED || s == BookingStatus::CANCELLED;
}

std::ostream& operator<<(std::ostream& os, BookingStatus s) {
    os << to_string(s);
    return os;
}
