build:
	gcc tema1.c -o tema1 functii.c -lpthread -lm

test:
	valgrind --leak-check=full -q --log-file=a.out ./tema1 3 5 test0/test.txt
debug:
	gcc tema1.c -o tema1 functii.c -lpthread -DDEBUG -g3 -O0 -Wall -lm
clean:
	rm -rf tema1
	rm -rf out*
