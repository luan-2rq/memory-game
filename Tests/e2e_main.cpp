#include <gtest/gtest.h>

#include <cstdlib>
#include <cstring>
#include <string>

namespace
{

int parseRepeatValue(const char* value)
{
    if (value == nullptr || *value == '\0')
        return 1;

    char* end = nullptr;
    const long parsed = std::strtol(value, &end, 10);
    if (end == value || *end != '\0' || parsed < 1 || parsed > 100000)
        return 1;

    return static_cast<int>(parsed);
}

void applyCustomRepeatFlag(int argc, char** argv)
{
    int repeat = 1;

    if (const char* envRepeat = std::getenv("MEMORYGAME_E2E_REPEAT"))
        repeat = parseRepeatValue(envRepeat);

    const char* kRepeat = "--repeat=";
    for (int i = 1; i < argc; ++i)
        if (std::strncmp(argv[i], kRepeat, std::strlen(kRepeat)) == 0)
            repeat = parseRepeatValue(argv[i] + std::strlen(kRepeat));

    GTEST_FLAG_SET(repeat, repeat);
}

// --category=<Name>  →  --gtest_filter=*_Cat_<Name>*
// Lets callers run a specific test category without knowing GTest internals:
//   ./memorygame_e2e --category=Smoke
//   ./memorygame_e2e --category=Regression
void applyCategoryFilter(int argc, char** argv)
{
    const char* kCat = "--category=";
    for (int i = 1; i < argc; ++i)
    {
        if (std::strncmp(argv[i], kCat, std::strlen(kCat)) != 0)
            continue;

        const std::string category(argv[i] + std::strlen(kCat));
        if (category.empty())
            continue;

        const std::string filter = "*_Cat_" + category + "*";
        ::testing::GTEST_FLAG(filter) = filter;
        return;
    }
}

}  // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    applyCustomRepeatFlag(argc, argv);
    applyCategoryFilter(argc, argv);
    return RUN_ALL_TESTS();
}
