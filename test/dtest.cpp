#include "doctest/doctest.h"

TEST_CASE("Showcasing assertions in doctest") {
    CHECK("hello" != "world");
    CHECK((7*6) == 42);
}
