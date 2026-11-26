#pragma once
#include "ContactManager.h"
#include "IContactStorage.h"
#include "PhoneNumber.h"
#include <cstddef>
#include <string>

class ConsoleUI {
private:
    ContactManager manager_;
    const IContactStorage& storage_;

    static std::string readLine(const std::string& prompt);
    static int readInt(const std::string& prompt, int minimum, int maximum);
    static std::string askName(const std::string& prompt, bool required = true);
    static std::string askEmail();
    static std::string askBirthDate();
    static PhoneNumber askPhone();

    void printContact(const Contact& contact, std::size_t index) const;
    void printAll() const;
    void addContact();
    bool editContact();
    void editPhones(Contact& contact);
    bool deleteContact();
    void searchContacts() const;
    void sortContacts();

public:
    explicit ConsoleUI(const IContactStorage& storage);
    void run();
};
