#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Passport.h"

class Client
{
private:
    int id;
    std::string firstName;
    std::string lastName;
    std::string phone;
    Passport passport;
public:
    Client(int id, const std::string& firstName, const std::string& lastName,
        const std::string& phone, const Passport& passport);
    std::string getFullName() const;
    int getId() const;
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getPhone() const { return phone; }
    Passport getPassport() const { return passport; }
};

#endif // CLIENT_H