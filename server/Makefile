CXX=g++
CC=gcc
KDLIB=../kdutils

INC+=-I3rd -I. -I..
INC+=-I3rd/libconfig-1.4.9
INC+=-I$(KDLIB)

OBJS=main.o Program.o
OBJS+=UdpServer.o
OBJS+=Controller.o
OBJS+=EthernetDevice.o
OBJS+=StorageEngine.o
OBJS+=Providers.o
OBJS+=providers/EthernetOutputProvider.o
OBJS+=providers/EthernetInputProvider.o
OBJS+=providers/EthernetTempProvider.o
OBJS+=providers/EthernetIRProvider.o
OBJS+=$(KDLIB)/Logger.o $(KDLIB)/kdutils.o
OBJS+=3rd/format.o
OBJS+=3rd/libconfig-1.4.9/grammar.o
OBJS+=3rd/libconfig-1.4.9/libconfig.o
OBJS+=3rd/libconfig-1.4.9/scanctx.o
OBJS+=3rd/libconfig-1.4.9/scanner.o
OBJS+=3rd/libconfig-1.4.9/strbuf.o
OBJS+=3rd/libconfig-1.4.9/libconfigcpp.o
# OBJS+=ICURL.o

DEFINES+=-DLIBCONFIG_STATIC
DEFINES+=-DLIBCONFIGXX_STATIC

LFLAGS=-L/usr/lib -lpthread -lzmq
CFLAGS=-ggdb -g -O1 $(DEFINES) -std=c++11

kdhome: $(OBJS)
	$(CXX) $(OBJS) $(INC) $(LFLAGS) $(CFLAGS) -o $@

pch.h.gch: pch.h
	$(CXX) -c $< $(INC) $(CFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< $(INC) $(CFLAGS) -o $@
%.o: %.cc
	$(CXX) -c $< $(INC) $(CFLAGS) -o $@
%.o: %.c
	$(CC) -c $< $(INC) $(CFLAGS) -o $@

clean:
	rm -f *.o $(KDLIB)/*.o providers/*.o kdhome 

cleanall:
	rm -f *.o $(KDLIB)/*.o \
		3rd/*.o \
		3rd/libconfig-1.4.9/*.o \
		providers/*.o \
		pch.h.gch \
		kdhome 
