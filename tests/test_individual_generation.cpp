#include "doctest.h"
#include "../util/arithmetic_parser.h"
#include "../include/cst_tree.h"
#include "../include/individual.h"
using namespace std;

//*****************************
//*     Types declaration     *
//****************************/

enum class TermType
{
    Var
};

enum class NonTermType
{
    Expr, Term, Factor
};

//*****************************
//*    Grammar declaration    *
//****************************/

// Term/Nonterm declaration.
const Terminal varTerm(TermType::Var, "var", { "1", "2", "3" });
const NonTerminal exprNonTerm(NonTermType::Expr, "EXPR");
const NonTerminal termNonTerm(NonTermType::Term, "TERM");
const NonTerminal factorNonTerm(NonTermType::Factor, "FACTOR");

// Grammar definition.
const ProductionRule<TermType, NonTermType> rule1(
        exprNonTerm,
        {
                ProductionElement<TermType, NonTermType>(exprNonTerm),
                ProductionElement<TermType, NonTermType>(termNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(exprNonTerm),
                SemanticElement<TermType, NonTermType>("+"),
                SemanticElement<TermType, NonTermType>(termNonTerm)
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(1));
            ctx->result() = std::to_string(n1 + n2);
        }
);

const ProductionRule<TermType, NonTermType> rule2(
        exprNonTerm,
        {
                ProductionElement<TermType, NonTermType>(termNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(termNonTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule<TermType, NonTermType> rule3(
        termNonTerm,
        {
                ProductionElement<TermType, NonTermType>(termNonTerm),
                ProductionElement<TermType, NonTermType>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(termNonTerm),
                SemanticElement<TermType, NonTermType>("*"),
                SemanticElement<TermType, NonTermType>(factorNonTerm)
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(1));
            ctx->result() = std::to_string(n1 * n2);
        }
);

const ProductionRule<TermType, NonTermType> rule4(
        termNonTerm,
        {
                ProductionElement<TermType, NonTermType>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(factorNonTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule<TermType, NonTermType> rule5(
        factorNonTerm,
        {
                ProductionElement<TermType, NonTermType>(exprNonTerm),
        },
        {
                SemanticElement<TermType, NonTermType>("("),
                SemanticElement<TermType, NonTermType>(exprNonTerm),
                SemanticElement<TermType, NonTermType>(")")
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule<TermType, NonTermType> rule6(
        factorNonTerm,
        {
                ProductionElement<TermType, NonTermType>(varTerm),
        },
        {
                SemanticElement<TermType, NonTermType>(varTerm)
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
    Grammar<TermType, NonTermType> grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    ConcreteSyntaxTree<TermType, NonTermType> cst;
    EvaluationContext evaluationContext;
    cst.CreateRandomTree(grammar, 100);
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
    Grammar<TermType, NonTermType> grammar{rule1, rule2, rule3, rule4, rule5, rule6 };

    cout << "Testing random Individual generation" << endl;
    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual<TermType, NonTermType>::NewRandomIndividual(grammar);

        string expression = ind->GetExpression();
        string evaluationResult = ind->Evaluate();
        string parserResult = std::to_string(evaluate_arithmetic_expression(expression));
        cout << "Generated expression: " << expression << endl;
        cout << "Evaluation: " << evaluationResult << endl;

        CHECK(evaluationResult == parserResult);
        delete ind;
    }
}
