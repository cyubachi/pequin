#!/bin/bash

# ログの行をそのまま引数にもらう

car_id=$1 
proof_file_name=$2 
restore_verify_files_arg=$*


# logging.inputsとlogging.outputsを復元
sh -c "bin/pepper_verifier_logging restore_verify_file $restore_verify_files_arg" 

mkdir -p logs/inputs/${car_id}
mkdir -p logs/outputs/${car_id}

mv prover_verifier_shared/logging.inputs logs/inputs/${car_id}/${proof_file_name}.inputs
mv prover_verifier_shared/logging.outputs logs/outputs/${car_id}/${proof_file_name}.outputs
