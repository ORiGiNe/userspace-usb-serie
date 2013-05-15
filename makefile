CPP = g++
EXT = cpp
BIN = prog_pc
ARDUINO_MAKEFILE=makefile.arduino

CFLAGS = -Wall -Wextra -DIAmNotOnThePandaBoard=0 $(DEBUG)

LDFLAGS = -lrt -pthread
OBJ = obj
COMMUN = commun
PC = pc
ARDUINO = arduino
SKETCHBOOK = ~/sketchbook
BINDIR = .
INCFLAGS = -I$(INCDIR1) -I$(INCDIR2)

SRC1 = $(wildcard $(COMMUN)/*.$(EXT))
OBJCOMMUN = $(SRC1:$(COMMUN)/%.$(EXT)=$(OBJ)/%.o)
SRC2 = $(wildcard $(PC)/*.$(EXT))
OBJPC = $(SRC2:$(PC)/%.$(EXT)=$(OBJ)/%.o)

all: $(BIN)
#	cd arduino
#	make -f arduino/$(ARDUINO_MAKEFILE) upload

.PHONY: debug clean mrproper .sketchbook install-ide arduino upload
debug:
	$(MAKE) $(MAKEFILE) DEBUG="-g -DORiGi insNe_DEBUG"

$(BIN): $(OBJCOMMUN) $(OBJPC)
		$(CPP) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CFLAGS)

$(OBJ)/%.o: $(COMMUN)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) -I$(COMMUN)

$(OBJ)/%.o: $(PC)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) -I$(COMMUN) -I$(PC)

clean:
		rm -vf $(OBJ)/*.o
		+make -f $(ARDUINO_MAKEFILE) clean

mrproper: clean
		rm -vf $(BINDIR)/$(BIN)
		+make -f $(ARDUINO_MAKEFILE) mrproper

# For install in IDE's sketchbook
.sketchbook:
	@if ! test -d $(SKETCHBOOK); \
	then mkdir $(SKETCHBOOK); \
	fi
	@if ! test -d $(SKETCHBOOK)/userspaceUsbSerie; \
	then mkdir $(SKETCHBOOK)/userspaceUsbSerie; \
	fi

TMP1 = $(wildcard $(ARDUINO)/*)
TMP2 = $(wildcard $(COMMUN)/*)
ARDUINOSRC = $(TMP1:$(ARDUINO)/%=%) $(TMP2:$(COMMUN)/%=%)

install-ide: .sketchbook $(ARDUINOSRC:%=$(SKETCHBOOK)/userspaceUsbSerie/%)
	@echo files succesfully copied

arduino: $(wildcard $(ARDUINOSRC)/*)
	+make -f $(ARDUINO_MAKEFILE)

upload: arduino
	+make -f $(ARDUINO_MAKEFILE) upload

$(SKETCHBOOK)/userspaceUsbSerie/%: $(ARDUINO)/%
	cp  $< $@

$(SKETCHBOOK)/userspaceUsbSerie/%: $(COMMUN)/%
	cp  $< $@

