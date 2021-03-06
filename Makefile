#####################
## Latria Makefile ##
#####################


################
#### MACROS ####
################

# General C Flags to compile latria with
CFLAGS += -Isrc -std=c90 -ansi -O3 -fno-common -fshort-enums -pedantic -W -Wall -fno-common -fshort-enums -Wcast-align -Wcast-qual -Wconversion -Wmissing-declarations -Wredundant-decls -Wnested-externs -Wpointer-arith -Wshadow

# Generic Debug Flags
# FULL Debug Mode Can use -DLAT_DEBUG=1
DEBUG_FLAGS := -DLAT_TESTS=1 -g

# Generic Include Compiler Flag
INCLUDE_COMPILER_FLAG := -DINCLUDECOMPILER=1

# Compiler only flag
COMPILER_ONLY_FLAG := -DCOMPILER_ONLY=1

# Linux Flags
LINUX_FLAGS := -DLINUXOS=1
LINUX_INCLUDE_COMPILER_FLAGS := $(LINUX_FLAGS) $(INCLUDE_COMPILER_FLAG)
LINUX_DEBUG_FLAGS := $(LINUX_FLAGS) $(DEBUG_FLAGS)
LINUX_DEBUG_INCLUDE_COMPILER_FLAGS := $(DEBUG_FLAGS) $(LINUX_FLAGS) $(INCLUDE_COMPILER_FLAG)

# Mac Flags
MAC_FLAGS := -DMACOSX=1 -Wstrict-prototypes
MAC_INCLUDE_COMPILER_FLAGS := $(MAC_FLAGS) $(INCLUDE_COMPILER_FLAG)
MAC_DEBUG_FLAGS := $(MAC_FLAGS) $(DEBUG_FLAGS)
MAC_DEBUG_INCLUDE_COMPILER_FLAGS := $(MAC_FLAGS) $(INCLUDE_COMPILER_FLAG) $(DEBUG_FLAGS)

# PRODUCT NAME & DIR
NAME_INTERPRETER := build/latria-i
NAME_COMPILER := build/latria-c
NAME_DISASSEMBLER := build/latria-d
NAME_INTERPRETER_COMPILER := build/latria
NAME_INTERPRETER_COMPILER_RELEASE_MAC := build/latria-osx
NAME_INTERPRETER_COMPILER_RELEASE_LINUX := build/latria-linux

# Debug Variants
NAME_INTERPRETER_DEBUG := build/latria-debug-i
NAME_COMPILER_DEBUG := build/latria-debug-c
NAME_DISASSEMBLER_DEBUG := build/latria-debug-d
NAME_INTERPRETER_COMPILER_DEBUG := build/latria-debug

#####################
#### INSTALL DIR ####
#####################

# install dir
DEST_DIR += /usr/local/bin


#################
#### SOURCES ####
#################

# c sources
LATRIA_C_SRCS := $(wildcard src/compiler/*.c) $(wildcard src/core/*.c) $(wildcard src/disassembler/*.c) $(wildcard src/http/*.c) $(wildcard src/interpreter/*.c) $(wildcard src/memory/*.c) $(wildcard src/prompt/*.c) $(wildcard src/regex/*.c) $(wildcard src/tests/*.c) $(wildcard src/vm/*.c)

# c++ sources
LATRIA_CXX_SRCS := $(wildcard src/compiler/*.cpp) $(wildcard src/core/*.cpp) $(wildcard src/disassembler/*.cpp) $(wildcard src/http/*.cpp) $(wildcard src/interpreter/*.cpp) $(wildcard src/memory/*.cpp) $(wildcard src/prompt/*.cpp) $(wildcard src/regex/*.cpp) $(wildcard src/tests/*.cpp) $(wildcard src/vm/*.cpp)

LATRIA_C_OBJS := ${LATRIA_C_SRCS:.c=.o}
LATRIA_CXX_OBJS := ${LATRIA_CXX_SRCS:.cpp=.o}
LATRIA_ALL_OBJS := $(LATRIA_C_OBJS) $(LATRIA_CXX_OBJS)

LATRIA_INCLUDE_DIRS :=
LATRIA_LIBRARY_DIRS :=
LATRIA_LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(LATRIA_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(LATRIA_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(LATRIA_LIBRARIES),-l$(library))

######################
#### Object Files ####
######################

# Interpreter Prompt
INTERPRETER_PROMPT := src/prompt/latria.o

# Compiler Prompt
COMPILER_PROMPT := src/prompt/latria_compiler.o

# Compiler Core
COMPILER_CORE := src/compiler/latria_c_io.o src/compiler/latria_c_lexical.o src/compiler/latria_c_statestack.o src/compiler/latria_compiler_interface.o

# Core Objects
CORE_OBJS := src/core/latria_char.o src/core/latria_string.o src/core/latria_function.o src/core/latria_jumps.o src/core/latria_math.o src/core/latria_opcodes.o src/core/latria_operators.o src/core/latria_referencestack.o src/core/latria_sys.o src/core/latria_vars.o

# Disassembler Objects
DISASSEMBLER_OBJS := src/disassembler/latria_disassembler.o

# HTTP Objects
HTTP_OBJS := src/http/latria_client.o src/http/latria_server.o src/http/latria_connection.o

# Iterpreter Objects
INTERPRETER_OBJS := src/interpreter/latria_io.o src/interpreter/latria_lexical.o

# Memory Objects
MEMORY_OBJS := src/memory/latria_gc.o

# Regex Objects
REGEX_OBJS := src/regex/latria_regex.o

# Test Objects
TEST_OBJS := src/tests/latria_ctest.o src/tests/latria_testframework.o src/tests/latria_tests.o src/tests/latria_whitespace_tests.o src/tests/latria_random_tests.o src/tests/latria_regex_tests.o src/tests/latria_integer_tests.o src/tests/latria_float_tests.o src/tests/latria_string_tests.o src/tests/latria_assignment_tests.o src/tests/latria_print_tests.o src/tests/latria_math_tests.o src/tests/latria_array_tests.o src/tests/latria_fileio_tests.o src/tests/latria_relational_tests.o src/tests/latria_controlflow_tests.o src/tests/latria_xlang_tests.o src/tests/latria_function_tests.o

# Virtual Machine Objects
VM_OBJS :=

#################
#### TARGETS ####
#################

.PHONY: all clean distclean debug help install

# builds interpreter-compiler & disassembler (defaults to mac)
all: CFLAGS += $(MAC_INCLUDE_COMPILER_FLAGS)
all: build-all

##### MAC BUILDS #####

# mac test all (runs tests and tries all builds)
mac-test-all: CFLAGS += $(MAC_DEBUG_INCLUDE_COMPILER_FLAGS)
mac-test-all: build-all-debug
	./build/latria-debug -t
	make clean mac
	make clean mac-interpreter
	make clean mac-interpreter-debug
	make clean mac-compiler
	make clean mac-compiler-debug
	make clean mac-disassembler

# mac test
mac-test: CFLAGS += $(MAC_DEBUG_INCLUDE_COMPILER_FLAGS)
mac-test: build-all-debug
	./build/latria-debug -t

# mac all
mac: CFLAGS += $(MAC_INCLUDE_COMPILER_FLAGS)
mac: build-all

# mac all debug
mac-debug: CFLAGS += $(MAC_DEBUG_INCLUDE_COMPILER_FLAGS)
mac-debug: build-all-debug

# mac interpreter
mac-interpreter: CFLAGS += $(MAC_FLAGS)
mac-interpreter: interpreter

# mac interpreter debug
mac-interpreter-debug: CFLAGS += $(MAC_DEBUG_FLAGS)
mac-interpreter-debug: interpreter-debug

# mac compiler
mac-compiler: CFLAGS += $(MAC_FLAGS) $(COMPILER_ONLY_FLAG)
mac-compiler: compiler

# mac compiler debug
mac-compiler-debug: CFLAGS += $(MAC_DEBUG_FLAGS) $(COMPILER_ONLY_FLAG)
mac-compiler-debug: compiler-debug

# mac disassembler
mac-disassembler: CFLAGS += $(MAC_FLAGS)
mac-disassembler: disassembler

# mac-disassembler-debug
mac-disassembler-debug: CFLAGS += $(MAC_DEBUG_FLAGS)
mac-disassembler-debug: disassembler-debug

##### LINUX BUILDS #####

# linux test all (runs tests and tries all builds)
linux-test-all: CFLAGS += $(LINUX_DEBUG_INCLUDE_COMPILER_FLAGS)
linux-test-all: build-all-debug
	./build/latria-debug -t
	make clean linux
	make clean linux-interpreter
	make clean linux-interpreter-debug
	make clean linux-compiler
	make clean linux-compiler-debug
	make clean linux-disassembler

# linux test
linux-test: CFLAGS += $(LINUX_DEBUG_INCLUDE_COMPILER_FLAGS)
linux-test: build-all-debug
	./build/latria-debug -t

# linux all
linux: CFLAGS += $(LINUX_INCLUDE_COMPILER_FLAGS)
linux: build-all

# linux all debug
linux-debug: CFLAGS += $(LINUX_DEBUG_INCLUDE_COMPILER_FLAGS)
linux-debug: build-all-debug

# linux interpreter
linux-interpreter: CFLAGS += $(LINUX_FLAGS)
linux-interpreter: interpreter

# linux interpreter debug
linux-interpreter-debug: CFLAGS += $(LINUX_DEBUG_FLAGS)
linux-interpreter-debug: interpreter-debug

# linux compiler
linux-compiler: CFLAGS += $(LINUX_FLAGS) $(COMPILER_ONLY_FLAG)
linux-compiler: compiler

# linux compiler debug
linux-compiler-debug: CFLAGS += $(LINUX_DEBUG_FLAGS) $(COMPILER_ONLY_FLAG)
linux-compiler-debug: compiler-debug

# linux disassembler
linux-disassembler: CFLAGS += $(LINUX_FLAGS)
linux-disassembler: disassembler

# linux-disassembler-debug
linux-disassembler-debug: CFLAGS += $(LINUX_DEBUG_FLAGS)
linux-disassembler-debug: disassembler-debug

## release targets ##

mac-release: CFLAGS += $(MAC_INCLUDE_COMPILER_FLAGS)
mac-release: build-release-mac

linux-release: CFLAGS += $(LINUX_INCLUDE_COMPILER_FLAGS)
linux-release: build-release-linux

## normal targets ##
build-all: $(NAME_INTERPRETER_COMPILER) $(NAME_DISASSEMBLER)
interpreter: $(NAME_INTERPRETER)
compiler: $(NAME_COMPILER)
disassembler: $(NAME_DISASSEMBLER)

## debug targets ##

build-all-debug: $(NAME_INTERPRETER_COMPILER_DEBUG) $(NAME_DISASSEMBLER_DEBUG)
interpreter-debug: $(NAME_INTERPRETER_DEBUG)
compiler-debug: $(NAME_COMPILER_DEBUG)
disassembler-debug: $(NAME_DISASSEMBLER_DEBUG)

####################
###    Builds    ###
####################

# build and zips releases for mac
build-release-mac:
	if [ ! -d latria-mac ]; then mkdir latria-mac; fi
	make clean mac
	mv build/latria latria-mac/latria
	make clean mac-interpreter
	mv build/latria-i latria-mac/latria-i
	make clean mac-compiler
	mv build/latria-c latria-mac/latria-c
	chmod u+x latria-mac/*
	if [ ! -d releases ]; then mkdir releases; fi
	zip -r releases/latria-mac latria-mac
	rm -r latria-mac

# builds and zips releases for linux
build-release-linux:
	if [ ! -d latria-linux ]; then mkdir latria-linux; fi
	make clean linux
	mv build/latria latria-linux/latria
	make clean linux-interpreter
	mv build/latria-i latria-linux/latria-i
	make clean linux-compiler
	mv build/latria-c latria-linux/latria-c
	chmod u+x latria-linux/*
	if [ ! -d releases ]; then mkdir releases; fi
	zip -r releases/latria-linux latria-linux
	rm -r latria-linux

$(NAME_INTERPRETER_COMPILER): $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS) $(COMPILER_CORE)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS) $(COMPILER_CORE) -o $(NAME_INTERPRETER_COMPILER)

$(NAME_INTERPRETER): $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS) -o $(NAME_INTERPRETER)

$(NAME_COMPILER): $(CORE_OBJS) $(COMPILER_CORE) $(COMPILER_PROMPT) $(MEMORY_OBJS)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(CORE_OBJS) $(COMPILER_CORE) $(COMPILER_PROMPT) $(MEMORY_OBJS) -o $(NAME_COMPILER)

$(NAME_DISASSEMBLER): $(DISASSEMBLER_OBJS)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(DISASSEMBLER_OBJS) -o $(NAME_DISASSEMBLER)

####################
### Debug Builds ###
####################
$(NAME_INTERPRETER_COMPILER_DEBUG): $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS) $(TEST_OBJS) $(COMPILER_CORE)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS) $(TEST_OBJS) $(COMPILER_CORE) -o $(NAME_INTERPRETER_COMPILER_DEBUG)

$(NAME_INTERPRETER_DEBUG): $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(INTERPRETER_PROMPT) $(CORE_OBJS) $(INTERPRETER_OBJS) $(HTTP_OBJS) $(MEMORY_OBJS) $(REGEX_OBJS) -o $(NAME_INTERPRETER_DEBUG)

$(NAME_COMPILER_DEBUG): $(CORE_OBJS) $(COMPILER_CORE) $(COMPILER_PROMPT) $(MEMORY_OBJS)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(CORE_OBJS) $(COMPILER_CORE) $(COMPILER_PROMPT) $(MEMORY_OBJS) -o $(NAME_COMPILER_DEBUG)

$(NAME_DISASSEMBLER_DEBUG): $(DISASSEMBLER_OBJS)
	if [ ! -d build ]; then mkdir build; fi
	$(LINK.cc) $(DISASSEMBLER_OBJS) -o $(NAME_DISASSEMBLER_DEBUG)

###########
## Clean ##
###########

# cleans objects
clean: iclean cclean dclean
	@- $(RM) $(LATRIA_ALL_OBJS)

# cleans interpreter builds
iclean:
	@- $(RM) $(NAME_INTERPRETER_COMPILER)
	@- $(RM) $(NAME_INTERPRETER_COMPILER_DEBUG)
	@- $(RM) $(NAME_INTERPRETER)
	@- $(RM) $(NAME_INTERPRETER_DEBUG)

# cleans compiler builds
cclean:
	@- $(RM) $(NAME_COMPILER)
	@- $(RM) $(NAME_COMPILER_DEBUG)

# cleans disassembler builds
dclean:
	@- $(RM) $(NAME_DISASSEMBLER)
	@- $(RM) $(NAME_DISASSEMBLER_DEBUG)

# same as 'clean'
distclean: clean

##########
## Help ##
##########

help:
	@echo ::latria::
	@echo Targets
	@echo
	@echo install
	@echo -installs built binaries
	@echo
	@echo clean
	@echo -removes built objects and binaries
	@echo
	@echo help
	@echo -prints this help info
	@echo
	@echo distclean
	@echo -same as clean
	@echo
	@echo all
	@echo -interpreter-compiler and disassembler for mac
	@echo
	@echo mac
	@echo -interpreter-compiler and disassembler for mac
	@echo
	@echo mac-test
	@echo -builds and runs tests
	@echo
	@echo mac-test-all
	@echo -runs tests and tries all builds
	@echo
	@echo mac-debug
	@echo -interpreter-compiler and disassembler for mac with debug flags and tests
	@echo
	@echo linux
	@echo -interpreter-compiler and disassembler for linux
	@echo
	@echo linux-test
	@echo -build debuggable linux build and runs tests
	@echo
	@echo linux-test-all
	@echo -runs tests and tries all builds
	@echo
	@echo linux-debug
	@echo -interpreter-compiler and disassembler for linux with debug flags and tests
	@echo
	@echo mac-interpreter
	@echo -interpreter for mac
	@echo
	@echo mac-interpreter-debug
	@echo -interpreter for mac with debug flags
	@echo
	@echo mac-compiler
	@echo -compiler for mac
	@echo
	@echo mac-compiler-debug
	@echo -compiler for mac with debug flags
	@echo
	@echo mac-disassembler
	@echo -disassembler for mac
	@echo
	@echo mac-disassembler-debug
	@echo -disassembler for mac with debug flags
	@echo
	@echo linux-interpreter
	@echo -interpreter for linux
	@echo
	@echo linux-interpreter-debug
	@echo -interpreter for linux with debug flags
	@echo
	@echo linux-compiler
	@echo -compiler for linux
	@echo
	@echo linux-compiler-debug
	@echo -compiler for linux with debug flags
	@echo
	@echo linux-disassembler
	@echo -disassembler for linux
	@echo
	@echo linux-disassembler-debug
	@echo -disassembler for linux with debug flags
	@echo

#############
## Install ##
#############

# installs all binaries present
install:
	sudo cp build/* $(DEST_DIR)
