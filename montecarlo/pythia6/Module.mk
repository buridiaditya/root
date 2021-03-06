# Module.mk for pythia6 module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODNAME      := pythia6
MODDIR       := $(ROOT_SRCDIR)/montecarlo/$(MODNAME)
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

PYTHIA6DIR   := $(MODDIR)
PYTHIA6DIRS  := $(PYTHIA6DIR)/src
PYTHIA6DIRI  := $(PYTHIA6DIR)/inc

##### libEGPythia6 #####
PYTHIA6L     := $(MODDIRI)/LinkDef.h
PYTHIA6DS    := $(call stripsrc,$(MODDIRS)/G__Pythia6.cxx)
PYTHIA6DO    := $(PYTHIA6DS:.cxx=.o)
PYTHIA6DH    := $(PYTHIA6DS:.cxx=.h)

PYTHIA6H     := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
PYTHIA6S     := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
PYTHIA6O     := $(call stripsrc,$(PYTHIA6S:.cxx=.o))

PYTHIA6DEP   := $(PYTHIA6O:.o=.d) $(PYTHIA6DO:.o=.d)

PYTHIA6LIB   := $(LPATH)/libEGPythia6.$(SOEXT)
PYTHIA6MAP   := $(PYTHIA6LIB:.$(SOEXT)=.rootmap)

PYTHIA6LDF   := $(LDFLAGS)
ifeq ($(FPYTHIA6LIB),)
ifeq ($(PLATFORM),linux)
# Pythia library not provided at config time
# we have to turn-off the "-Wl,--no-undefined" linker flag
PYTHIA6LDF   := $(filter-out %no-undefined,$(LDFLAGS))
endif
ifeq ($(PLATFORM),macosx)
# Pythia library not provided at config time
# we have to add the "-undefined dynamic_lookup" linker flag
PYTHIA6LDF   += -undefined dynamic_lookup
endif
endif

# used in the main Makefile
PYTHIA6H_REL := $(patsubst $(MODDIRI)/%.h,include/%.h,$(PYTHIA6H))
ALLHDRS     += $(PYTHIA6H_REL)
ALLLIBS     += $(PYTHIA6LIB)
ALLMAPS     += $(PYTHIA6MAP)
ifeq ($(CXXMODULES),yes)
  CXXMODULES_HEADERS := $(patsubst include/%,header \"%\"\\n,$(PYTHIA6H_REL))
  CXXMODULES_MODULEMAP_CONTENTS += module Montecarlo_$(MODNAME) { \\n
  CXXMODULES_MODULEMAP_CONTENTS += $(CXXMODULES_HEADERS)
  CXXMODULES_MODULEMAP_CONTENTS += "export \* \\n"
  CXXMODULES_MODULEMAP_CONTENTS += link \"$(PYTHIA6LIB)\" \\n
  CXXMODULES_MODULEMAP_CONTENTS += } \\n
endif

# include all dependency files
INCLUDEFILES += $(PYTHIA6DEP)

##### local rules #####
.PHONY:         all-$(MODNAME) clean-$(MODNAME) distclean-$(MODNAME)

include/%.h:    $(PYTHIA6DIRI)/%.h
		cp $< $@

$(PYTHIA6LIB):  $(PYTHIA6O) $(PYTHIA6DO) $(ORDER_) $(MAINLIBS) $(PYTHIA6LIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(PYTHIA6LDF)" \
		   "$(SOFLAGS)" libEGPythia6.$(SOEXT) $@ \
		   "$(PYTHIA6O) $(PYTHIA6DO)" \
		   "$(PYTHIA6LIBEXTRA) $(FPYTHIA6LIBDIR) $(FPYTHIA6LIB)"

$(call pcmrule,PYTHIA6)
	$(noop)

$(PYTHIA6DS):   $(PYTHIA6H) $(PYTHIA6L) $(ROOTCLINGEXE) $(call pcmdep,PYTHIA6)
		$(MAKEDIR)
		@echo "Generating dictionary $@..."
		$(ROOTCLINGSTAGE2) -f $@ $(call dictModule,PYTHIA6) -c $(PYTHIA6H) $(PYTHIA6L)

$(PYTHIA6MAP):  $(PYTHIA6H) $(PYTHIA6L) $(ROOTCLINGEXE) $(call pcmdep,PYTHIA6)
		$(MAKEDIR)
		@echo "Generating rootmap $@..."
		$(ROOTCLINGSTAGE2) -r $(PYTHIA6DS) $(call dictModule,PYTHIA6) -c $(PYTHIA6H) $(PYTHIA6L)

all-$(MODNAME): $(PYTHIA6LIB)

clean-$(MODNAME):
		@rm -f $(PYTHIA6O) $(PYTHIA6DO)

clean::         clean-$(MODNAME)

distclean-$(MODNAME): clean-$(MODNAME)
		@rm -f $(PYTHIA6DEP) $(PYTHIA6DS) $(PYTHIA6DH) \
		   $(PYTHIA6LIB) $(PYTHIA6MAP)

distclean::     distclean-$(MODNAME)

##### extra rules ######
$(PYTHIA6O):    CXXFLAGS += $(FPYTHIA6CPPFLAGS)
