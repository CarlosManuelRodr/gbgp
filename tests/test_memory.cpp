#include "doctest.h"
#include "../include/syntax_tree.h"
#include "../include/individual.h"
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
const Terminal varTerm(Var, "var", {"1", "2", "3" });
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

/****************************
*       Test routines       *
****************************/
TEST_CASE("Test memory allocation/deallocation")
{
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6};

    cout << "Testing CST Creation/destruction" << endl;
    auto* newTree = new SyntaxTree();
    grammar.CreateRandomTree(*newTree);
    delete newTree;
}

TEST_CASE("Test traversal copy/delete")
{
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6};

    SyntaxTree tree;
    grammar.CreateRandomTree(tree, 20);

    std::vector<TreeNode*> traversal = tree.GetTreeTraversal();
    std::vector<TreeNode*> copy = SyntaxTree::CopyTreeTraversal(traversal);

    SyntaxTree::DeleteTreeTraversal(copy);
}