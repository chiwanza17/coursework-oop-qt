#include "Contact.h"
#include <utility>

Contact::Contact(std::string firstName, std::string lastName,
                 std::string email, PhoneNumber firstPhone)
    : firstName_(std::move(firstName)), lastName_(std::move(lastName)),
      email_(std::move(email)), phoneNumbers_{std::move(firstPhone)} {}

const std::string& Contact::getFirstName() const { return firstName_; }
const std::string& Contact::getLastName() const { return lastName_; }
const std::string& Contact::getPatronymic() const { return patronymic_; }
const std::string& Contact::getAddress() const { return address_; }
const std::string& Contact::getBirthDate() const { return birthDate_; }
const std::string& Contact::getEmail() const { return email_; }
const std::vector<PhoneNumber>& Contact::getPhoneNumbers() const { return phoneNumbers_; }

void Contact::setFirstName(std::string value) { firstName_ = std::move(value); }
void Contact::setLastName(std::string value) { lastName_ = std::move(value); }
void Contact::setPatronymic(std::string value) { patronymic_ = std::move(value); }
void Contact::setAddress(std::string value) { address_ = std::move(value); }
void Contact::setBirthDate(std::string value) { birthDate_ = std::move(value); }
void Contact::setEmail(std::string value) { email_ = std::move(value); }
void Contact::addPhoneNumber(PhoneNumber value) { phoneNumbers_.push_back(std::move(value)); }
void Contact::replacePhoneNumber(std::size_t index, PhoneNumber value) {
    phoneNumbers_.at(index) = std::move(value);
}
bool Contact::removePhoneNumber(std::size_t index) {
    if (index >= phoneNumbers_.size() || phoneNumbers_.size() == 1) return false;
    phoneNumbers_.erase(phoneNumbers_.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}
