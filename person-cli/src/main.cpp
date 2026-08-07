#include <iostream>
#include <string>

#include "person.hpp"
#include "person_registry.hpp"

namespace {

std::string prompt(const std::string& label) {
    std::cout << label;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void printPerson(const Person& person) {
    std::cout << "  First Name: " << person.getFirstName() << "\n"
              << "  Last Name : " << person.getLastName() << "\n"
              << "  SSN       : " << person.getSsn() << "\n";
}

void listAll(const PersonRegistry& registry) {
    const auto& people = registry.people();
    if (people.empty()) {
        std::cout << "(no entries)\n";
        return;
    }
    for (std::size_t i = 0; i < people.size(); ++i) {
        const Person& person = people[i];
        std::cout << "[" << i << "] " << person.getFirstName() << " "
                  << person.getLastName() << " (SSN: " << person.getSsn() << ")"
                  << (person.isValid() ? "" : "   <-- INVALID") << "\n";
    }
}

// Prints the positions of any entries that fail validation.
void reportInvalid(const PersonRegistry& registry) {
    const std::vector<std::size_t> invalid = registry.invalidIndices();
    if (invalid.empty()) {
        std::cout << "All entries are valid.\n";
        return;
    }
    std::cout << "Invalid entries found at position(s): ";
    for (std::size_t i = 0; i < invalid.size(); ++i) {
        std::cout << invalid[i];
        if (i + 1 < invalid.size()) std::cout << ", ";
    }
    std::cout << "\n";
}

void handleAdd(PersonRegistry& registry) {
    const std::string first_name = prompt("First name: ");
    const std::string last_name = prompt("Last name : ");
    const std::string ssn = prompt("SSN (9 characters): ");

    switch (registry.add(Person(first_name, last_name, ssn))) {
        case PersonRegistry::AddResult::Ok:
            std::cout << "Added.\n";
            break;
        case PersonRegistry::AddResult::DuplicateSsn:
            std::cout << "Rejected: a person with SSN " << ssn
                      << " already exists.\n";
            break;
        case PersonRegistry::AddResult::Invalid:
            std::cout << "Rejected: invalid entry (SSN must be exactly 9 "
                         "characters).\n";
            break;
    }
}

void handleQuery(const PersonRegistry& registry) {
    const std::string ssn = prompt("SSN: ");
    if (const std::optional<Person> person = registry.query(ssn)) {
        std::cout << "Found:\n";
        printPerson(*person);
    } else {
        std::cout << "No person with SSN " << ssn << ".\n";
    }
}

void handleUpdate(PersonRegistry& registry) {
    const std::string ssn = prompt("SSN of person to update: ");
    const std::string first_name = prompt("New first name: ");
    const std::string last_name = prompt("New last name : ");
    if (registry.update(ssn, first_name, last_name)) {
        std::cout << "Updated.\n";
    } else {
        std::cout << "No person with SSN " << ssn << ".\n";
    }
}

void handleDelete(PersonRegistry& registry) {
    const std::string ssn = prompt("SSN of person to delete: ");
    if (registry.remove(ssn)) {
        std::cout << "Deleted.\n";
    } else {
        std::cout << "No person with SSN " << ssn << ".\n";
    }
}

void handleClean(PersonRegistry& registry, const std::string& file_name) {
    const std::string quarantine_file = file_name + ".invalid.json";
    const std::size_t removed = registry.cleanInvalid(quarantine_file);
    if (removed == 0) {
        std::cout << "No invalid entries to clean.\n";
    } else {
        std::cout << "Moved " << removed << " invalid entry(ies) to "
                  << quarantine_file << ".\n";
    }
}

const char* kMenu =
    "\nCommands:\n"
    "  1) Add a person\n"
    "  2) Query a person (by SSN)\n"
    "  3) Update a person (by SSN)\n"
    "  4) Delete a person (by SSN)\n"
    "  5) Clean invalid entries\n"
    "  6) List all entries\n"
    "  0) Exit\n"
    "Choose: ";

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid command line arguments provided\n";
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "person_cli")
                  << " <people.json>\n";
        return 1;
    }

    const std::string file_name = argv[1];
    PersonRegistry registry(file_name);
    if (!registry.load()) {
        std::cerr << "Warning: could not read '" << file_name
                  << "'. Starting with an empty registry.\n";
    }

    std::cout << "Loaded " << registry.size() << " entry(ies) from "
              << file_name << "\n";
    reportInvalid(registry);

    while (true) {
        std::cout << kMenu;
        std::string choice;
        if (!std::getline(std::cin, choice)) {
            break;
        }

        if (choice == "0") {
            break;
        } else if (choice == "1") {
            handleAdd(registry);
        } else if (choice == "2") {
            handleQuery(registry);
        } else if (choice == "3") {
            handleUpdate(registry);
        } else if (choice == "4") {
            handleDelete(registry);
        } else if (choice == "5") {
            handleClean(registry, file_name);
        } else if (choice == "6") {
            listAll(registry);
        } else if (!choice.empty()) {
            std::cout << "Unknown command.\n";
        }
    }

    std::cout << "Bye.\n";
    return 0;
}
