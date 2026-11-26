#include "FileStorage.h"
#include "ContactManager.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <utility>
#include <vector>

FileStorage::FileStorage(std::string filePath) : filePath_(std::move(filePath)) {}

bool FileStorage::save(const ContactManager& manager) const {
    try {
        auto parent = std::filesystem::path(filePath_).parent_path();
        if (!parent.empty()) std::filesystem::create_directories(parent);
    } catch (...) {
        return false;
    }

    std::ofstream out(filePath_, std::ios::trunc);
    if (!out) return false;
    out << manager.size() << '\n';
    for (const auto& contact : manager.all()) {
        out << std::quoted(contact.getFirstName()) << ' '
            << std::quoted(contact.getLastName()) << ' '
            << std::quoted(contact.getPatronymic()) << ' '
            << std::quoted(contact.getAddress()) << ' '
            << std::quoted(contact.getBirthDate()) << ' '
            << std::quoted(contact.getEmail()) << ' '
            << contact.getPhoneNumbers().size() << '\n';
        for (const auto& phone : contact.getPhoneNumbers()) {
            out << std::quoted(phone.getNumber()) << ' '
                << static_cast<int>(phone.getType()) << '\n';
        }
    }
    return static_cast<bool>(out);
}

bool FileStorage::load(ContactManager& manager) const {
    std::ifstream in(filePath_);
    if (!in) return !std::filesystem::exists(filePath_);

    std::size_t count = 0;
    if (!(in >> count) || count > 100000) return false;

    std::vector<Contact> loaded;
    for (std::size_t i = 0; i < count; ++i) {
        std::string first, last, patronymic, address, birthday, email;
        std::size_t numberCount = 0;
        if (!(in >> std::quoted(first) >> std::quoted(last) >> std::quoted(patronymic)
                 >> std::quoted(address) >> std::quoted(birthday) >> std::quoted(email)
                 >> numberCount) || numberCount == 0 || numberCount > 10000)
            return false;

        std::string number;
        int type = 0;
        if (!(in >> std::quoted(number) >> type) || type < 1 || type > 3) return false;
        Contact contact(first, last, email, PhoneNumber(number, static_cast<PhoneType>(type)));
        contact.setPatronymic(patronymic);
        contact.setAddress(address);
        contact.setBirthDate(birthday);

        for (std::size_t j = 1; j < numberCount; ++j) {
            if (!(in >> std::quoted(number) >> type) || type < 1 || type > 3) return false;
            contact.addPhoneNumber(PhoneNumber(number, static_cast<PhoneType>(type)));
        }
        loaded.push_back(std::move(contact));
    }

    manager.replaceAll(std::move(loaded));
    return true;
}
