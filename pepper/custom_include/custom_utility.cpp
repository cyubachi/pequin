/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <time.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <common/utility.h>


#define SCRIPT_HASH "4bf718cefd5c0998eda4900642e23296d790ccaf3d1a7b4d6ee2296b6908e7c5"

void create_input_file(std::string input_file_name, std::vector<int> inputs) {
    std::ofstream input_file(input_file_name);
    
    for (int i = 0; i < inputs.size(); i++) {
        input_file << inputs[i] << std::endl;
    }
    input_file.close();
}

bool file_exists (std::string filename) {
    struct stat st;
    int ret = stat(filename.c_str(), &st);
    if (0 == ret) {
      return true;
    }
    return false;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
    if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

std::string read_file_to_string (std::string filename) {
    std::ifstream file(filename);
    std::stringstream buf;
    std::string line;
    while (getline(file, line)) {
        buf << line << std::endl;
    }
    file.close();
    return buf.str();
}

std::string basename(const std::string& path) {
    return path.substr(path.find_last_of('/') + 1);
}

char* sha256_hash_array(const std::string str, unsigned char* output_hash)
{
    memset(output_hash, 0, 32);
    sha256(str.size(), (unsigned char *)str.c_str(), output_hash);
}

std::string sha256_str(const std::string str)
{
    
    unsigned char hash[32];
    sha256_hash_array(str, hash);
    std::stringstream ss;
    for(int i = 0; i < 32; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}


std::string sha256_file (std::string filename) {
    std::string file_contents = read_file_to_string(filename);
    return sha256_str(file_contents);
}


bool check_parent_process_script () {
    pid_t pid=getpid();
    pid_t ppid=getppid();
    
    char parent_cwd_path[256];
    char parent_cmdline_path[256];
    char parent_working_dir[256];
    std::string full_script_path;
    memset(parent_cwd_path, '\0', sizeof(parent_cwd_path));
    memset(parent_cmdline_path, '\0', sizeof(parent_cmdline_path));
    memset(parent_working_dir, '\0', sizeof(parent_working_dir));
    
    
    sprintf(parent_cwd_path, "/proc/%d/cwd", (int) ppid);
    sprintf(parent_cmdline_path, "/proc/%d/cmdline", (int) ppid);
    
    printf("parent_cwd_path: %s\n", parent_cwd_path);
    printf("parent_cmdline_path: %s\n", parent_cmdline_path);
    
    std::ifstream ifs(parent_cmdline_path);
    std::string cmdline = read_file_to_string (parent_cmdline_path);
    std::vector<std::string> cmd_args = split(cmdline, '\0');
    std::string python_exec = cmd_args[0];
    std::string script_file = cmd_args[1];

    int count = readlink(parent_cwd_path, parent_working_dir, sizeof(parent_working_dir));
    if (count == 0) {
        return false;
    }
    parent_working_dir[count] = '\0';
    full_script_path = std::string(parent_working_dir) + '/' + basename(script_file);
    
    std::cout << "python_exec: " <<  python_exec << std::endl;
    std::cout << "script_file: " <<  script_file << std::endl;
    std::cout << "parent_working_dir: " <<  parent_working_dir << std::endl;
    std::cout << "script_file_sha256_hash:" << sha256_file(full_script_path) << std::endl;

    if (sha256_file(full_script_path) != SCRIPT_HASH ) {
        std::cout << "python script invalid." << std::endl;
        return false;
        
    }
    return true;
}

std::string trim(const std::string& string, const char* trimCharacterList) {
    std::string result;
    std::string::size_type left = string.find_first_not_of(trimCharacterList);
    if (left != std::string::npos)
    {
        std::string::size_type right = string.find_last_not_of(trimCharacterList);
        result = string.substr(left, right - left + 1);
    }
    return result;
}
