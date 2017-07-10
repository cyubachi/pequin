#!/bin/bash

# ログの行をそのまま引数にもらう

proof_file_name=$1 
restore_verify_files_arg=$*


# logging.inputsとlogging.outputsを復元
sh -c "bin/pepper_verifier_logging restore_verify_file $restore_verify_files_arg" 

mkdir -p logs/inputs
mkdir -p logs/outputs

mv prover_verifier_shared/logging.inputs logs/inputs/${proof_file_name}.inputs
mv prover_verifier_shared/logging.outputs logs/outputs/${proof_file_name}.outputs
