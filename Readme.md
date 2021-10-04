# Implementing a Virtual Simple Simulated Disk
Dylan Morgen
morgendc203@potsdam.edu

# Required Documentation:

## Problem
In this assignment I implemented RAMVSSD and FileVSSD. 
Catch2 framework was provided, along with tests. 

## Testing Criteria

### Test Input
To test, run ./build/vssdTest 

### Expected Output
There will be error messages provided by me. Passing will show the last line of
output as such:
`All tests passed (1988 assertions in 6 test cases)`


### Compilation and Execution Instructions
```make``` 
will make all three executables


# Provided Documentation:

## Introduction
This test code is provided to permit testing of implementations of the Very Simple Simulated Disk protocol. The Virtual Very Simple Simulated Disk class defines the "driver" API for VSSD and the test code exercises the disks through that interface. The only other function calls are to constructors for the implementation classes (think of this as turning on a connected disk drive) and the associated destructors (shutting down the disk).

## User Interaction
### Start up
Run the built executable. It takes no command-line arguments. When run it waits for the user to type commands. Note: a command must appear on a single line.
### Commands
   When run, the program reads `cin` a line at a time. Each line is parsed as a command. Note this means that a command must all be typed on a single line.
#### `q`
    Quit the simulation. Shut down any open disk and terminate.
#### `d`
    Disconnect. Shut down any open disk.
#### `c <block-size> <block-count> [o <fname> | n <fname> | r]`
    Connect to the disk with the given geometry and
    `o <fname>` connect the given file-based simulated disk.
      - If the file does not exist or is not of the right size given the disk geometry an error is reported.
    `n <fname>` create (or open, truncate), initialize and connect the given file-based simulated disk.
      - Initializing the file means writing the given number of blocks of the given size /filled with zero (0) bytes/.
    `r` create, initialize, and connect a memory-based simulated disk.
      - To set aside the memory for the RAM disk, use `new` on a `char` array (a `char` is one byte in size).
      - To initialize the RAM disk, /fill its extent with zero (0) bytes/ (`memset` in `<cstring>` can help with filling buffers with a fixed value; `memcpy` can be used to read/write sectors on the RAM disk).

#### `w <block> <data> [s <disk_status>]`
    Write the data to the given block number.
    - `[s <disk_status]` optionally check the disk status after the operation.

#### `r <block> [x <data>] [s <disk_status>]`
    Read the data from the given block number. If not examined, dump the block.
    - `[x <data>]` optionally create a block of data from `<data>` and examine the read value to see if it matches.
    - `[s <disk_status]` optionally check the disk status after the operation.

#### Data
    On commands that require data, data can be provided as a hex-encoded string or a C++ style quoted string.
    - `0x414243444546474849` encodes the string "ABCDEFGHI". Any 8-bit byte value (including 0x00) is acceptable in the string. There must be an even number of nybbles and there must be at least one byte of data.
    - `"one fish\ntwo fish"` encodes the string with a carriage return in the middle. Standard esc1ape sequences #except## \0..., \x..., \u..., and \U... should all work.

   When data is used for a block, the data string is replicated to fill the block completely. The last copy of the string may be incomplete.

## Design Considerations
---------------------------------------------------------------------------------
Note: the commands assume you are in the same directory as `Makefile`
---------------------------------------------------------------------------------

### To Clean
Removes all compiled files so that the next compilation will be complete rather than
incremental.
```shell
make clean
```

### To Compile
```shell
make
```

### To Test
To keep them simple to read, this project has no tests.

## To Run
Each project, when built, shows the name of the executable as the parameter of the -o commandline argument. So, in TextFileReading, the commandline make uses is
```shell
g++ -std=c++17 -Wall -Werror -g  -o build/readBy ...
```

This tells you that the executable is build in `./build/readBy`. The executable is named in the `Makefile`; the source file containing the `main` function is in `src/main/`.

To run the program, type its path at the commandline
```shell
build/readBy
```

If it takes commandline parameters, they come after the executable
