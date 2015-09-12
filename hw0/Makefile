all: main wc map
main: main.o
	gcc main.o -o main
wc: wc.o
	gcc wc.o -o wc
map: map.o
	gcc map.o -o map
main.o: main.c
	gcc -c main.c
wc.o: wc.c
	gcc -c wc.c
map.o: map.c
	gcc -c map.c
clean_main: 
	rm main.o main
clean_wc:
	rm wc.o wc
clean_map:
	rm map.o map
clean:
	rm *.o main wc map
