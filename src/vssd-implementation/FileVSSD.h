/**
 * FileVSSD Provides a Virtual Simple Simulated Disk stored in memory
 *
 * This implementation works with the fstream library. It has all the
 * functionality of RAMVSSD, plus persistence.
 *
 * @author Dylan C. Morgen
 * @email morgendc203@potsdam.edu
 * @course CIS 310 Operating Systems
 * @assignment pVSSD
 * @due 10/4/2021
 */

#include <fstream>
#include <iostream>
#include <string>

#include "VVSSD.h"

class FileVSSD : public VVSSD {
 private:
  std::string fn;
  std::fstream file;
  unsigned int bs;
  unsigned int bc;
  DiskStatus stat = DiskStatus::NOT_READY;

 public:
  /**
   * FileVSSD Constructor #1
   * Creates a new file. Truncates if necessary.
   *
   * @param  {std::size_t} block_size  : amount of bytes in block
   * @param  {std::size_t} block_count : amount of blocks
   * @param  {std::string} filename    : filename
   */
  FileVSSD(std::size_t block_size, std::size_t block_count,
           std::string filename);

  /**
   * FileVSSD Constructor #2
   * Opens an existing file, if header info is set right
   *
   * @param  {std::string} filename : filename to open
   */
  FileVSSD(std::string filename);

  virtual ~FileVSSD();

  /**
   * Return the size (in bytes) of the blocks used by this device.
   *o
   * @return 0 and set status if the disk is not ready; number of bytes
   *         per sector otherwise.
   */
  std::size_t blockSize() const;

  /**
   * Return the total number of blocks on the disk.
   *
   * @return 0 and set status if the disk is not ready; number of
   *         sectors otherwise.
   */
  std::size_t blockCount() const;

  /**
   * Return the status of the disk (typically the last call).
   */
  DiskStatus status() const;

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
