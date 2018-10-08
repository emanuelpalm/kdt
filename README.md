# Kademlia Peer Implementation

This repository contains a partially implemented node for a DHT based on
Kademlia. It uses LMDB for file storage and OpenSSL for generating safe random
numbers. The code should compile on any POSIX compatible system on which the
mentioned dependencies are available.

The purpose of this application was originally to serve as foundation for
various kinds of peer-to-peer network experiments. Its future is, however,
currently not certain.

## Building and Running

Building requires CMAKE 3.8 or later, git, the OpenSSL development headers,
and the LMDB development headers. If on Ubuntu, the following commands should
install all required dependencies:

```sh
$ sudo apt install cmake git libssl-dev liblmdb-dev
```

On a POSIX system, the following commands should be enough to download and
build the project, assuming all dependencies are installed.

```sh
$ git clone https://github.com/emanuelpalm/kdt.git
$ cd kdt
$ mkdir build
$ cd build
$ cmake ../
$ make
```

Two binaries are produced: namely, `kdt-test` and `kdt`. The former runs a unit
test suite, while the latter starts the application. No command line arguments
are read, so any configuration data has to be provided via environment
variables. The two most significant such are `KDT_INTERFACE` and `KDT_PEER`.
The former selects which local network interface to use, what port, as well as
what internet and transport protocols to use. The latter variable determines
what node, if any, to use for joining the network after the application has
started.

The following example, also assuming a POSIX environment, could be suitable for
joining a local KDT network:

```sh
KDT_INTERFACE="IPv4/TCP 0.0.0.0:39882" KDT_PEER="IPv4/TCP 192.168.0.4" ./kdt
```

## Implementation Details

The only communication method currently implemented uses TCP for reliable
datagram transmission. Both IPv4 and IPv6 are supported, but not at once. To
improve performance, all message buffers are pre-allocated. Custom allocators
are provided for allocating memory for outgoing and ingoing messages,
respectively.

### Repository Organization

The `cmake/` folder contains CMAKE modules.

The `src/main/kdt/` folder contains all project folders, which in turn
significantly contains the following files and directories:

| Entry    | Description                                                     |
|:---------|:----------------------------------------------------------------|
| `kdm/`   | Actual peer implementation, based on the `pnet` module.         |
| `pnet/`  | Peer-to-peer network layer.                                     |
| `def.h`  | Global definitions, affecting memory allocations, timings, etc. |
| `err.h`  | Error type.                                                     |
| `kint.h` | Kademlia integer type, with various utility functions.          |
| `kvs.h`  | Key/value store.                                                |
| `mem.h`  | Memory buffer stream implementation.                            |

Each folder and `.c/.h` file pair in the `src/main/kdt` folder is considered as
being a module. Folders contain a `.h` file with the same name as the folder
serving as entry-point for the module. If a module folder contains a folder
named `internal/`, the contents of that folder must only be accessed directly
from within the module.

The `src/test/` folder contains a unit test library called `unit` as well as
unit tests for some of the `main` folder modules.

## Contributions

Contributions _may_ be considered from any source.

If you are interested in finishing the implementation, a good start could be to
search for `TODO` statements. The following command should list them, if
executed from the project root folder via a POSIX terminal:

```sh
grep -irn --include="*.c" TODO .
```