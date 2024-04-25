#pragma once
#include <json/json.h>
#include <fstream>

Json::Value add_new_entry(const Json::Value &request);
Json::Value delete_entry(const Json::Value &request);
Json::Value search_entry(const Json::Value &request);
Json::Value get_all_entry();
Json::Value create_status_message(int status, std::string message);
bool is_empty(std::fstream& pFile);
