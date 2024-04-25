#include "include/handler.h"
#include "include/constants.h"
#include <iostream>
#include <json/json.h>
#include <fstream>
#include <string>

Json::Value create_status_message(int status, std::string message) {
    Json::Value response;
    response["message"] = message;
    response["status"] = status;
    return response;
}

bool is_empty(std::fstream& pFile) {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

Json::Value add_new_entry(const Json::Value &request) {
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder builder;
    Json::Value new_entry = request["data"];

    std::fstream file(DATA_FILE, std::fstream::in);

    if (file.good() && !is_empty(file)) {
        if (!reader.parse(file, root, false)) {
            return create_status_message(CODE_SERVER_ERROR, "Failed to parse data file.");
        }
    }

    file.close();

    root.append(new_entry);

    std::ofstream output_file(DATA_FILE, std::ofstream::binary);
    if (!output_file.good()) {
        return create_status_message(CODE_SERVER_ERROR, "Failed to open data file for writing.");
    }

    Json::StreamWriter *writer = builder.newStreamWriter();
    writer->write(root, &output_file);
    delete writer;
    output_file.close();

    return create_status_message(CODE_SERVER_OK, "Entry added successfully.");
}

Json::Value delete_entry(const Json::Value &request) {
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder builder;
    std::fstream file(DATA_FILE, std::fstream::in);

    const std::string phone_number = request["data"]["phone"].asString();
    if (!reader.parse(file, root, false) || !root.isArray()) {
        return create_status_message(CODE_SERVER_ERROR, "Failed to parse data or data is not an array.");
    }
    file.close();

    Json::Value updatedRoot;
    bool found = false;

    for (const auto &entry: root) {
        if (entry["phone"].asString() == phone_number) {
            found = true;
        } else {
            updatedRoot.append(entry);
        }
    }

    if (!found) {
        return create_status_message(CODE_SERVER_ERROR, "No matching entry found.");
    }

    std::fstream output_file(DATA_FILE, std::fstream::out | std::fstream::trunc);
    Json::StreamWriter *writer = builder.newStreamWriter();
    writer->write(updatedRoot, &output_file);
    delete writer;

    return create_status_message(CODE_SERVER_OK, "Entries deleted successfully.");
}

Json::Value search_entry(const  Json::Value &request) {
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder builder;
    std::string field = request["data"]["field"].asString();
    std::string query = request["data"]["query"].asString();
    std::fstream file(DATA_FILE, std::fstream::in);

    if (!reader.parse(file, root, false) || !root.isArray()) {
        return create_status_message(CODE_SERVER_ERROR, "Failed to parse data or data is not an array.");
    }
    file.close();

    Json::Value searchRoot;
    bool found = false;

    for (const auto &entry: root) {
        if (entry[field].asString() == query) {
            searchRoot.append(entry);
            found = true;
        }
    }

    if (!found) {
        return create_status_message(CODE_SERVER_ERROR, "No matching entry found.");
    }

    searchRoot["status"] = CODE_SERVER_OK;
    return searchRoot[0];
}

Json::Value get_all_entry() {
    Json::Value root;
    Json::Reader reader;
    Json::StreamWriterBuilder builder;
    std::fstream file(DATA_FILE, std::fstream::in);

    if (!reader.parse(file, root, false) || !root.isArray()) {
        return create_status_message(CODE_SERVER_ERROR, "Failed to parse data or data is not an array.");
    }
    file.close();

    Json::Value status;
    status["status"] = CODE_SERVER_OK;
    root.append(status);
    return root;
}