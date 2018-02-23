all:
	bison -d -o parser.c parser.y
	flex -i -o scanner.c  scanner.l
	gcc -std=c99 -pedantic -pedantic -o slc *.c

ans :
	./slc<prog22.sl>prog22.mep
	./mepa/mepa --limit 12000 --progfile prog22.mep < data22.in > prog22.res
