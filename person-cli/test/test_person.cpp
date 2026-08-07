#include "test_framework.hpp"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

#include <json/json.h>

#include "person.hpp"
#include "person_registry.hpp"

namespace {

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path, std::ofstream::binary | std::ofstream::trunc);
    out << content;
}

// ---------------------------------------------------------------------------
// Person validation
// ---------------------------------------------------------------------------
void test_person_getters_and_validation() {
    Person good("Ada", "Lovelace", "111111111");
    CHECK(good.isValid());
    CHECK_EQ(good.getFirstName(), std::string("Ada"));
    CHECK_EQ(good.getLastName(), std::string("Lovelace"));
    CHECK_EQ(good.getSsn(), std::string("111111111"));

    // SSN shorter than 9 characters -> invalid.
    Person shortSsn("A", "B", "123");
    CHECK(!shortSsn.isValid());

    // SSN longer than 9 characters -> invalid.
    Person longSsn("A", "B", "1234567890");
    CHECK(!longSsn.isValid());

    // Empty names are allowed (length >= 0), SSN still length 9.
    Person emptyNames("", "", "123456789");
    CHECK(emptyNames.isValid());

    // A non-string field makes the entry invalid.
    Json::Value raw;
    raw["firstName"] = "X";
    raw["lastName"] = 42; // number, not a string
    raw["SSN"] = "123456789";
    Person badType(raw);
    CHECK(!badType.isValid());

    // A missing SSN makes the entry invalid.
    Json::Value missing;
    missing["firstName"] = "X";
    missing["lastName"] = "Y";
    Person noSsn(missing);
    CHECK(!noSsn.isValid());
}

// ---------------------------------------------------------------------------
// Loading + invalid-row reporting
// ---------------------------------------------------------------------------
void test_load_and_invalid_indices() {
    const std::string path = "test_people.json";
    writeFile(path, R"([
      {"firstName": "Ada",   "lastName": "Lovelace", "SSN": "111111111"},
      {"firstName": "Alan",  "lastName": "Turing",   "SSN": "22222222"},
      {"firstName": "Grace", "lastName": "Hopper",   "SSN": "333333333"}
    ])");

    PersonRegistry registry(path);
    CHECK(registry.load());
    CHECK_EQ(registry.size(), static_cast<std::size_t>(3));

    const std::vector<std::size_t> invalid = registry.invalidIndices();
    CHECK_EQ(invalid.size(), static_cast<std::size_t>(1));
    CHECK_EQ(invalid[0], static_cast<std::size_t>(1)); // the short SSN

    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// Command 1 - Add (incl. duplicate + invalid rejection + persistence)
// ---------------------------------------------------------------------------
void test_add() {
    const std::string path = "test_add.json";
    writeFile(path,
              R"([{"firstName": "Ada", "lastName": "Lovelace", "SSN": "111111111"}])");

    PersonRegistry registry(path);
    CHECK(registry.load());

    CHECK(registry.add(Person("Alan", "Turing", "222222222")) ==
          PersonRegistry::AddResult::Ok);
    CHECK_EQ(registry.size(), static_cast<std::size_t>(2));

    // Duplicate SSN is rejected.
    CHECK(registry.add(Person("Dup", "Licate", "111111111")) ==
          PersonRegistry::AddResult::DuplicateSsn);
    CHECK_EQ(registry.size(), static_cast<std::size_t>(2));

    // Invalid SSN length is rejected.
    CHECK(registry.add(Person("Bad", "Ssn", "123")) ==
          PersonRegistry::AddResult::Invalid);
    CHECK_EQ(registry.size(), static_cast<std::size_t>(2));

    // The successful add was persisted to disk.
    PersonRegistry reloaded(path);
    CHECK(reloaded.load());
    CHECK_EQ(reloaded.size(), static_cast<std::size_t>(2));

    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// Command 2 - Query
// ---------------------------------------------------------------------------
void test_query() {
    const std::string path = "test_query.json";
    writeFile(path, R"([
      {"firstName": "Ada",  "lastName": "Lovelace", "SSN": "111111111"},
      {"firstName": "Alan", "lastName": "Turing",   "SSN": "222222222"}
    ])");

    PersonRegistry registry(path);
    CHECK(registry.load());

    const std::optional<Person> found = registry.query("222222222");
    CHECK(found.has_value());
    CHECK_EQ(found->getFirstName(), std::string("Alan"));

    const std::optional<Person> missing = registry.query("999999999");
    CHECK(!missing.has_value());

    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// Command 3 - Update
// ---------------------------------------------------------------------------
void test_update() {
    const std::string path = "test_update.json";
    writeFile(path,
              R"([{"firstName": "Ada", "lastName": "Lovelace", "SSN": "111111111"}])");

    PersonRegistry registry(path);
    CHECK(registry.load());

    CHECK(registry.update("111111111", "Augusta", "King"));
    const std::optional<Person> updated = registry.query("111111111");
    CHECK(updated.has_value());
    CHECK_EQ(updated->getFirstName(), std::string("Augusta"));
    CHECK_EQ(updated->getLastName(), std::string("King"));

    // Updating a non-existent SSN fails.
    CHECK(!registry.update("999999999", "No", "One"));

    // Change was persisted.
    PersonRegistry reloaded(path);
    CHECK(reloaded.load());
    CHECK_EQ(reloaded.query("111111111")->getLastName(), std::string("King"));

    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// Command 4 - Delete
// ---------------------------------------------------------------------------
void test_remove() {
    const std::string path = "test_remove.json";
    writeFile(path, R"([
      {"firstName": "Ada",  "lastName": "Lovelace", "SSN": "111111111"},
      {"firstName": "Alan", "lastName": "Turing",   "SSN": "222222222"}
    ])");

    PersonRegistry registry(path);
    CHECK(registry.load());

    CHECK(registry.remove("111111111"));
    CHECK_EQ(registry.size(), static_cast<std::size_t>(1));
    CHECK(!registry.query("111111111").has_value());

    // Deleting a missing SSN fails.
    CHECK(!registry.remove("111111111"));

    std::remove(path.c_str());
}

// ---------------------------------------------------------------------------
// Command 5 - Clean invalid entries
// ---------------------------------------------------------------------------
void test_clean_invalid() {
    const std::string path = "test_clean.json";
    const std::string quarantine = "test_clean.invalid.json";
    writeFile(path, R"([
      {"firstName": "Ada",   "lastName": "Lovelace", "SSN": "111111111"},
      {"firstName": "Bad",   "lastName": "Ssn",      "SSN": "123"},
      {"firstName": "Grace", "lastName": "Hopper",   "SSN": "333333333"}
    ])");

    PersonRegistry registry(path);
    CHECK(registry.load());

    const std::size_t removed = registry.cleanInvalid(quarantine);
    CHECK_EQ(removed, static_cast<std::size_t>(1));
    CHECK_EQ(registry.size(), static_cast<std::size_t>(2));
    CHECK(registry.invalidIndices().empty());

    // The quarantine file holds exactly the invalid entry.
    PersonRegistry quarantined(quarantine);
    CHECK(quarantined.load());
    CHECK_EQ(quarantined.size(), static_cast<std::size_t>(1));
    CHECK_EQ(quarantined.people()[0].getFirstName(), std::string("Bad"));

    // The main file now holds only the valid entries.
    PersonRegistry reloaded(path);
    CHECK(reloaded.load());
    CHECK_EQ(reloaded.size(), static_cast<std::size_t>(2));

    std::remove(path.c_str());
    std::remove(quarantine.c_str());
}

} // namespace

int main() {
    test_person_getters_and_validation();
    test_load_and_invalid_indices();
    test_add();
    test_query();
    test_update();
    test_remove();
    test_clean_invalid();
    return tf::report();
}
