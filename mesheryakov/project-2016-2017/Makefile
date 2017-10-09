all: main.cpp server.cpp service.cpp scripts/get_random_number.cpp scripts/get_random_number.cpp
	g++ main.cpp server.cpp service.cpp -std=c++11 -o runserver
	g++ scripts/get_random_number.cpp -o scripts/get_random_number.cgi
	echo "It's works!" > index.html
clean:
	rm -f runserver index.html scripts/*.cgi
