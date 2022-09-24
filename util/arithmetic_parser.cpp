#include "arithmetic_parser.h"
#include <peglib.h>
using namespace std;

peg::parser parser(R"(
        # Grammar for Calculator...
        Additive    <- Multitive '+' Additive / Multitive
        Multitive   <- Primary '*' Multitive / Primary
        Primary     <- '(' Additive ')' / Number
        Number      <- < [0-9]+ >
        %whitespace <- [ \t]*
    )");

void initialize_arithmetic_parser()
{
    // Setup actions
    parser["Additive"] = [](const peg::SemanticValues &vs)
    {
        switch (vs.choice())
        {
            case 0: // "Multitive '+' Additive"
                return any_cast<int>(vs[0]) + any_cast<int>(vs[1]);
            default: // "Multitive"
                return any_cast<int>(vs[0]);
        }
    };

    parser["Multitive"] = [](const peg::SemanticValues &vs)
    {
        switch (vs.choice())
        {
            case 0: // "Primary '*' Multitive"
                return any_cast<int>(vs[0]) * any_cast<int>(vs[1]);
            default: // "Primary"
                return any_cast<int>(vs[0]);
        }
    };

    parser["Number"] = [](const peg::SemanticValues &vs)
    {
        return vs.token_to_number<int>();
    };

    // Parse
    parser.enable_packrat_parsing(); // Enable packrat parsing.
}

int evaluate_arithmetic_expression(const string& expression)
{
    int result;
    bool success = parser.parse(expression, result);
    return success ? result : -1;
}