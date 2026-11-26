#pragma once
#include "Contact.h"
#include <cstddef>
#include <string>
#include <vector>

class ContactManager {
private:
    std::vector<Contact> contacts_;

public:
    void add(Contact contact);
    std::size_t size() const;
    Contact& at(std::size_t index);
    const std::vector<Contact>& all() const;
    void replaceAll(std::vector<Contact> contacts);
    bool remove(std::size_t index);
    void sortBy(int field);
    std::vector<std::size_t> search(const std::string& query) const;
};
