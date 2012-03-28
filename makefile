#make only the pc-way !
CPP = g++
EXT = cpp
BIN = test

CFLAGS = -Wall -g -Wextra -DIAmNotOnThePandaBoard=0  
#CFLAGS = -O3 -DIAmNotOnThePandaBoard=0

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
	 
$(BIN): $(OBJCOMMUN) $(OBJPC)
		$(CPP) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CFLAGS)
		 
$(OBJ)/%.o: $(COMMUN)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) -I$(COMMUN) 

$(OBJ)/%.o: $(PC)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) -I$(COMMUN) -I$(PC) 
		 
clean:
		rm -vf $(OBJ)/*.o

mrproper: clean
		rm -vf $(BINDIR)/$(BIN)

#install ide

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

$(SKETCHBOOK)/userspaceUsbSerie/%: $(ARDUINO)/%
	cp  $< $@

$(SKETCHBOOK)/userspaceUsbSerie/%: $(COMMUN)/%
	cp  $< $@

