#include "doctest.h"
#include "../util/peglib.h"
#include "../include/cst_tree.h"
#include "../include/individual.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum class ArithmeticTerm
{
    NullTerm, Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class ArithmeticNonTerm
{
    NullNonTerm, Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal<ArithmeticTerm> varTerm(ArithmeticTerm::Variable, "var", { "1", "2", "3" });
const Terminal<ArithmeticTerm> plusTerm(ArithmeticTerm::Plus, "Plus", { "+" });
const Terminal<ArithmeticTerm> timesTerm(ArithmeticTerm::Times, "Times", { "*" });
const Terminal<ArithmeticTerm> leftParenthesisTerm(ArithmeticTerm::LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal<ArithmeticTerm> rightParenthesisTerm(ArithmeticTerm::RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal<ArithmeticNonTerm> exprNonTerm(ArithmeticNonTerm::Expr, "EXPR");
const NonTerminal<ArithmeticNonTerm> termNonTerm(ArithmeticNonTerm::Term, "TERM");
const NonTerminal<ArithmeticNonTerm> factorNonTerm(ArithmeticNonTerm::Factor, "FACTOR");

// Grammar definition.
const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule1(
        exprNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(plusTerm),
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(plusTerm),
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule2(
        exprNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule3(
        termNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm),
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(timesTerm),
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm),
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(timesTerm),
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule4(
        termNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule5(
        factorNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(leftParenthesisTerm),
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(rightParenthesisTerm),
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(leftParenthesisTerm),
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(rightParenthesisTerm)
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule6(
        factorNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(varTerm),
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(varTerm)
        }
);

/************************************
*  Make a PEGLIB arithmetic parser  *
************************************/
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

struct EvaluationResult
{
    bool success;
    int value;
};

EvaluationResult evaluate_arithmetic_expression(const string& expression)
{
    EvaluationResult result{};
    result.success = parser.parse(expression, result.value);
    return result;
}

/****************************
*       Test routines       *
****************************/
TEST_CASE("Test individual generation")
{
    initialize_arithmetic_parser();

    // GP Generator grammar
    Grammar<ArithmeticTerm, ArithmeticNonTerm> grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

    cout << "Testing random Individual generation" << endl;
    for (int i = 0; i < 100; i++)
    {
        Individual<ArithmeticTerm, ArithmeticNonTerm, EvaluationResult>* ind =
                Individual<ArithmeticTerm, ArithmeticNonTerm, EvaluationResult>::NewRandomIndividual(grammar, evaluate_arithmetic_expression);

        EvaluationResult evaluationResult = ind->Evaluate();
        cout << "Generated expression: " << ind->GetExpression() << endl;
        cout << "Evaluation: " << evaluationResult.value << endl;

        CHECK(evaluationResult.success);
        delete ind;
    }
}