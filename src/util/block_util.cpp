#include "block_util.h"
#include <cstring>
using namespace std;

namespace block_util {
  /**
   * Fill the block-sized buffer with copies of the pattern. The last
   * copy may be incomplete as the buffer_size may run out.
   */
  void fill_block(BlockPtr block, size_t block_size, const string & pattern) {
    size_t offset = 0;
    while ((block_size - offset) > pattern.length()) {
      memcpy(block + offset, pattern.data(), pattern.length());
      offset += pattern.length();
    }
    if (block_size > offset)
      memcpy(block + offset, pattern.data(), block_size - offset);
  }

  ostream & dump_block(ostream & fout, BlockPtr block, size_t block_size) {
    for (size_t i = 0; i < block_size; ++i)
      fout << block[i];
    return fout;
  }

}
