#ifndef BLOCK_UTIL_H
#define BLOCK_UTIL_H
#include <fstream>
#include <string>

namespace block_util {
  // A type for pointers at blocks in memory
  typedef char * BlockPtr;

  /**
   * Fill the block-sized buffer with copies of the pattern. The last
   * copy may be incomplete as the buffer_size may run out.
   *
   * @param block a naked pointer into memory that has room for buffer_size bytes
   * @param block_size the size in bytes of the buffer to be filled
   * @param pattern string containing the byte values to copy across the block
   */
  void fill_block(BlockPtr block, size_t block_size, const std::string & pattern);

  /**
   * Stupid dump of binary data in the buffer into an output stream. No
   * character conversion at all. Bytes print according to the installed
   * code page or other text parameters of the shell.
   *
   * @param fout the output stream to write to
   * @param block the buffer contents to display
   * @param block_size size, in bytes of buffer to print
   */
  std::ostream & dump_block(std::ostream & fout, BlockPtr block, size_t block_size);
}



#endif /* BLOCK_UTIL_H */
