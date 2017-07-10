#!/bin/bash

cd `dirname $0`

scp -i ~/.ssh/obd.pem -r ec2-user@52.192.183.173:~/pequin/pepper/bin/arm_pepper_verifier_logging bin/pepper_verifier_logging
scp -i ~/.ssh/obd.pem -r ec2-user@52.192.183.173:~/pequin/pepper/first_key prover_verifier_shared/key
./restore_input_and_output_all.sh

scp -i ~/.ssh/obd.pem  -r logs ec2-user@52.192.183.173:~/pequin/pepper

rm -rf logs/inputs
rm -rf logs/outputs
rm bin/pepper_verifier_logging
