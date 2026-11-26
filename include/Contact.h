#pragma once
#include "PhoneNumber.h"
#include <cstddef>
#include <string>
#include <vector>

class Contact {
private:
    std::string firstName_;
    std::string lastName_;
    std::string patronymic_;
    std::string address_;
    std::string birthDate_;
    std::string email_;
    std::vector<PhoneNumber> phoneNumbers_;

public:
    Contact(std::string firstName, std::string lastName,
            std::string email, PhoneNumber firstPhone);

    const std::string& getFirstName() const;
    const std::string& getLastName() const;
    const std::string& getPatronymic() const;
    const std::string& getAddress() const;
    const std::string& getBirthDate() const;
    const std::string& getEmail() const;
    const std::vector<PhoneNumber>& getPhoneNumbers() const;

    void setFirstName(std::string value);
    void setLastName(std::string value);
    void setPatronymic(std::string value);
    void setAddress(std::string value);
    void setBirthDate(std::string value);
    void setEmail(std::string value);
    void addPhoneNumber(PhoneNumber value);
    void replacePhoneNumber(std::size_t index, PhoneNumber value);
    bool removePhoneNumber(std::size_t index);
};
