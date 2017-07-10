# The latria language

[![Build Status](https://travis-ci.org/montymxb/latria.svg?branch=master)](https://travis-ci.org/montymxb/latria)
[![Build status](https://ci.appveyor.com/api/projects/status/g6mo13n1405xks1u?svg=true)](https://ci.appveyor.com/project/montymxb/latria)

A compact, cross platform, embeddable language with an emphasis on multi-language communication and lightweight scripting.

## What's neat about latria?

- can run on **mac**/**linux**/**windows**/**android**/**ios** and more (almost anything since 1990, give or take). 
- a *very* tiny footprint. The compiler and interpreter combined weighs in at ~150kb on unix systems, and about ~250kb on windows.
- it's *fast*, in some cases even faster than [lua](https://www.lua.org/about.html). And it can be optimized to go even faster.
- scripts can be compiled in advance and are system agnostic. A script built by latria on windows can be run by latria on mac, linux, ios, etc.
- very few dependencies, making it easy to build from scratch for your systems specifically.
- can capture the results of another language, allowing you to 'glue' together individual programs/scripts/etc. written in multiple different languages with ease.
- embedding it won't bloat your existing project.
- it's licensed under the MIT open source license

## Getting Started

The [latest release](https://github.com/montymxb/latria/releases/latest/) has binaries for **mac**, **linux** and **windows**. These precombiled versions will work in most cases, provided you have a recent version of either of the aforementioned operation systems. If you need something a little more custom you can always build latria from source by following the [building](#user-content-building) instructions below.

Be sure to read the [guide](https://latria.uphouseworks.com/Guide/) to familiarize yourself with the latria language.

## Building

Latria is capable of being built on linux, mac and windows from the same source. The methods vary however, as will be explained in a moment.

*To note* latria does not use the standard `./configure`, `make`, `make install` approach. Instead you run `make [platform]` and `make install`.
Note that if you're not sure what's available to 'make' you can always run ```make help```

For building on mac you can invoke make as follows.
```
make mac
```

For linux you can invoke make the same way, but for linux.
```
make linux
```

Now for windows there is an alternate Makefile provided specially for `nmake`. You can invoke it and build for windows as follows. Note that you will need to install [Visual Studio](https://www.visualstudio.com/) and have access to a [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs) to have access to `nmake` from a developer command prompt.
```
nmake /F NMakefile latria
```

For building on the 'big 3' that's essentially all you have to do! If you have issues or questions regarding those platforms or any others please open an issue.

## Testing

Testing on Latria is currently run by an embedded suite of tests. You can run them using make as follows:

mac & linux
```bash
# clean if needed
make clean

# build and run just the tests
make mac-test
## OR ##
make linux-test
```

You can run a more comprehensive test on mac/linux as follows:
```bash
# clean if you built before
make clean

# build and run the tests, as well as try all builds
make mac-test-all
### OR ###
make linux-test-all
```


windows
```batch
:: clean if needed
nmake /F NMakefile clean

:: run tests
nmake /F NMakefile test
```

You can run a more comprehensive test on windows as follows:
```batch
:: clean if needed
nmake /F NMakefile clean

:: run tests and try all builds
nmake /F NMakefile test-all
```

If you're having issues with latria consider using [valgrind](http://valgrind.org/) to profile a debuggable build. I highly recommend it, and this will often quickly point you in the right direction. You can make a debuggable build on mac/linux as follows:
```bash
make mac-debug
### OR ###
make linux-debug

# note that a debuggable build with the compiler and interpreter 
# can run it's own tests by passing -t
# it's how 'make test' runs the tests as well
./build/latria-debug -t
```
And for windows:
```batch
nmake /F NMakefile latria-debug

:: note that this deggable build with the compiler and interpreter
:: can also run it's own tests by passing -t
build\latria-debug.exe -t
```

## Contributing

Latria is written in ANSI C, and is licensed under the MIT open source license. Feel free to part it out, improve on it, or implement it in your systems.

If you have suggestions or improvements you may submit a pull request to have a feature or bug fix integrated into the language.

If you plan to contribute please read [Contributing](CONTRIBUTING.md) first.
