#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-bsh");
    BOOST_CHECK(GetBoolArg("-bsh"));
    BOOST_CHECK(GetBoolArg("-bsh", false));
    BOOST_CHECK(GetBoolArg("-bsh", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-bsho"));
    BOOST_CHECK(!GetBoolArg("-bsho", false));
    BOOST_CHECK(GetBoolArg("-bsho", true));

    ResetArgs("-bsh=0");
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", false));
    BOOST_CHECK(!GetBoolArg("-bsh", true));

    ResetArgs("-bsh=1");
    BOOST_CHECK(GetBoolArg("-bsh"));
    BOOST_CHECK(GetBoolArg("-bsh", false));
    BOOST_CHECK(GetBoolArg("-bsh", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-n-nobsh");
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", false));
    BOOST_CHECK(!GetBoolArg("-bsh", true));

    ResetArgs("-n-nobsh=1");
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", false));
    BOOST_CHECK(!GetBoolArg("-bsh", true));

    ResetArgs("-bsh -n-nobsh");  // -bsh should win
    BOOST_CHECK(GetBoolArg("-bsh"));
    BOOST_CHECK(GetBoolArg("-bsh", false));
    BOOST_CHECK(GetBoolArg("-bsh", true));

    ResetArgs("-bsh=1 -n-nobsh=1");  // -bsh should win
    BOOST_CHECK(GetBoolArg("-bsh"));
    BOOST_CHECK(GetBoolArg("-bsh", false));
    BOOST_CHECK(GetBoolArg("-bsh", true));

    ResetArgs("-bsh=0 -n-nobsh=0");  // -bsh should win
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", false));
    BOOST_CHECK(!GetBoolArg("-bsh", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--bsh=1");
    BOOST_CHECK(GetBoolArg("-bsh"));
    BOOST_CHECK(GetBoolArg("-bsh", false));
    BOOST_CHECK(GetBoolArg("-bsh", true));

    ResetArgs("--n-nobsh=1");
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", false));
    BOOST_CHECK(!GetBoolArg("-bsh", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-bsh", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-bsh", "eleven"), "eleven");

    ResetArgs("-bsh -bar");
    BOOST_CHECK_EQUAL(GetArg("-bsh", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-bsh", "eleven"), "");

    ResetArgs("-bsh=");
    BOOST_CHECK_EQUAL(GetArg("-bsh", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-bsh", "eleven"), "");

    ResetArgs("-bsh=11");
    BOOST_CHECK_EQUAL(GetArg("-bsh", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-bsh", "eleven"), "11");

    ResetArgs("-bsh=eleven");
    BOOST_CHECK_EQUAL(GetArg("-bsh", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-bsh", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-bsh", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-bsh", 0), 0);

    ResetArgs("-bsh -bar");
    BOOST_CHECK_EQUAL(GetArg("-bsh", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-bsh=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-bsh", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-bsh=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-bsh", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--bsh");
    BOOST_CHECK_EQUAL(GetBoolArg("-bsh"), true);

    ResetArgs("--bsh=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-bsh", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-n-nobsh");
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", true));
    BOOST_CHECK(!GetBoolArg("-bsh", false));

    ResetArgs("-n-nobsh=1");
    BOOST_CHECK(!GetBoolArg("-bsh"));
    BOOST_CHECK(!GetBoolArg("-bsh", true));
    BOOST_CHECK(!GetBoolArg("-bsh", false));

    ResetArgs("-nobsh=0");
    BOOST_CHECK(GetBoolArg("-bsh"));
    BOOST_CHECK(GetBoolArg("-bsh", true));
    BOOST_CHECK(GetBoolArg("-bsh", false));

    ResetArgs("-bsh --nobsh");
    BOOST_CHECK(GetBoolArg("-bsh"));

    ResetArgs("-nobsh -bsh"); // bshalways wins:
    BOOST_CHECK(GetBoolArg("-bsh"));
}

BOOST_AUTO_TEST_SUITE_END()
