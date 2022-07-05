FLAG= -O3 -fsanitize=address -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
LASAN= -lasan 
all: tcp
tcp:server.o client.o
a.out:main.o record.o command.o
	g++ $(FLAG) $^ $(LASAN) -o $@
%.o: %.cpp
	g++ -c $(FLAG) $< $(LASAN) -o $@

server.o: 
	g++ -c $(FLAG) server.cpp $(LASAN)  -o server

client.o: 
	g++ -c $(FLAG) client.cpp $(LASAN)  -o client


record.o:record.cpp 
command.o:command.cpp

a.out:command.o main.o record.o 
main.o:main.cpp 

clean:
	rm -f *.o
	rm -f *.out
	rm -f server
	rm -f client
