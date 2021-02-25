#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
typedef struct player{
	int id;
	int score;
	int rank;
} Player;

void ini_Player(Player players[8], int player_id[8]){
	for(int i = 0; i < 8; i++){
		players[i].id = player_id[i];
		players[i].score = 0; 
	}
}
int find_index(int player_id[8], int winner){
	for(int i = 0; i < 8; i++){
		if(player_id[i] == winner) return i;
	}
}
int cmp_score(const void *A, const void *B){
	return ((Player *)B)->score - ((Player *)A)->score;
}
int cmp_id(const void *A, const void *B){
	return ((Player *)A)->id - ((Player *)B)->id;
}
void get_rank(Player players[8]){
	players[0].rank = 1;
	for(int i = 1; i < 8; i++){
		if(players[i].score == players[i - 1].score) players[i].rank = players[i - 1].rank;
		else players[i].rank = i + 1;
	}
}


int main(int argc, char** argv){
	pid_t child1, child2;
	int child1_1[2], child1_2[2], child2_1[2], child2_2[2]; //1:parent read child write 2:parent write child read
	int child3[2], child4[2];
	pipe(child1_1); pipe(child1_2); pipe(child2_1); pipe(child2_2);
	FILE *read_1, *read_2, *write_1, *write_2;
	if(argv[3][0] == '0'){
		int fd_r, fd_w;
		char file_name[40];
		sprintf(file_name, "fifo_%s.tmp", argv[1]);
		fd_r = open(file_name, O_RDONLY);
		//printf("%d", fd_r);
		fd_w = open("fifo_0.tmp", O_WRONLY);
		dup2(fd_r, 0);
		dup2(fd_w, 1);
		close(fd_r); close(fd_w);
	}
	

	if(argv[3][0] != '2'){
		if((child1 = fork()) > 0){
			if((child2 = fork()) > 0){
				read_1 = fdopen(child1_1[0], "r"); read_2 = fdopen(child2_1[0], "r"); write_1 = fdopen(child1_2[1], "w"); write_2 = fdopen(child2_2[1], "w");
				close(child1_1[1]); close(child1_2[0]);
				close(child2_1[1]); close(child2_2[0]);
			}
			else if(child2 == 0){
				dup2(child2_2[0], 0);
				dup2(child2_1[1], 1);
				close(child1_1[0]); close(child1_1[1]); close(child1_2[0]); close(child1_2[1]);
				close(child2_1[0]); close(child2_1[1]); close(child2_2[0]); close(child2_2[1]);
				char deep[2];
				deep[0] = argv[3][0] + 1; deep[1] = '\0';
				execl("./host", argv[0], argv[1], argv[2], deep, NULL);
			}
		}
		else if(child1 == 0){
			dup2(child1_2[0], 0);
			dup2(child1_1[1], 1);
			close(child1_1[0]); close(child1_1[1]); close(child1_2[0]); close(child1_2[1]);
			close(child2_1[0]); close(child2_1[1]); close(child2_2[0]); close(child2_2[1]);
			char deep[2];
			deep[0] = argv[3][0] + 1; deep[1] = '\0';
			execl("./host", argv[0], argv[1], argv[2], deep, NULL);
		}	
	}
	//printf("%d %s\n", getpid(), argv[3]);//
	char buf[50];
	int player_id[8];
	Player players[8];
	
	while(1){
		//get player id from parent
		for(int i = 0; i < ((argv[3][0] == '0')? 8 : (2 * (3 - atoi(argv[3])))); i++) scanf("%d", &player_id[i]);
		if(argv[3][0] == '0') ini_Player(players, player_id);
		//sent half player id to their two children
		if(argv[3][0] == '0'){
			fprintf(write_1, "%d %d %d %d\n", player_id[0], player_id[1], player_id[2], player_id[3]);
			fflush(write_1);
			fprintf(write_2, "%d %d %d %d\n", player_id[4], player_id[5], player_id[6], player_id[7]);
			fflush(write_2);
		}
		else if(argv[3][0] == '1'){
			fprintf(write_1, "%d %d\n", player_id[0], player_id[1]);
			fflush(write_1);
			fprintf(write_2, "%d %d\n", player_id[2], player_id[3]);
			fflush(write_2);
		}
		else{
			//check if end
			if(player_id[0] == -1) exit(0);
			pipe(child3); pipe(child4);
			if((child1 = fork()) > 0){
				if((child2 = fork()) > 0){
					read_1 = fdopen(child3[0], "r"); read_2 = fdopen(child4[0], "r");
					close(child3[1]); close(child4[1]);
				}
				else if(child2 == 0){
					dup2(child4[1], 1);
					close(child3[0]); close(child3[1]); close(child4[0]); close(child4[1]);
					sprintf(buf, "%d", player_id[0]);
					execl("./player", "player", buf, NULL);
				}
			}
			else if(child1 == 0){
				dup2(child3[1], 1);
				close(child3[0]); close(child3[1]); close(child4[0]); close(child4[1]);
				sprintf(buf, "%d", player_id[1]);
				execl("./player", "player", buf, NULL);
			}

		}
		//check if end
		if(player_id[0] == -1){
			waitpid(child1, NULL, 0);
			waitpid(child2, NULL, 0);
			exit(0);
		}

		//10 run
		for(int i = 0; i < 10; i++){
			//get winner and bid from their two children
			char winner[2][10], bid[2][10];
			fscanf(read_1, "%s", winner[0]);
			fscanf(read_1, "%s", bid[0]);
			fscanf(read_2, "%s", winner[1]);
			fscanf(read_2, "%s", bid[1]);
			
			/*if(argv[3][0] == '2'){
				char s[50];
				sprintf(s, "run: %d winner1: %s bid1: %s\n", i + 1, winner, bid);
				write(99, s, strlen(s));
			}*/
			if(argv[3][0] == '0'){
				int index;
				if(atoi(bid[0]) > atoi(bid[1])){
					index = find_index(player_id, atoi(winner[0]));
					players[index].score++;
				}
				else{
				 	index = find_index(player_id, atoi(winner[1]));
					players[index].score++;
				}
			}
			//sent best winner and bid to parent
			else{
				if(atoi(bid[0]) > atoi(bid[1])) printf("%s %s\n", winner[0], bid[0]);
				else printf("%s %s\n", winner[1], bid[1]);
				fflush(stdout);
			}
		}
		//wait player
		if(argv[3][0] == '2'){
			wait(NULL);
			wait(NULL);
		}
		//sent best winner and bid to parent
		if(argv[3][0] == '0'){
			qsort(players, 8, sizeof(Player), cmp_score);
			get_rank(players);
			qsort(players, 8, sizeof(Player), cmp_id);
			printf("%s\n", argv[2]);
			for(int i = 0; i < 8; i++){
				printf("%d %d\n", players[i].id, players[i].rank);
			}
			fflush(stdout);
	    }

	}
	return 0;
}












