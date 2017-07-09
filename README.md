# latria

[![Build Status](https://travis-ci.org/montymxb/latria.svg?branch=master)](https://travis-ci.org/montymxb/latria)

A compact, cross platform, embeddable language with an emphasis on multi-language communication and lightweight scripting.
<br/><br/>
Built following ANSI C, latria can run on mac/linux/windows/android/ios and more.<br/>

<h2>Binaries</h2>

The [latest release](https://github.com/montymxb/latria/releases/latest/) has binaries for **mac** and **linux**. **windows** is currently being worked into the automatic deployment flow, but may be present if uploaded by hand.

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

Now for windows there is an alternate Makefile provided specially for nmake. You can invoke it and build for windows as follows (Note you will need to install Visual Studio and have access to a Developer Command Prompt to build from)
```
nmake /F NMakefile latria
```

For building on the 'big 3' that's essentially all you have to do! If you have issues or questions regarding those platforms or any others let me know or open an issue.

<h2>Testing.</h2>

Testing on Latria is currently run by an embedded suite of tests. You can build them into a binary as follows:

mac & linux
```bash
# clean if needed
make clean

make mac-debug
## OR ##
make linux-debug

make install

# run tests
latria -t
```

windows
```batch
:: clean if needed
nmake /F NMakefile clean

nmake /F NMakeFile latria-debug

:: run tests
latria.exe -t
```

If you plan to contribute please checkout <a href='CONTRIBUTING.md'>Contributing</a> first.



