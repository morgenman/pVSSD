/**
 * See RAMVSSD.h for header comment
 */

#include "RAMVSSD.h"

#include <iomanip>
#include <iostream>
#include <memory>

using namespace std;

// Some secret sauce of mine, stolen from stack overflow, used in printRAM()
#define HEX(x) setw(2) << setfill('0') << hex << (unsigned int)(x)

RAMVSSD::RAMVSSD(size_t block_size, size_t block_count) {
  // Set count and size variables
  bc = block_count;
  bs = block_size;

  // size_t is an unsigned long
  size_t size = block_size * block_count;

  // make_unique returns a pointer to a dynamically managed block of memory
  // `size` bytes long
  data = make_unique<char[]>(size);

  // Data should be 0'd out by default

  stat = DiskStatus::OK;
}

RAMVSSD::~RAMVSSD() { data.release(); }

void RAMVSSD::printRAM() {
  // For every byte
  for (unsigned int i = 0; i < bs * bc; i++) {
    // NL on block. Each row is one block now.
    if (i == 0 || ((i % bs) == 0)) cout << "\n";
    // Format to hex and print. See the macro for HEX()
    cout << HEX(data.get()[i]) << " ";
  }
  cout << "\n";
}

size_t RAMVSSD::blockSize() const { return bs; }

size_t RAMVSSD::blockCount() const { return bc; }

DiskStatus RAMVSSD::status() const { return stat; }

DiskStatus RAMVSSD::read(blocknumber_t sector, void* buffer) {
  // Range Checking
  if (sector >= bc) {
    cout << "ERROR: Index out of range\n";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }

  // I could not for the life of me get this to work inline. So it's defined
  // explicitly
  void* seek = data.get() + (sector * bs);
  // Copy bs bytes from seek (void*) which points at relevant chunk of data to
  // buffer (void*)
  memcpy(buffer, seek, bs);

  stat = DiskStatus::OK;
  return stat;
}

DiskStatus RAMVSSD::write(blocknumber_t sector, void* buffer) {
  // Set to not ready. If command fails, status will be still not ready
  stat = DiskStatus::NOT_READY;

  // Range Checking
  if (sector >= bc) {
    cout << "ERROR: Index out of range\n";
    stat = DiskStatus::BLOCK_OUT_OF_RANGE;
    return stat;
  }

  // Not done inline as per earlier comment.
  void* seek = data.get() + (sector * bs);
  // Copy bs bytes from buffer (void*) to seek (void*), which points at relevant
  // chunk of data
  memcpy(seek, buffer, bs);

  stat = DiskStatus::OK;
  return stat;
}

DiskStatus RAMVSSD::sync() { return stat; }
