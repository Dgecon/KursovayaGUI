#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client
{
private:
    int id;
    std::string firstName;
    std::string lastName;
    std::string phone;
    std::string passport;
public:
    Client(int id, const std::string& firstName, const std::string& lastName,
        const std::string& phone, const std::string& passport);
    std::string getFullName() const;
    int getId() const;
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getPhone() const { return phone; }
    std::string getPassport() const { return passport; }
};

#endif // CLIENT_H