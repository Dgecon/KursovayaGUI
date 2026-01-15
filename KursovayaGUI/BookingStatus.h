#pragma once

#include <cstdint>
#include <string>
#include <ostream>
#include <functional>

enum class BookingStatus : uint8_t
{
 UNKNOWN =0,
 CONFIRMED,
 CHECKED_IN,
 COMPLETED,
 CANCELLED
};

std::string to_string(BookingStatus s);
BookingStatus booking_status_from_string(const std::string& s);
bool is_finalized(BookingStatus s);

std::ostream& operator<<(std::ostream& os, BookingStatus s);

namespace std {
 template<>
 struct hash<BookingStatus>
 {
 size_t operator()(BookingStatus s) const noexcept
 {
 return std::hash<uint8_t>()(static_cast<uint8_t>(s));
 }
 };
}

