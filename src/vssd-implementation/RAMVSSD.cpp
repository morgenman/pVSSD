#include "RAMVSSD.h"

#include <stdio.h>
#include <string.h>

#include <iomanip>
#include <iostream>

using namespace std;
#define HEX(x) setw(2) << setfill('0') << hex << (int)(x)

RAMVSSD::RAMVSSD(size_t block_size, size_t block_count) {
  bs = block_size;
  bc = block_count;
  int size = block_size * (block_count + 1);
  data = make_unique<unsigned char[]>(size);
  seek = data.get();
  unsigned char temp[size] = {1};
  cout << "0 is: " << (int)temp[0] << ", 5 is: " << (int)temp[4] << "\n";
  memcpy(seek, &temp, size);
  memcpy(seek, &bs, sizeof(bs));
  seek += sizeof(bs);
  memcpy(seek, &bc, sizeof(bc));
  seek += sizeof(bc);

  printRAM();
}
void RAMVSSD::printRAM() {
  int size = bs * (bc + 1);
  for (int i = 0; i < size; i++) {
    if (i == 0 || ((i % bs) == 0)) cout << "\n";
    cout << HEX(seek[i]) << " ";
  }
  cout << "\n";
}

RAMVSSD::~RAMVSSD() { data.release(); }

size_t RAMVSSD::blockSize() const { return 0; }

size_t RAMVSSD::blockCount() const { return 0; }

DiskStatus RAMVSSD::status() const { return DiskStatus::NOT_YET_IMPLEMENTED; }

DiskStatus RAMVSSD::read(blocknumber_t sector, void* buffer) {
  seek = (unsigned char*)data.get();
  seek += sector * bs;
  memcpy(buffer, seek, bs);
  return DiskStatus::OK;
}

DiskStatus RAMVSSD::write(blocknumber_t sector, void* buffer) {
  seek = (unsigned char*)data.get();
  seek += sector * bs;
  memcpy(seek, buffer, bs);
  return DiskStatus::OK;
}

DiskStatus RAMVSSD::sync() { return DiskStatus::NOT_YET_IMPLEMENTED; }
