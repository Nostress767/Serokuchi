#######################
# Main configurations #
#######################

INCDIR       := include
SRCDIR       := src

ifdef DEBUG
	TARGET   := debug
	CPPFLAGS := -Wall -c -I$(INCDIR) -finput-charset=UTF-8 -MMD -DDEBUG
	CFLAGS   := -Og
	CXXFLAGS := -Og -std=c++20
	LDFLAGS  := -lgdi32 -mconsole -lxinput
else
	TARGET   := release
	CPPFLAGS := -Wall -c -I$(INCDIR) -finput-charset=UTF-8 -MMD
	CFLAGS   := -O2
	CXXFLAGS := -O2 -std=c++20
	LDFLAGS  := -lgdi32 -mwindows -lxinput
endif

RSCDIR       := assets
OBJDIR       := obj/$(TARGET)

CC           := gcc
CXX          := g++
EXE          := target/$(TARGET)/Serokuchi.exe

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

#######################

#############
# Functions #
#############

# Convert 1° arg suffix to ".o"
define to_obj
	$(addsuffix .o,$(basename $(1)))
endef

# Get ".o" files from 2° arg base folder of a 1° arg extension
define get_obj
	$(call to_obj,$(filter %.$(1),$(addprefix $(OBJDIR)/,$(2))))
endef

# Compile recipe with 1° arg compiler and 2° arg flags
define create_src_obj =
	@mkdir -p $(dir $@)
	$(1) $(CPPFLAGS) $(2) $< -MF $(basename $@).d -o $@
endef

# Resource canned recipe
define CREATE_RSC_OBJ =
	@mkdir -p $(dir $@)
	cd $(dir $<) && objcopy -I binary -O pe-i386 -B i386 $(notdir $<) $(basename $(abspath $@)).o
endef

#############

# Get all source and asset files recursively
SRC       := $(shell find $(SRCDIR)/ -type f)
RSC       := $(shell find $(RSCDIR)/ -type f)

SRCOBJS   := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
RSCOBJS   := $(addprefix $(OBJDIR)/,$(call to_obj,$(RSC)))
MAKEFILES := $(addsuffix .d,$(basename $(SRCOBJS)))

###########
# Targets #
###########

# Default target
all: $(EXE)

## Compile sources

# C
$(call get_obj,c  ,$(SRC)) : $(OBJDIR)/%.o : %.c
	$(call create_src_obj,$(CC) ,$(CFLAGS)  )

# C++
$(call get_obj,cpp,$(SRC)) : $(OBJDIR)/%.o : %.cpp
	$(call create_src_obj,$(CXX),$(CXXFLAGS))
##

## Compile assets

# NOTE # Come back to this if there is any way to further automate this {
assets_filetypes := ogg png tmj

$(call get_obj,ogg,$(RSC)) : $(OBJDIR)/%.o : %.ogg
	$(CREATE_RSC_OBJ)

$(call get_obj,png,$(RSC)) : $(OBJDIR)/%.o : %.png
	$(CREATE_RSC_OBJ)

$(call get_obj,tmj,$(RSC)) : $(OBJDIR)/%.o : %.tmj
	$(CREATE_RSC_OBJ)

# }

# XXX # The purpose of this is to avoid having to include every new asset file {
.PHONY: create_asset_files
create_asset_files:
# This (re-)creates *type*_files.hpp header files that contain the asset global variable prototypes
	@$(foreach type,$(assets_filetypes),printf "#pragma once\n\n#include \"$(RSCDIR)/$(type)_media.hpp\"\n\n" > $(INCDIR)/$(RSCDIR)/$(type)_files.hpp;)
# This appends all the included files of that filetype to this file (specific for each extension)
	@$(foreach testo,$(foreach type,$(assets_filetypes),$(filter %.$(type),$(RSC))),echo "___include_$(subst .,,$(suffix $(testo)))($(basename $(notdir $(testo))),$(subst .,,$(suffix $(testo))));" >> $(INCDIR)/$(RSCDIR)/$(subst .,,$(suffix $(testo)))_files.hpp;)
# This (re-)creates *type*_files.cpp source files that contain the asset global variable definition
	@$(foreach type,$(assets_filetypes),printf "#include \"$(RSCDIR)/$(type)_files.hpp\"\n\n" > $(SRCDIR)/$(RSCDIR)/$(type)_files.cpp;)
# This appends all the included files of that filetype to this file (specific for each extension)
	@$(foreach testo,$(foreach type,$(assets_filetypes),$(filter %.$(type),$(RSC))),echo "___define_$(subst .,,$(suffix $(testo)))($(basename $(notdir $(testo))),$(subst .,,$(suffix $(testo))));" >> $(SRCDIR)/$(RSCDIR)/$(subst .,,$(suffix $(testo)))_files.cpp;)
# }
##

## Compile executable

$(EXE): create_asset_files $(RSCOBJS) $(SRCOBJS)
	@mkdir -p target/$(TARGET)
	$(CXX) $(SRCOBJS) $(RSCOBJS) -o $@ $(LDFLAGS)
##

clean:
	-rm -rf $(EXE) $(OBJDIR)

###########

########################################
# Automatic makefile generation (-MMD) #
########################################

ifneq ($(MAKECMDGOALS),clean)
$(MAKEFILES): ;
-include $(MAKEFILES)
endif

########################################

