# person-cli

A small C++ command-line application that manages a set of person records
stored in a JSON file.

Each record looks like:

```json
{ "firstName": "F1", "lastName": "L1", "SSN": "111111111" }
```

The backing file is a JSON array of these objects (a single top-level object is
also accepted and treated as a one-entry store).

## What it does

1. Takes a JSON file as its only command-line argument.
2. Reads and validates every entry. **Validation criteria:**
   - `firstName` and `lastName` must be strings (any length ≥ 0).
   - `SSN` must be a string of length **exactly 9**.
3. Prints the positions of any invalid rows on startup.
4. Exposes an interactive menu of commands:
   1. **Add a person** — prompts for the fields and appends to the file.
      Rejected if the SSN already exists, or if the entry is invalid.
   2. **Query a person** — looks a person up by SSN.
   3. **Update a person** — updates the `firstName` / `lastName` for an SSN.
   4. **Delete a person** — removes the person with a given SSN.
   5. **Clean invalid entries** — moves every invalid entry to
      `<file>.invalid.json` for inspection and keeps only the valid entries.
   6. **List all entries** — prints every entry, flagging invalid ones.

Mutating commands persist their changes back to the JSON file immediately.

## Layout

```
person-cli/
├── CMakeLists.txt
├── include/
│   ├── person.hpp            # single record: getters/setters + validation
│   └── person_registry.hpp   # collection: load/save + the 5 commands
├── src/
│   ├── person.cpp
│   ├── person_registry.cpp
│   └── main.cpp              # argument parsing + interactive menu
├── test/
│   ├── CMakeLists.txt
│   ├── test_framework.hpp    # tiny dependency-free assert harness
│   └── test_person.cpp       # tests for validation + all 5 commands
└── data/
    └── people.json           # sample data (contains two invalid rows)
```

## Dependencies

- A C++17 compiler
- [CMake](https://cmake.org/) ≥ 3.14
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
  (`apt install libjsoncpp-dev`)

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/person_cli data/people.json
```

The sample `data/people.json` intentionally contains two invalid rows (an
8-character SSN and a non-string last name), so the app reports invalid
positions on startup and `Clean invalid entries` has something to move.

## Test

```sh
cd build
ctest --output-on-failure
```
