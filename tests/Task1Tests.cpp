// Standalone C++17 tests for Task 1. Pass a new scratch contacts file path.
#include "ConsoleUI.h"
#include "FileStorage.h"

#include <array>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
class Redirect {
public:
    Redirect(std::ios& stream, std::streambuf* buffer)
        : stream_(stream), original_(stream.rdbuf(buffer)) {}
    ~Redirect() { stream_.rdbuf(original_); }
    Redirect(const Redirect&) = delete;
    Redirect& operator=(const Redirect&) = delete;

private:
    std::ios& stream_;
    std::streambuf* original_;
};

std::string run(const FileStorage& storage, const std::string& commands) {
    std::istringstream input(commands);
    std::ostringstream output;
    std::ostringstream errors;
    Redirect replaceInput(std::cin, input.rdbuf());
    Redirect replaceOutput(std::cout, output.rdbuf());
    Redirect replaceErrors(std::cerr, errors.rdbuf());
    ConsoleUI(storage).run();
    if (!errors.str().empty()) throw std::runtime_error(errors.str());
    return output.str();
}

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void expect(const std::string& output, std::initializer_list<std::string> lines) {
    for (const auto& line : lines) {
        require(output.find(line + '\n') != std::string::npos, "Missing output: " + line);
    }
}

void count(const std::string& output, const std::string& message, int expected) {
    int occurrences = 0;
    for (auto pos = output.find(message); pos != std::string::npos;
         pos = output.find(message, pos + message.size())) {
        ++occurrences;
    }
    require(occurrences == expected, "Unexpected count for " + message);
}

void testValidation(const FileStorage& storage) {
    expect(run(storage, "2\n0\n"), {"No contacts found."});
    const auto output = run(storage,
        "1\n\n2Anna\n-Anna\nAnna-\nAnn_a\n Anna 2-Marie \n"
        "\n9Smith\n " u8"\u041F\u0435\u0442\u0440\u043E\u0432\u0430" " \n"
        "-Middle\n Middle \n  10 \"Main\" Street  \n"
        "2020-00-01\n2020-13-01\n2020-01-00\n2020-01-32\n2020-04-31\n"
        "2019-02-29\n1900-02-29\n0000-01-01\n29.02.2000\n9999-12-31\n 2000-02-29 \n"
        "\nmissingat\nann+1@example.com\nann@bad-domain.com\n@example.com\nann@\n"
        " ann1 \t@ example2 . com \n"
        "\n+18121234567\n78121234567\n+88121234567\n+7812123456\n881212345678\n"
        "+7812 1234567\n+7(81)1234567\n8(8121234567\n8(812)1234-5-67\n"
        "+7(812)123456-7\n+7812123456x\n8(812)123-45-67 ext 1\n +78121234567 \n1\n0\n");
    count(output, "Invalid name:", 8);
    count(output, "Invalid birth date.", 10);
    count(output, "Invalid email.", 6);
    count(output, "Invalid phone format.", 13);
    expect(run(storage, "2\n0\n"), {
        "[1] Anna 2-Marie " u8"\u041F\u0435\u0442\u0440\u043E\u0432\u0430",
        "  Patronymic: Middle", "  Address: 10 \"Main\" Street",
        "  Birth date: 2000-02-29", "  Email: ann1@example2.com",
        "  Phone 1 (Home): +78121234567",
    });
}

void testEditingAndDeletion(const FileStorage& storage) {
    const auto output = run(storage,
        "3\n1\n1\nZoe\n2\nAdams\n3\nZulu Middle\n4\nAlpha Street\n"
        "5\n2000-01-01\n6\nzulu@example.com\n7\n"
        "3\n1\n1\n 88121234567 \n2\n1\n+7(812)1234567\n3\n"
        "1\n8(812)1234567\n1\n1\n+7(812)123-45-67\n2\n"
        "1\n8(812)123-45-67\n3\n3\n1\n2\n1\n88001234567\n1\n0\n0\n0\n");
    expect(output, {"A contact must have at least one phone number."});
    expect(output, {
        "2. 88121234567 (Work)", "3. +7(812)1234567 (Service)",
        "4. 8(812)1234567 (Home)", "5. +7(812)123-45-67 (Work)",
        "6. 8(812)123-45-67 (Service)",
    });
    const auto saved = run(storage, "2\n0\n");
    expect(saved, {
        "[1] Zoe Adams", "  Patronymic: Zulu Middle", "  Address: Alpha Street",
        "  Birth date: 2000-01-01", "  Email: zulu@example.com",
        "  Phone 1 (Home): 88001234567", "  Phone 2 (Service): +7(812)1234567",
        "  Phone 3 (Home): 8(812)1234567", "  Phone 4 (Work): +7(812)123-45-67",
        "  Phone 5 (Service): 8(812)123-45-67",
    });
    require(saved.find("+78121234567") == std::string::npos, "Deleted phone was reloaded");
    require(saved.find("88121234567") == std::string::npos, "Replaced phone was reloaded");
}

void testSortSearchAndDelete(const FileStorage& storage) {
    run(storage, "1\nAnna\nZed\nAlpha\nZulu Lane\n2002-01-01\n"
                 "alpha@example.com\n88121234567\n2\n7\n0\n");
    const std::array<std::string, 7> first = {
        "Anna Zed", "Zoe Adams", "Anna Zed", "Anna Zed",
        "Zoe Adams", "Zoe Adams", "Zoe Adams",
    };
    for (std::size_t i = 0; i < first.size(); ++i) {
        expect(run(storage, "6\n" + std::to_string(i + 1) + "\n0\n"), {"[1] " + first[i]});
    }
    for (const std::string query : {"ZOE", "Adams", "Zulu Middle", "Alpha Street",
                                   "2000-01-01", "zulu@example.com", "8800123"}) {
        const auto output = run(storage, "5\n" + query + "\n0\n");
        require(output.find("Zoe Adams\n") != std::string::npos, "Search missed: " + query);
        require(output.find("Anna Zed\n") == std::string::npos, "Unexpected match: " + query);
    }
    expect(run(storage, "5\nunmatched term\n0\n"), {"No matches."});

    run(storage, "4\n1\nn\n0\n");
    expect(run(storage, "2\n0\n"), {"[1] Zoe Adams", "[2] Anna Zed"});
    expect(run(storage, "4\n1\ny\n0\n"), {"Contact deleted."});
    expect(run(storage, "2\n0\n"), {"[1] Anna Zed"});
    expect(run(storage, "4\n1\ny\n0\n"), {"Contact deleted."});
    expect(run(storage, "2\n0\n"), {"No contacts found."});
}
}  // namespace

int main(int argc, char* argv[]) {
    try {
        require(argc == 2, "Usage: task1-tests.exe <new scratch contacts file>");
        require(!std::filesystem::exists(argv[1]), "Use a new scratch file path");
        const FileStorage storage(argv[1]);
        testValidation(storage);
        testEditingAndDeletion(storage);
        testSortSearchAndDelete(storage);
        std::cout << "Task 1 C++ tests passed: validation, editing, search, sort, deletion and file storage.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Task 1 test failed: " << error.what() << '\n';
        return 1;
    }
}
