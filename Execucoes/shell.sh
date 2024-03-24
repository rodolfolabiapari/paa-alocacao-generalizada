#!/bin/bash

eval "gcc -Ofast gap.c GAP-Genetic.c -o GAP-Genetic.o"
eval "gcc -Ofast gap.c GAP-GRASP.c   -o GAP-GRASP.o"
eval "gcc -Ofast gap.c GAP-Greedy.c  -o GAP-Greedy.o"
eval "gcc -Ofast gap.c GAP-SA.c      -o GAP-SA.o"

#instancias=( 
#  ../Instancias/gap_a/a05100
#  ../Instancias/gap_a/a10100
#  ../Instancias/gap_a/a20100
#  ../Instancias/gap_a/a05200
#  ../Instancias/gap_a/a10200
#  ../Instancias/gap_a/a20200
#
#  ../Instancias/gap_c/c05100
#  ../Instancias/gap_c/c10100
#  ../Instancias/gap_c/c20100
#  ../Instancias/gap_c/c05200
#  ../Instancias/gap_c/c10200
#  ../Instancias/gap_c/c20200
#
#  ../Instancias/gap_e/e05100
#  ../Instancias/gap_e/e10100
#  ../Instancias/gap_e/e20100
#  ../Instancias/gap_e/e05200
#  ../Instancias/gap_e/e10200
#  ../Instancias/gap_e/e20200
#  )


instancias=(
  ../Instancias/gap_c/c05100
  ../Instancias/gap_c/c10100
  ../Instancias/gap_c/c20100
  ../Instancias/gap_c/c05200
  ../Instancias/gap_c/c10200
  ../Instancias/gap_c/c20200

  ../Instancias/gap_e/e05100
  ../Instancias/gap_e/e10100
  ../Instancias/gap_e/e20100
  ../Instancias/gap_e/e05200
  ../Instancias/gap_e/e10200
  ../Instancias/gap_e/e20200
  )

algoritmos=( GAP-Genetic.o GAP-GRASP.o GAP-Greedy.o GAP-SA.o )
#algoritmos=( GAP-Genetic.o GAP-GRASP.o GAP-SA.o )

configuracoes=( p_ga.txt p_grasp.txt p_greedy.txt p_sa.txt )
#configuracoes=( p_ga.txt p_grasp.txt p_sa.txt )


echo "Quantas iteracoes?"
read quantidade_iteracoes;
echo


for indice_algoritmo in "${!algoritmos[@]}"
do
	echo $indice_algoritmo

  for instancia in "${instancias[@]}"
  do
  	echo $instancia

    for (( i = 0; i < "$quantidade_iteracoes"; i++ )); do
      echo "$i"

      cmd="./${algoritmos[$indice_algoritmo]} ${configuracoes[$indice_algoritmo]} $instancia $i"
      date
      echo $cmd
      $cmd
      echo
      echo "-------------------------------------------------------------"
      echo
    done
    echo

  done
  echo

done


#if ["$i" -ge "$quantidade_iteracoes"]
