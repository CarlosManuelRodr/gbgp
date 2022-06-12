#include <iostream>
#include <vector>
#include "cst_tree.h"
using namespace std;

/****************************
*                           *
*    Grammar declaration    *
*                           *
****************************/

// Types declaration.
enum class ArithmeticTerm
{
    NullTerm [[maybe_unused]], Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class ArithmeticNonTerm
{
    NullNonTerm [[maybe_unused]], Expr, Term, Factor
};

// Term/Nonterm declaration.
const Terminal<ArithmeticTerm> varTerm(ArithmeticTerm::Variable, "var", { "a", "b" });
const Terminal<ArithmeticTerm> plusTerm(ArithmeticTerm::Plus, "Plus", { "+" });
const Terminal<ArithmeticTerm> timesTerm(ArithmeticTerm::Times, "Times", { "*" });
const Terminal<ArithmeticTerm> leftParenthesisTerm(ArithmeticTerm::LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal<ArithmeticTerm> rightParenthesisTerm(ArithmeticTerm::RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal<ArithmeticNonTerm> exprNonTerm(ArithmeticNonTerm::Expr, "EXPR");
const NonTerminal<ArithmeticNonTerm> termNonTerm(ArithmeticNonTerm::Term, "TERM");
const NonTerminal<ArithmeticNonTerm> factorNonTerm(ArithmeticNonTerm::Factor, "FACTOR");

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

Grammar<ArithmeticTerm, ArithmeticNonTerm> grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

/****************************
*                           *
*       Test routines       *
*                           *
****************************/

/// <summary>
/// Construct tree representing the expression "a+a*a".
/// </summary>
/// <returns>The ConcreteSyntaxTree corresponding to this expression.</returns>
ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> ConstructSampleExpr()
{
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> ast(grammar);
    ast.GetRoot()->generatorPR = rule1;
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftExpr = ast.AddNode(ast.GetRoot(), exprNonTerm, rule2);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* middleSum = ast.AddNode(ast.GetRoot(), plusTerm);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightTerm = ast.AddNode(ast.GetRoot(), termNonTerm, rule3);

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftTerm = ast.AddNode(leftExpr, termNonTerm, rule4);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftFactor = ast.AddNode(leftTerm, factorNonTerm, rule6);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftVar = ast.AddNode(leftFactor, varTerm, "a");


    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftTerm = ast.AddNode(rightTerm, termNonTerm, rule4);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightMultiplication = ast.AddNode(rightTerm, timesTerm);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightRightFactor = ast.AddNode(rightTerm, factorNonTerm, rule6);

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftFactor = ast.AddNode(rightLeftTerm, factorNonTerm, rule6);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftVar = ast.AddNode(rightLeftFactor, varTerm, "a");

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightRightVar = ast.AddNode(rightRightFactor, varTerm, "a");

    return ast;
}


int main()
{
    // First tree
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> ast1(grammar);
    ast1.GetRoot()->generatorPR = rule1;
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftExpr1 = ast1.AddNode(ast1.GetRoot(), exprNonTerm, rule2);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* middleSum1 = ast1.AddNode(ast1.GetRoot(), plusTerm);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightTerm1 = ast1.AddNode(ast1.GetRoot(), termNonTerm, rule3);

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftTerm1 = ast1.AddNode(leftExpr1, termNonTerm, rule4);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftFactor1 = ast1.AddNode(leftTerm1, factorNonTerm, rule6);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftVar1 = ast1.AddNode(leftFactor1, varTerm, "a");


    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftTerm1 = ast1.AddNode(rightTerm1, termNonTerm, rule4);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightMultiplication1 = ast1.AddNode(rightTerm1, timesTerm);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightRightFactor1 = ast1.AddNode(rightTerm1, factorNonTerm, rule6);

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftFactor1 = ast1.AddNode(rightLeftTerm1, factorNonTerm, rule6);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftVar1 = ast1.AddNode(rightLeftFactor1, varTerm, "a");

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightRightVar1 = ast1.AddNode(rightRightFactor1, varTerm, "a");

    // Second tree
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> ast2(grammar);
    ast2.GetRoot()->generatorPR = rule1;
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftExpr2 = ast2.AddNode(ast2.GetRoot(), exprNonTerm, rule2);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* middleSum2 = ast2.AddNode(ast2.GetRoot(), plusTerm);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightTerm2 = ast2.AddNode(ast2.GetRoot(), termNonTerm, rule3);

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftTerm2 = ast2.AddNode(leftExpr2, termNonTerm, rule4);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftFactor2 = ast2.AddNode(leftTerm2, factorNonTerm, rule6);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* leftVar2 = ast2.AddNode(leftFactor2, varTerm, "a");


    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftTerm2 = ast2.AddNode(rightTerm2, termNonTerm, rule4);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightMultiplication2 = ast2.AddNode(rightTerm2, timesTerm);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightRightFactor2 = ast2.AddNode(rightTerm2, factorNonTerm, rule6);

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftFactor2 = ast2.AddNode(rightLeftTerm2, factorNonTerm, rule6);
    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightLeftVar2 = ast2.AddNode(rightLeftFactor2, varTerm, "b");

    TreeNode<ArithmeticTerm, ArithmeticNonTerm>* rightRightVar2 = ast2.AddNode(rightRightFactor2, varTerm, "b");


    cout << ast1.SynthesizeExpression() << endl;
    cout << ast2.SynthesizeExpression() << endl;

    cout << endl;

    cout << "Original" << endl;
    ast1.PrintTree();

    cout << "\nWithout subtree" << endl;
    ast1.RemoveSubtree(rightTerm1);
    ast1.PrintTree();

    cout << endl;

    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> subtree = ast2.GetSubtree(rightTerm2);
    subtree.PrintTree();

    cout << endl;

    ast1.InsertSubtree(rightTerm1, subtree.GetRoot());
    ast1.PrintTree();
    cout << ast1.SynthesizeExpression() << endl;
}