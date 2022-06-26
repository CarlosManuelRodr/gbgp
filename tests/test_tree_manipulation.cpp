#include "doctest.h"
#include "../include/cst_tree.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum class ArithmeticTerm
{
    NullTerm, Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class ArithmeticNonTerm
{
    NullNonTerm, Expr, Term, Factor
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal<ArithmeticTerm> varTerm(ArithmeticTerm::Variable, "var", { "a", "b", "c" });
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

/****************************
*       Test routines       *
****************************/
TEST_CASE("Testing subtree insertion")
{
    // First tree
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> ast1;
    ast1.SetRootRule(rule1);
    auto leftExpr1 = ast1.AddNode(ast1.Root(), exprNonTerm, rule2);
    auto middleSum1 = ast1.AddNode(ast1.Root(), plusTerm);
    auto rightTerm1 = ast1.AddNode(ast1.Root(), termNonTerm, rule3);

    auto leftTerm1 = ast1.AddNode(leftExpr1, termNonTerm, rule4);
    auto leftFactor1 = ast1.AddNode(leftTerm1, factorNonTerm, rule6);
    auto leftVar1 = ast1.AddNode(leftFactor1, varTerm, "a");

    auto rightLeftTerm1 = ast1.AddNode(rightTerm1, termNonTerm, rule4);
    auto rightMultiplication1 = ast1.AddNode(rightTerm1, timesTerm);
    auto rightRightFactor1 = ast1.AddNode(rightTerm1, factorNonTerm, rule6);

    auto rightLeftFactor1 = ast1.AddNode(rightLeftTerm1, factorNonTerm, rule6);
    auto rightLeftVar1 = ast1.AddNode(rightLeftFactor1, varTerm, "a");

    auto rightRightVar1 = ast1.AddNode(rightRightFactor1, varTerm, "a");

    // Second tree
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> ast2;
    ast2.SetRootRule(rule1);
    auto leftExpr2 = ast2.AddNode(ast2.Root(), exprNonTerm, rule2);
    auto middleSum2 = ast2.AddNode(ast2.Root(), plusTerm);
    auto rightTerm2 = ast2.AddNode(ast2.Root(), termNonTerm, rule3);

    auto leftTerm2 = ast2.AddNode(leftExpr2, termNonTerm, rule4);
    auto leftFactor2 = ast2.AddNode(leftTerm2, factorNonTerm, rule6);
    auto leftVar2 = ast2.AddNode(leftFactor2, varTerm, "c");

    auto rightLeftTerm2 = ast2.AddNode(rightTerm2, termNonTerm, rule4);
    auto rightMultiplication2 = ast2.AddNode(rightTerm2, timesTerm);
    auto rightRightFactor2 = ast2.AddNode(rightTerm2, factorNonTerm, rule6);

    auto rightLeftFactor2 = ast2.AddNode(rightLeftTerm2, factorNonTerm, rule6);
    auto rightLeftVar2 = ast2.AddNode(rightLeftFactor2, varTerm, "b");

    auto rightRightVar2 = ast2.AddNode(rightRightFactor2, varTerm, "b");

    CHECK(ast1.SynthesizeExpression() == "a+a*a");
    CHECK(ast2.SynthesizeExpression() == "c+b*b");

    ast1.RemoveSubtree(rightTerm1);
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> subtree = ast2.GetSubtree(rightTerm2);
    ast1.InsertSubtree(rightTerm1, subtree.Root());

    CHECK(ast1.SynthesizeExpression() == "a+b*b");
}