#include "person.hpp"

#include <utility>

namespace {

// SSN entries must be strings of exactly this length.
constexpr std::string::size_type kSsnLength = 9;

std::string asStringOrEmpty(const Json::Value& value) {
    return value.isString() ? value.asString() : std::string();
}

} // namespace

Person::Person(const Json::Value& value) : _person(value) {}

Person::Person(std::string first_name, std::string last_name, std::string ssn) {
    _person["firstName"] = std::move(first_name);
    _person["lastName"] = std::move(last_name);
    _person["SSN"] = std::move(ssn);
}

std::string Person::getFirstName() const {
    if (!_person.isObject()) return std::string();
    return asStringOrEmpty(_person["firstName"]);
}

std::string Person::getLastName() const {
    if (!_person.isObject()) return std::string();
    return asStringOrEmpty(_person["lastName"]);
}

std::string Person::getSsn() const {
    if (!_person.isObject()) return std::string();
    return asStringOrEmpty(_person["SSN"]);
}

void Person::setFirstName(const std::string& first_name) {
    _person["firstName"] = first_name;
}

void Person::setLastName(const std::string& last_name) {
    _person["lastName"] = last_name;
}

void Person::setSsn(const std::string& ssn) {
    _person["SSN"] = ssn;
}

bool Person::isValid() const {
    if (!_person.isObject()) return false;

    const Json::Value& first_name = _person["firstName"];
    const Json::Value& last_name = _person["lastName"];
    const Json::Value& ssn = _person["SSN"];

    if (!first_name.isString() || !last_name.isString() || !ssn.isString()) {
        return false;
    }

    return ssn.asString().length() == kSsnLength;
}

Json::Value Person::toJson() const {
    return _person;
}
