CXX=g++
KDLIB=kdlibs

INC+=-I/usr/include/libusb-1.0
INC+=-I/usr/include/lua5.1
INC+=-I/usr/include/mysql
INC+=-I3rd -I.
INC+=-I$(KDLIB)

OBJS=main.o Program.o
OBJS+=UdpServer.o
OBJS+=Controller.o
OBJS+=EthernetDevice.o
OBJS+=providers/EthernetOutputProvider.o
OBJS+=providers/EthernetInputProvider.o
OBJS+=providers/EthernetIRProvider.o
OBJS+=$(KDLIB)/Logger.o $(KDLIB)/kutils.o
OBJS+=3rd/format.o
OBJS+=lua_funcs_gen.o lua_funcs.o /usr/lib/libtolua++5.1.a
OBJS+=3rd/mongoose-5.0/mongoose.o
OBJS+=ICURL.o

LFLAGS=-L/usr/lib -llua5.1 -lluabind -lmysqlclient -lpthread -lcurl
CFLAGS=-ggdb -g -O0 $(DEFINES) -std=c++11

kdhome: lua_gen $(OBJS)
	$(CXX) $(OBJS) $(INC) $(LFLAGS) $(CFLAGS) -o $@

lua_gen:
	./gen_lua.sh

pch.h.gch: pch.h
	$(CXX) -c $< $(INC) $(CFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< $(INC) $(CFLAGS) -o $@

clean:
	rm -f *.o $(KDLIB)/*.o providers/*.o kdhome 

cleanall:
	rm -f *.o $(KDLIB)/*.o 3rd/*.o providers/*.o pch.h.gch kdhome 
