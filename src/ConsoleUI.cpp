#include "ConsoleUI.h"
#include "ContactValidator.h"
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace {
std::string lowerAscii(std::string text) {
    for (char& ch : text)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return text;
}
}

ConsoleUI::ConsoleUI(const IContactStorage& storage) : storage_(storage) {}

std::string ConsoleUI::readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string text;
    if (!std::getline(std::cin, text)) throw std::runtime_error("Input closed");
    return ContactValidator::trim(text);
}

int ConsoleUI::readInt(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        auto input = readLine(prompt);
        try {
            std::size_t parsed = 0;
            int number = std::stoi(input, &parsed);
            if (parsed == input.size() && number >= minimum && number <= maximum) return number;
        } catch (...) {}
        std::cout << "Invalid choice. Try again.\n";
    }
}

std::string ConsoleUI::askName(const std::string& prompt, bool required) {
    while (true) {
        auto text = readLine(prompt);
        if ((!required && text.empty()) || ContactValidator::validName(text)) return text;
        std::cout << "Invalid name: start with a letter; use letters, digits, spaces or hyphens.\n";
    }
}

std::string ConsoleUI::askEmail() {
    while (true) {
        auto email = ContactValidator::normalizeEmail(readLine("Email: "));
        if (ContactValidator::validEmail(email)) return email;
        std::cout << "Invalid email. Example: anna@gmail.com\n";
    }
}

std::string ConsoleUI::askBirthDate() {
    while (true) {
        auto date = readLine("Birth date (YYYY-MM-DD, optional): ");
        if (ContactValidator::validBirthDate(date)) return date;
        std::cout << "Invalid birth date. Enter a real date earlier than today.\n";
    }
}

PhoneNumber ConsoleUI::askPhone() {
    std::string number;
    while (true) {
        number = readLine("Phone (+78121234567 or 8(812)123-45-67): ");
        if (ContactValidator::validPhone(number)) break;
        std::cout << "Invalid phone format.\n";
    }
    int type = readInt("Type (1 Home, 2 Work, 3 Service): ", 1, 3);
    return PhoneNumber(number, static_cast<PhoneType>(type));
}

void ConsoleUI::printContact(const Contact& c, std::size_t index) const {
    std::cout << "\n[" << index + 1 << "] " << c.getFirstName() << " " << c.getLastName() << '\n';
    std::cout << "  Patronymic: " << c.getPatronymic()
              << "\n  Address: " << c.getAddress()
              << "\n  Birth date: " << c.getBirthDate()
              << "\n  Email: " << c.getEmail() << '\n';
    for (std::size_t i = 0; i < c.getPhoneNumbers().size(); ++i) {
        const auto& p = c.getPhoneNumbers()[i];
        std::cout << "  Phone " << i + 1 << " (" << phoneTypeName(p.getType())
                  << "): " << p.getNumber() << '\n';
    }
}

void ConsoleUI::printAll() const {
    if (manager_.size() == 0) std::cout << "No contacts found.\n";
    for (std::size_t i = 0; i < manager_.size(); ++i)
        printContact(manager_.all()[i], i);
}

void ConsoleUI::addContact() {
    std::cout << "\n--- New contact ---\n";
    auto first = askName("First name: ");
    auto last = askName("Last name: ");
    auto patronymic = askName("Patronymic (optional): ", false);
    auto address = readLine("Address (optional): ");
    auto birthday = askBirthDate();
    auto email = askEmail();
    Contact contact(first, last, email, askPhone());
    contact.setPatronymic(patronymic);
    contact.setAddress(address);
    contact.setBirthDate(birthday);
    manager_.add(std::move(contact));
    std::cout << "Contact added.\n";
}

void ConsoleUI::editPhones(Contact& contact) {
    while (true) {
        for (std::size_t i = 0; i < contact.getPhoneNumbers().size(); ++i) {
            const auto& phone = contact.getPhoneNumbers()[i];
            std::cout << i + 1 << ". " << phone.getNumber()
                      << " (" << phoneTypeName(phone.getType()) << ")\n";
        }
        int choice = readInt("Phone menu: 1 Add, 2 Replace, 3 Delete, 0 Back: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) {
            contact.addPhoneNumber(askPhone());
            continue;
        }
        int index = readInt("Phone position: ", 1,
                            static_cast<int>(contact.getPhoneNumbers().size())) - 1;
        if (choice == 2) contact.replacePhoneNumber(index, askPhone());
        if (choice == 3 && !contact.removePhoneNumber(index))
            std::cout << "A contact must have at least one phone number.\n";
    }
}

bool ConsoleUI::editContact() {
    if (manager_.size() == 0) { std::cout << "No contacts to edit.\n"; return false; }
    printAll();
    int index = readInt("Contact number to edit: ", 1, static_cast<int>(manager_.size())) - 1;
    Contact& contact = manager_.at(index);
    bool changed = false;
    while (true) {
        std::cout << "\n1 First name  2 Last name  3 Patronymic  4 Address\n"
                     "5 Birth date  6 Email  7 Phones  0 Done\n";
        int option = readInt("Field: ", 0, 7);
        if (option == 0) return changed;
        switch (option) {
            case 1: contact.setFirstName(askName("First name: ")); break;
            case 2: contact.setLastName(askName("Last name: ")); break;
            case 3: contact.setPatronymic(askName("Patronymic (optional): ", false)); break;
            case 4: contact.setAddress(readLine("Address (optional): ")); break;
            case 5: contact.setBirthDate(askBirthDate()); break;
            case 6: contact.setEmail(askEmail()); break;
            case 7: editPhones(contact); break;
        }
        changed = true;
        std::cout << "Updated.\n";
    }
}

bool ConsoleUI::deleteContact() {
    if (manager_.size() == 0) { std::cout << "No contacts to delete.\n"; return false; }
    printAll();
    int index = readInt("Contact number to delete: ", 1, static_cast<int>(manager_.size())) - 1;
    if (lowerAscii(readLine("Confirm delete? (y/n): ")) != "y") return false;
    manager_.remove(index);
    std::cout << "Contact deleted.\n";
    return true;
}

void ConsoleUI::searchContacts() const {
    auto query = readLine("Search text (name, email, address, phone, etc.): ");
    auto matches = manager_.search(query);
    for (std::size_t index : matches) printContact(manager_.all()[index], index);
    if (matches.empty()) std::cout << "No matches.\n";
}

void ConsoleUI::sortContacts() {
    std::cout << "1 First name 2 Last name 3 Patronymic 4 Email\n"
                 "5 Birth date 6 Address 7 First phone\n";
    manager_.sortBy(readInt("Sort by: ", 1, 7));
    printAll();
}

void ConsoleUI::run() {
    if (!storage_.load(manager_)) {
        std::cerr << "Could not load contact data. Fix or back up data/contacts.txt before continuing.\n";
        return;
    }
    while (true) {
        std::cout << "\n=== TELEPHONE DIRECTORY ===\n"
                     "1 Add contact\n2 Show contacts\n3 Edit contact\n4 Delete contact\n"
                     "5 Search contacts\n6 Sort contacts\n7 Save\n0 Exit\n";
        int choice;
        try {
            choice = readInt("Choose: ", 0, 7);
        } catch (const std::runtime_error&) {
            if (!storage_.save(manager_)) std::cerr << "Error saving contacts.\n";
            return;
        }
        bool changed = false;
        if (choice == 0) {
            if (!storage_.save(manager_)) std::cerr << "Error saving contacts.\n";
            return;
        }
        switch (choice) {
            case 1: addContact(); changed = true; break;
            case 2: printAll(); break;
            case 3: changed = editContact(); break;
            case 4: changed = deleteContact(); break;
            case 5: searchContacts(); break;
            case 6: sortContacts(); changed = true; break;
            case 7:
                if (storage_.save(manager_)) std::cout << "Saved to data/contacts.txt\n";
                else std::cout << "Error saving contacts.\n";
                break;
        }
        if (changed && !storage_.save(manager_))
            std::cerr << "Warning: changes were not saved to disk.\n";
    }
}
