# simple makefile

MKDIR_P = mkdir -p

# main directories
BIN_DIR = ./bin
OBJ_DIR = ./build
SRC_DIR = ./src
INC_DIR = ./include

ABC_DIR = ./abc
GLUCOSE_DIR = ./glucose

# lexer options
LEXER = reflex
LEXER_FLAGS =

PARSER_LEX = $(SRC_DIR)/parser/bfparser.l
PARSER_CPP = $(SRC_DIR)/parser/bfparser.cpp
PARSER_INC = $(INC_DIR)/parser/bfparser.h


# sub project directories
INC_DIRS = $(shell find $(INC_DIR) -type d)
INC_DIRS += $(GLUCOSE_DIR) $(ABC_DIR)/src

# compiler and linker options
EXE_NAME = resyn

CXX = g++

CXX_FLAGS = -W -Wall -Wextra -s -O3 -static -std=c++14 -DABC_NAMESPACE=ABC -DLIN64
INC_FLAGS = $(addprefix -I,$(INC_DIRS))
LD_FLAGS = -static
LIB_FLAGS = -lreflex -lm -ldl -rdynamic -lz

# collect sources ...
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
#DEPS = $(OBJS:.o=.d)
LIBS = $(GLUCOSE_DIR)/simp/lib.a $(ABC_DIR)/libabc.a

# rules for c++ files
$(OBJ_DIR)/%.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(INC_FLAGS) $(CXX_FLAGS) -c $< -o $@

#rules for target
$(BIN_DIR)/$(EXE_NAME): $(OBJS)
	$(MKDIR_P) $(BIN_DIR)
	$(CXX) $(LD_FLAGS) $(OBJS) $(LIBS) $(LIB_FLAGS) -o $@


.PHONY: abc glucose parser superclean clean

abc:
	cd $(ABC_DIR); make ABC_USE_NAMESPACE=ABC ABC_USE_PIC=1 ABC_USE_NO_PTHREADS=1 ABC_USE_NO_READLINE=1 libabc.a
	
glucose:
	cd $(GLUCOSE_DIR)/simp; make libr

parser:
	$(MKDIR_P) $(dir $(PARSER_INC))
	$(LEXER) -o $(PARSER_CPP) --header-file=$(PARSER_INC) $(PARSER_LEX) 

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(BIN_DIR)/*

superclean: clean
	rm $(PARSER_CPP)
	rm $(PARSER_INC)
	cd $(GLUCOSE_DIR)/simp; make clean
	cd $(ABC_DIR); make clean
