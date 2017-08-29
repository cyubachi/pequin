#ifndef LOGGING_UTILITY_H
#define LOGGING_UTILITY_H

//#define LOG_FILE_PATH "/home/pi/pequin-pepper/pequin-master/pepper/logs"
#define LOG_FILE_PATH "/pequin/pepper/logs"
#define FULL_LOG_FILE_NAME LOG_FILE_PATH "/car_data.log"
#define PROOF_FILE_PATH LOG_FILE_PATH "/proofs"

// 要らなくなったので消す
//bool move_previous_files(std::string input_fn, std::string output_fn, std::string proof_fn);
void write_current_key_file(std::string key_fn, std::string current_key);
std::string get_current_key(std::vector<int> v, std::string previous_key);
void get_current_key_array(std::vector<int> v, std::string previous_key, unsigned char *hash);
void append_log(std::vector<int> v);

#endif /* LOGGING_UTILITY_H */

