#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Passport.h"

class Client
{
private:
    int id;
    std::string firstName;
    std::string patronymic;
    std::string lastName;
    std::string phone;
    Passport passport;
    bool active = true;

    // new fields
    bool isChild = false;
    bool isForeigner = false;
    std::string birthCertificate; // for children
    std::string visa; // for foreigners
    std::string internationalPassport; // for foreigners
public:
    Client(int id, const std::string& firstName, const std::string& lastName,
        const std::string& phone, const Passport& passport, bool active = true,
        bool isChild = false, bool isForeigner = false,
        const std::string& birthCertificate = std::string(),
        const std::string& visa = std::string(),
        const std::string& internationalPassport = std::string(),
        const std::string& patronymic = std::string());
    std::string getFullName() const;
    int getId() const;
    std::string getFirstName() const { return firstName; }
    std::string getPatronymic() const;
    std::string getLastName() const { return lastName; }
    std::string getPhone() const { return phone; }
    const Passport& getPassport() const { return passport; }

    // setters for editing
    void setFirstName(const std::string& fn) { firstName = fn; }
    void setPatronymic(const std::string& p) { patronymic = p; }
    void setLastName(const std::string& ln) { lastName = ln; }
    void setPhone(const std::string& ph) { phone = ph; }
    void setPassport(const Passport& p) { passport = p; }

    bool isActive() const { return active; }
    void setActive(bool val) { active = val; }

    // new getters/setters
    bool getIsChild() const { return isChild; }
    void setIsChild(bool v) { isChild = v; }

    bool getIsForeigner() const { return isForeigner; }
    void setIsForeigner(bool v) { isForeigner = v; }

    const std::string& getBirthCertificate() const { return birthCertificate; }
    void setBirthCertificate(const std::string& s) { birthCertificate = s; }

    const std::string& getVisa() const { return visa; }
    void setVisa(const std::string& s) { visa = s; }

    const std::string& getInternationalPassport() const { return internationalPassport; }
    void setInternationalPassport(const std::string& s) { internationalPassport = s; }
};

#endif // CLIENT_H