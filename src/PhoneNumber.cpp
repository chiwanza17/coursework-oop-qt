#include "PhoneNumber.h"
#include <utility>

PhoneNumber::PhoneNumber(std::string number, PhoneType type)
    : number_(std::move(number)), type_(type) {}

const std::string& PhoneNumber::getNumber() const { return number_; }
PhoneType PhoneNumber::getType() const { return type_; }

std::string phoneTypeName(PhoneType type) {
    switch (type) {
        case PhoneType::Home: return "Home";
        case PhoneType::Work: return "Work";
        case PhoneType::Service: return "Service";
    }
    return "Unknown";
}
