#make only the pc-way !
CPP = g++
EXT = cpp
BIN = test

CFLAGS = -Wall -g -Wextra 
#CFLAGS = -O3

FLAGS = ``
OBJDIR1 = obj
OBJDIR2 = obj
SRCDIR1 = commun
SRCDIR2 = pc
INCDIR1 = $(SRCDIR1)
INCDIR2 = $(SRCDIR2)
BINDIR = .
LDFLAGS = -I$(INCDIR1) -I$(INCDIR2) $(FLAGS)

SRC1 = $(wildcard $(SRCDIR1)/*.$(EXT))
OBJ1 = $(SRC1:$(SRCDIR1)/%.$(EXT)=$(OBJDIR1)/%.o)
SRC2 = $(wildcard $(SRCDIR2)/*.$(EXT))
OBJ2 = $(SRC2:$(SRCDIR2)/%.$(EXT)=$(OBJDIR2)/%.o)

all: $(BIN)
	 
$(BIN): $(OBJ1) $(OBJ2)
		echo $@ $^ $<
		$(CPP) -o $(BINDIR)/$@ $^ $(LDFLAGS) $(CFLAGS)
		 
$(OBJDIR1)/%.o: $(SRCDIR1)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) $(LDFLAGS) 

$(OBJDIR2)/%.o: $(SRCDIR2)/%.$(EXT)
		$(CPP) -o $@ -c $< $(CFLAGS) $(LDFLAGS) 
		 
clean:
		rm -vf $(OBJDIR1)/*.o
		rm -vf $(OBJDIR2)/*.o

mrproper: clean
		rm -vf $(BINDIR)/$(BIN)
			
#install: $(BIN)
#		cp $(BINDIR)/$(BIN) /usr/bin/
