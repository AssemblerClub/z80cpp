##-----------------------------LICENSE NOTICE------------------------------------
##  This file is part of CPCtelera: An Amstrad CPC Game Engine 
##  Copyright (C) 2015 ronaldo / Fremos / Cheesetea / ByteRealms (@FranGallegoBR)
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU Lesser General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.
##------------------------------------------------------------------------------

###########################################################################
##                          CPCTELERA ENGINE                             ##
##                  Main Building Makefile for Projects                  ##
##-----------------------------------------------------------------------##
## This file contains the rules for building a CPCTelera project. These  ##
## These rules work generically for every CPCTelera project.             ##
## Usually, this file should be left unchanged:                          ##
##  * Project's build configuration is to be found in build_config.mk    ##
##  * Global paths and tool configuration is located at $(CPCT_PATH)/cfg/##
###########################################################################
.PHONY: all clean cleanall

# CONFIGURATION
CC      := clang++
CXXFLAGS:= -O3 -Wall -pedantic -std=c++17 -Wno-gnu-anonymous-struct -Wno-nested-anon-types
#CXXFLAGS:= -g -Wall -pedantic -std=c++17 -Wno-gnu-anonymous-struct -Wno-nested-anon-types
INCDIRS := -Isrc
LINKLIBS:=
PROJNAME:= z80emu
TARGET  := $(PROJNAME)
MKDIR   := mkdir -p 
RM      := rm -f


# ANSI Sequences for terminal colored printing
COLOR_RED=\033[1;31;49m
COLOR_YELLOW=\033[1;33;49m
COLOR_NORMAL=\033[0;39;49m

####
## SECTION 2: LIBRARY CONFIG
##
## This section establishes source and object subfolders and the binary objects to
## be built. Normally, you want to change the OBJ files you want to be built, selecting
## only the ones that contain the actual code that will be used by you in your application.
##
## SRCDIR       >> Directory under which assembly source files are located
## OBJDIR       >> Directory where generated intermediate object files will be written to
## SUBDIRS      >> All first level subdirectories found in $(SRCDIR)/. They are assumed to contain code files.
## OBJSUBDIRS   >> Subdirectory structure for object files that replicates SUBDIRS
## SRCFILES     >> All .s files to be found under $(SRCDIR)/ and first level SUBDIRS, with full relative path
## OBJFILES     >> intermediate .rel files that will be needed for your application (their names
##                 have to correspond to existing source files in the SRCDIR, but with .rel extension)
#####

SRCEXT    :=cpp
HEXT      :=hpp
OBJEXT    :=o
SRCDIR    :=src
OBJDIR    :=obj
SUBDIRS   :=$(filter-out ., $(shell find $(SRCDIR) -type d -print))
OBJSUBDIRS:=$(foreach DIR, $(SUBDIRS), $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(DIR)))
SRCFILES  :=$(foreach DIR, $(SUBDIRS), $(wildcard $(DIR)/*.$(SRCEXT)))
OBJFILES  :=$(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRCFILES:%.$(SRCEXT)=%.$(OBJEXT)))


# MAIN TARGET
.DEFAULT_GOAL := all
all: $(OBJSUBDIRS) $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) $(CXXFLAGS) $(INCDIRS) $(LINKLIBS) -o $(TARGET)

#################
# OBJFILEDEPS: Calculate 1st level dependencies for a given OBJFILE
#   It ensures that depencies exits in the filesystem
#
# $(1): Object file for which to calculate dependencies
#
define OBJFILEDEPS
	$(shell find \
		$(patsubst $(OBJDIR)%,$(SRCDIR)%,\
			$(1:%.$(OBJEXT)=%.$(SRCEXT)) \
			$(1:%.$(OBJEXT)=%.$(HEXT)) \
		) 2> /dev/null \
	)
endef


#################
# PRINT: Print a nice and colorful message
#
# $(1): Subsystem that shows the message
# $(2): Message to print
#
define PRINT
	@printf "$(COLOR_RED)["
	@printf $(1)
	@printf "]$(COLOR_YELLOW) "
	@printf $(2)
	@printf "$(COLOR_NORMAL)\n"
endef

#################
# COMPILECPPFILE: General rule to compile a CPP, having source file and object file in different places
#
# $(1): Object file to be created  (with its relative path)
# $(2): Source dependencies (must include a .cpp file)
#
define COMPILECPPFILE
$(1): $(2)
	$(CC) $(INCDIRS) $(CXXFLAGS) -c $(filter %.$(SRCEXT), $(2)) -o $(1)

endef

## COMPILING SOURCEFILES AND SAVE OBJFILES IN THEIR CORRESPONDENT SUBDIRS
$(foreach OF, $(OBJFILES), $(eval $(call COMPILECPPFILE, $(OF), $(call OBJFILEDEPS,$(OF)))))

# CREATE OBJDIR & SUBDIRS IF THEY DO NOT EXIST
$(OBJSUBDIRS): 
	@$(MKDIR) $@

# CLEANING TARGETS
cleanall: clean
	@$(call PRINT,$(PROJNAME),"Deleting $(TARGET)")
	$(RM) $(TARGET)

clean: 
	@$(call PRINT,$(PROJNAME),"Deleting folder: $(OBJDIR)/")
	$(RM) -r ./$(OBJDIR)


##
## ASMTESTS
##

## Calculate ASM Objects
##
define GETASMOBJ
$(patsubst $(ASMSRCDIR)%, $(ASMOBJDIR)%, $(1:%.$(ASMSRCEXT)=%.$(ASMOBJEXT)))
endef

## CONFIG
##
ASZ80	   :=$(wildcard ${CPCT_PATH}/tools/sdcc-3.6*/bin/sdasz80)
SDCC	   :=$(wildcard ${CPCT_PATH}/tools/sdcc-3.6*/bin/sdcc)
HEX2BIN  :=${CPCT_PATH}/tools/hex2bin-2.0/bin/hex2bin
ASMSRCDIR:=tests
ASMOBJDIR:=$(ASMSRCDIR)/obj
ASMBINDIR:=$(ASMSRCDIR)/bin
ASMSRCEXT:=s
ASMOBJEXT:=bin
ASMFILES :=$(wildcard $(ASMSRCDIR)/*.$(ASMSRCEXT))
ASMOBJS  :=$(call GETASMOBJ,$(ASMFILES))

tests: $(ASMOBJDIR) $(ASMBINDIR) $(ASMOBJS) 

$(ASMOBJDIR) $(ASMBINDIR):
	@$(MKDIR) $@

cleantests:
	@$(call PRINT,$(PROJNAME),"Deleting folder: $(ASMOBJDIR)/")
	$(RM) -r ./$(ASMOBJDIR)

cleanalltests: cleantests
	@$(call PRINT,$(PROJNAME),"Deleting folder: $(ASMBINDIR)/")
	$(RM) -r ./$(ASMBINDIR)

#################
# COMPILEASMFILE: General rule to compile an ASM, having source file and object file in different places
#
# $(1): Object file to be created  (with its relative path)
# $(2): Source file asm
#
define COMPILEASMFILE
$(1): $(2)
	$(ASZ80) -ols "$(basename $(1)).rel" "$(2)"
	$(SDCC) -mz80 --no-std-crt0 --code-loc 0 --data-loc 0 "$(basename $(1)).rel" -o "$(basename $(1)).ihx"
	$(HEX2BIN) -p 00 "$(basename $(1)).ihx"
	mv "$(strip $(1))" "$(ASMBINDIR)"

endef

## COMPILE ALL ASMFILES
$(foreach ASF, $(ASMFILES), $(eval $(call COMPILEASMFILE,$(call GETASMOBJ,$(ASF)),$(ASF))))

