#include "Client.h"
#include <string>

Client::Client(int id, const std::string& firstName, const std::string& lastName, const std::string& phone, const Passport& passport)
    : id(id), firstName(firstName), lastName(lastName), phone(phone), passport(passport)
{
}

std::string Client::getFullName() const {
	return firstName + " " + lastName;
}

int Client::getId() const {
	return id;
}
