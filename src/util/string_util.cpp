#include "string_util.h"
#include <vector>
#include <utility>
#include <iomanip>
#include <iostream>
using namespace std;

namespace str_util {
/**
 * Return a copy of the original string with left-side whitespace trimmed.
 *
 * @param orig original string to copy, except for left-side whitespace
 * @return copy of orig without left-side whitespace (" \t\n\r"); result
 *         will be empty if there are no non-whitespace characters.
 * @note NOT locale aware
 */
string trim_left(const string & orig) {
  size_t nonWhiteSpace = orig.find_first_not_of(" \t\n\r");
  if (nonWhiteSpace == string::npos)
    return "";
  return orig.substr(nonWhiteSpace);
}

/**
 * Return a copy of the original string with right-side whitespace trimmed.
 *
 * @param orig original string to copy, except for right-side whitespace
 * @return copy of orig without right-side whitespace (" \t\n\r"); result
 *         will be empty if there are no non-whitespace characters.
 * @note NOT locale aware
 */
string trim_right(const string & orig) {
  size_t nonWhiteSpace = orig.find_last_not_of(" \t\n\r");
  if (nonWhiteSpace == string::npos)
    return "";
  return orig.substr(0, nonWhiteSpace+1);
}

// string_replace modified from
// https://stackoverflow.com/questions/27617903
//   how-to-parse-a-string-with-raw-escape-sequences
// Note: the escape-handling code on that page is broken.

/**
 * Globally replace instances of to_be_replaced with the replacement string.
 * No recursive replacement (the inserted text is not scanned). Returns a
 * copy with the text replaced.
 *
 * @param orig the original string to be copied with replacements made
 * @param to_be_replaced the string to be replaced in orig; should not be empty
 * @param replacement string to put in place of to_be_replaced
 * @return copy of orig with all instances of to_be_replaced overwritten with
 *         replacement
 */
string string_globally_replace(const string & orig,
                      const string & to_be_replaced,
                      const string & replacement) {
  string result = orig;
  auto pos = result.find(to_be_replaced);
  while (pos != string::npos) {
    result.replace(pos, to_be_replaced.length(), replacement);
    pos = pos + replacement.length();
    pos = result.find(to_be_replaced, pos);
  }

  return result;
}

/**
 * Internalize a C/C++ slash-escaped string. Convert character combinations like
 * '\n' into a new line character. Returns an internalized copy of the string.
 *
 * @param orig original, slash-escaped (possibly) string to internalize
 * @return a copy of orig with \ escape sequences replaced with their internal
 *         representations.
 * @note DOES NOT handle octal, hex, or Unicode character encodings
 */
string intern_escaped_string(const string & orig) {
  string result;
  // Non-standard for loop: < rather than !=; can increment twice (string ends with \)
  for (size_t i = 0; i < orig.length(); ++i) {
    if (orig[i] == '\\') {
      i++;
      auto ch = (i < orig.length())?orig[i]:'\0';

      switch (ch) {
        case '0':
          result += '\0';
          break;
        case 'a':
          result += '\a';
          break;
        case 'b':
          result += '\b';
          break;
        case 'f':
          result += '\f';
          break;
        case 'n':
          result += '\n';
          break;
        case 'r':
          result += '\r';
          break;
        case 't':
          result += '\t';
          break;
        case 'v':
          result += '\v';
          break;
        case '\'':
          result += '\'';
          break;
        case '\"':
          result += '\"';
          break;
        default:
          if (i < orig.length())
            result += ch;
      }
    } else
      result += orig[i];
  }
  return result;
}

/**
 * Does the string consist of a valid hex encoding? "Ox" or "0X" at
 * the beginning and a sequence of hex digits after that.
 *
 * @param str string to check
 * @return true if string is non-empty with only hex digits; false otherwise
 * @cite see https://stackoverflow.com/questions/8899069/
 * how-to-find-if-a-given-string-conforms-to-hex-notation-eg-0x34ff-without-regex
 * for discussion and original version of this code; code extended for
 * 0X/0x.
 */
bool is_hex_digit_string(string const & str) {
  return ((str.compare(0, 2, "0x") == 0) || (str.compare(0, 2, "0X") == 0)) &&
      (str.length() > 2) &&
      (str.find_first_not_of("0123456789abcdefABCDEF", 2) == string::npos);
}

/**
 * Copy a string of the form "0xdeadbeef" into the internal
 * representation of the eight-bit characters represented by the pairs
 * of hex digits, returning the internalized version.
 *
 * @param hex_str a string containing a hex-encoded string: must begin
 *        with 0x or 0X, must be a valid hex encoding of even length
 *        after that.
 * @return returns a string containing the characters described by the hex
 *         sequence is hex_str; if hex_str is not of the right form, the
 *         return string is empty
 * @cite seen somewhere on the 'Net while looking at hex conversion; cannot
 * find matching original as modified to use char[]. strtol was in original
 */
string intern_hex_encoded_string(string const & hex_str) {
  string str;
  if (is_hex_digit_string(hex_str) && (hex_str.length() % 2 == 0)) {
    char hexData[3];
    hexData[2] = '\0';
    for (size_t i = 2; i < hex_str.length(); i += 2) {
      hexData[0] = hex_str[i];
      hexData[1] = hex_str[i+1];
      str += static_cast<char>(strtol(hexData, 0, 16));
    }
  }
  return str;
}

/**
 * Extract characters from is (after initial whitespace), interpreting
 * contiguous pairs as hex encodings of 8-bit characters. The encoded
 * characters are converted into the string they represent.
 *
 * @param is the stream to read from
 * @param str the string to put the hex into
 * @return the stream
 */
istream & get_hex_string(istream & is, string & str) {
  string hex_str;
  if (is.bad()) cout << "already bad!!!" << endl;
  if ((is >> hex_str) &&
      (hex_str.length() % 2 == 0) &&
      is_hex_digit_string(hex_str)) {
    str = hex_str; // empty the string
  } else {
    cout << "hs " << hex_str << endl;
    // we do not know why things are bad but man are they bad. If stream is
    // okay and there is something in the string, put the string back on the
    // stream
    if (is.good()) {
      for (size_t i = 0; i < hex_str.length(); i++)
        is.unget();
      is.setstate(ios_base::failbit);
    }
  }
  return is;
}

/**
 * Extract characters from is, expecting that the first non-whitespace
 * character will be a double quote; extracted characters are stored
 * into str until a matching (unescaped) double quote or delim
 * character is reached. As with std::getline, the terminating
 * character is consumed; unlike getline, the terminating character
 * must come _after_ the starting character (the double quote).
 *
 * @param is the input stream to read
 * @param str where the characters in the quoted string are stored;
 *        the string is unchanged if there is no quoted string to read
 * @param openQ the opening quote character; defaults to double quote
 * @param closeQ the closing quote character; defaults to double quote.
 *        closeQ is escaped by being preceded with a \ and should not,
 *        itself, be the \ character
 * @param delim the delimiter to end on if there is no ending quote
 * @return the input stream
 */
istream & get_quoted_string(istream & is, string & str,
                            char openQ, char closeQ, char delim) {
  char ch;
  // get first non-whitespace character
  if ((is >> skipws >> ch) && (ch == openQ)) {
    // past the leading quote; read to the next one
    str.clear(); // empty the result string now
    while ((is >> noskipws >> ch) && (ch != closeQ) && (ch != delim)) {
      str += ch;
      if (ch == '\\') { // handle escaped character
        if (is >> noskipws >> ch) {
          str += ch;
        } else {
          break;
        }
      } // handle escape
    }
      if (ch != closeQ) {
        // no proper closing quote mark so set the logical fail
        // bit on the input stream unless there is already a set error
        if (is.good())
          is.setstate(ios_base::failbit);
    }
  } else {
    // whitespace is gone; there was no openQ so unget ch and logical error
    // unless there is a previous IO error
    if (is.good()) {
      is.unget();
      is.setstate(ios_base::failbit);
    }
  }
  return is;
}
}
