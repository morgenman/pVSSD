#include "DiskStatus.h"
#include <map>
using namespace std;
/**
 * Convert a DiskStatus value into a string (of its value in the enum).
 *
 * @param ds the DiskStatus to convert
 * @return string with name of status
 */
string toString(DiskStatus ds) {
  static map<DiskStatus, string> _toString = {
    {OK, "OK"},
    {NOT_READY, "NOT_READY"},
    {BLOCK_OUT_OF_RANGE, "BLOCK_OUT_OF_RANGE"},
    {ERROR, "ERROR"},
    {NOT_YET_IMPLEMENTED, "NOT_YET_IMPLEMENTED"},
    {NO_SUCH_STATUS, "NO_SUCH_STATUS"}
  };
  if (auto match = _toString.find(ds); match != _toString.end())
    return match->second;
  else
    return "UNKNOWN_STATUS";
}

/**
 * Convert a string (hopefully a name of a DiskStatus value) to a DiskStatus
 *
 * @param status string containing the string to convert
 * @return matching DiskStatus if there is one or NO_SUCH_STATUS otherwise
 */
DiskStatus fromString(std::string status) {
  static map<string, DiskStatus> _fromString = {
    {"OK", OK},
    {"NOT_READY", NOT_READY},
    {"BLOCK_OUT_OF_RANGE", BLOCK_OUT_OF_RANGE},
    {"ERROR", ERROR},
    {"NOT_YET_IMPLEMENTED", NOT_YET_IMPLEMENTED}
  };
  if (auto match = _fromString.find(status); match != _fromString.end())
    return match->second;
  else
    return NO_SUCH_STATUS;
}
