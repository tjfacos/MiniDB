//
// Created by root on 7/27/25.
//

#ifndef SCHEMA_H
#define SCHEMA_H

#include <string>
#include <unordered_map>
#include <vector>

#include "core/util/constants.h"
#include "core/util/helpers.h"

class Schema {

public:

    enum Type {
        INT,
        FLOAT,
        STRING,
        BINARY,
        BOOL
    };

    struct Attribute {

        std::string name;
        Type        type;
        uint8_t     size;

        bool    is_unique   ;
        bool    is_nullable ;

    };

private:

    std::string table;
    std::vector<Attribute> attributes;

    inline static std::unordered_map<uint8_t, Schema::Type> typeOfFlag{
        {TYPE::INT_FLAG    , INT    },
        {TYPE::FLOAT_FLAG  , FLOAT  },
        {TYPE::STRING_FLAG , STRING },
        {TYPE::BINARY_FLAG , BINARY },
        {TYPE::BOOL_FLAG   , BOOL   }
    };

    inline static std::unordered_map<Type, uint8_t> flagOfType = util::invertMap(typeOfFlag);

public:

    Schema(const std::string &table);
    ~Schema();

    std::vector<Attribute> getAttributes();

    static Schema createSchema(const std::string &table, std::vector<Attribute> &attributes);

};

#endif //SCHEMA_H
