#include "RAMVSSD.h"

#include <iomanip>
#include <iostream>
#include <memory>

using namespace std;
#define HEX(x) setw(2) << setfill('0') << hex << (unsigned int)(x)

RAMVSSD::RAMVSSD(size_t block_size, size_t block_count) {
  bc = block_count;
  bs = block_size;

  size_t size = block_size * block_count;

  data = make_unique<char[]>(size);
  stat = DiskStatus::OK;
}

RAMVSSD::~RAMVSSD() { data.release(); }

void RAMVSSD::printRAM() {
  for (unsigned int i = 0; i < bs * bc; i++) {
    if (i == 0 || ((i % bs) == 0)) cout << "\n";
    cout << HEX(data.get()[i]) << " ";
  }
  cout << "\n";
}

size_t RAMVSSD::blockSize() const { return bs; }

size_t RAMVSSD::blockCount() const { return bc; }

DiskStatus RAMVSSD::status() const { return stat; }

DiskStatus RAMVSSD::read(blocknumber_t sector, void* buffer) {
  if (sector >= bc) {
    cout << "ERROR: Index out of range\n";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }
  void* seek = data.get() + (sector * bs);
  memcpy(buffer, seek, bs);
  return DiskStatus::OK;
}

DiskStatus RAMVSSD::write(blocknumber_t sector, void* buffer) {
  stat = DiskStatus::NOT_READY;
  if (sector >= bc) {
    cout << "ERROR: Index out of range\n";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }
  void* seek = data.get() + (sector * bs);
  memcpy(seek, buffer, bs);
  stat = DiskStatus::OK;

  return stat;
}

DiskStatus RAMVSSD::sync() { return stat; }
