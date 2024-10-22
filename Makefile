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
	rm -f ./Client  # 현재 디렉터리의 client 실행 파일 삭제
	rm -f ./Server  # 현재 디렉터리의 server 실행 파일 삭제
