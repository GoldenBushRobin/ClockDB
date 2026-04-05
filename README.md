# ClockDB

ClockDB is a db builder and fuzzy searcher for Pokemon's SMUSUM clock lookup.

## Requirements

The requirements are cmake and make, along with C11 and C++20 compilers with openmp support.

## Getting Started

At the project root directory, create a build directory.

```shell
mkdir build && cd build
```

Build the make file and compile binaries.

```shell
cmake .. && make
```

Build the database with the desired offset .
For the main rng, the offset is 417 for Sun/Moon and 477 for UltraSun/UltraMoon.
For the id generation, the offset is 1012 for Sun/Moon and 1132 for UltraSun/UltraMoon.

```shell
./build --offset 477
```

Can search the database for seeds after building the database for a specific offset

```shell
./search --offset 477
```

## Future Development

Docker support for building and searching

Local http server for querying