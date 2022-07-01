#include "doctest.h"
#include "../include/cst_tree.h"
using namespace std;

//*****************************
//*    Evaluation context     *
//****************************/

class ArithmeticContext : public EvaluationContext
{
public:
    int x{}, y{};
};

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
const Terminal varTerm(Var, "var", { "x", "y" });
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
            auto* arithmeticContext = dynamic_cast<ArithmeticContext*>(ctx);
            string var = ctx->SemanticValue(0);
            int varValue = var == "x" ? arithmeticContext->x : arithmeticContext->y;
            ctx->result() = to_string(varValue);
        }
);

//*****************************
//*       Test routines       *
//****************************/
TEST_CASE("Test arithmetic evaluation")
{
    // GP Generator grammar
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    ConcreteSyntaxTree cst;

    ArithmeticContext arithmeticContext;
    arithmeticContext.x = 4;
    arithmeticContext.y = 5;

    cst.CreateRandomTree(grammar, 100);
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    bool evaluationState = cst.Evaluate(&arithmeticContext);
    if (evaluationState)
        cout << arithmeticContext.result() << endl;

    CHECK(evaluationState == true);
}