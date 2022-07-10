#include "doctest.h"
#include "../include/cst_tree.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum Terms
{
    Var, Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

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
TEST_CASE("Test rule representation")
{
    string rule1String = rule1.ToString();
    string rule2String = rule2.ToString();
    string rule3String = rule3.ToString();
    string rule4String = rule4.ToString();
    string rule5String = rule5.ToString();
    string rule6String = rule6.ToString();

    cout << "Rule 1: " << rule1String << endl;
    cout << "Rule 2: " << rule2String << endl;
    cout << "Rule 3: " << rule3String << endl;
    cout << "Rule 4: " << rule4String << endl;
    cout << "Rule 5: " << rule5String << endl;
    cout << "Rule 6: " << rule6String << endl;

    CHECK(rule1String == "EXPR -> EXPR + TERM");
    CHECK(rule2String == "EXPR -> TERM");
    CHECK(rule3String == "TERM -> TERM * FACTOR");
    CHECK(rule4String == "TERM -> FACTOR");
    CHECK(rule5String == "FACTOR -> ( EXPR )");
    CHECK(rule6String == "FACTOR -> var");
}