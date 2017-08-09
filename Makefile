all: pomidor

pomidor: src/*.c
	$(CC) -o pomidor $(wildcard src/*.c) -Wall

clean:
	rm pomidor

