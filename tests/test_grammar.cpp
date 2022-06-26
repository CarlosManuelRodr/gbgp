#include "doctest.h"
#include "../include/cst_tree.h"
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

/****************************
*       Test routines       *
****************************/
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

    CHECK(rule1String == "EXPR -> EXPR Plus TERM");
    CHECK(rule2String == "EXPR -> TERM");
    CHECK(rule3String == "TERM -> TERM Times FACTOR");
    CHECK(rule4String == "TERM -> FACTOR");
    CHECK(rule5String == "FACTOR -> LeftParenthesis EXPR RightParenthesis");
    CHECK(rule6String == "FACTOR -> var");
}