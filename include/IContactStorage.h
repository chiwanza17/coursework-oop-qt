#pragma once

class ContactManager;

class IContactStorage {
public:
    virtual ~IContactStorage() = default;
    virtual bool save(const ContactManager& manager) const = 0;
    virtual bool load(ContactManager& manager) const = 0;
};
