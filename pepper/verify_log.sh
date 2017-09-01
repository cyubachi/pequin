#!/bin/bash

# ログの行をそのまま引数にもらう

cd `dirname $0`

proof_file_name=$1 
car_id=$2 


# logging.inputs,logging.outputs,logging.proofを置く
cp logs/inputs/${car_id}/${proof_file_name}.inputs prover_verifier_shared/logging.inputs
cp logs/outputs/${car_id}/${proof_file_name}.outputs prover_verifier_shared/logging.outputs
cp logs/proofs/${car_id}/${proof_file_name}.proof prover_verifier_shared/logging.proof

# verifierを実行
# bin/pepper_verifier_logging verify logging.vkey logging.inputs logging.outputs logging.proof 
bin/pepper_verifier_logging verify logging.vkey logging.inputs logging.outputs logging.proof | tail -1 | grep SUCCESS > /dev/null 2>&1

exit $?
