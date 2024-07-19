client: client.c
	gcc -o client.out client.c

server: serve.c
	gcc -o serve.out serve.c

all: client server
clean:
	rm -f *.out

# use baer -- make all to generate compile_commands.json if you are using clangd or sth others