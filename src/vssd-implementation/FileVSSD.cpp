#include "FileVSSD.h"

using namespace std;

FileVSSD::FileVSSD(size_t block_size, size_t block_count, string filename) {
  fn = filename;
  bs = block_size;
  bc = block_count;
  file.open(filename, ios::in | ios::out | ios::binary | ios::trunc);
  file.write((const char*)&bs, sizeof(bs));
  file.write((const char*)&bc, sizeof(bc));
  file.close();
}

FileVSSD::FileVSSD(string filename) {
  ofstream file;
  fn = filename;
}

FileVSSD::~FileVSSD() {
  // do nothing
}

size_t FileVSSD::blockSize() const { return 0; }

size_t FileVSSD::blockCount() const { return 0; }

DiskStatus FileVSSD::status() const { return DiskStatus::NOT_YET_IMPLEMENTED; }

DiskStatus FileVSSD::read(blocknumber_t sector, void* buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus FileVSSD::write(blocknumber_t sector, void* buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus FileVSSD::sync() { return DiskStatus::NOT_YET_IMPLEMENTED; }
