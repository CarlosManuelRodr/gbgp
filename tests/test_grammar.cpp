#include "doctest.h"
#include "../include/grammar.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum Terms
{
    Var, Plus, Times, LeftParenthesis, RightParenthesis, // Terminals
    Expr, Term, Factor // NonTerminals
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "a", "b", "c" });
const Terminal plusTerm(Plus, "Plus", { "+" });
const Terminal timesTerm(Times, "Times", { "*" });
const Terminal leftParenthesisTerm(LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal rightParenthesisTerm(RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal exprNonTerm(Expr, "EXPR");
const NonTerminal termNonTerm(Term, "TERM");
const NonTerminal factorNonTerm(Factor, "FACTOR");

// Grammar definition.
const ProductionRule rule1(
        exprNonTerm,
        { ProductionElement(exprNonTerm), ProductionElement(plusTerm), ProductionElement(termNonTerm) }
);

const ProductionRule rule2(
        exprNonTerm,
        { ProductionElement(termNonTerm) }
);

const ProductionRule rule3(
        termNonTerm,
        { ProductionElement(termNonTerm), ProductionElement(timesTerm), ProductionElement(factorNonTerm) }
);

const ProductionRule rule4(
        termNonTerm,
        { ProductionElement(factorNonTerm) }
);

const ProductionRule rule5(
        factorNonTerm,
        { ProductionElement(leftParenthesisTerm), ProductionElement(exprNonTerm), ProductionElement(rightParenthesisTerm) }
);

const ProductionRule rule6(
        factorNonTerm,
        { ProductionElement(varTerm) }
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

    CHECK((rule1String == "EXPR -> EXPR Plus TERM"));
    CHECK((rule2String == "EXPR -> TERM"));
    CHECK((rule3String == "TERM -> TERM Times FACTOR"));
    CHECK((rule4String == "TERM -> FACTOR"));
    CHECK((rule5String == "FACTOR -> LeftParenthesis EXPR RightParenthesis"));
    CHECK((rule6String == "FACTOR -> var"));
}