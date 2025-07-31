#include "core/schema/Schema.h"

int main() {

    std::vector<Schema::Attribute> table_attributes{
        Schema::Attribute{"id", Schema::Type::INT, TYPE::INT_SIZE, true, false},
        Schema::Attribute{"user", Schema::Type::STRING, 30, false, true},
        Schema::Attribute{"adult", Schema::Type::BOOL, TYPE::BOOL_SIZE, false, false},
    };

    Schema sch = Schema::createSchema("test_table", table_attributes);
}
