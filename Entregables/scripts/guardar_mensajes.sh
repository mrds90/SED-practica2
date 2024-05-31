#!/bin/bash
# Configuración del broker y topic a escuchar.
# Configuración inicial:
broker="192.168.0.4"
port="1883"
topic="marcos_practica2/measurement"
archivo="out/recibidos.csv"

#--------------------------------------------------

if [ ! -d "out" ]; then
    mkdir out
fi

# Agregar encabezado al archivo CSV si está vacío o no existe
if [ ! -f $archivo ] || [ ! -s $archivo ]; then
    echo "timestamp,value" > $archivo
fi

# Ponemos el cliente de mosquitto a escuchar
mosquitto_sub -t $topic -h $broker -p $port | while read value; do
    ts=$(date "+%s%3N") # Obtenemos la marca de tiempo en milisegundos
    # Guardamos valores en formato CSV:
    echo "$ts,$value" >> $archivo   # guardamos datos en archivo CSV
    echo "$value"                   # mostramos el resultado por consola
done
