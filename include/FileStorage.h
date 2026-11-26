#pragma once
#include "IContactStorage.h"
#include <string>

class FileStorage : public IContactStorage {
private:
    std::string filePath_;

public:
    explicit FileStorage(std::string filePath = "data/contacts.txt");
    bool save(const ContactManager& manager) const override;
    bool load(ContactManager& manager) const override;
};
