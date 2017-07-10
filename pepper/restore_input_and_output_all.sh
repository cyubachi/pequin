#!/bin/bash

for i in `seq 1 50`
do
  args=`head -${i} logs/car_data.log  | tail -1 | sed 's/,/ /g'`
  bash restore_input_and_output.sh $args
done
