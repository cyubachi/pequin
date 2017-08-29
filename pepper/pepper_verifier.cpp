#include <iostream>
#include <fstream>

#include <gmp.h>

#include <common/utility.h>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/ec_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <libsnark/algebra/fields/fp.hpp>
#include <libsnark/algebra/curves/public_params.hpp>
#include <libsnark/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libsnark/common/profiling.hpp>

#include <common/utility.h>
#include <gen/input_gen.h>

#include "common_defs.h"

#include <sstream>

#include "custom_include/custom_utility.h"
#include "custom_include/logging_utility.h"

#define RESTORE_VERIFY_FILE_NUM (16 + 3)

#ifndef NAME
#error "Must define NAME as name of computation."
#endif

void print_usage(char* argv[]) {
    std::cout << "usage: " << std::endl <<
        "(1) " << argv[0] << " setup <verification key file> <proving key file>" << std::endl <<
        "(2) " << argv[0] << " gen_input <inputs file" << std::endl <<
        "(3) " << argv[0] << " verify <verification key file> <inputs file> <outputs file> <proof file>" << std::endl;
}

void run_setup(int num_constraints, int num_inputs,
               int num_outputs, int num_vars, mpz_t p,
               string vkey_file, string pkey_file) {
    
    std::ifstream Amat("./bin/" + std::string(NAME) + ".qap.matrix_a");
    std::ifstream Bmat("./bin/" + std::string(NAME) + ".qap.matrix_b");
    std::ifstream Cmat("./bin/" + std::string(NAME) + ".qap.matrix_c");

    libsnark::alt_bn128_pp::init_public_params();
    libsnark::r1cs_constraint_system<FieldT> q;

    int Ai, Aj, Bi, Bj, Ci, Cj;
    mpz_t  Acoef, Bcoef, Ccoef;
    mpz_init(Acoef);
    mpz_init(Bcoef);
    mpz_init(Ccoef);

    Amat >> Ai;
    Amat >> Aj;
    Amat >> Acoef;

    if (mpz_sgn(Acoef) == -1)
        mpz_add(Acoef, p, Acoef);
    
    //    std::cout << Ai << " " << Aj << " " << Acoef << std::std::endl;

    Bmat >> Bi;
    Bmat >> Bj;
    Bmat >> Bcoef;
    if (mpz_sgn(Bcoef) == -1)
        mpz_add(Bcoef, p, Bcoef);
    
    Cmat >> Ci;
    Cmat >> Cj;
    Cmat >> Ccoef;

    if (mpz_sgn(Ccoef) == -1)
        mpz_mul_si(Ccoef, Ccoef, -1);

    else if(mpz_sgn(Ccoef) == 1)
        {
            mpz_mul_si(Ccoef, Ccoef, -1);
            mpz_add(Ccoef, p, Ccoef);
        }
    
    int num_intermediate_vars = num_vars;
    int num_inputs_outputs = num_inputs + num_outputs;
    
    q.primary_input_size = num_inputs_outputs;
    q.auxiliary_input_size = num_intermediate_vars;
    
    for (int currentconstraint = 1; currentconstraint <= num_constraints; currentconstraint++)
        {
            libsnark::linear_combination<FieldT> A, B, C;
            
            while(Aj == currentconstraint && Amat)
                {                  
                    if (Ai <= num_intermediate_vars && Ai != 0)
                        Ai += num_inputs_outputs;
                    else if (Ai > num_intermediate_vars)
                        Ai -= num_intermediate_vars;
                    
                    FieldT AcoefT(Acoef);
                    A.add_term(Ai, AcoefT);
                    if(!Amat)
                        break;
                    Amat >> Ai;
                    Amat >> Aj;
                    Amat >> Acoef; 
                    if (mpz_sgn(Acoef) == -1)
                        mpz_add(Acoef, p, Acoef);
                }
            
            while(Bj == currentconstraint && Bmat)
                {
                    if (Bi <= num_intermediate_vars && Bi != 0)
                        Bi += num_inputs_outputs;
                    else if (Bi > num_intermediate_vars)
                        Bi -= num_intermediate_vars;
                    //         std::cout << Bi << " " << Bj << " " << Bcoef << std::std::endl;
                    FieldT BcoefT(Bcoef);
                    B.add_term(Bi, BcoefT);
                    if (!Bmat)
                        break;
                    Bmat >> Bi;
                    Bmat >> Bj;
                    Bmat >> Bcoef;
                    if (mpz_sgn(Bcoef) == -1)
                        mpz_add(Bcoef, p, Bcoef);
                }
            
            while(Cj == currentconstraint && Cmat)
                {
                    if (Ci <= num_intermediate_vars && Ci != 0)
                        Ci += num_inputs_outputs;
                    else if (Ci > num_intermediate_vars)
                    Ci -= num_intermediate_vars;
                    //Libsnark constraints are A*B = C, vs. A*B - C = 0 for Zaatar.
                    //Which is why the C coefficient is negated. 
                    
                    // std::cout << Ci << " " << Cj << " " << Ccoef << std::std::endl;
                    FieldT CcoefT(Ccoef);
                    C.add_term(Ci, CcoefT);
                    if (!Cmat)
                        break;
                    Cmat >> Ci;
                    Cmat >> Cj;
                    Cmat >> Ccoef;
                    if (mpz_sgn(Ccoef) == -1)
                        mpz_mul_si(Ccoef, Ccoef, -1);
                    else if (mpz_sgn(Ccoef) == 1)
                        {
                            mpz_mul_si(Ccoef, Ccoef, -1);
                            mpz_add(Ccoef, p, Ccoef);
                        }
		  
                }
            
            q.add_constraint(libsnark::r1cs_constraint<FieldT>(A, B, C));
            
            //dump_constraint(r1cs_constraint<FieldT>(A, B, C), va, variable_annotations);
        }
    
    Amat.close();
    Bmat.close();
    Cmat.close();

    libsnark::start_profiling();
    libsnark::r1cs_ppzksnark_keypair<libsnark::alt_bn128_pp> keypair = libsnark::r1cs_ppzksnark_generator<libsnark::alt_bn128_pp>(q);
    libsnark::r1cs_ppzksnark_processed_verification_key<libsnark::alt_bn128_pp> pvk = libsnark::r1cs_ppzksnark_verifier_process_vk<libsnark::alt_bn128_pp>(keypair.vk); 


    std::ofstream vkey(vkey_file);
    std::ofstream pkey(pkey_file);

    vkey << pvk;
    pkey << keypair.pk;
    pkey.close(); vkey.close();

}

void verify (string verification_key_fn, string inputs_fn, string outputs_fn,
             string proof_fn, int num_inputs, int num_outputs, mpz_t prime) {

    libsnark::alt_bn128_pp::init_public_params();
   
    libsnark::r1cs_variable_assignment<FieldT> inputvec;
    libsnark::r1cs_ppzksnark_proof<libsnark::alt_bn128_pp> proof;
    
    std::cout << "loading proof from file: " << proof_fn << std::endl;
    std::ifstream proof_file(proof_fn);
    if (!proof_file.good()) {
        std::cerr << "ERROR: " << proof_fn << " not found. " << std::endl;
        exit(1);
    }
    proof_file >> proof; 
    proof_file.close();

    std::cout << "loading inputs from file: " << inputs_fn << std::endl;
    std::ifstream inputs_file(inputs_fn);

    std::cout << "loading outputs from file: " << outputs_fn << std::endl;
    std::ifstream outputs_file(outputs_fn);

    mpq_t tmp; mpq_init(tmp);
    mpz_t tmp_z; mpz_init(tmp_z);
    
    for (int i = 0; i < num_inputs; i++) {
        inputs_file >> tmp;
        convert_to_z(tmp_z, tmp, prime);
        FieldT currentVar(tmp_z);
        inputvec.push_back(currentVar);
    }

    for (int i = 0; i < num_outputs; i++) {
        outputs_file >> tmp;
        convert_to_z(tmp_z, tmp, prime);
        FieldT currentVar(tmp_z);
        inputvec.push_back(currentVar); 
    }

    mpq_clear(tmp); mpz_clear(tmp_z);
    
    inputs_file.close();
    outputs_file.close();

    cout << "loading vk from file: " << verification_key_fn << std::endl;
    std::ifstream vkey(verification_key_fn);
    libsnark::r1cs_ppzksnark_processed_verification_key<libsnark::alt_bn128_pp> pvk;
    vkey >> pvk;
    vkey.close();

    cout << "verifying..." << std::endl;
    libsnark::start_profiling();
    bool result = libsnark::r1cs_ppzksnark_online_verifier_strong_IC<libsnark::alt_bn128_pp>(pvk, inputvec, proof);
   
    if (result) {
        cout << "VERIFICATION SUCCESSFUL" << std::endl;
    }
    else {
        cout << "VERIFICATION FAILED" << std::endl;
    }

}

int main (int argc, char* argv[]) {
    if (argc <= 2) {
        print_usage(argv);
        exit(1);
    }
    
    struct comp_params p = parse_params("./bin/" + string(NAME) + ".params");

    mpz_t prime;
    mpz_init_set_str(prime, "21888242871839275222246405745257275088548364400416034343698204186575808495617", 10);



    if (!strcmp(argv[1], "setup")) {
        if (argc != 4) {
            print_usage(argv);
            exit(1);
        }
        string verification_key_fn = std::string(v_dir) + argv[2];
        string proving_key_fn = std::string(p_dir) + argv[3];
        std::cout << "Creating proving/verification keys, will write to " << verification_key_fn
                  << ", " << proving_key_fn << std::endl;
        run_setup(p.n_constraints, p.n_inputs, p.n_outputs, p.n_vars, prime, verification_key_fn, proving_key_fn);
    }
    else if (!strcmp(argv[1], "gen_input")) {
        if (argc != 3) {
            print_usage(argv);
            exit(1);
        }

        std::string input_filename = std::string(shared_dir) + argv[2];
        std::cout << "Generating inputs, will write to " << input_filename << std::endl;

        mpq_t * input_q;
        alloc_init_vec(&input_q, p.n_inputs);

        gen_input(input_q, p.n_inputs);

        std::ofstream inputs_file(input_filename);
        
        for (int i = 0; i < p.n_inputs; i++) {
            inputs_file << input_q[i] << std::endl;
        }
        inputs_file.close();

        clear_del_vec(input_q, p.n_inputs);
    }
    else if(!strcmp(argv[1], "verify")) {
        if(argc != 6) {
            print_usage(argv);
            exit(1);
        }
        std::string verification_key_fn = std::string(v_dir) + argv[2];
        std::string inputs_fn = std::string(shared_dir) + argv[3];
        std::string outputs_fn = std::string(shared_dir) + argv[4];
        std::string proof_fn = std::string(shared_dir) + argv[5];
        verify(verification_key_fn, inputs_fn, outputs_fn, proof_fn, p.n_inputs, p.n_outputs, prime);
    }
    else if(!strcmp(argv[1], "restore_verify_file") && argc == RESTORE_VERIFY_FILE_NUM) {
        std::string input_fn = std::string(shared_dir) + NAME ".inputs";
        std::string output_fn = std::string(shared_dir) + NAME ".outputs";
        // 鍵の内容を読み込む
        std::string previous_key_fn = std::string(shared_dir) + "/key";
        std::string previous_key = read_file_to_string(previous_key_fn);
        previous_key = trim(previous_key, " \t\v\r\n");


        std::stringstream params;
        std::vector<int> status_v;
        for (int i = 2; i < argc; i++) {
            params << argv[i] << std::endl;
            status_v.push_back(atoi(argv[i]));
        }
        params << previous_key << std::endl;
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
    else {
        print_usage(argv);
        exit(1);
    }
}

