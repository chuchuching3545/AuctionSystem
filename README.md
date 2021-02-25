# AuctionSystem

## Problem Description

![image](https://www.csie.ntu.edu.tw/~b06303036/picture)
There are *N* players using **one** auction system. The auction system has *M* hosts. Every host holds an auction at a time. The auction system will continuously choose **8** players and distribute them to every host, until all combinations of players are distributed. Every combination will only be distributed once, i.e., there will be <img src= "https://render.githubusercontent.com/render/math?math=%5Ctextstyle+%5Cbegin%7Bequation%2A%7D%0A%5Cbegin%7Bpmatrix%7D%0AN+%5C%5C%0A8%0A%5Cend%7Bpmatrix%7D%0A%5Cend%7Bequation%2A%7D%0A">  auctions to hold. If <img src= "https://render.githubusercontent.com/render/math?math=%5Ctextstyle+%5Cbegin%7Bequation%2A%7D%0A%5Cbegin%7Bpmatrix%7D%0AN+%5C%5C%0A8%0A%5Cend%7Bpmatrix%7D%0A%5Cend%7Bequation%2A%7D%0A">  > *M*, there will not be enough hosts to hold all auctions at once. Once running out of host, the auction system has to wait until an auction has ended, and it can distribute another 8 players to the available host.

Once a host received 8 players from the auction system, it will recursively fork itself **2** times. In other words, the host (root) first forks itself into two child hosts. The two child hosts then fork themselves into a total of four leaf hosts, two each. So, Each leaf host will be distributed to 2 players.

After the tree structure is constructed, there will be **10** rounds of bid comparison in an auction to decide the winner. For each round, child hosts have to report winner to their parents recursively. The root host collects the winners of all 10 rounds, and it ranks the players. Afterwards, the root host reports the rankings to the auction system. The auction system then transforms the rankings into scores and sum up scores of all <img src= "https://render.githubusercontent.com/render/math?math=%5Ctextstyle+%5Cbegin%7Bequation%2A%7D%0A%5Cbegin%7Bpmatrix%7D%0AN+%5C%5C%0A8%0A%5Cend%7Bpmatrix%7D%0A%5Cend%7Bequation%2A%7D%0A"> auctions. The implementation details will be explained later.

For example, suppose there are 9 players, A, B, C, D, E, F, G, H, I, and 2 hosts: H1, H2. We get <img src= "https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle+%5Cbegin%7Bequation%2A%7D%0A%5Cbegin%7Bpmatrix%7D%0A9+%5C%5C%0A8%0A%5Cend%7Bpmatrix%7D%0A%5Cend%7Bequation%2A%7D%0A"> = 9 combinations of players. Say we first assign (A, B, C, D, E, F, G, H) to H1 and (A, B, C, D, E, F, G, I) to H2, then other combinations have to wait for H1 or H2 to finish.

For H1, it will fork itself into two child hosts: H11, H12. They will be assigned (A, B, C, D) and (E, F, G, H) respectively. Same for H2.

For H11, it will further fork itself into H111 and H112. They will be assigned (A, B) and (C, D) respectively. Same for H12, H21, H22.

In a round, H111 compares the bid of A and B. It reports the winner to H11. Then H11 compares the bid of the winners of H111 and H112, and reports the winner. Eventually, H1 (root host) compares the bid of the winners of H11 and H12, and gets the winner of all 8 players.

After 10 rounds, the root host reports the ranking to the auction system. The auction system sums up scores of 9 auctions, and outputs the result.

## auction_system.sh
**Run**
```bash=
$ bash auction_system.sh [n_host] [n_player]
```
* It requires two arguments
	* `n_host` : the number of hosts (1 ≤ M ≤ 10)
	- `n_player` : the number of players (8 ≤ N ≤ 12)

## host.c
**Run**
```bash=
$ make host
$ ./host [host_id] [key] [depth]
```
* It requires three arguments
	* `host_id` : the id of host
	- `key` : a key randomly generated for each host (0 ≤ key ≤ 65535)
		* Used for auction system to map the responses from hosts, since all hosts write to the same FIFO file.
		- The host_id and key of forked child hosts should be the same as their parents.
	+ `depth` : the depth of hosts
		* Starting from 0 and incrementing by 1 per fork

## player.c
**Run**
```bash=
$ make player
$ ./player [player_id]
```
* It requires one argument
	- `player_id` : the id of player

## Sample Execution
**Run**
```bash=
$ bash auction_system.sh 1 8
```
**Output**
```shell=
1 7
2 7
3 7
4 7
5 1
6 3
7 3
8 1
```
