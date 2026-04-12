# ClockDB

ClockDB is a db builder and fuzzy searcher for Pokemon's SMUSUM clock lookup.

This has been made compatable with 3DSRngTool is [this fork](https://github.com/GoldenBushRobin/3DSRNGTool).

## Requirements

The requirements are cmake and make, along with C11 and C++20 compilers with openmp support. For the server, [crowcpp](https://crowcpp.org/) is required.

## Getting Started

At the project root directory, create a build directory.

```shell
mkdir build && cd build
```

Build the make file and compile binaries.

```shell
cmake .. && make
```

Build the database with the desired offset.

For the main rng, the offset is 417 for Sun/Moon and 477 for UltraSun/UltraMoon. For id generation, offset is 1012 for Sun/Moon and 1132 for UltraSun/UltraMoon.

```shell
./build --offset 477
```

Can search the database for seeds after building the database for a specific offset

```shell
./search --offset 477
```

After building your databases, you can run a local server for queries

```shell
./server
```

## Future Development

Docker support for building and searching

Local http server for querying
