#####################
## Latria Makefile ##
#####################


################
#### MACROS ####
################

#General C Flags to compile latria with
CFLAGS += -std=c90 -ansi -O3 -fno-common -fshort-enums -pedantic -W -Wall -fno-common -fshort-enums -Wcast-align -Wcast-qual -Wconversion -Wmissing-declarations -Wredundant-decls -Wnested-externs -Wpointer-arith -Wshadow

#Generic Debug Flags
DEBUG_FLAGS := -DLAT_TESTS=1 -g

#Generic Include Compiler Flag
INCLUDE_COMPILER_FLAG := -DINCLUDECOMPILER=1

#Linux Flags
LINUX_FLAGS := -DLINUXOS=1
LINUX_INCLUDE_COMPILER_FLAGS := $(LINUX_FLAGS) $(INCLUDE_COMPILER_FLAG)
LINUX_DEBUG_FLAGS := $(LINUX_FLAGS) $(DEBUG_FLAGS)
LINUX_DEBUG_INCLUDE_COMPILER_FLAGS := $(DEBUG_FLAGS) $(LINUX_FLAGS) $(INCLUDE_COMPILER_FLAG)

#Mac Flags
MAC_FLAGS := -DMACOSX=1 -Wstrict-prototypes
MAC_INCLUDE_COMPILER_FLAGS := $(MAC_FLAGS) $(INCLUDE_COMPILER_FLAG)
MAC_DEBUG_FLAGS := $(MAC_FLAGS) $(DEBUG_FLAGS)
MAC_DEBUG_INCLUDE_COMPILER_FLAGS := $(MAC_FLAGS) $(INCLUDE_COMPILER_FLAG) $(DEBUG_FLAGS)

##-fno-common -fshort-enums -O3 -Wcast-align -Wcast-qual -Wconversion -Wmissing-declarations -Wredundant-decls -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wwrite-strings

#PRODUCT NAME & DIR
NAME_INTERPRETER := latriai
NAME_COMPILER := latriac
NAME_DISASSEMBLER := latriad
NAME_INTERPRETER_COMPILER := latria


#################
#### SOURCES ####
#################

LATRIA_C_SRCS := $(wildcard *.c)
LATRIA_CXX_SRCS := $(wildcard *.cpp)
LATRIA_C_OBJS := ${LATRIA_C_SRCS:.c=.o}
LATRIA_CXX_OBJS := ${LATRIA_CXX_SRCS:.cpp=.o}
LATRIA_OBJS := $(LATRIA_C_OBJS) $(LATRIA_CXX_OBJS)

LATRIA_INCLUDE_DIRS :=
LATRIA_LIBRARY_DIRS :=
LATRIA_LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(LATRIA_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(LATRIA_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(LATRIA_LIBRARIES),-l$(library))

######################
#### Object Files ####
######################

#Interpreter Object Files
I_OBJS := Latria_Function.o Latria_GarbageCollection.o Latria_Lexical.o Latria_Libs.o Latria_Math.o Latria_Operators.o Latria_ReferenceStack.o Latria_Sys.o Latria_Vars.o Latria.o Latria_OPCodes.o Latria_Regex.o Latria_Jumps.o Latria_IO.o Latria_Tests.o

#Compiler Object Files
C_OBJS := Latria_C_IO.o Latria_C_Lexical.o Latria_C_Libs.o Latria_C_OpCodes.o Latria_C_StateStack.o Latria_Compiler.o Latria_Compiler_Interface.o Latria_C_Dup_Libs.o

#Embedded Compiler Object Files
C_E_OBJS := Latria_C_IO.o Latria_C_Lexical.o Latria_C_Libs.o Latria_C_OpCodes.o Latria_C_StateStack.o Latria_Compiler_Interface.o

#Disassembler Object Files
D_OBJS := Latria_Decompiler.o

#install dir
DESTDIR += /usr/local/bin

#################
#### TARGETS ####
#################

.PHONY: all clean distclean debug help install

#builds interpreter-compiler & disassembler (defaults to mac)
all: CFLAGS += $(MAC_INCLUDE_COMPILER_FLAGS)
all: build-all

##### MAC BUILDS #####

#mac all
mac: CFLAGS += $(MAC_INCLUDE_COMPILER_FLAGS)
mac: build-all

#mac all debug
mac-debug: CFLAGS += $(MAC_DEBUG_INCLUDE_COMPILER_FLAGS)
mac-debug: build-all

#mac interpreter
mac-interpreter: CFLAGS += $(MAC_FLAGS)
mac-interpreter: interpreter

#mac interpreter debug
mac-interpreter-debug: CFLAGS += $(MAC_DEBUG_FLAGS)
mac-interpreter-debug: interpreter

#mac compiler
mac-compiler: CFLAGS += $(MAC_FLAGS)
mac-compiler: compiler

#mac compiler debug
mac-compiler-debug: CFLAGS += $(MAC_DEBUG_FLAGS)
mac-compiler-debug: compiler

#mac disassembler
mac-disassembler: CFLAGS += $(MAC_FLAGS)
mac-disassembler: disassembler

#mac-disassembler-debug
mac-disassembler-debug: CFLAGS += $(MAC_DEBUG_FLAGS)
mac-disassembler-debug: disassembler

##### LINUX BUILDS #####

#linux all
linux: CFLAGS += $(LINUX_INCLUDE_COMPILER_FLAGS)
linux: build-all

#linux all debug
linux-debug: CFLAGS += $(LINUX_DEBUG_INCLUDE_COMPILER_FLAGS)
linux-debug: build-all

#linux interpreter
linux-interpreter: CFLAGS += $(LINUX_FLAGS)
linux-interpreter: interpreter

#linux interpreter debug
linux-interpreter-debug: CFLAGS += $(LINUX_DEBUG_FLAGS)
linux-interpreter-debug: interpreter

#linux compiler
linux-compiler: CFLAGS += $(LINUX_FLAGS)
linux-compiler: compiler

#linux compiler debug
linux-compiler-debug: CFLAGS += $(LINUX_DEBUG_FLAGS)
linux-compiler-debug: compiler

#linux disassembler
linux-disassembler: CFLAGS += $(LINUX_FLAGS)
linux-disassembler: disassembler

#linux-disassembler-debug
linux-disassembler-debug: CFLAGS += $(LINUX_DEBUG_FLAGS)
linux-disassembler-debug: disassembler


#build-all
build-all: $(NAME_INTERPRETER_COMPILER) $(NAME_DISASSEMBLER)

interpreter: $(NAME_INTERPRETER)

compiler: $(NAME_COMPILER)

disassembler: $(NAME_DISASSEMBLER)

$(NAME_INTERPRETER_COMPILER): $(I_OBJS) $(C_E_OBJS)
	$(LINK.cc) $(I_OBJS) $(C_E_OBJS) -o $(NAME_INTERPRETER_COMPILER)
	
$(NAME_INTERPRETER): $(I_OBJS)
	$(LINK.cc) $(I_OBJS) -o $(NAME_INTERPRETER_COMPILER)
	
$(NAME_COMPILER): $(C_OBJS)
	$(LINK.cc) $(C_OBJS) -o $(NAME_COMPILER)
	
$(NAME_DISASSEMBLER): $(D_OBJS)
	$(LINK.cc) $(D_OBJS) -o $(NAME_DISASSEMBLER)
	
clean: cclean dclean
	@- $(RM) $(NAME_INTERPRETER_COMPILER)
	@- $(RM) $(LATRIA_OBJS)
	
cclean:
	@- $(RM) $(NAME_COMPILER)
	
dclean:
	@- $(RM) $(NAME_DISASSEMBLER)
	
distclean: clean

help:
	@echo ::latria::
	@echo Targets
	@echo
	@echo install
	@echo clean
	@echo help
	@echo distclean
	@echo
	@echo all
	@echo -interpreter-compiler and disassembler for mac
	@echo
	@echo make mac
	@echo -interpreter-compiler and disassembler for mac
	@echo
	@echo make mac-debug
	@echo -interpreter-compiler and disassembler for mac with debug flags and tests
	@echo
	@echo make linux
	@echo -interpreter-compiler and disassembler for linux
	@echo
	@echo make linux-debug
	@echo -interpreter-compiler and disassembler for linux with debug flags and tests
	@echo
	@echo make mac-interpreter
	@echo -interpreter for mac
	@echo
	@echo make mac-interpreter-debug
	@echo -interpreter for mac with debug flags
	@echo
	@echo make mac-compiler
	@echo -compiler for mac
	@echo
	@echo make mac-compiler-debug
	@echo -compiler for mac with debug flags
	@echo
	@echo make mac-disassembler
	@echo -disassembler for mac
	@echo
	@echo make mac-disassembler-debug
	@echo -disassembler for mac with debug flags
	@echo
	@echo make linux-interpreter
	@echo -interpreter for linux
	@echo
	@echo make linux-interpreter-debug
	@echo -interpreter for linux with debug flags
	@echo
	@echo make linux-compiler
	@echo -compiler for linux
	@echo
	@echo make linux-compiler-debug
	@echo -compiler for linux with debug flags
	@echo
	@echo make linux-disassembler
	@echo -disassembler for linux
	@echo
	@echo make linux-disassembler-debug
	@echo -disassembler for linux with debug flags
	@echo
	
#install all
install: iinstall cinstall dinstall
	
#interpreter install
iinstall:
	@- sudo cp $(NAME_INTERPRETER_COMPILER) $(DESTDIR)
	
#compiler install
cinstall:
	@- sudo cp $(NAME_COMPILER) $(DESTDIR)
	
#disassembler install
dinstall:
	@- sudo cp $(NAME_DISASSEMBLER) $(DESTDIR)
