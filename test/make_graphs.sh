#!/bin/sh

self=$(dirname "$(realpath "$0")")

mkdir -p "$2"

current=$(realpath $1/current)
previous=$(realpath $1/previous)
output=$(realpath $2)

cd $current
for csv in *.csv
do
	echo $csv
	gnuplot -e "results='$current/$csv'; prev_results='$previous/$csv'; set output '$output/${csv%.csv}.png'" "$self/res.plot"
done

