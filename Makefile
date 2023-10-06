build:
	gcc -o ./bin/main ./src/main.c ./src/http/http.c

run:  
	make build
	./main