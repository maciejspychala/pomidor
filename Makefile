all: pomidor

pomidor: src/*.c
	$(CC) -o pomidor $(wildcard src/*.c)

clean:
	rm pomidor

