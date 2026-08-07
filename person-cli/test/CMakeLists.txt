add_executable(person_tests test_person.cpp)
target_link_libraries(person_tests PRIVATE person_core)

add_test(NAME person_tests COMMAND person_tests)
