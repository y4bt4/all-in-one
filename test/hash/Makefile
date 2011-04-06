obj = words.o myhash.o test_myhash.o

all : test_myhash

test_myhash: $(obj)
	cc -o test_myhash $(obj)

words: words.c
myhash: myhash.c
test_myhash: test_myhash.c

clean:
	rm -f *.o test_myhash
test:
	./test_myhash 100 10 5
