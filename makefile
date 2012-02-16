#make only the pc-way !
CPP = g++
EXT = cpp
BIN = test

#CFLAGS = -Wall -g -Wextra 
CFLAGS = -O3

LDFLAGS = -lrt -pthread
OBJ = obj
COMMUN = commun
PC = pc
ARDUINO = arduino
SKETCHBOOK = ~/sketchbook
INCDIR1 = $(COMMUN)
INCDIR2 = $(PC)
BINDIR = .
INCFLAGS = -I$(INCDIR1) -I$(INCDIR2) #$(FLAGS)

SRC1 = $(wildcard $(COMMUN)/*.$(EXT))
OBJCOMMUN = $(SRC1:$(COMMUN)/%.$(EXT)=$(OBJ)/%.o)
SRC2 = $(wildcard $(PC)/*.$(EXT))
OBJPC = $(SRC2:$(PC)/%.$(EXT)=$(OBJ)/%.o)

all: $(BIN)
	 
$(BIN): $(OBJCOMMUN) $(OBJPC)
		$(CPP) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CFLAGS)
		 
$(OBJ)/%.o: $(COMMUN)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) $(INCFLAGS) 

$(OBJ)/%.o: $(PC)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) $(INCFLAGS) 
		 
clean:
		rm -vf $(OBJ)/*.o

mrproper: clean
		rm -vf $(BINDIR)/$(BIN)

install-ide:
	@cp -Ru $(ARDUINO)/* $(COMMUN)/* $(SKETCHBOOK)/userspaceUsbSerie/ #copie les sources cote ide. plus qu'a compiler avec celui ci.
	@#edite le fichier de config pour que sa compile sur arduino
	@sed -e 's/\/\/\s*#define\s\+IAmNotOnThePandaBoard/#define IAmNotOnThePandaBoard/' $(COMMUN)/Config.h > $(SKETCHBOOK)/userspaceUsbSerie/Config.h
	@echo done

#install: $(BIN)
#		cp $(BINDIR)/$(BIN) /usr/bin/
