#pragma once
#include <string>
void init_py_commands();

void bind_char(char key, const std::string& command);
void bind_special(int key, const std::string& command);
void process_key(char key);
void process_special(int key);
void run_string(const std::string& command, bool is_silent=false);
