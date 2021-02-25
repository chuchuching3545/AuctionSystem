#!/sbin/bash


function find_host(){                 #arg1=num_host  arg2=key
	for ((j = 1; j <= $1; j++)); do
		if [ ${arr[$j]} == $2 ]; then
			return $j
		fi
	done
}

function conbination(){
	local i=1
	if [ $pos -eq 8 ]
	then
		C[$index]="${num[0]} ${num[1]} ${num[2]} ${num[3]} ${num[4]} ${num[5]} ${num[6]} ${num[7]}"          #C is the conbination
		let index+=1
		return 0
	else
		for (( i = $1; i <= $2 ; ++i )); do
			if [ ${isexist_l[$i]} -eq 1 ]; then
				continue
			else
				num[$pos]=$i
				isexist_l[$i]=1
				let pos+=1
				conbination `expr $i + 1` $2
				isexist_l[$i]=0
				let pos-=1
			fi
		done
	fi
	return 0
}
function isexist(){
	for ((j = 1; j < $i; ++j)); do
		if [ ${arr[$j]} = $rdnum ]; then
			return 1
		fi
	done
	return 0
}
index=0
pos=0
for (( i = 0; i <= $2; i++ )); do
	isexist_l[$i]=0
done
conbination 1 $2


#initial
for ((i = 1; i <= $2; i++)); do
	final_score[$i]=0             #final_score[i] is the final score of i-th player
done


for ((i = 1; i <= $1; ++i)); do
	rdnum=`od -vAn -N2 -tu2 < /dev/urandom`
	isexist
	if [ $? = 1 ]; then
		let i-=1
	else
		arr[$i]=$rdnum                 #arr[i] is the key of i-th host
	fi
done



index_C=0
mkfifo fifo_0.tmp
for ((i = 1; i <= index && i <= $1; i++)); do
	mkfifo fifo_$i.tmp
	cmd="exec $[$i+2]<> fifo_$i.tmp"
	eval $cmd        #shoud open it for writing
	echo ${C[$index_C]} > fifo_$i.tmp
	let index_C+=1
	./host $i ${arr[$i]} 0 &
done
	
while read line; do
	find_host $1 $line
	cur_host=$?
	for ((i = 0; i < 8; ++i)); do
		read line
		player_id=$(echo "$line" | cut -d' ' -f 1)
		rank=$(echo "$line" | cut -d' ' -f 2)
		let final_score[$player_id]+=`expr 8 - $rank`
	done
	if [ $index_C == $index ]; then
		echo "-1 -1 -1 -1 -1 -1 -1 -1" > fifo_$cur_host.tmp
	else
		echo ${C[$index_C]} > fifo_$cur_host.tmp
		let index_C+=1 
	fi
done < fifo_0.tmp

wait

for ((i = 1; i <= index && i <= $1; ++i)); do
	cmd="exec $[$i+2]> $-"
	eval $cmd
done
for ((i = 0; i <= $1; ++i)); do
	rm -f fifo_$i.tmp
done
for ((i = 1; i <= $2; ++i)); do
	echo "$i ${final_score[$i]}"
done




