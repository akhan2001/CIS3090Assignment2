all: a2encrypt.o a2decrypt.o

a2encrypt.o: a2encrypt.c
	gcc a2encrypt.c -o a2encrypt

a2decrypt.o: a2decrypt.c
	mpicc a2decrypt.c -o a2decrypt

clean:
	rm a2encrypt a2decrypt