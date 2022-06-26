#include "doctest.h"
#include "../include/cst_tree.h"
#include "../include/individual.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum class TermType
{
    NullTerm, Var, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class NonTermType
{
    NullNonTerm, Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal varTerm(TermType::Var, "var", {"1", "2", "3" });
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
                ProductionElement<TermType, NonTermType>(factorNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(termNonTerm),
                SemanticElement<TermType, NonTermType>("*"),
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
                ProductionElement<TermType, NonTermType>(exprNonTerm),
        },
        {
                SemanticElement<TermType, NonTermType>("("),
                SemanticElement<TermType, NonTermType>(exprNonTerm),
                SemanticElement<TermType, NonTermType>(")")
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
    auto* ind = Individual<TermType, NonTermType>::NewRandomIndividual(grammar);
    delete ind;
}