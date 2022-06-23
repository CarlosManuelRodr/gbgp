#include "doctest.h"
#include "../util/arithmetic_parser.h"
#include "../include/cst_tree.h"
#include "../include/individual.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum class ArithmeticTerm
{
    Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class ArithmeticNonTerm
{
    Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal varTerm(ArithmeticTerm::Variable, "var", { "1", "2", "3" });
const Terminal plusTerm(ArithmeticTerm::Plus, "Plus", { "+" });
const Terminal timesTerm(ArithmeticTerm::Times, "Times", { "*" });
const Terminal leftParenthesisTerm(ArithmeticTerm::LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal rightParenthesisTerm(ArithmeticTerm::RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal exprNonTerm(ArithmeticNonTerm::Expr, "EXPR");
const NonTerminal termNonTerm(ArithmeticNonTerm::Term, "TERM");
const NonTerminal factorNonTerm(ArithmeticNonTerm::Factor, "FACTOR");

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
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(2));
            ctx->result() = std::to_string(n1 + n2);
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule2(
        exprNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
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
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(2));
            ctx->result() = std::to_string(n1 * n2);
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule4(
        termNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
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
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(1);
        }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule6(
        factorNonTerm,
        {
                ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(varTerm),
        },
        {
                SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(varTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

/****************************
*       Test routines       *
****************************/
TEST_CASE("Test individual evaluation")
{
    // GP Generator grammar
    Grammar<ArithmeticTerm, ArithmeticNonTerm> grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> cst(grammar);
    EvaluationContext evaluationContext;
    cst.CreateRandomTree(100);
    cst.PrintTree();
    bool evaluationState = cst.Evaluate(&evaluationContext);
    if (evaluationState)
        cout << evaluationContext.result() << endl;

    CHECK(evaluationState == true);
}

TEST_CASE("Test individual generation")
{
    initialize_arithmetic_parser();

    // GP Generator grammar
    Grammar<ArithmeticTerm, ArithmeticNonTerm> grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

    cout << "Testing random Individual generation" << endl;
    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual<ArithmeticTerm, ArithmeticNonTerm>::NewRandomIndividual(grammar);

        string expression = ind->GetExpression();
        string evaluationResult = ind->Evaluate();
        string parserResult = std::to_string(evaluate_arithmetic_expression(expression));
        cout << "Generated expression: " << expression << endl;
        cout << "Evaluation: " << evaluationResult << endl;

        CHECK(evaluationResult == parserResult);
        delete ind;
    }
}
