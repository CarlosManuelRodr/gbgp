#include "doctest.h"
#include "../include/cst_tree.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum class TermType
{
    Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class NonTermType
{
    Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal varTerm(TermType::Variable, "var", { "1", "2", "3" });
const Terminal plusTerm(TermType::Plus, "Plus", { "+" });
const Terminal timesTerm(TermType::Times, "Times", { "*" });
const Terminal leftParenthesisTerm(TermType::LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal rightParenthesisTerm(TermType::RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal exprNonTerm(NonTermType::Expr, "EXPR");
const NonTerminal termNonTerm(NonTermType::Term, "TERM");
const NonTerminal factorNonTerm(NonTermType::Factor, "FACTOR");

// Grammar definition.
const ProductionRule<TermType, NonTermType> rule1(
        exprNonTerm,
        {
                ProductionElement<TermType, NonTermType>(exprNonTerm),
                ProductionElement<TermType, NonTermType>(plusTerm),
                ProductionElement<TermType, NonTermType>(termNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(exprNonTerm),
                SemanticElement<TermType, NonTermType>(plusTerm),
                SemanticElement<TermType, NonTermType>(termNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule2(
        exprNonTerm,
        {
                ProductionElement<TermType, NonTermType>(termNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(termNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule3(
        termNonTerm,
        {
                ProductionElement<TermType, NonTermType>(termNonTerm),
                ProductionElement<TermType, NonTermType>(timesTerm),
                ProductionElement<TermType, NonTermType>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(termNonTerm),
                SemanticElement<TermType, NonTermType>(timesTerm),
                SemanticElement<TermType, NonTermType>(factorNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule4(
        termNonTerm,
        {
                ProductionElement<TermType, NonTermType>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(factorNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule5(
        factorNonTerm,
        {
                ProductionElement<TermType, NonTermType>(leftParenthesisTerm),
                ProductionElement<TermType, NonTermType>(exprNonTerm),
                ProductionElement<TermType, NonTermType>(rightParenthesisTerm),
        },
        {
                SemanticElement<TermType, NonTermType>(leftParenthesisTerm),
                SemanticElement<TermType, NonTermType>(exprNonTerm),
                SemanticElement<TermType, NonTermType>(rightParenthesisTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule6(
        factorNonTerm,
        {
                ProductionElement<TermType, NonTermType>(varTerm),
        },
        {
                SemanticElement<TermType, NonTermType>(varTerm)
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