all: compil run

run: 
	./prog 1

compil: graphe.c
	gcc -Wall graphe.c -o prog  


clean:
	rm -rf *.o
	rm -rf *.dot
	rm -rf *.txt
	rm -rf *.png
	rm -rf prog

debug:
	valgrind ./prog

