#include "doctest/doctest.h"
#include "life.h"

TEST_CASE("Test coord struct") {
    auto C = LIFE::coord{1, 2};
    CHECK(C.x == 1);
    CHECK(C.y == 2);
}

TEST_CASE("Test getN") {
    auto C1 = LIFE::coord{1, 2};
    auto C2 = LIFE::getN(C1);
    CHECK(C2.x == 1);
    CHECK(C2.y == 3);
}

TEST_CASE("Test getS") {
    auto C1 = LIFE::coord{1, 2};
    auto C2 = LIFE::getS(C1);
    CHECK(C2.x == 1);
    CHECK(C2.y == 1);
}

TEST_CASE("Test getE") {
    auto C1 = LIFE::coord{1, 2};
    auto C2 = LIFE::getE(C1);
    CHECK(C2.x == 2);
    CHECK(C2.y == 2);
}

TEST_CASE("Test getW") {
    auto C1 = LIFE::coord{1, 2};
    auto C2 = LIFE::getW(C1);
    CHECK(C2.x == 0);
    CHECK(C2.y == 2);
}

TEST_CASE("Test getNW") {
    auto C1 = LIFE::coord{1, 2};
    auto C2 = LIFE::getNW(C1);
    CHECK(C2.x == 0);
    CHECK(C2.y == 3);
}

TEST_CASE("Test getSE") {
    auto C1 = LIFE::coord{1, 2};
    auto C2 = LIFE::getSE(C1);
    CHECK(C2.x == 2);
    CHECK(C2.y == 1);
}
