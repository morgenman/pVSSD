#include "catch_amalgamated.hpp"
#include <cstring>
#include <string>
#include "block_util.h"
#include "VVSSD.h"

using namespace std;

#if (defined __has_include) && __has_include("RAMVSSD.h")
#include "RAMVSSD.h"
using block_util::BlockPtr;

TEST_CASE("RAMVSSD sized, zeroed at construction", "[vssd][ramvssd]") {
  constexpr int block_size = 4096;
  constexpr int block_count = 128;

  constexpr int stride = 7;

  BlockPtr zero = new char[block_size];
  memset(zero, 0, block_size);

  unique_ptr<VVSSD> vssd = make_unique<RAMVSSD>(block_size, block_count);

  BlockPtr block = new char[block_size];

  REQUIRE(vssd->status() == OK);
  REQUIRE(vssd->blockSize() == block_size);
  REQUIRE(vssd->blockCount() == block_count);

  SECTION("blocks initialized to zero") {
    for (auto q = 0; q < block_count; q += stride) {
      vssd->read(q, block);
      REQUIRE(vssd->status() == OK);
      REQUIRE((memcmp(zero, block, block_size) == 0));
    }
  }

  SECTION("check the last block is zero") {
    vssd->read((block_count - 1), block);
    REQUIRE(vssd->status() == OK);
    REQUIRE((memcmp(zero, block, block_size) == 0));
  }

  SECTION("range check works") {
    REQUIRE(vssd->read(block_count, block) == DiskStatus::BLOCK_OUT_OF_RANGE);
  }
}

TEST_CASE("VSSD persistence w/o reopen (cannot reopen)", "[vssd][ramvssd]") {
  constexpr int block_size = 1024;
  constexpr int block_count = 256;

  constexpr int stride = 1;

  BlockPtr zero = new char[block_size];
  memset(zero, 0, block_size);

  unique_ptr<VVSSD> vssd = make_unique<RAMVSSD>(block_size, block_count);

  BlockPtr written = new char[block_size];
  BlockPtr read = new char[block_size];

  const char * testPhrases[] = {
    "Africa, Anarctica, Asia, Australia, Europe, North America, South America",
    "one fish, two fish, red fish, blue fish",
    "X",
  };
  const size_t testPhraseCount = sizeof(testPhrases)/sizeof(testPhrases[0]);

  size_t phraseNdx = 0;

  // write some blocks
  for (auto writeNdx = 0; writeNdx < block_count; writeNdx += stride) {
    block_util::fill_block(written, block_size, testPhrases[phraseNdx]);
    vssd->write(writeNdx, written);
    INFO("writeNdx = " << writeNdx);
    REQUIRE(vssd->status() == OK);
    phraseNdx = (phraseNdx + 1) % testPhraseCount;
  }

  SECTION("read what was written") {
    phraseNdx = 0;

    // read back and check the blocks; -1 on range because of special check
    for (auto readNdx = 0; readNdx < block_count - 1; readNdx += stride) {
      block_util::fill_block(written, block_size, testPhrases[phraseNdx]);
      vssd->read(readNdx, read);
      INFO("phraseNdx = " << phraseNdx);
      INFO("readNdx = " << readNdx);
      REQUIRE(vssd->status() == OK);
      REQUIRE((memcmp(written, read, block_size) == 0));
      phraseNdx = (phraseNdx + 1) % testPhraseCount;
    }

  }

  SECTION("neighboring sectors remain unchanged") {
    for (auto base = 0; base < block_count; base += stride) {
      auto before = base - 1;
      auto after = base + 1;

      if ((0 <= before) && (before != base - stride)) {
        vssd->read(before, read);
        INFO("blockNdx (before) = " << before);
        REQUIRE(vssd->status() == OK);
        REQUIRE((memcmp(zero, read, block_size) == 0));

      }
      if ((after < block_count) && (after != base + stride)) {
        vssd->read(after, read);
        INFO("blockNdx (after) = " << after);
        REQUIRE(vssd->status() == OK);
        REQUIRE((memcmp(zero, read, block_size) == 0));
      }
    }
  }

  // avoid off-by-one errors
  SECTION("last block works"){
    phraseNdx = 0;
    block_util::fill_block(written, block_size, testPhrases[phraseNdx]);
    vssd->write((block_count - 1), written);
    INFO("writeNdx = " << (block_count - 1));
    REQUIRE(vssd->status() == OK);

    block_util::fill_block(written, block_size, testPhrases[phraseNdx]);
    vssd->read((block_count - 1), read);
    INFO("readNdx = " << (block_count - 1));
    REQUIRE(vssd->status() == OK);
    REQUIRE((memcmp(written, read, block_size) == 0));
  }
}
#endif
