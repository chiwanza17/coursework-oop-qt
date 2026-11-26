#pragma once
#include <string>

class ContactValidator {
public:
    static std::string trim(const std::string& value);
    static std::string normalizeEmail(std::string value);
    static bool validName(const std::string& value);
    static bool validEmail(const std::string& value);
    static bool validPhone(const std::string& value);
    static bool validBirthDate(const std::string& value);
};
