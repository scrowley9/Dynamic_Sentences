CFILES=dynamic_sentence.c

dynamic_sentence:
	gcc $(CFILES) -o $@

gdb:
	gcc $(CFILES) -g
	gdb ./a.out

clean:
	rm -rf dynamic_sentence