#include <catch2/catch.hpp>
#include "square.h"


TEST_CASE("Equality")
{
    constexpr auto sq1 = hume::Square{43};
    constexpr auto sq2 = hume::Square{43};
    REQUIRE(sq1 != sq2);
}
