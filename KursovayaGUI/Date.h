#ifndef DATE_H
#define DATE_H

#include <string>
class Date {
private:
    int day;
    int month;
    int year;

public:
    Date(int d = 1, int m = 1, int y = 2024);

    int getDay() const;
    int getMonth() const;
    int getYear() const;

    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);

    bool isValid() const;
    std::string toString() const;

    bool isAfter(const Date& other) const;
    bool isBefore(const Date& other) const;
    bool equals(const Date& other) const;
    int DifferenceInDays(const Date& other) const;
};

#endif

