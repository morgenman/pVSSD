#ifndef VVSSD_H
  #define VVSSD_H
#include <cstdlib>
#include "DiskStatus.h"

/**
 * Abstract virtual class (read: interface) for a Virtual Simple
 * Simulated Disk. This is the base class of all VSSD implementations.
 *
 * Note: if the disk's status is not OK, no disk operation should take
 * place. The function should just return (the bad status if
 * necessary) without modifying the underlying disk.
 *
 * The geometry of a disk is the size of the sectors/blocks it
 * contains and the number of blocks that it contains. These are both
 * required when specifying a disk that is to be initialized (either
 * because this is the first time the disk has been started, or the
 * program wants the disk reset to new condition, or the disk is a
 * non-persistent medium and cannot "remember" any previous time it
 * was started).
 *
 * Limitations on the geometry
 *  - block_size must be at least 32 bytes
 *  - block_count must be at least 2 since the block at disk[0] is reserved for
 *    the disk system itself and is subtracted from the blocks available to the
 *    user.
 *
 */
class VVSSD {
 public:
  /// a name for the type passing block numbers into read and write
  typedef std::size_t blocknumber_t;

  /// the named value for a non-valid block number
  static const blocknumber_t NoBlockNumber = 0;
  /**
   * Any class with virtual functions should have a virtual
   * destructor. Declaring a virtual destructor means the definition
   * is required, even if it does nothing (as the compiler generate
   * one would). So this destructor is declared to be default. The
   * compiler writes and links one in.
   */
  virtual ~VVSSD() = default;

  /**
   * Return the size (in bytes) of the blocks used by this device.
   *
   * @return 0 and set status if the disk is not ready; number of bytes
   *         per block otherwise.
   */
  virtual std::size_t blockSize() const = 0;

  /**
   * Return the total number of blocks on the disk.
   *
   * @return 0 and set status if the disk is not ready; number of
   *         blocks otherwise.
   */
  virtual std::size_t blockCount() const = 0;

  /**
   * Return the status of the disk (typically the last call).
   */
  virtual DiskStatus status() const = 0;

  /**
   * Read indicated block if possible.
   *
   * Read the block of data indicated by the block number into the
   * provided buffer.
   *
   * @param block the index of the block to read; must check for valid range
   * @param buffer pointer to memory where data is to be stored; must be
   *        checked for null
   * @return OK if all is well, appropriate error code otherwise.
   */
  virtual DiskStatus read(blocknumber_t block, void * buffer) = 0;

  /**
   * Write indicated block if possible.
   *
   * Write the given buffer of data to the block with the given block number.
   *
   * @param block the index of the block to write; must check for valid range
   * @param buffer pointer to memory where data is to be stored; must be
   *        checked for null
   * @return OK if all is well, appropriate error code otherwise.
   */
  virtual DiskStatus write(blocknumber_t block, void * buffer) = 0;

  /**
   * Synchronize all in-memory structures out to the disk.
   *
   * @return returns a status code for the operation. OK if all went well.
   */
 virtual DiskStatus sync() = 0;
};
  #endif /* VVSSD_H */
