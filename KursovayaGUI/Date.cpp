#include "Date.h"

// Helper functions based on Howard Hinnant's algorithms for civil date conversions
// Suitable for proleptic Gregorian calendar

namespace {
    // returns days since civil1970-01-01
    long days_from_civil(int y, unsigned m, unsigned d) {
        y -= m <=2;
        const long era = (y >=0 ? y : y-399) /400;
        const unsigned yoe = static_cast<unsigned>(y - era *400); // [0,399]
        const unsigned doy = (153*(m + (m >2 ? -3 :9)) +2)/5 + d-1; // [0,365]
        const unsigned doe = yoe *365 + yoe/4 - yoe/100 + doy; // [0,146096]
        return era *146097 + static_cast<long>(doe) -719468;
    }

    bool is_leap_year(int y) {
        return (y%4==0 && y%100!=0) || (y%400==0);
    }

    unsigned days_in_month(int y, unsigned m) {
        static const unsigned mdays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        if (m==2 && is_leap_year(y)) return 29;
        return mdays[m];
    }
}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

int Date::getDay() const { return day; }
int Date::getMonth() const { return month; }
int Date::getYear() const { return year; }

void Date::setDay(int d) {
    if (d >= 1 && d <= 31) day = d;
}

void Date::setMonth(int m) {
    if (m >= 1 && m <= 12) month = m;
}

void Date::setYear(int y) {
    year = y;
}

bool Date::isValid() const {
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;
    unsigned dim = days_in_month(year, static_cast<unsigned>(month));
    if (day > static_cast<int>(dim)) return false;
    return true;
}

std::string Date::toString() const {
    return std::to_string(day) + "." + std::to_string(month) + "." + std::to_string(year);
}

bool Date::isAfter(const Date& other) const {
    if (year != other.year) return year > other.year;
    if (month != other.month) return month > other.month;
    return day > other.day;
}

bool Date::isBefore(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::equals(const Date& other) const {
    return (day == other.day) && (month == other.month) && (year == other.year);
}

int Date::DifferenceInDays(const Date& other) const {
    long a = days_from_civil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
    long b = days_from_civil(other.year, static_cast<unsigned>(other.month), static_cast<unsigned>(other.day));
    long diff = a - b;
    if (diff > INT32_MAX) return INT32_MAX;
    if (diff < INT32_MIN) return INT32_MIN;
    return static_cast<int>(diff);
}