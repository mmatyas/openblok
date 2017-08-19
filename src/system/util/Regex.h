#pragma once


#ifdef USE_BOOST_REGEX
    #include <boost/regex.hpp>
    using boost::regex;
    using boost::regex_replace;
    using boost::regex_match;
#else
    #include <regex>
    using std::regex;
    using std::regex_replace;
    using std::regex_match;
#endif
