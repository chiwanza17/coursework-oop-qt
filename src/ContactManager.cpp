#include "ContactManager.h"
#include <algorithm>
#include <cctype>
#include <utility>

namespace {
std::string lowerAscii(std::string text) {
    for (char& ch : text)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return text;
}
}

void ContactManager::add(Contact contact) { contacts_.push_back(std::move(contact)); }
std::size_t ContactManager::size() const { return contacts_.size(); }
Contact& ContactManager::at(std::size_t index) { return contacts_.at(index); }
const std::vector<Contact>& ContactManager::all() const { return contacts_; }
void ContactManager::replaceAll(std::vector<Contact> contacts) { contacts_ = std::move(contacts); }

bool ContactManager::remove(std::size_t index) {
    if (index >= contacts_.size()) return false;
    contacts_.erase(contacts_.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

void ContactManager::sortBy(int field) {
    auto key = [field](const Contact& contact) -> std::string {
        switch (field) {
            case 1: return lowerAscii(contact.getFirstName());
            case 2: return lowerAscii(contact.getLastName());
            case 3: return lowerAscii(contact.getPatronymic());
            case 4: return lowerAscii(contact.getEmail());
            case 5: return contact.getBirthDate();
            case 6: return lowerAscii(contact.getAddress());
            default: return contact.getPhoneNumbers().front().getNumber();
        }
    };
    std::stable_sort(contacts_.begin(), contacts_.end(), [&](const Contact& a, const Contact& b) {
        return key(a) < key(b);
    });
}

std::vector<std::size_t> ContactManager::search(const std::string& query) const {
    std::vector<std::size_t> matches;
    const auto term = lowerAscii(query);
    for (std::size_t i = 0; i < contacts_.size(); ++i) {
        const auto& c = contacts_[i];
        auto text = c.getFirstName() + " " + c.getLastName() + " " + c.getPatronymic() + " "
                  + c.getAddress() + " " + c.getBirthDate() + " " + c.getEmail();
        for (const auto& phone : c.getPhoneNumbers()) text += " " + phone.getNumber();
        if (lowerAscii(text).find(term) != std::string::npos) matches.push_back(i);
    }
    return matches;
}
