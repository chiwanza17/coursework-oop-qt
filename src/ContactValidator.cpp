#include "ContactValidator.h"
#include <algorithm>
#include <cctype>
#include <codecvt>
#include <ctime>
#include <locale>
#include <regex>

std::string ContactValidator::trim(const std::string& value) {
    auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

std::string ContactValidator::normalizeEmail(std::string value) {
    value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }), value.end());
    return value;
}

bool ContactValidator::validName(const std::string& value) {
    if (value.empty()) return false;
    try {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        auto wide = converter.from_bytes(value);
        // Includes Latin letters, Cyrillic letters, numbers, spaces and hyphens.
        static const std::wregex pattern(LR"(^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9 -]*$)");
        return std::regex_match(wide, pattern) && wide.back() != L'-';
    } catch (...) {
        return false;
    }
}

bool ContactValidator::validEmail(const std::string& value) {
    static const std::regex pattern(R"(^[A-Za-z0-9]+@[A-Za-z0-9]+(\.[A-Za-z0-9]+)*$)");
    return std::regex_match(value, pattern);
}

bool ContactValidator::validPhone(const std::string& value) {
    static const std::regex pattern(
        R"(^(\+7|8)([0-9]{10}|\([0-9]{3}\)[0-9]{7}|\([0-9]{3}\)[0-9]{3}-[0-9]{2}-[0-9]{2})$)");
    return std::regex_match(value, pattern);
}

bool ContactValidator::validBirthDate(const std::string& value) {
    if (value.empty()) return true;
    static const std::regex pattern(R"(^([0-9]{4})-([0-9]{2})-([0-9]{2})$)");
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) return false;
    int year = std::stoi(match[1]);
    int month = std::stoi(match[2]);
    int day = std::stoi(match[3]);
    if (year < 1 || month < 1 || month > 12) return false;
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) days[1] = 29;
    if (day < 1 || day > days[month - 1]) return false;
    std::time_t now = std::time(nullptr);
    std::tm today{};
#ifdef _WIN32
    localtime_s(&today, &now);
#else
    localtime_r(&now, &today);
#endif
    int todayAsNumber = (today.tm_year + 1900) * 10000 + (today.tm_mon + 1) * 100 + today.tm_mday;
    return year * 10000 + month * 100 + day < todayAsNumber;
}
