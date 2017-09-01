#!/bin/bash -x

cd `dirname $0`

if [ -f sync_car_data.lock ]
then
    echo "now processing."
    exit 1
fi

touch sync_car_data.lock

SSH_LOGIN="ec2-user@52.192.183.173"
#SSH_LOGIN="chubachi@172.10.2.150"
KEY_OPTION=" -i ${HOME}/.ssh/obd.pem "
#KEY_OPTION=""

mkdir -p logs/car_data/

scp ${KEY_OPTION} -r ${SSH_LOGIN}:~/pequin/pepper/logs/car_data/* logs/car_data/
ssh ${KEY_OPTION} ${SSH_LOGIN} "rm -f ~/pequin/pepper/logs/car_data/*/append_car_data.log"

./restore_input_and_output_all.sh

scp ${KEY_OPTION} -r logs/inputs/* ${SSH_LOGIN}:~/pequin/pepper/logs/inputs
scp ${KEY_OPTION} -r logs/outputs/* ${SSH_LOGIN}:~/pequin/pepper/logs/outputs
rm -rf logs/inputs/* 
rm -rf logs/outputs/* 
rm -rf logs/car_data/* 

#ssh ${KEY_OPTION} ${SSH_LOGIN} "~/pequin/pepper/logs/cat_processed_car_data.sh"

rm -f sync_car_data.lock

