//
// Created by root on 7/27/25.
//

#include "Schema.h"

#include <fstream>

#include "common/util/logging.h"

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

        char attr_name[size_of_name];

        // Get Name
        file.read(attr_name, size_of_name);

        // Get other fields
        file.read(reinterpret_cast<char *>(&type), sizeof(uint8_t));
        file.read(reinterpret_cast<char *>(&arg), sizeof(uint8_t));
        file.read(reinterpret_cast<char *>(&flags), sizeof(uint8_t));

        // Get Attribute Type from enum
        Type attr_type = typeOfFlag[type];

        // Get Attribute Size in Bytes
        uint8_t attr_size;
        switch (attr_type) {
            case INT    : attr_size = INT_SIZE  ; break;
            case FLOAT  : attr_size = FLOAT_SIZE; break;
            case BOOL   : attr_size = BOOL_SIZE ; break;
            default     : attr_size = arg;
        }

        // Get Flags
        bool is_unique      = flags & UNIQUE_FLAG;
        bool is_nullable    = flags & NULLABLE_FLAG;

        // Construct Attribute
        attributes[i] = Attribute{
            std::string{attr_name},
            attr_type,
            attr_size,
            is_unique,
            is_nullable
        };

    }
}

Schema::~Schema() = default;

std::vector<Schema::Attribute> Schema::getAttributes() {
    return attributes;
}
