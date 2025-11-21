#pragma once
#include <string>
#include "Date.h"
class Passport
{
public:
    int series;
    int number;
    std::string givenBy; 
    Date dateOfIssue;
    std::string code;
    std::string fio;
	Date dateOfBirth;
	std::string toString() const;  
    Passport();
    Passport(int ser, int num, const std::string& given, const Date& issueDate,
		const std::string& c, const std::string& fullName, const Date& birthDate);
    int getSeries() const;
    int getNumber() const;   
    std::string getGivenBy() const;
    Date getDateOfIssue() const;
    std::string getCode() const;
    std::string getFio() const;
    Date getDateOfBirth() const;
    void setSeries(int ser);
    void setNumber(int num);
    void setGivenBy(const std::string& given);
    void setDateOfIssue(const Date& issueDate);
    void setCode(const std::string& c);
	void setFio(const std::string& fullName);
	void setDateOfBirth(const Date& birthDate);
};

