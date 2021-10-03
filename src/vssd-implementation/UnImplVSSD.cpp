#include "UnImplVSSD.h"

UnImplVSSD::UnImplVSSD(std::size_t block_size, std::size_t block_count) {
  // do nothing
}

UnImplVSSD::~UnImplVSSD() {
  // do nothing
}

std::size_t UnImplVSSD::blockSize() const {
  return 0;
}

std::size_t UnImplVSSD::blockCount() const {
  return 0;
}

DiskStatus UnImplVSSD::status() const {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus UnImplVSSD::read(blocknumber_t sector, void * buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus UnImplVSSD::write(blocknumber_t sector, void * buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus UnImplVSSD::sync() {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}
