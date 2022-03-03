#!/bin/bash

file_log="./testout.log"
rm -f $file_log


# **** test di tipo 1 ****
for (( i = 1; i <= 50; i++)) do
		./client client_$i 1 &>> $file_log &
	done
# *********************************************************

echo 
echo "----- Segnale SIGUSR1 al server -----" 
killall -10 objstore

wait #attendo che terminino


# ****  test di tipo 2 ****

for (( i = 1; i <= 30; i++)) do
	./client client_$i 2 &>> $file_log &
done


# **** test di tipo 2 ****

for (( i = 31; i <= 50; i++)) do
	./client client_$i 3 &>> $file_log &
done

# ******************************************************
wait #attendo che terminino

# terminazione server
killall -QUIT objstore


echo 
echo "---------- Elaborazione $file_log ----------"
 line=($(awk '{print $3}' $file_log)) #line è l'array delle operazione totali 

 # somma contenuto array line (op. totali) 
 for i in ${line[@]};do
 	op_tot=$((op_tot+i))
done 


line=($(awk '{print $5}' $file_log)) #line è l'array delle operazione succ.

 # somma contenuto array line (op.succ) 
 for i in ${line[@]};do
 	op_succ=$((op_succ+i))
done

line=($(awk '{print $7}' $file_log)) #line è l'array delle operazione fallite

 # somma contenuto array line (op.fallite) 
 for i in ${line[@]};do
 	op_failed=$((op_failed+i))
done 

n_client=$(awk '{print $1}' $file_log | sort -u | wc -l)
echo "Numero clients totali: $n_client" 
echo "Operazioni totali: $op_tot"
echo "Operazioni effettuate con successo: $op_succ"
echo "Operazioni fallite: $op_failed"

 echo -n "Memoria allocata: "
 du -sh ./data | cut -f 1
