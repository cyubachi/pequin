#!/bin/bash

cd `dirname $0`


SSH_LOGIN="ec2-user@52.192.183.173"
#SSH_LOGIN="chubachi@172.10.2.150"
KEY_OPTION=" -i ${HOME}/.ssh/obd.pem "
#KEY_OPTION=

scp ${KEY_OPTION} -r logs/car_data.log ${SSH_LOGIN}:~/pequin/pepper/logs/tmp_append_car_data.log
if [ $? -ne 0 ]
then
    exit 1
fi
ssh ${KEY_OPTION} ${SSH_LOGIN} "~/pequin/pepper/logs/cat_tmp_append_car_data.sh"
if [ $? -ne 0 ]
then
    exit 1
fi
scp ${KEY_OPTION} -r logs/proofs/* ${SSH_LOGIN}:~/pequin/pepper/logs/proofs
if [ $? -ne 0 ]
then
    exit 1
fi

rm -rf logs/proofs/*
rm -f logs/car_data.log
touch logs/car_data.log
