#include "ContactValidator.h"
#include <iostream>

int main() {
    const char* accepted[] = {
        "+78121234567",
        "88121234567",
        "+7(812)1234567",
        "8(812)1234567",
        "+7(812)123-45-67",
        "8(812)123-45-67",
    };
    const char* rejected[] = {
        "", "+18121234567", "78121234567", "+88121234567",
        "+7812123456", "881212345678", "+7812 1234567",
        "+7(81)1234567", "8(8121234567", "8(812)1234-5-67",
        "+7(812)123456-7", "+7812123456x", "8(812)123-45-67 ext 1",
    };

    bool passed = true;
    for (const char* number : accepted) {
        if (!ContactValidator::validPhone(number)) {
            std::cerr << "Expected acceptance: [" << number << "]\n";
            passed = false;
        }
    }
    for (const char* number : rejected) {
        if (ContactValidator::validPhone(number)) {
            std::cerr << "Expected rejection: [" << number << "]\n";
            passed = false;
        }
    }
    if (passed) std::cout << "All 19 phone format checks passed.\n";
    return passed ? 0 : 1;
}
