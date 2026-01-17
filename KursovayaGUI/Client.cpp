#include "Client.h"
#include <string>

Client::Client(int id, const std::string& firstName, const std::string& lastName, const std::string& phone, const Passport& passport, bool active,
	bool isChild, bool isForeigner, const std::string& birthCertificate, const std::string& visa, const std::string& internationalPassport, const std::string& patronymic)
	: id(id), firstName(firstName), patronymic(patronymic), lastName(lastName), phone(phone), passport(passport), active(active), isChild(isChild), isForeigner(isForeigner), birthCertificate(birthCertificate), visa(visa), internationalPassport(internationalPassport)
{
}

std::string Client::getFullName() const {
	if (!patronymic.empty()) return firstName + " " + patronymic + " " + lastName;
	return firstName + " " + lastName;
}

int Client::getId() const {
	return id;
}

std::string Client::getPatronymic() const {
	return patronymic;
}
