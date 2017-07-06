#!/bin/bash

for i in `seq 2 51`
do
  args=`head -${i} logs/car_data.log  | tail -1 | sed 's/,/ /g'`
  bash verify_log.sh $args
  if [ $? -ne 0 ]
  then
    flg="NG"
  else
    flg="OK"
  fi
  echo $args " ... " $flg 
done
