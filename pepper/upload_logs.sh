#!/bin/bash

cd `dirname $0`


SSH_LOGIN="ec2-user@52.192.183.173"
#SSH_LOGIN="chubachi@172.10.2.150"
KEY_OPTION=" -i ${HOME}/.ssh/obd.pem "
#KEY_OPTION=
ssh ${KEY_OPTION} ${SSH_LOGIN} "mkdir -p ~/pequin/pepper/logs/car_data/"
if [ $? -ne 0 ]
then
    echo "mkdir failed"
    exit 1
fi

scp ${KEY_OPTION} -r logs/car_data/* ${SSH_LOGIN}:~/pequin/pepper/logs/car_data/
if [ $? -ne 0 ]
then
    echo "copy log file failed."
    exit 1
fi
#ssh ${KEY_OPTION} ${SSH_LOGIN} "~/pequin/pepper/logs/cat_tmp_append_car_data.sh"
#if [ $? -ne 0 ]
#then
#    exit 1
#fi
scp ${KEY_OPTION} -r logs/proofs/* ${SSH_LOGIN}:~/pequin/pepper/logs/proofs
if [ $? -ne 0 ]
then
    echo "copy proof file failed."
    exit 1
fi

rm -rf logs/proofs/*
rm -rf logs/car_data/*
