FLAG= -O3 -fsanitize=address -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
LASAN= -lasan 
all: tcp
tcp: record.o command.o server.o client.o  server.out client.out

server.out: server.o record.o command.o
	g++ $(FLAG) $^ $(LASAN) -o server
client.out: client.o
	g++ $(FLAG) $^ $(LASAN) -o client
%.o: %.cpp
	g++ -c $(FLAG) $< $(LASAN) -o $@

server.o:
	g++  -c $(FLAG) server.cpp $(LASAN)   -o server.o

client.o: 
	g++   -c $(FLAG) client.cpp $(LASAN)   -o client.o


server.o:command.o record.o 
record.o:record.cpp 
command.o:command.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f server
	rm -f client