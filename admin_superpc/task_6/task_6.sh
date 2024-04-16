#!/bin/bash

if [ $# -ne 1 ]; then
    printf "No file\n"
    printf "Use: ./name.sh file.txt\n"
    exit 1
fi

file=$1

njobs=0
total_time=0
read job prev_id time < $file

printf "%-30s%15s%15s%15s\n" USERNAME SUM_TIME TOTAL_JOBS AVG_RUN
for i in {1..75}; do
   printf "-"
done
printf "\n"

while read job id time; do

    if [ $id == $prev_id ]; then
        njobs=$(( $njobs + 1 ))
        total_time=$(( $total_time + $time ))
    else
        printf "%-30s%15s%15s%15s\n" $prev_id $total_time $njobs $(bc -l <<< "scale=2; $total_time / $njobs")
        njobs=1
        total_time=$time
        prev_id=$id
    fi

done < $file

printf "%-30s%15s%15s%15s\n" $prev_id $total_time $njobs $(bc -l <<< "scale=2; $total_time / $njobs")

#LC_NUMERIC="en_US.UTF-8" printf "%-30s%15s%15s%15.2lf\n" $prev_id $total_time $njobs $(bc -l <<< "$total_time / $njobs")
#$(( $total_time / $njobs ))
