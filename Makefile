rsv.o: rsv_table.c rsv.c rsv.h
	gcc -c -g -Wall rsv.c

test: rsv.o rsv_test.c
	gcc -g -Wall rsv_test.c rsv.o -o rsv_test
	./rsv_test

clean:
	rm -f rsv_test *.o a.out *.rsv
