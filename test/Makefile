.PHONY: test

CFLAGS=-I ../src -Wall -g -DSENTINEL

test: test-rbtree
	./test-rbtree
	valgrind ./test-rbtree

test-rbtree: test-rbtree.o ../src/rbtree.o

../src/rbtree.o:
	$(MAKE) -C ../src rbtree.o

clean:
	rm -f test-rbtree *.o
	rm -f ../src/rbtree.o

# .PHONY: test

# CFLAGS=-I ../src -Wall -g -DSENTINEL

# test: test-rbtree
# 	./test-rbtree
# 	valgrind ./test-rbtree

# test-rbtree: test-rbtree.o ../src/rbtree copy.o

# ../src/rbtree.o:
# 	$(MAKE) -C ../src rbtree copy.o

# clean:
# 	rm -f test-rbtree *.o
# 	rm -f ../src/rbtree copy.o