/**
 * RAMVSSD Provides a Virtual Simple Simulated Disk stored in memory
 *
 * This implementation works using the unique_ptr method in c++
 * It has all the functionality of the file based implementation aside from
 * persistence
 *
 * @author Dylan C. Morgen
 * @email morgendc203@potsdam.edu
 * @course CIS 310 Operating Systems
 * @assignment pVSSD
 * @due 10/4/2021
 */

#include <cstring>
#include <memory>
#include <string>

#include "VVSSD.h"

class RAMVSSD : public VVSSD {
 private:
  std::unique_ptr<char[]> data;
  unsigned int bs;
  unsigned int bc;
  DiskStatus stat = DiskStatus::NOT_READY;

  /**
   * printRAM prints out the ram in HEX. It shows you how data is stored.
   * This is not used by the current implementation, but is useful for
   * diagnosing issues.
   */
  void printRAM();

 public:
  /**
   * RAMVSSD is the constructor. It does not have to initalize all bytes to 0,
   * that happens automatically.
   *
   * @param  {std::size_t} block_size  : the amount of bytes in each block.
   * @param  {std::size_t} block_count : the amount of blocks
   */
  RAMVSSD(std::size_t block_size, std::size_t block_count);

  /**
   * ~RAMVSSD is the destructor. Frees memory
   *
   */
  virtual ~RAMVSSD();

 public:
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
