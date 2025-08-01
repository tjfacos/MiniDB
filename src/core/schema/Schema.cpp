//
// Created by root on 7/27/25.
//

#include "Schema.h"

#include <cstring>
#include <filesystem>
#include <fstream>

#include "common/util/logging.h"
#include "core/util/helpers.h"

Schema::Schema(const std::string &table) : table(table) {

    std::string path = "db/" + table + "/" + table + ".schema";
    std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

    if (!file.is_open()) {
        util::error("Failed to open schema file '" + path + "'");
    }

    // Number of attributes
    uint8_t num_attributes;
    file.read(reinterpret_cast<char *>(&num_attributes), sizeof(uint8_t));

    // Get that many attributes
    attributes.reserve(num_attributes);

    for (uint8_t i = 0; i < num_attributes; i++) {

        uint8_t size_of_name, type, arg, flags;

        // Get Size of Name
        file.read(reinterpret_cast<char *>(&size_of_name), sizeof(uint8_t));

        std::vector<char> buffer(size_of_name);

        // Get Name
        file.read(buffer.data(), size_of_name);

        // Get other fields
        file.read(reinterpret_cast<char *>(&type), sizeof(uint8_t));
        file.read(reinterpret_cast<char *>(&arg), sizeof(uint8_t));
        file.read(reinterpret_cast<char *>(&flags), sizeof(uint8_t));

        // Get Attribute Type from enum
        Type attr_type = typeOfFlag.at(type);

        // Get Attribute Size in Bytes

        uint8_t attr_size;
        switch (attr_type) {
            case INT    : attr_size = TYPE::INT_SIZE  ; break;
            case FLOAT  : attr_size = TYPE::FLOAT_SIZE; break;
            case BOOL   : attr_size = TYPE::BOOL_SIZE ; break;
            default     : attr_size = arg;
        }

        // Get Flags
        bool is_unique      = flags & TYPE::UNIQUE_FLAG;
        bool is_nullable    = flags & TYPE::NULLABLE_FLAG;

        // Construct Attribute
        Attribute attr{
            std::string{buffer.begin(), buffer.end()},
            attr_type,
            attr_size,
            is_unique,
            is_nullable
        };

        attributes.push_back(attr);

    }
}

Schema::~Schema() = default;

std::vector<Schema::Attribute> Schema::getAttributes() {
    return attributes;
}

Schema Schema::createSchema(const std::string &table, std::vector<Attribute> &attributes) {

    std::string path = "db/" + table + "/" + table + ".schema";
    std::filesystem::path table_directory = std::filesystem::path(path).parent_path();

    if (!std::filesystem::exists(table_directory)) {
        std::filesystem::create_directories(table_directory);
    }

    std::ofstream file(path.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

    if (!file.is_open()) {
        util::error("Failed to create schema file '" + path + "'");
    }

    std::vector<uint8_t> contents;
    contents.push_back(attributes.size());

    for (Attribute attr : attributes) {

        contents.push_back(strlen(attr.name.c_str()));
        contents.insert(contents.end(), attr.name.begin(), attr.name.end());
        contents.push_back(flagOfType.at(attr.type));

        if (attr.type == Type::STRING || attr.type == Type::BINARY) {
            contents.push_back(attr.size);
        } else {
            contents.push_back(0x00);
        }

        uint8_t attr_flags = 0;
        if (attr.is_unique)     attr_flags |= TYPE::UNIQUE_FLAG;
        if (attr.is_nullable)   attr_flags |= TYPE::NULLABLE_FLAG;
        contents.push_back(attr_flags);

    }

    file.write(reinterpret_cast<char *>(contents.data()), contents.size());

    file.close();

    return Schema{table};
}
