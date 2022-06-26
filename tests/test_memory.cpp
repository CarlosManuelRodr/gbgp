#include "doctest.h"
#include "../include/cst_tree.h"
#include "../include/individual.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum class TermType
{
    NullTerm, Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class NonTermType
{
    NullNonTerm, Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal<TermType> varTerm(TermType::Variable, "var", { "1", "2", "3" });
const Terminal<TermType> plusTerm(TermType::Plus, "Plus", { "+" });
const Terminal<TermType> timesTerm(TermType::Times, "Times", { "*" });
const Terminal<TermType> leftParenthesisTerm(TermType::LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal<TermType> rightParenthesisTerm(TermType::RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal<NonTermType> exprNonTerm(NonTermType::Expr, "EXPR");
const NonTerminal<NonTermType> termNonTerm(NonTermType::Term, "TERM");
const NonTerminal<NonTermType> factorNonTerm(NonTermType::Factor, "FACTOR");

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
TEST_CASE("Test memory allocation/deallocation")
{
    Grammar<TermType, NonTermType> grammar{rule1, rule2, rule3, rule4, rule5, rule6};

    cout << "Testing CST Creation/destruction" << endl;
    auto* newTree = new ConcreteSyntaxTree<TermType, NonTermType>();
    newTree->CreateRandomTree(grammar);
    delete newTree;

    cout << "Testing Individual Creation/destruction" << endl;
    auto* ind = Individual<TermType, NonTermType, int>::NewRandomIndividual(grammar);
    delete ind;
}