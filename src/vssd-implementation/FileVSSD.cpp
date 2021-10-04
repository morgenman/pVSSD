/**
 * See FileVSSD.h for header comment
 */

#include "FileVSSD.h"

#include <cstring>
#include <iostream>

using namespace std;

FileVSSD::FileVSSD(size_t block_size, size_t block_count, string filename) {
  // Set count, size, and name variables. fn is used by sync().
  fn = filename;
  bs = block_size;
  bc = block_count;

  // Open the file, truncating
  file.open(filename, ios::in | ios::out | ios::binary | ios::trunc);
  // Write block info
  file.write((const char*)&bs, sizeof(bs));
  file.write((const char*)&bc, sizeof(bc));

  // Signature: CAFECA75 (Cafe Cats). Hopefully our endianess is synced
  // If not, swap cases 1 and 3
  unsigned char ca = 202;
  unsigned char fe = 254;
  unsigned char ts = 117;
  unsigned char zero = 0;
  if (bs > 8) {
    for (unsigned int i = 8; i < bs; i++) {
      switch (i % 4) {
        case 0:
        case 2:
          file.write((const char*)&ca, sizeof(ca));
          break;
        case 1:
          file.write((const char*)&fe, sizeof(fe));
          break;
        case 3:
          file.write((const char*)&ts, sizeof(ts));
          break;
      }
    }
  }

  // Zero out the rest
  for (unsigned int i = 0; i < block_count; i++) {
    for (unsigned int j = 0; j < block_size; j++) {
      file.write((const char*)&zero, sizeof(zero));
    }
  }

  stat = DiskStatus::OK;
}

FileVSSD::FileVSSD(string filename) {
  fn = filename;

  file.open(filename, ios::in | ios::out | ios::binary);

  // file size calculation. fsize is the filesize.
  std::streampos fsize = 0;
  // Seek to beginning of file
  file.seekg(0);
  // Set the size to the read position (should be 0)
  fsize = file.tellg();
  // Seek to end of file
  file.seekg(0, std::ios::end);
  // Set size to end-beginning.
  fsize = file.tellg() - fsize;
  // Seek back to beginning
  file.seekg(0);

  // Make sure file exists.
  if (fsize == 0) {
    cout << "ERROR: File '" << filename
         << "' does not exist. Unable to open.\n";
    stat = DiskStatus::ERROR;
  } else {
    // Value is were header ints will be stored temporarily
    unsigned int value;
    // Buffer is for reading header ints.
    char* buffer = new char[4];
    // Read four bytes into the buffer
    file.read(buffer, 4);
    // Copy the first four bytes into value. Should be a readable int now
    memcpy(&value, buffer, sizeof(value));
    // Copy over to block size variable
    bs = value;
    // Read four more bytes (seeked already)
    file.read(buffer, 4);
    // Copy over into value
    memcpy(&value, buffer, sizeof(value));
    // Copy into block size variable
    bc = value;

    // Check size in header to see if it matches actual filesize
    if (fsize != bs * (bc + 1)) {
      stat = DiskStatus::ERROR;
      cout << "ERROR: File size does not match header information.\n";
      cout << "\tFile size is: " << fsize << " Bytes.\n";
      cout << "\tExpected file size is: " << bs * (bc + 1)
           << " Bytes (8 Bytes for Geometry, " << bs - 8
           << " Bytes for the signature, \n\tand " << bs * bc
           << " Bytes for data) \n";
    } else {
      // Read four more bytes (seeked already)
      file.read(buffer, 4);
      // Copy over into value
      memcpy(&value, buffer, sizeof(value));
      // check to see if signature is right
      if (value != 1976237770) {
        stat = DiskStatus::ERROR;
        cout << "ERROR: Signature does not match CA FE CA 75\n";
      } else
        stat = DiskStatus::OK;
    }
  }
}

FileVSSD::~FileVSSD() { file.close(); }

size_t FileVSSD::blockSize() const { return bs; }

size_t FileVSSD::blockCount() const { return bc; }

DiskStatus FileVSSD::status() const { return stat; }

DiskStatus FileVSSD::read(blocknumber_t sector, void* buffer) {
  // Range Checking
  if (sector >= bc) {
    cout << "ERROR: Index out of range\n";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }

  // Seek to the right sector. +1 for header sector
  file.seekg((sector + 1) * bs);
  // Don't ask me why this type-casting is needed please
  file.read((std::basic_istream<char>::char_type*)buffer, bs);
  return stat;
}

DiskStatus FileVSSD::write(blocknumber_t sector, void* buffer) {
  // The following code block allows you to easily disable write on unsafe
  // DiskStatus.

  // if (stat != DiskStatus::OK) {
  //   cout
  //       << "ERROR: Status is not OK, to avoid data loss writing is
  //       disabled.\n";
  //   return stat;
  // }

  // Set to not ready. If command fails, status will still be not ready
  stat = DiskStatus::NOT_READY;

  // Range Checking
  if (sector >= bc) {
    cout << "ERROR: Index out of range\n";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }

  // Seek to the right sector. +1 for header sector
  file.seekp((sector + 1) * bs);
  // Don't ask me why this type-casting is needed please
  file.write((std::basic_istream<char>::char_type*)buffer, bs);

  stat = DiskStatus::OK;
  return stat;
}

DiskStatus FileVSSD::sync() {
  // Pretty sure this is never called.
  stat = DiskStatus::NOT_READY;
  file.close();
  file.open(fn, ios::in | ios::out | ios::binary);
  stat = DiskStatus::OK;
  return stat;
}
