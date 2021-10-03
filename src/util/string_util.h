#ifndef STRING_UTIL_H
  #define STRING_UTIL_H

#include <string>
// A small collection of free-functions for manipulating std::string
// objects.

namespace str_util {

// return copy of original string with left-side whitespace trimmed
std::string trim_left(const std::string & orig);

// return copy of original string with right-side whitespace trimmed
std::string trim_right(const std::string & orig);

// return copy of orig with to_be_replaced replaced with replacement
std::string string_globally_replace(const std::string & orig,
                                    const std::string & to_be_replaced,
                                    const std::string & replacement);

// return a copy of orig with standard C++ escape sequences internalized
std::string intern_escaped_string(const std::string & orig);

// skip leading whitespace and read a quoted string into str
std::istream & get_quoted_string(std::istream & is,
                                 std::string & str,
                                 char openQ = '\"',
                                 char closeQ = '\"',
                                 char delim = '\n');

  // is string non-empty hex sequence (w/ 0[Xx] prefix)
bool is_hex_digit_string(std::string const & str);

// return a copy of a hex encoded string internalized
std::string intern_hex_encoded_string(std::string const & hex_str);

// skip leading whitespace and read a hex encoded string into str
std::istream & get_hex_string(std::istream & is, std::string & str);
}
#endif /* STRING_UTIL_H */
