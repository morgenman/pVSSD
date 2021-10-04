#include "RAMVSSD.h"

#include <stdio.h>
#include <string.h>

#include <iostream>
using namespace std;

RAMVSSD::RAMVSSD(size_t block_size, size_t block_count) {
  // char temp[block_size * (block_count + 1)] = {0};
  bs = block_size;
  bc = block_count;
  unsigned int size = block_size * (block_count + 1);
  cout << "size: " << size << "\n";
  data = (char*)malloc(sizeof(char) * size);
  // seek = data;
  // data[1] = 'a';
  cout << "test";
  // TODO use uniquePointer implementation
  // cout << data[1] << "\n";
}

RAMVSSD::~RAMVSSD() { free(data); }

size_t RAMVSSD::blockSize() const { return 0; }

size_t RAMVSSD::blockCount() const { return 0; }

DiskStatus RAMVSSD::status() const { return DiskStatus::NOT_YET_IMPLEMENTED; }

DiskStatus RAMVSSD::read(blocknumber_t sector, void* buffer) {
  return DiskStatus::NOT_YET_IMPLEMENTED;
}

DiskStatus RAMVSSD::write(blocknumber_t sector, void* buffer) {
  seek = (char*)data;
  seek += sector * bs;
  memcpy(seek, (char*)&buffer, bs);
  return DiskStatus::OK;
}

DiskStatus RAMVSSD::sync() { return DiskStatus::NOT_YET_IMPLEMENTED; }
