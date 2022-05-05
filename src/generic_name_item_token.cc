#include "triglav.h"
using namespace triglav;

const std::string generic_name_item_token::dummy_placeholder_name = "dummy";

std::shared_ptr<type_expression> generic_name_item_token::dummy_type_expression_pointer;

void generic_name_item_token::report(std::ostream & s) const
{
    s << name;
}
