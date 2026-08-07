#ifndef PERSON_PERSON_HPP
#define PERSON_PERSON_HPP

#include <string>

#include <json/json.h>

// Represents a single person entry as read from / written to the JSON store.
//
// The raw Json::Value is kept internally so that type-based validation
// (e.g. "firstName must be a string") remains meaningful even when the source
// file contains malformed entries.
class Person {
public:
    Person() = default;

    // Construct from a raw JSON object (a single entry from the store).
    explicit Person(const Json::Value& value);

    // Construct from explicit field values.
    Person(std::string first_name, std::string last_name, std::string ssn);

    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getSsn() const;

    void setFirstName(const std::string& first_name);
    void setLastName(const std::string& last_name);
    void setSsn(const std::string& ssn);

    // Validation criteria:
    //   - firstName and lastName are strings (any length >= 0 is allowed)
    //   - SSN is a string whose length is exactly 9
    bool isValid() const;

    // The underlying JSON object, suitable for serialisation.
    Json::Value toJson() const;

private:
    Json::Value _person;
};

#endif // PERSON_PERSON_HPP
