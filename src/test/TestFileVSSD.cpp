#include "catch_amalgamated.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include "block_util.h"
#include "VVSSD.h"

using namespace std;

#if (defined __has_include) && __has_include("FileVSSD.h")
#include "FileVSSD.h"
using block_util::BlockPtr;

TEST_CASE("FileVSSD sized, zeroed at construction", "[vssd][filevssd]") {
  constexpr int block_size = 4096;
  constexpr int block_count = 128;

  constexpr int stride = 7;

  BlockPtr zero = new char[block_size];
  memset(zero, 0, block_size);

  const char * fname = "FileVSSD-temp-data-file-zero.dat";

  unique_ptr<VVSSD> vssd = make_unique<FileVSSD>(block_size, block_count, fname);

  BlockPtr block = new char[block_size];

  REQUIRE(vssd->status() == OK);
  REQUIRE(vssd->blockSize() == block_size);
  REQUIRE(vssd->blockCount() == block_count);

  SECTION("blocks initialized to zero") {
    for (auto readNdx = 0; readNdx < block_count; readNdx += stride) {
      vssd->read(readNdx, block);
      INFO("readNdx = " << readNdx);
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

  // erase the file; could comment out to examine it
  remove(fname);
}

TEST_CASE("FileVSSD persistent w/o reopening", "[vssd][filevssd]") {
  constexpr int block_size = 2048;
  constexpr int block_count = 512;

  constexpr int stride = 13;

  BlockPtr zero = new char[block_size];
  memset(zero, 0, block_size);

  const char * fname = "FileVSSD-temp-data-file-persist.dat";

  unique_ptr<VVSSD> vssd = make_unique<FileVSSD>(block_size, block_count, fname);

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

  // erase the file; could comment out to examine it
  remove(fname);

}


TEST_CASE("FileVSSD persistent w/ reopening", "[vssd][filevssd]") {
  constexpr int block_size = 1024;
  constexpr int block_count = 256;

  constexpr int stride = 11;

  BlockPtr zero = new char[block_size];
  memset(zero, 0, block_size);

  const char * fname = "FileVSSD-temp-data-file-persist-reopen.dat";

  unique_ptr<VVSSD> vssd = make_unique<FileVSSD>(block_size, block_count, fname);

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

  // Release and reopen the data file
  vssd = nullptr;
  vssd = make_unique<FileVSSD>(fname);

  SECTION("reopen with correct settings") {
    REQUIRE(vssd->status() == OK);
    REQUIRE(vssd->blockSize() == block_size);
    REQUIRE(vssd->blockCount() == block_count);
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
  // erase the file; could comment out to examine it
  remove(fname);
}

TEST_CASE("FileVSSD will not open broken files", "[vssd][filevssd]") {
  constexpr int block_size = 1024;
  constexpr int block_count = 256;

  BlockPtr zero = new char[block_size];
  memset(zero, 0, block_size);
  const char *fnameNoSignature = "FileVSSD-temp-data-file-no-signature.dat";

  const char * text = R"(
           We are uncovering better ways of developing
          software by doing it and helping others do it.
             Through this work we have come to value:

       Individuals and interactions over processes and tools
         Working software over comprehensive documentation
         Customer collaboration over contract negotiation
            Responding to change over following a plan

          That is, while there is value in the items on
         the right, we value the items on the left more.
)";

  std::ofstream out(fnameNoSignature);
  out << text;
  out.close();

  unique_ptr<VVSSD> vssd = make_unique<FileVSSD>(fnameNoSignature);
  SECTION("Will not open improper format") {
    REQUIRE(vssd->status() != OK);
  }

  const char *fnameBadSize = "FileVSSD-temp-data-file-bad-size.dat";
  vssd = make_unique<FileVSSD>(block_size, block_count,fnameBadSize);

  vssd = nullptr;
  // lop off one block.
  truncate(fnameBadSize, (block_count - 1) * block_size);

  vssd = make_unique<FileVSSD>(fnameBadSize);
  SECTION("will not open improper size") {
    REQUIRE(vssd->status() != OK);
  }

  remove(fnameNoSignature);
  remove(fnameBadSize);
}
#endif
