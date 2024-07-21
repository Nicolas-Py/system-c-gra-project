#makefile created by Ziang liu on 24.02.2024

#------------------------
#configuartion start
#------------------------

#entry point and target name. main.c executes the run simulation which is called from module.cpp
C_SOURCE = src/main.c
CPP_SOURCE = src/modules.cpp

#this is used to convert .c to .o
C_OBJECT = $(C_SOURCE:.c=.o)

#this is used to convert .cpp to .o
CPP_OBJECT = $(CPP_SOURCE:.cpp=.o)

# name for executable/target name
TARGET := cache_simulation

#path to systemc installation
SCPATH = ../systemc

#flags for the compiler which links to SystemC installation
CXXFLAGS := -std=c++14  -I$(SCPATH)/include -L$(SCPATH)/lib -lsystemc -lm


#------------------------
#configuartion end
#------------------------

#check for clang or gcc
CXX := $(shell command -v g++ || command -v clang++)
ifeq ($(strip $(CXX)),)
    $(error Neither clang++ nor g++ is available. Exiting.)
endif

CC := $(shell command -v gcc || command -v clang)
ifeq ($(strip $(CC)),)
    $(error Neither clang nor g is available. Exiting.)
endif


#build rules
all: debug


#compile c and c++ into object files
#source code (.i) is compile to assembly code (.s) by gcc or clang, then the assembly code is translated into machine code (.o)
# example format: "gcc -c $< -o $@
# gcc -c invokes the GCC compiler with the -c flag to compile the source file
# $< automatically expands to main.c (in the case of main.o)
# -o $@ specifies the name of the output file (in this case it would be main.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


#to build project in debug mode
debug: CXXFLAGS += -g
debug: $(TARGET)

#to build project in release mode
release: CXXFLAGS += -O2
release: $(TARGET)

#linkling
$(TARGET): $(C_OBJECT) $(CPP_OBJECT)
	$(CXX) $(CXXFLAGS) $(C_OBJECT) $(CPP_OBJECT) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf src/*.o

.PHONY: all debug release clean

