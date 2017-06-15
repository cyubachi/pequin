#ifndef CUSTOM_UTILITY_H
#define CUSTOM_UTILITY_H
void create_input_file(std::string input_file_name, std::vector<int> inputs);
bool file_exists (std::string filename);
std::vector<std::string> split(const std::string &s, char delim);
std::string read_file_to_string (std::string filename);
std::string basename(const std::string& path);
char* sha256_hash_array(const std::string str, unsigned char* output_hash);
std::string sha256_str(const std::string str);
std::string sha256_file (std::string filename);
std::vector<std::string> split(const std::string &s, char delim);
bool check_parent_process_script ();
std::string trim(const std::string& string, const char* trimCharacterList);
#endif /* CUSTOM_UTILITY_HPP */


