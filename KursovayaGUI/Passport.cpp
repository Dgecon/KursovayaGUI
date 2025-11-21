#include "Passport.h"

Passport::Passport()
    : series(0), number(0), givenBy(""), dateOfIssue(), code(""), fio(""), dateOfBirth() {
}

Passport::Passport(int ser, int num, const std::string& given, const Date& issueDate,
    const std::string& c, const std::string& fullName, const Date& birthDate)
    : series(ser), number(num), givenBy(given), dateOfIssue(issueDate),
      code(c), fio(fullName), dateOfBirth(birthDate) {
}

std::string Passport::toString() const {
    return "Серия паспорта: " + std::to_string(series) +
           ", Номер: " + std::to_string(number) +
           ", Кем выдан: " + givenBy +
           ", Дата выдачи: " + dateOfIssue.toString() +
           ", Код подразделения: " + code +
           ", ФИО: " + fio +
           ", Дата рождения: " + dateOfBirth.toString();
}
int Passport::getSeries() const {
    return series;
}
int Passport::getNumber() const {
    return number;
}   
std::string Passport::getGivenBy() const {
   return givenBy;
}
Date Passport::getDateOfIssue() const {
    return dateOfIssue;
}
std::string Passport::getCode() const {
    return code;
}
std::string Passport::getFio() const {
    return fio;
}
Date Passport::getDateOfBirth() const {
    return dateOfBirth;
}
void Passport::setSeries(int ser) {
    series = ser;
}
void Passport::setNumber(int num) {
    number = num;
}
void Passport::setGivenBy(const std::string& given) {
    givenBy = given;
}
void Passport::setDateOfIssue(const Date& issueDate) {
    dateOfIssue = issueDate;
}
void Passport::setCode(const std::string& c) {
    code = c;
}
void Passport::setFio(const std::string& fullName) {
    fio = fullName;
}
void Passport::setDateOfBirth(const Date& birthDate) {
    dateOfBirth = birthDate;
}
