#include "FileVSSD.h"

#include <cstring>
#include <iostream>

using namespace std;

FileVSSD::FileVSSD(size_t block_size, size_t block_count, string filename) {
  fn = filename;
  bs = block_size;
  bc = block_count;
  file.open(filename, ios::in | ios::out | ios::binary | ios::trunc);
  file.write((const char*)&bs, sizeof(bs));
  file.write((const char*)&bc, sizeof(bc));

  // Signature: CAFECA75 (Cafe Cats). Hopefully our endianess is synced
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
  char* buffer = new char[4];
  file.read(buffer, 4);
  unsigned int value;
  memcpy(&value, buffer, sizeof(value));
  bs = value;
  file.read(buffer, 4);
  memcpy(&value, buffer, sizeof(value));
  bc = value;

  file.seekg(0);
  std::streampos fsize = 0;
  fsize = file.tellg();

  file.seekg(0, std::ios::end);
  fsize = file.tellg() - fsize;
  if (fsize != bs * (bc + 1)) {
    stat = DiskStatus::ERROR;
    cout << "ERROR: File size does not match header information.\n";
    cout << "\tFile size is: " << fsize << " Bytes.\n";
    cout << "\tExpected file size is: " << bs * (bc + 1)
         << " Bytes (8 Bytes for Geometry, " << bs - 8
         << " Bytes for the signature, \n\tand " << bs * bc
         << " Bytes for data) \n";
  } else
    stat = DiskStatus::OK;
}

FileVSSD::~FileVSSD() { file.close(); }

size_t FileVSSD::blockSize() const { return bs; }

size_t FileVSSD::blockCount() const { return bc; }

DiskStatus FileVSSD::status() const { return stat; }

DiskStatus FileVSSD::read(blocknumber_t sector, void* buffer) {
  stat = DiskStatus::NOT_READY;
  if (sector > bc) {
    cout << "out of range";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }
  file.seekg((sector + 1) * bs);
  file.read((std::basic_istream<char>::char_type*)buffer, bs);
  stat = DiskStatus::OK;
  return stat;
}

DiskStatus FileVSSD::write(blocknumber_t sector, void* buffer) {
  if (stat != DiskStatus::OK) {
    cout
        << "ERROR: Status is not OK, to avoid data loss writing is disabled.\n";
    return stat;
  }
  stat = DiskStatus::NOT_READY;
  if (sector > bc) {
    cout << "out of range";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }
  file.seekp((sector + 1) * bs);
  file.write((std::basic_istream<char>::char_type*)buffer, bs);
  stat = DiskStatus::OK;

  return stat;
}

DiskStatus FileVSSD::sync() { return DiskStatus::NOT_YET_IMPLEMENTED; }
