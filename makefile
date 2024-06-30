output: main.o
	gcc -o output main.o -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

main.o: main.c
	gcc -c main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	sudo rm *.o ./output

