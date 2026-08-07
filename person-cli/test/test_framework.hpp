#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>

// A deliberately tiny, dependency-free assertion harness so the tests build
// with nothing beyond the project itself (and jsoncpp, which the code needs).
namespace tf {

struct Stats {
    int passed = 0;
    int failed = 0;
};

inline Stats& stats() {
    static Stats s;
    return s;
}

inline void check(bool condition, const std::string& expression,
                  const char* file, int line) {
    if (condition) {
        stats().passed++;
    } else {
        stats().failed++;
        std::cerr << "FAILED " << file << ":" << line << ": " << expression
                  << "\n";
    }
}

inline int report() {
    std::cout << stats().passed << " checks passed, " << stats().failed
              << " failed\n";
    return stats().failed == 0 ? 0 : 1;
}

} // namespace tf

#define CHECK(cond) ::tf::check((cond), #cond, __FILE__, __LINE__)
#define CHECK_EQ(a, b) \
    ::tf::check((a) == (b), #a " == " #b, __FILE__, __LINE__)

#endif // TEST_FRAMEWORK_HPP
