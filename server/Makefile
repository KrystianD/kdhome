CXX=g++
CC=gcc
KDLIB=../kdlibs

INC+=-I/usr/include/libusb-1.0
INC+=-I/usr/include/lua5.1
INC+=-I/usr/include/mysql
INC+=-I3rd -I. -I..
INC+=-I3rd/libconfig-1.4.9
INC+=-I3rd/luabind-0.9.1/luabind/
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
OBJS+=$(KDLIB)/Logger.o $(KDLIB)/kutils.o
OBJS+=3rd/format.o
OBJS+=lua_funcs_gen.o lua_funcs.o /usr/lib/libtolua++5.1.a
OBJS+=3rd/mongoose.o
OBJS+=3rd/libconfig-1.4.9/grammar.o
OBJS+=3rd/libconfig-1.4.9/libconfig.o
OBJS+=3rd/libconfig-1.4.9/scanctx.o
OBJS+=3rd/libconfig-1.4.9/scanner.o
OBJS+=3rd/libconfig-1.4.9/strbuf.o
OBJS+=3rd/libconfig-1.4.9/libconfigcpp.o
OBJS+=ICURL.o

OBJS+=3rd/luabind-0.9.1/src/class.o
OBJS+=3rd/luabind-0.9.1/src/class_info.o
OBJS+=3rd/luabind-0.9.1/src/class_registry.o
OBJS+=3rd/luabind-0.9.1/src/class_rep.o
OBJS+=3rd/luabind-0.9.1/src/create_class.o
OBJS+=3rd/luabind-0.9.1/src/error.o
OBJS+=3rd/luabind-0.9.1/src/exception_handler.o
OBJS+=3rd/luabind-0.9.1/src/function.o
OBJS+=3rd/luabind-0.9.1/src/inheritance.o
OBJS+=3rd/luabind-0.9.1/src/link_compatibility.o
OBJS+=3rd/luabind-0.9.1/src/object_rep.o
OBJS+=3rd/luabind-0.9.1/src/open.o
OBJS+=3rd/luabind-0.9.1/src/pcall.o
OBJS+=3rd/luabind-0.9.1/src/scope.o
OBJS+=3rd/luabind-0.9.1/src/stack_content_by_name.o
OBJS+=3rd/luabind-0.9.1/src/weak_ref.o
OBJS+=3rd/luabind-0.9.1/src/wrapper_base.o

DEFINES+=-DLIBCONFIG_STATIC
DEFINES+=-DLIBCONFIGXX_STATIC

LFLAGS=-L/usr/lib -llua5.1 -lpthread -lcurl
CFLAGS=-ggdb -g -O1 $(DEFINES) #-std=c++11

kdhome: lua_gen $(OBJS)
	$(CXX) $(OBJS) $(INC) $(LFLAGS) $(CFLAGS) -o $@

lua_gen:
	./gen_lua.sh

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
		3rd/luabind-0.9.1/*.o \
		providers/*.o \
		pch.h.gch \
		kdhome 
