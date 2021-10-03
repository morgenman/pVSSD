#include "RAMVSSD.h"

RAMVSSD::RAMVSSD(std::size_t block_size, std::size_t block_count) {
  // do nothing
}

RAMVSSD::~RAMVSSD() {
  // do nothing
}

std::size_t RAMVSSD::blockSize() const { return 0; }

std::size_t RAMVSSD::blockCount() const { return 0; }

DiskStatus RAMVSSD::status() const { return DiskStatus::NOT_YET_IMPLEMENTED; }

DiskStatus RAMVSSD::read(blocknumber_t sector, void* buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus RAMVSSD::write(blocknumber_t sector, void* buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus RAMVSSD::sync() { return DiskStatus::NOT_YET_IMPLEMENTED; }
