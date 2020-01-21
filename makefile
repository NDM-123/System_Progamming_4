CC=gcc

FLAG=-Wall


all:frequency   



frequency: frequency.c

	$(CC) $(FLAG) frequency.c -g -o frequency



.PHONY: clean all



clean:

	rm -f *.o *.a  frequency