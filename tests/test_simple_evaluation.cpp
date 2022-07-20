#include "doctest.h"
#include "../include/grammar.h"
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
        }
);

const ProductionRule rule2(
        exprNonTerm,
        {
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(termNonTerm)
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
        }
);

const ProductionRule rule4(
        termNonTerm,
        {
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(factorNonTerm)
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
        }
);

const ProductionRule rule6(
        factorNonTerm,
        {
                ProductionElement(varTerm),
        },
        {
                SemanticElement(varTerm)
        }
);

//*****************************
//*       Test routines       *
//****************************/
TEST_CASE("Test simple evaluation")
{
    // GP Generator grammar
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    SyntaxTree cst;

    EvaluationContext context;

    grammar.CreateRandomTree(cst, 100);

    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    bool evaluationState = cst.Evaluate(context);
    if (evaluationState)
        cout << context.result() << endl;

    CHECK(evaluationState == true);
}