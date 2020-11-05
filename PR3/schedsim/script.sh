#!/bin/bash
listaSimuladores=( "RR" "SJF" "FCFS" "PRIO" )
echo 'Indica el numero de CPUs que quieres usar'
read maxCPUs
while [ $maxCPUs -gt 8 ]; do
    echo 'El numero de CPUs no puede ser superior a 8'
    echo 'Indica el numero de CPUs que quieres usar'
    read maxCPUs
done
echo 'Indica el nombre del fichero que quieres ejecutar'
read fichero
while [ ! -f $fichero ]; do
    echo 'El fichero no existe'
    echo 'Indica el nombre del fichero que quieres ejecutar'
    read fichero
done
if [ -d resultados ]; then
rm -r resultados
fi
mkdir resultados
for item in "${listaSimuladores[@]}"; do
    for (( cpus = 1 ; $cpus<=$maxCPUs; cpus++ )) do
        ./schedsim -n "$cpus" -s "$item" -i "$fichero"
        for (( i=0 ; $i<$cpus ; i++ )) do
            mv CPU_$i.log resultados/"$item"-CPU-$i.log
        done
    done
    cd ../gantt-gplot
    for (( j=0 ; $j<$maxCPUs ; j++ )) do
        ./generate_gantt_chart ../schedsim/resultados/"$item"-CPU-"$j".log
    done
    cd ../schedsim
done
