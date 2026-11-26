#pragma once
#include <string>

enum class PhoneType { Home = 1, Work = 2, Service = 3 };

class PhoneNumber {
private:
    std::string number_;
    PhoneType type_;

public:
    PhoneNumber(std::string number, PhoneType type);
    const std::string& getNumber() const;
    PhoneType getType() const;
};

std::string phoneTypeName(PhoneType type);
