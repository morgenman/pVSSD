#ifndef DISKSTATUS_H
  #define DISKSTATUS_H
#include <string>
/**
 * The type returned as a result of calls to disk functions.
 ***************************************************************
 * Implementation may modify this file between given lines
 ***************************************************************
 */
enum DiskStatus {
    OK,
    NOT_READY,
    BLOCK_OUT_OF_RANGE,
    // Implementation Error Codes Begin: Must update conversion maps, too
    // Implementation Error Codes End
    ERROR,
    NOT_YET_IMPLEMENTED,
    NO_SUCH_STATUS = 255
  };

// Convert a DiskStatus value into a string
std::string toString(DiskStatus ds);

// Convert a string (name of a DiskStatus) into a DiskStatus
DiskStatus fromString(std::string);
  #endif /* DISKSTATUS_H */
