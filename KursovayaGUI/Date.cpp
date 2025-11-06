#include "Date.h"

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
    if (y >= 2020 && y <= 2030) year = y;
}

bool Date::isValid() const {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
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
    int days1 = year * 365 + month * 30 + day;
    int days2 = other.year * 365 + other.month * 30 + other.day;
    return days1 - days2;
}