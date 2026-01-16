#include "Client.h"
#include <string>

Client::Client(int id, const std::string& firstName, const std::string& lastName, const std::string& phone, const Passport& passport, bool active,
	bool isChild, bool isForeigner, const std::string& birthCertificate, const std::string& visa, const std::string& internationalPassport)
	: id(id), firstName(firstName), lastName(lastName), phone(phone), passport(passport), active(active), isChild(isChild), isForeigner(isForeigner), birthCertificate(birthCertificate), visa(visa), internationalPassport(internationalPassport)
{
}

std::string Client::getFullName() const {
	return firstName + " " + lastName;
}

int Client::getId() const {
	return id;
}
