#!/bin/bash

# ログの行をそのまま引数にもらう

proof_file_name=$1 
restore_verify_files_arg=$*


# logging.inputsとlogging.outputsを復元
#echo "bin/pepper_prover_logging restore_verify_file $restore_verify_files_arg"
sh -c "bin/pepper_prover_logging restore_verify_file $restore_verify_files_arg" 

# logging.proofを復元
cp logs/proofs/${proof_file_name}.proof prover_verifier_shared/logging.proof

# verifierを実行
bin/pepper_verifier_logging verify logging.vkey logging.inputs logging.outputs logging.proof | tail -1 | grep SUCCESS > /dev/null 2>&1

exit $?
