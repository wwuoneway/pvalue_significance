CXX=g++
PROGNAME      = runPSigCalc
DIR_SRC       = ./src
DIR_INC       = ./include
INCLUDES      = $(wildcard $(DIR_INC)/*.h)
SOURCES       = runPSigCalc.cxx $(wildcard $(DIR_SRC)/*.cxx)
OBJECTS       = $(patsubst %.cxx, %.o, $(SOURCES))
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTLIBS     := $(shell root-config --nonew --libs)
CFLAGS       += $(ROOTCFLAGS) -I$(DIR_INC) -I$(ROOTSYS)/include
LIBS         += $(ROOTLIBS)
#  Not sure why Minuit isn't being included -- put in by hand
#
LIBS         += -lMinuit -lMinuit2 -lRooFit -lRooFitCore -lRooStats
LDFLAGS       = -O

$(PROGNAME):    $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

%.o : %.cxx $(INCLUDES)
	$(CXX) ${CFLAGS} -c  -g -o $@ $<

test:
	@echo $(ROOTCFLAGS)

clean:
	rm -f ${PROGNAME} ${OBJECTS}
