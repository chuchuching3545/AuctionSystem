all : host player
host : host.c
	gcc -std=gnu99 -O3 host.c -o host
player : player.c
	gcc -std=gnu99 -O3 player.c -o player 

clean:
	rm host player
