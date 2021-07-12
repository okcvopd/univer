all: client server
server: server.o conn.o database.o library.o query.o
	g++ server.o conn.o database.o library.o query.o -o server
client: client.o conn.o database.o library.o query.o
	g++ client.o conn.o database.o library.o query.o -o client
server.o: server.cpp conn.hpp database.h query.h
	g++ -Wall -Wextra -Werror -pedantic -std=gnu++0x -c server.cpp
client.o: client.cpp library.h conn.hpp database.h query.h
	g++ -Wall -Wextra -Werror -pedantic -std=gnu++0x -c client.cpp
conn.o: conn.cpp conn.hpp database.h query.h
	g++ -Wall -Wextra -Werror -pedantic -std=gnu++0x -c conn.cpp
library.o: library.cpp library.h
	g++ -Wall -Wextra -Werror -pedantic -std=gnu++0x -c library.cpp
query.o: query.cpp query.h library.h database.h
	g++ -Wall -Wextra -Werror -pedantic -std=gnu++0x -c query.cpp
database.o: database.cpp database.h library.h query.h
	g++ -Wall -Wextra -Werror -pedantic -std=gnu++0x -c database.cpp
clean:
	rm -f *.o server client
