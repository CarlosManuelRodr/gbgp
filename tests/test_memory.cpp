#include "doctest.h"
#include "../include/syntax_tree.h"
#include "../include/individual.h"
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