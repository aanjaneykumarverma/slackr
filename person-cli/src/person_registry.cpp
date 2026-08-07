#include "person_registry.hpp"

#include <fstream>
#include <utility>

#include <json/json.h>

namespace {

std::string serialize(const Json::Value& root) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    return Json::writeString(builder, root);
}

} // namespace

PersonRegistry::PersonRegistry(std::string file_name)
    : _file_name(std::move(file_name)) {}

bool PersonRegistry::load() {
    _people.clear();

    std::ifstream person_file(_file_name, std::ifstream::binary);
    if (!person_file.is_open()) {
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    if (!Json::parseFromStream(builder, person_file, &root, &errors)) {
        return false;
    }

    if (root.isArray()) {
        for (const Json::Value& entry : root) {
            _people.emplace_back(entry);
        }
    } else if (root.isObject()) {
        _people.emplace_back(root);
    }
    // A null / other root is treated as an empty store.

    return true;
}

bool PersonRegistry::save() const {
    Json::Value root(Json::arrayValue);
    for (const Person& person : _people) {
        root.append(person.toJson());
    }

    std::ofstream out(_file_name, std::ofstream::binary | std::ofstream::trunc);
    if (!out.is_open()) {
        return false;
    }
    out << serialize(root) << '\n';
    return out.good();
}

std::vector<std::size_t> PersonRegistry::invalidIndices() const {
    std::vector<std::size_t> indices;
    for (std::size_t i = 0; i < _people.size(); ++i) {
        if (!_people[i].isValid()) {
            indices.push_back(i);
        }
    }
    return indices;
}

int PersonRegistry::indexOfSsn(const std::string& ssn) const {
    for (std::size_t i = 0; i < _people.size(); ++i) {
        if (_people[i].getSsn() == ssn) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

PersonRegistry::AddResult PersonRegistry::add(const Person& person) {
    if (!person.isValid()) {
        return AddResult::Invalid;
    }
    if (indexOfSsn(person.getSsn()) >= 0) {
        return AddResult::DuplicateSsn;
    }
    _people.push_back(person);
    save();
    return AddResult::Ok;
}

std::optional<Person> PersonRegistry::query(const std::string& ssn) const {
    const int index = indexOfSsn(ssn);
    if (index < 0) {
        return std::nullopt;
    }
    return _people[static_cast<std::size_t>(index)];
}

bool PersonRegistry::update(const std::string& ssn,
                            const std::string& first_name,
                            const std::string& last_name) {
    const int index = indexOfSsn(ssn);
    if (index < 0) {
        return false;
    }
    Person& person = _people[static_cast<std::size_t>(index)];
    person.setFirstName(first_name);
    person.setLastName(last_name);
    save();
    return true;
}

bool PersonRegistry::remove(const std::string& ssn) {
    const int index = indexOfSsn(ssn);
    if (index < 0) {
        return false;
    }
    _people.erase(_people.begin() + index);
    save();
    return true;
}

std::size_t PersonRegistry::cleanInvalid(const std::string& quarantine_file) {
    Json::Value quarantined(Json::arrayValue);
    std::vector<Person> valid;
    valid.reserve(_people.size());

    for (const Person& person : _people) {
        if (person.isValid()) {
            valid.push_back(person);
        } else {
            quarantined.append(person.toJson());
        }
    }

    const std::size_t removed = _people.size() - valid.size();
    if (removed > 0) {
        std::ofstream out(quarantine_file,
                          std::ofstream::binary | std::ofstream::trunc);
        if (out.is_open()) {
            out << serialize(quarantined) << '\n';
        }
    }

    _people = std::move(valid);
    save();
    return removed;
}
