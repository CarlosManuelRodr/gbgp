#include "doctest.h"
#include "../include/cst_tree.h"
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
const Terminal<TermType> varTerm(TermType::Var, "var", { "a", "b", "c" });
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
TEST_CASE("Testing subtree insertion")
{
    // First tree
    ConcreteSyntaxTree<TermType, NonTermType> ast1;
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
    ConcreteSyntaxTree<TermType, NonTermType> ast2;
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
    ConcreteSyntaxTree<TermType, NonTermType> subtree = ast2.GetSubtree(rightTerm2);
    ast1.InsertSubtree(rightTerm1, subtree.Root());

    CHECK(ast1.SynthesizeExpression() == "a+b*b");
}