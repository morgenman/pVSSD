

#include <fstream>
#include <iostream>
#include <string>

#include "VVSSD.h"

/**
 * Example disk implementation to show how to extend a class in C++.
 *
 * All methods indicate that the given function is not implemented.
 */
class FileVSSD : public VVSSD {
 private:
  std::string fn;
  std::fstream file;
  unsigned int bs;
  unsigned int bc;

 public:
  FileVSSD(std::size_t block_size, std::size_t block_count,
           std::string filename);

  FileVSSD(std::string filename);

  virtual ~FileVSSD();

  /**
   * Return the size (in bytes) of the blocks used by this device.
   *o
   * @return 0 and set status if the disk is not ready; number of bytes
   *         per sector otherwise.
   */
  virtual std::size_t blockSize() const;

  /**
   * Return the total number of blocks on the disk.
   *
   * @return 0 and set status if the disk is not ready; number of
   *         sectors otherwise.
   */
  virtual std::size_t blockCount() const;

  /**
   * Return the status of the disk (typically the last call).
   */
  virtual DiskStatus status() const;

  /**
   * Read indicated block if possible.
   *
   * Read the sector of data indicated by the sector number into the
   * provided buffer.
   *
   * @param sector the index of the sector to read; must check for valid range
   * @param buffer pointer to memory where data is to be stored; must be
   *        checked for null
   * @return OK if all is well, appropriate error code otherwise.
   */
  virtual DiskStatus read(blocknumber_t block, void* buffer);

  /**
   * Write indicated block if possible.
   *
   * Write the given buffer of data to the sector with the given sector number.
   *
   * @param sector the index of the sector to write; must check for valid range
   * @param buffer pointer to memory where data is to be stored; must be
   *        checked for null
   * @return OK if all is well, appropriate error code otherwise.
   */
  virtual DiskStatus write(blocknumber_t block, void* buffer);

  /**
   * Synchronize all in-memory structures out to the disk.
   *
   * @return returns a status code for the operation. OK if all went well.
   */
  virtual DiskStatus sync();
};
