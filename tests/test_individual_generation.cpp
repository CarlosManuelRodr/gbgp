#include "doctest.h"
#include "../util/arithmetic_parser.h"
#include "../include/individual.h"
using namespace std;

//*****************************
//*     Types declaration     *
//****************************/

enum Terms
{
    Var, Expr, Term, Factor
};

//*****************************
//*    Grammar declaration    *
//****************************/

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "1", "2", "3" });
const NonTerminal exprNonTerm(Expr, "EXPR");
const NonTerminal termNonTerm(Term, "TERM");
const NonTerminal factorNonTerm(Factor, "FACTOR");

// Grammar definition.
const ProductionRule rule1(
        exprNonTerm,
        {
                ProductionElement(exprNonTerm),
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(exprNonTerm),
                SemanticElement("+"),
                SemanticElement(termNonTerm)
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(1));
            ctx->result() = std::to_string(n1 + n2);
        }
);

const ProductionRule rule2(
        exprNonTerm,
        {
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(termNonTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule rule3(
        termNonTerm,
        {
                ProductionElement(termNonTerm),
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(termNonTerm),
                SemanticElement("*"),
                SemanticElement(factorNonTerm)
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(1));
            ctx->result() = std::to_string(n1 * n2);
        }
);

const ProductionRule rule4(
        termNonTerm,
        {
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(factorNonTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule rule5(
        factorNonTerm,
        {
                ProductionElement(exprNonTerm),
        },
        {
                SemanticElement("("),
                SemanticElement(exprNonTerm),
                SemanticElement(")")
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule rule6(
        factorNonTerm,
        {
                ProductionElement(varTerm),
        },
        {
                SemanticElement(varTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

//*****************************
//*       Test routines       *
//****************************/
TEST_CASE("Test individual evaluation")
{
    // GP Generator grammar
    SyntaxTree cst;
    EvaluationContext evaluationContext;
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };

    grammar.CreateRandomTree(cst, 100);
    cst.PrintTree();

    cout << cst.SynthesizeExpression() << endl;

    bool evaluationState = cst.Evaluate(&evaluationContext);
    if (evaluationState)
        cout << evaluationContext.result() << endl;

    CHECK(evaluationState == true);
}

TEST_CASE("Test individual generation")
{
    initialize_arithmetic_parser();

    // GP Generator grammar
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };

    cout << "Testing random Individual generation" << endl;
    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual(grammar);
        ind.CreateRandom();

        string expression = ind.GetExpression();
        string evaluationResult = ind.Evaluate();
        string parserResult = std::to_string(evaluate_arithmetic_expression(expression));
        cout << "Generated expression: " << expression << endl;
        cout << "Evaluation: " << evaluationResult << endl;

        CHECK(evaluationResult == parserResult);
    }
}
