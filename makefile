all:
	gcc -c inaudio.c -Wall -Wextra
	gcc -c preprocessing.c -Wall -Wextra
	gcc -c linklist.c -Wall -Wextra
	gcc -c compare.c -Wall -Wextra
	gcc -c link.c -Wall -Wextra
	gcc -c neuron.c
	gcc -c mtrand.c 
	ar rcs liblink.a *.o
	gcc -o mfcc main.c -L. -llink -ldl -lm -lasound -lpthread -Wall -Wextra

clean:
	rm mfcc *~
