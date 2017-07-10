# latria

[![Build Status](https://travis-ci.org/montymxb/latria.svg?branch=master)](https://travis-ci.org/montymxb/latria)
[![Build status](https://ci.appveyor.com/api/projects/status/g6mo13n1405xks1u?svg=true)](https://ci.appveyor.com/project/montymxb/latria)

A compact, cross platform, embeddable language with an emphasis on multi-language communication and lightweight scripting.
<br/><br/>
Built following ANSI C, latria can run on mac/linux/windows/android/ios and more.<br/>

<h2>Binaries</h2>

The [latest release](https://github.com/montymxb/latria/releases/latest/) has binaries for **mac**, **linux** and **windows**.

<h2>about.</h2>

latria was born out a desire to understand language.

As scripters or programmers we wrestle everyday to communicate with machines all around us. Our ability to harness a computer is directly linked to how well we can communicate with it.

Generally this leads to a desire for shorter, more succinct statements that convey intention with unmistakable clarity.

Latria leverages a high level programming design to maximize verbosity without increasing workload. 


<h2>cross lang.</h2>

Bridge the language barrier, use all your &lt;code/&gt;

Traditionally, one would pick a particular language and stay with it, exclusively.

Over time our preferences change and we learn which languages may be best suited for certain tasks in our mind.

Like any developer you most likely have various scripts and programs written for various tasks, in various languages.

Latria can mediate quite nicely, allowing you to now only run various system level commands and collect their results in one fell swoop.

Bring back your code! Latria can handle the output itself or pipe it into another program, bringing your various libraries new purpose in your projects, no need to rewrite your old code base for a small project.

<h2>open source goodness.</h2>

Latria is written in ANSI C, and is licensed under the MIT open source license. Feel free to part it out, improve on it, or implement it in your systems. This is an early build, and is still being actively built.

If you have suggestions or improvements you may submit a pull request to have a feature or bug fix integrated into the language.

Additionally documentation in the form of a guide is available at <a href="http://latria.uphouseworks.com">latria.uphouseworks.com</a>, allowing anyone to get started with latria scripting.

In the meantime feel free to reach out to me at friedman.benjamin@gmail.com


<h2>Building.</h2>

Latria is capable of being built on linux, mac and windows from the same source. The methods vary however, but for all of them you'll need to navigate to latria/Latria/src/ to start.

<i>To note</i> latria does not use a standard ./configure, make, make install approach. Instead you run ```make [platform]``` and ```make install```.
Note that if you're not sure you can always run ```make help```

For building on mac you can invoke make as follows.
```
make mac
```

For linux you can invoke make the same way, but for linux.
```
make linux
```

Now for windows there is an alternate Makefile provided specially for nmake. You can invoke it and build for windows as follows (Note you will need to install [Visual Studio](https://www.visualstudio.com/) and have access to a [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs) to build from)
```
nmake /F NMakefile latria
```

For building on the 'big 3' that's essentially all you have to do! If you have issues or questions regarding those platforms or any others let me know or open an issue.

<h2>Testing.</h2>

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

If you plan to contribute please checkout <a href='CONTRIBUTING.md'>Contributing</a> first.



