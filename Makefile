.PHONY: all client server clean

all: client server

client:
	cd ./client && qmake && make
	mv ./client/Client ./

server:
	cd ./server && qmake && make
	mv ./server/Server ./

clean:
	cd ./client && make clean
	cd ./server && make clean
	rm -f ./Client  # ���� ���͸��� client ���� ���� ����
	rm -f ./Server  # ���� ���͸��� server ���� ���� ����
