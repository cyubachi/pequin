#!/bin/bash

RECORD_NUM=`wc -l logs/car_data.log | cut -f1 -d' '`
#RECORD_NUM=50

for i in `seq 1 $RECORD_NUM`
do
  args=`head -${i} logs/car_data.log  | tail -1 | sed 's/,/ /g'`
  bash restore_input_and_output.sh $args
done
