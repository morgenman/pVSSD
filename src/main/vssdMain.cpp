#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "UnImplVSSD.h"
#include "VVSSD.h"
#include "block_util.h"
#include "string_util.h"

// commented out until they are actually implemented; remove comments
// as each type of disk is implemented
#if (defined __has_include) && __has_include("FileVSSD.h")
#include "FileVSSD.h"
#define has_FileVSSD 1
#endif
#if (defined __has_include) && __has_include("RAMVSSD.h")
#include "RAMVSSD.h"
#define has_RAMVSSD 1
#endif
using namespace std;

// line number in the input; used for reporting errors
unsigned int line_number = 0;

/* Question: is this code designed to permit some sort of #include type
   command so that one stream of input could open another, finish it, and
   then resume the first? Why do you answer as you do? This might make an
   interesting coding question on an exam.
*/

/**
 * Create a disk with the given parameters. Note that the call to the
 * templated function make_unique<> takes the same parameters as one of the
 * constructors for the class in the angle brackets.
 *
 * Further, for this to compile, the type in the brackets must be a class
 * that extends VVSSD. The commented out lines should be included as the
 * different types of disk become operational.
 *
 * @param dType string encoding of the disk type
 * @param block_size required block size for the disk being opened; must be > 0
 * @param block_count required number of blocks in the disk; must be > 0
 * @param filename name of file for disk types that need it; defaults to ""
 * @param truncate should the disk be reinitialized? defaults to false
 * @return a pointer to the newly created disk or nullptr.
 */
unique_ptr<VVSSD> make_disk(string dType, size_t block_size, size_t block_count,
                            const string &filename = "",
                            bool truncate = false) {
  if (dType == "UnImplVSSD") {
    return make_unique<UnImplVSSD>(block_size, block_count);
  }
  // conditionally compile in the FileVSSD code. Only if we included the
  // matching header file (where we defined the macro as 1)
#if has_FileVSSD
  if (dType == "FileVSSD") {
    if (truncate)  // new file
      return make_unique<FileVSSD>(block_size, block_count, filename);
    else {  // existing file
      auto retval = make_unique<FileVSSD>(filename);
      return retval;
    }
  }
#endif

  // conditionally compile in the RAMVSSD code. Only if we included the matching
  // header file (where we defined the macro as 1)
#if has_RAMVSSD
  if (dType == "RAMVSSD") {
    return make_unique<RAMVSSD>(block_size, block_count);
  }
#endif
  return nullptr;
}

/**
 * Display an error on the standard error stream.
 *
 * @param message required message about the error
 * @param blocknumber optional block number to include
 * @param ds optional disk status
 */
void error(const string &message,
           VVSSD::blocknumber_t block = VVSSD::NoBlockNumber,
           DiskStatus ds = NO_SUCH_STATUS) {
  cerr << line_number << ": " << message;
  if (block != VVSSD::NoBlockNumber) cerr << " block[" << block << "]";
  if (ds != NO_SUCH_STATUS) cerr << " DiskStatus = " << toString(ds);
  cerr << endl;
}

/**
 * Read a pattern of bytes from the given input stream. The pattern is
 * either a quoted string or a sequence of hex-encoded
 * characters. Which it is is determined by the first non-whitespace
 * character encountered.
 *
 * A quoted string can include C++ string literal escaped characters
 * (except for character entities in octal, hex, or Unicode) and they
 * will be internalized in the returned string. A hex-encoded string
 * is formatted like a C++ hex literal except that there must be an
 * even number of characters since it encodes a sequence of 8-bit
 * characters; the hex codes are internalized to the equivalent characters
 * and that string is returned.
 *
 * @param is the input stream to read from
 * @return a string with the internalized value of the hex or quoted
 * string if all goes well; an empty string if there were any
 * problems.
 */
string read_operand(istream &is) {
  string str;
  char ch;
  if (is >> skipws >> ch) {
    if (ch == '\"') {
      is.unget();
      if (str_util::get_quoted_string(is, str)) {
        return str_util::intern_escaped_string(str);
      }
    } else if (ch == '0') {
      if ((is >> noskipws >> ch) && ((ch == 'X') || (ch == 'x'))) {
        is.unget();
        is.unget();
        if (str_util::get_hex_string(is, str)) {
          return str_util::intern_hex_encoded_string(str);
        }
      } else {
        is.unget();
        is.unget();
      }
    } else {
      is.unget();
    }
  }
  return str;
}

/**
 * Read a DiskStatus value from the input stream
 *
 * @param is the input stream to read
 * @return the parsed disk status of the next token or NO_SUCH_STATUS on error
 */
DiskStatus read_disk_status(istream &is) {
  string status;

  if (is >> status) {
    return fromString(status);
  } else {
    return NO_SUCH_STATUS;
  }
}

istream &overws(istream &is) {
  char ch;
  if (is >> skipws >> ch) is.unget();
  return is;
}

/**
 * Disconnect the disk/block pair.
 *
 * @param disk the disk pointer to make sure is not connected
 * @param block the block pointer to clean up and disconnect
 * @note the passing (and setting of these) as a pair all of the time
 * implies they should be put into a class/struct of some sort.
 */
void disconnect(unique_ptr<VVSSD> &disk, block_util::BlockPtr &block) {
  disk = nullptr;  // unique_ptr calls the delete automatically
  if (block != nullptr) delete[] block;
  block = nullptr;
}

/**
 * main program
 */
int main(int argc, char *argv[]) {
  unique_ptr<VVSSD> disk = nullptr;
  block_util::BlockPtr block = nullptr;
  cout << "Welcome to the Very Simple Simulated Disk protocol.\n";
  cout << "Commands:\n";
  cout << "\tc <block-size> <block-count> [o <fname> | n <fname> | r]\n";
  cout << "\t\t(c)onnect to a disk. Either o(pen) an existing filedisk,"
          "\n\t\tcreate a (n)ew filedisk, or use a (r)AM disk.\n\n";
  cout << "\tw <block> <data> [s <disk_status>]\n";
  cout << "\t\t(w)rite the data to the block number. \n\t\tOptionally check "
          "(s)tatus after operation\n\n";
  cout << "\tr <block> [x <data>] [s <disk_status>]\n";
  cout << "\t\t(r)ead the data from the given block number.\n"
          "\t\tOptionally create block of data from <data> "
          "and e(x)amine whether \n\t\tthe value matches. If not examined, "
          "dump the block.\n"
          "\t\tOptionally check (s)tatus after operation\n\n";
  cout << "\td\t(d)isconnect from system, shut down any open disks\n\n";
  cout << "\tq\t(q)uit the simulation\n\n";

  const string valid_commands = "cdqrsw";
  string prompt = "cmd: ";
  string line;
  while ((cout << prompt) && getline(cin, line)) {
    line_number++;
    line = str_util::trim_left(str_util::trim_right(line));

    if (line.empty()) continue;

    stringstream cmdin(line, ios::in);
    string cmd;

    if (!(cmdin >> cmd)) {
      error("unable to read command from \"" + line + "\"");
      break;
    }

    if (valid_commands.find_first_of(cmd) == string::npos) {
      error("unknown command in \"" + line + "\"");
      continue;
    }

    if (cmd == "q") {
      break;
    } else if (cmd == "c") {
      disconnect(disk, block);
      size_t block_size;
      size_t block_count;
      if ((cmdin >> block_size) && (cmdin >> block_count)) {
        string d_type;
        if (cmdin >> d_type) {
          if ((d_type == "n") || (d_type == "o")) {
            // file-based
            string fname;
            if (cmdin >> fname) {
              bool init = (d_type == "n");
              disk =
                  make_disk("FileVSSD", block_size, block_count, fname, init);
              if (disk->status() == OK) {
                block = new char[disk->blockSize()];
              } else {
                error("connecting error", VVSSD::NoBlockNumber, disk->status());
                disconnect(disk, block);
              }
            } else {
              error("missing file name for disk image device");
            }
          } else if (d_type == "r") {
            disk = make_disk("RAMVSSD", block_size, block_count);
            if (disk->status() == OK) {
              block = new char[disk->blockSize()];
            }
          } else if (d_type == "u") {
            disk = make_disk("UnImplVSSD", block_size, block_count);
          } else {
            error("no disk connected: \"" + d_type + "\" unknown disk type");
          }
        } else {
          error("no valid disk type for connection");
        }
      } else {
        error("missing <block_size> or <block_count> for connection");
      }
    } else if (disk ==
               nullptr) {  // -------------------------------------------
      // no need to process other commands; there is no connected device
      error("no disk currently connected");
      continue;
    } else if (cmd == "d") {
      disconnect(disk, block);
    } else if (cmd == "w") {
      size_t blockNumber;
      string operand;
      if (cmdin >> blockNumber) {
        operand = read_operand(cmdin);
        if (operand != "") {
          if (overws(cmdin)) {
            cout << "there is more!" << endl;
          }
          block_util::fill_block(block, disk->blockSize(), operand);
          DiskStatus ds = disk->write(blockNumber, block);
          if (ds != OK) {
            error("write generates an error", blockNumber, ds);
          }
        } else {
          error("missing <pattern> for write");
        }
      } else {
        error("missing <block_number> for write");
      }
    } else if (cmd == "r") {
      size_t blockNumber;
      string operand;
      if (cmdin >> blockNumber) {
        DiskStatus ds = disk->read(blockNumber, block);
        if (ds != OK) {
          error("read generates an error", blockNumber, ds);
        } else {
          block_util::dump_block(cout, block, disk->blockSize());
        }
      } else {
        error("missing <block_number> for read");
      }
    } else if (cmd == "s") {
    } else {
    }
  }
}
