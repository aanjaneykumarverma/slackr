#ifndef PERSON_PERSON_REGISTRY_HPP
#define PERSON_PERSON_REGISTRY_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "person.hpp"

// Manages the collection of Person entries backed by a JSON file.
//
// The backing file is expected to contain a JSON array of person objects, e.g.
//   [
//     {"firstName": "F1", "lastName": "L1", "SSN": "111111111"},
//     {"firstName": "F2", "lastName": "L2", "SSN": "222222222"}
//   ]
// A single top-level object is also accepted and treated as a one-entry store.
//
// Mutating operations (add / update / remove / cleanInvalid) persist the
// change back to the backing file immediately.
class PersonRegistry {
public:
    enum class AddResult {
        Ok,           // person added and persisted
        Invalid,      // person failed validation (e.g. SSN not length 9)
        DuplicateSsn, // an entry with the same SSN already exists
    };

    explicit PersonRegistry(std::string file_name);

    // Read entries from the backing file. Returns false if the file cannot be
    // opened or does not contain valid JSON.
    bool load();

    // Write the current entries back to the backing file. Returns false on I/O
    // failure.
    bool save() const;

    const std::vector<Person>& people() const { return _people; }
    std::size_t size() const { return _people.size(); }

    // Positions (0-based) of entries that fail validation.
    std::vector<std::size_t> invalidIndices() const;

    // Command 1 - Add a person. Rejected if invalid or if the SSN already
    // exists.
    AddResult add(const Person& person);

    // Command 2 - Query a person by SSN.
    std::optional<Person> query(const std::string& ssn) const;

    // Command 3 - Update the first/last name of the person with this SSN.
    // Returns false if no matching person exists.
    bool update(const std::string& ssn,
                const std::string& first_name,
                const std::string& last_name);

    // Command 4 - Delete the person with this SSN. Returns false if no
    // matching person exists.
    bool remove(const std::string& ssn);

    // Command 5 - Move every invalid entry to `quarantine_file` (as a JSON
    // array) and keep only the valid entries. Returns the number of entries
    // quarantined. The backing file is rewritten with the surviving entries.
    std::size_t cleanInvalid(const std::string& quarantine_file);

private:
    // Index of the first entry with a matching SSN, or -1 if none.
    int indexOfSsn(const std::string& ssn) const;

    std::string _file_name;
    std::vector<Person> _people;
};

#endif // PERSON_PERSON_REGISTRY_HPP
