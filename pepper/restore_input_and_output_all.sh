#!/bin/bash

for f in `ls -1 ~/pequin/pepper/logs/car_data/*/append_car_data.log`
do
  RECORD_NUM=`wc -l ${f} | cut -f1 -d' '`
  for i in `seq 1 $RECORD_NUM`
  do
    args=`head -${i} ${f} | tail -1 | sed 's/,/ /g'`
    bash restore_input_and_output.sh $args
  done
done
