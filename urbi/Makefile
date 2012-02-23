CC = g++
INCLUDES = -I $${URBI_ROOT}/include -I ../commun
CFLAGS = -shared -fPIC
TARGET = ULib.so
SOURCES = \
	../commun/Peripherique.cpp \
	../commun/Commande.cpp \
	../pc/GaopProtocol.cpp \
	UGaop.cpp \
	../pc/Led.cpp \
        ULed.cpp

default: target

target: $(SOURCES)
	$(CC) $(INCLUDES) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	urbi-launch --start $(TARGET) -- main.u

interactive: $(TARGET)
	urbi-launch --start $(TARGET) -- --interactive

clean:
	rm $(TARGET)
