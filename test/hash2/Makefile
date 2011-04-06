obj = words.o myhash.o test_myhash.o hires.o
cc = gcc
flag = -O3

all : test_myhash

test_myhash: $(obj)
	$(cc) $(flag) -o test_myhash $(obj)

hires.o: hires.c hires.h
	$(cc) $(flag) -c -o hires.o hires.c
words.o: words.c words.h
	$(cc) $(flag) -c -o words.o words.c
myhash.o: myhash.c myhash.h
	$(cc) $(flag) -c -o myhash.o myhash.c
test_myhash.o: test_myhash.c
	$(cc) $(flag) -c -o test_myhash.o test_myhash.c

clean:
	rm -f *.o test_myhash
test: all
	./test_myhash 10000000
full:
	g++ $(flag) -o test_myhash hires.c words.c myhash.c test_myhash.c

