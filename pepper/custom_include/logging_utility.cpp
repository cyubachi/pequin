#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <sstream>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/stat.h>

#include "custom_utility.h"
#include "logging_utility.h"

bool move_previous_files(std::string input_fn, std::string output_fn, std::string proof_fn) {
    if (file_exists(input_fn) && file_exists(output_fn) && file_exists(proof_fn)) {
        // 全部揃ってた時にだけ移動する
        std::string dest_dir_path = std::string(LOG_FILE_PATH) + std::string("/") + sha256_file(proof_fn);
        std::cout << "dest_dir_path: " << dest_dir_path << std::endl;
        
        const int err = mkdir(dest_dir_path.c_str() , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);        
        if (err == -1) {
            std::cout << "directory create error." << std::endl;
        }
        rename(input_fn.c_str(), (dest_dir_path + "/chubachi.inputs").c_str());
        rename(output_fn.c_str(), (dest_dir_path + "/chubachi.outputs").c_str());
        rename(proof_fn.c_str(), (dest_dir_path + "/chubachi.proof").c_str());
        return true;
    }
    
    // 全部無い場合は初回起動ということでOK
    if (!file_exists(input_fn) && !file_exists(output_fn) && !file_exists(proof_fn)) {
        return true;
    }
    
    // 中途半端にファイルが残っていたらエラー
    std::cout << "input file or output file or proof file is not exists." << std::endl;
    return false;
}

void write_current_key_file(std::string key_fn, std::string current_key) {
    std::ofstream previous_key_file_stream;
    previous_key_file_stream.open(key_fn, std::ios::trunc);
    previous_key_file_stream << current_key << std::endl;
    previous_key_file_stream.close();
}

std::string get_current_key(std::vector<int> v, std::string previous_key) {
    
    std::stringstream ss;
    for(int i = 0; i < v.size(); i++)
    {
        ss << std::to_string(v[i]) << std::endl;
    }
    ss << previous_key << std::endl;
    return sha256_str(ss.str());
}

void get_current_key_array(std::vector<int> v, std::string previous_key, unsigned char *hash) {
    std::stringstream ss;
    for(int i = 0; i < v.size(); i++)
    {
        ss << std::to_string(v[i]) << std::endl;
    }
    ss << previous_key << std::endl;
    sha256_hash_array(ss.str(), hash);
}


void append_log(std::vector<int> v) {
    std::ofstream log_file;
    log_file.open(FULL_LOG_FILE_NAME, std::ios::app);
    for (int i = 0; i < v.size(); i++) {
        if (i > 0) {
            log_file << ",";
        }
        log_file << std::to_string(v[i]).c_str();
    }
    log_file << std::endl;
    log_file.close();
}


