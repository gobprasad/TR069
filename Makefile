CC=g++
CFLAGS=-c -Wall -D_WITHDIGEST_ -g
LDFLAGS=-I/usr/include/libxml2/
SOURCES=TRClient.cpp \
		src/WebClient.cpp \
		src/XmlInterface.cpp \
		src/Event.cpp \
		src/RpcMethods.cpp \
		src/Database.cpp
INCLUDE=-Iinc/ 

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=TRClient

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -lxml2 -lcurl -O3 -o $@

.cpp.o:
	$(CC) $(INCLUDE) $(LDFLAGS) $(CFLAGS) $< -o $@
	
clean:
	rm -rf $(EXECUTABLE) *.o src/*.o
