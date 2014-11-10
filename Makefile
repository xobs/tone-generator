all:
	$(CC) -Wall -lm notes-v1.c -o notes-v1
	$(CC) -Wall -lm notes-v2.c -o notes-v2

clean:
	rm -f notes-v1 notes-v2
