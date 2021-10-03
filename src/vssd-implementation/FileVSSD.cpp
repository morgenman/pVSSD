#include "FileVSSD.h"

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
}

FileVSSD::FileVSSD(string filename) {
  ofstream file;
  fn = filename;
}

FileVSSD::~FileVSSD() { file.close(); }

size_t FileVSSD::blockSize() const { return bs; }

size_t FileVSSD::blockCount() const { return bc; }

DiskStatus FileVSSD::status() const { return DiskStatus::OK; }

DiskStatus FileVSSD::read(blocknumber_t sector, void* buffer) {
  if (sector > bc) return DiskStatus::BLOCK_OUT_OF_RANGE;
  file.seekg((sector + 1) * bs);
  file.read((std::basic_istream<char>::char_type*)buffer, bs);
  return DiskStatus::OK;
}

DiskStatus FileVSSD::write(blocknumber_t sector, void* buffer) {
  file.seekp((sector + 1) * bs);
  file.write((std::basic_istream<char>::char_type*)buffer, bs);
  // file.write()
  return DiskStatus::OK;
}

DiskStatus FileVSSD::sync() { return DiskStatus::NOT_YET_IMPLEMENTED; }
