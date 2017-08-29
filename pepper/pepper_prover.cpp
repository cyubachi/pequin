//prover takes proving key, pws, computation inputs as input.
//generates outputs, proofs.


#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/ec_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

#include <libsnark/algebra/curves/public_params.hpp>
#include <libsnark/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libsnark/common/profiling.hpp>

#include <iostream>
#include <fstream>

#include <gmp.h>
#include "libv/computation_p.h"

#include "common_defs.h"
#include <cstdlib>
#include <string>
#include <sstream>
#include <common/utility.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h>

#include "custom_include/custom_utility.h"
#include "custom_include/logging_utility.h"


#ifndef NAME
#error "Must define NAME as name of computation."
#endif

#define OBD_PARAM_NUM 17
#define SETUP_PARAM_NUM 3
#define CHECK_HASH_PARAM_NUM 3
#define PROVER_PARAM_NUM 6
// bin + OBD_PARAM_NUM + "restore_verify_file" + 1497430094(timestamp) + key
#define RESTORE_VERIFY_FILE_NUM (OBD_PARAM_NUM + 3)
#define TOTAL_PARAM_NUM (PROVER_PARAM_NUM + OBD_PARAM_NUM)

void print_usage(char* argv[]) {
    std::cout << "usage: " << std::endl <<
        "(1)" << argv[0] << " setup" << std::endl <<
        "(2) " << argv[0] << " prove <proving key file> <inputs file> <outputs file> <proof file>" << std::endl;
}

int main (int argc, char* argv[]) {
    
//    if (argc != 2 && argc != 6 && argc != 9) {
    if (argc != RESTORE_VERIFY_FILE_NUM && argc != SETUP_PARAM_NUM && argc != TOTAL_PARAM_NUM) {
        print_usage(argv);
        exit(1);
    }
    
    // 各種ファイル名の定義
    std::string pk_filename;
    std::string input_fn;
    std::string output_fn;
//    std::string proof_fn = std::string(shared_dir) + argv[5];
    std::string proof_fn;

    // 鍵の内容を読み込む
    std::string previous_key_fn = std::string(shared_dir) + "/key";
    std::string previous_key = read_file_to_string(previous_key_fn);
    previous_key = trim(previous_key, " \t\v\r\n");
     
    bool only_setup = false;
    bool check_hash = false;
    if (!strcmp(argv[1], "setup")) {
        only_setup = true;
    }
    
    if (!strcmp(argv[1], "check_hash") && argc == CHECK_HASH_PARAM_NUM) {
        check_hash = true;
    }


    if (!strcmp(argv[1], "restore_verify_file") && argc == RESTORE_VERIFY_FILE_NUM) {
        input_fn = std::string(shared_dir) + NAME ".inputs";
        output_fn = std::string(shared_dir) + NAME ".outputs";

        std::stringstream params;
        std::vector<int> status_v;
        for (int i = 2; i < argc; i++) {
            params << argv[i] << std::endl;
            status_v.push_back(atoi(argv[i]));
        }
        params << previous_key << std::endl;
        // std::cout << params.str() << std::endl;
        unsigned char restore_key_hash[32];
        sha256_hash_array(params.str(), restore_key_hash);
        
        std::ofstream restore_inputs(input_fn);
        std::ofstream restore_outputs(output_fn);
        // pwsの先頭の多項式が空なので0がセットされるのは固定。
        restore_outputs << std::to_string(0) << std::endl;
        for (int i = 0; i < 32; i++) {
            restore_inputs << std::to_string((int)restore_key_hash[i]) << std::endl;
            restore_outputs << std::to_string((int)restore_key_hash[i]) << std::endl;
        }
        restore_inputs.close();
        std::string next_key = get_current_key(status_v, previous_key);
        restore_outputs.close();
        write_current_key_file(previous_key_fn, next_key);
        return 0;
    }

    struct comp_params p = parse_params("./bin/" + std::string(NAME) + ".params");
    std::cout << "NUMBER OF CONSTRAINTS:  " << p.n_constraints << std::endl;

    mpz_t prime;
    mpz_init_set_str(prime, "21888242871839275222246405745257275088548364400416034343698204186575808495617", 10);

    if (only_setup) {
        ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, "default_shared_db", "", only_setup);
        return 0;
    }
    
    // TODO: ここにあるべきではない
    // ファイルのチェック用
    if (check_hash) {
        std::cout << sha256_file(std::string(argv[2])) << std::endl;
        return 0;
    }

    // 親プロセスのスクリプトハッシュのチェック
    //if (!check_parent_process_script()) {
    //    printf("invalid parent process.\n");
    //    exit(1);
    //}        

    // 各種ファイル名の定義
    pk_filename = std::string(p_dir) + argv[2];
    input_fn = std::string(shared_dir) + argv[3];
    output_fn = std::string(shared_dir) + argv[4];
    std::string car_id = argv[6];
    std::string proof_file_path = std::string(getenv("HOME")) + std::string(PROOF_FILE_PATH) + "/" + car_id;
    proof_fn = proof_file_path + "/" + argv[7] + ".proof";

    //std::string proof_fn = string(shared_dir) + argv[5];
    struct stat st;
    int ret = stat(proof_file_path.c_str(), &st);
    if (0 != ret) {
        std::cout << "path:" << proof_file_path << " does not exists." << std::endl;
        char cmd[PATH_MAX];
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", proof_file_path.c_str());
        system(cmd);
    } else {
        std::cout << "path:" << proof_file_path << " does exists." << std::endl;
    }

    // 鍵1
    std::cout << "previous key: " + previous_key << std::endl;

    
    // OBDから渡される値をvectorに入れていく
    std::vector<int> input_v, engine_status_v;
    for (int i = 6; i < argc; i++) {
        engine_status_v.push_back(std::stoi(argv[i]));
        std::cout << argv[i] << std::endl;
    }
    append_log(engine_status_v);

    // 鍵1とエンジン状態1を使って鍵2を作る
    unsigned char current_key_hash[32];
    // 鍵2をunsigned charの配列として取る（それがinputsになる）
    get_current_key_array(engine_status_v, previous_key, current_key_hash);
    for (int i = 0; i < sizeof(current_key_hash); i++) {
        input_v.push_back((int)current_key_hash[i]);
    }

    // 鍵2
    std::string current_key = get_current_key(engine_status_v, previous_key);
    // 鍵1を削除(文字列として鍵2を持ってきて鍵ファイルを上書きする)
    write_current_key_file(previous_key_fn, current_key);

    // verifier実行時にinputファイルが必要なためここで作る
    // （ファイルにするのは必須ではない。必要があればパラメータから作るようにする）
    create_input_file(input_fn, input_v);
    
    std::cout << "initialize prover." << std::endl;
    ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, "default_shared_db", input_fn, only_setup);
//    ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, "default_shared_db", v);
    std::cout << "compute from pws." << std::endl;
    prover.compute_from_pws(("./bin/" + std::string(NAME) + ".pws").c_str());
    
    std::cout << "init public params." << std::endl;
    libsnark::alt_bn128_pp::init_public_params();
    std::ifstream pkey(pk_filename);
    if (!pkey.is_open()) {
        std::cerr << "ERROR: " << pk_filename << " not found. Try running ./verifier_setup <computation> first." << std::endl;
    }

    std::cout << "define keypair." << std::endl;
    libsnark::r1cs_ppzksnark_keypair<libsnark::alt_bn128_pp> keypair;
    std::cout << "reading proving key from file..." << std::endl;
    pkey >> keypair.pk;
    libsnark::r1cs_ppzksnark_primary_input<libsnark::alt_bn128_pp> primary_input;
    libsnark::r1cs_ppzksnark_auxiliary_input<libsnark::alt_bn128_pp> aux_input;
    
    for (int i = 0; i < p.n_inputs; i++) {
        FieldT currentVar(prover.input[i]);
        primary_input.push_back(currentVar);
    }
    for (int i = 0; i < p.n_outputs; i++) {
        FieldT currentVar(prover.output[i]);
        primary_input.push_back(currentVar);
    }
    for (int i = 0; i < p.n_vars; i++) {
        FieldT currentVar(prover.F1[i]);
        aux_input.push_back(currentVar);
    }

    libsnark::start_profiling();
    libsnark::r1cs_ppzksnark_proof<libsnark::alt_bn128_pp> proof = libsnark::r1cs_ppzksnark_prover<libsnark::alt_bn128_pp>(keypair.pk, primary_input, aux_input);

    std::ofstream proof_file(proof_fn);
    proof_file << proof; 
    proof_file.close();
    
    std::stringstream proof_str;
    proof_str << proof;
    
    std::ofstream output_file(output_fn);
    for (int i = 0; i < p.n_outputs; i++) {
        output_file << prover.input_output_q[p.n_inputs + i] << std::endl;
    }
    output_file.close();

}

