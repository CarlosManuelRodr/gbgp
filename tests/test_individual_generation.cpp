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
const Terminal<TermType, int> varTerm(TermType::Var, "var", { 1, 2, 3 });
const NonTerminal exprNonTerm(NonTermType::Expr, "EXPR");
const NonTerminal termNonTerm(NonTermType::Term, "TERM");
const NonTerminal factorNonTerm(NonTermType::Factor, "FACTOR");

// Grammar definition.
const ProductionRule<TermType, NonTermType, int> rule1(
        exprNonTerm,
        {
                ProductionElement<TermType, NonTermType, int>(exprNonTerm),
                ProductionElement<TermType, NonTermType, int>(termNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType, int>(exprNonTerm),
                SemanticElement<TermType, NonTermType, int>("+"),
                SemanticElement<TermType, NonTermType, int>(termNonTerm)
        },
        [](EvaluationContext<int>* ctx) {
            int n1 = ctx->SemanticValue(0);
            int n2 = ctx->SemanticValue(1);
            ctx->result() = n1 + n2;
        }
);

const ProductionRule<TermType, NonTermType, int> rule2(
        exprNonTerm,
        {
                ProductionElement<TermType, NonTermType, int>(termNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType, int>(termNonTerm)
        },
        [](EvaluationContext<int>* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule<TermType, NonTermType, int> rule3(
        termNonTerm,
        {
                ProductionElement<TermType, NonTermType, int>(termNonTerm),
                ProductionElement<TermType, NonTermType, int>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType, int>(termNonTerm),
                SemanticElement<TermType, NonTermType, int>("*"),
                SemanticElement<TermType, NonTermType, int>(factorNonTerm)
        },
        [](EvaluationContext<int>* ctx) {
            int n1 = ctx->SemanticValue(0);
            int n2 = ctx->SemanticValue(1);
            ctx->result() = n1 * n2;
        }
);

const ProductionRule<TermType, NonTermType, int> rule4(
        termNonTerm,
        {
                ProductionElement<TermType, NonTermType, int>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType, int>(factorNonTerm)
        },
        [](EvaluationContext<int>* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule<TermType, NonTermType, int> rule5(
        factorNonTerm,
        {
                ProductionElement<TermType, NonTermType, int>(exprNonTerm),
        },
        {
                SemanticElement<TermType, NonTermType, int>("("),
                SemanticElement<TermType, NonTermType, int>(exprNonTerm),
                SemanticElement<TermType, NonTermType, int>(")")
        },
        [](EvaluationContext<int>* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule<TermType, NonTermType, int> rule6(
        factorNonTerm,
        {
                ProductionElement<TermType, NonTermType, int>(varTerm),
        },
        {
                SemanticElement<TermType, NonTermType, int>(varTerm)
        },
        [](EvaluationContext<int>* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

//*****************************
//*       Test routines       *
//****************************/
TEST_CASE("Test individual evaluation")
{
    // GP Generator grammar
    Grammar<TermType, NonTermType, int> grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    ConcreteSyntaxTree<TermType, NonTermType, int> cst;
    EvaluationContext<int> evaluationContext;
    cst.CreateRandomTree(grammar, 100);
    cst.PrintTree();
    bool evaluationState = cst.Evaluate(&evaluationContext);
    if (evaluationState)
    {
        string synth = cst.SynthesizeExpression();
        cout << synth << endl;
        cout << evaluationContext.result() << endl;
    }

    CHECK(evaluationState == true);
}

TEST_CASE("Test individual generation")
{
/*    initialize_arithmetic_parser();

    // GP Generator grammar
    Grammar<TermType, NonTermType, int> grammar{rule1, rule2, rule3, rule4, rule5, rule6 };

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
    }*/
}
