#include "doctest.h"
#include "../include/syntax_tree.h"
#include "../include/prune_rule.h"
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
        {
                ProductionElement(exprNonTerm),
                ProductionElement(plusTerm),
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(exprNonTerm),
                SemanticElement(plusTerm),
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
                ProductionElement(timesTerm),
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(termNonTerm),
                SemanticElement(timesTerm),
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
                ProductionElement(leftParenthesisTerm),
                ProductionElement(exprNonTerm),
                ProductionElement(rightParenthesisTerm),
        },
        {
                SemanticElement(leftParenthesisTerm),
                SemanticElement(exprNonTerm),
                SemanticElement(rightParenthesisTerm)
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
TEST_CASE("Test tree node")
{
    auto termNode1 = TreeNode(varTerm, "b");
    auto termNode2 = TreeNode(varTerm, "b");
    auto termNode3 = TreeNode(varTerm);

    CHECK(termNode1 == termNode2);
    CHECK(termNode1 != termNode3);

    auto nonTermNode1 = TreeNode(factorNonTerm,{ TreeNode(varTerm, "b") });
    auto nonTermNode2 = TreeNode(factorNonTerm,{ TreeNode(varTerm, "c") });
    auto nonTermNode3 = TreeNode(factorNonTerm);
    auto nonTermNode4 = TreeNode(exprNonTerm);

    CHECK(nonTermNode1 == nonTermNode2);
    CHECK(nonTermNode1 == nonTermNode3);
    CHECK(nonTermNode2 == nonTermNode3);
    CHECK(nonTermNode4 != nonTermNode1);
}

TEST_CASE("Testing subtree insertion")
{
    // First tree. Procedural construction.
    SyntaxTree ast1;
    ast1.SetRootRule(rule1);
    auto leftExpr1 = SyntaxTree::AddNode(ast1.Root(), exprNonTerm, rule2);
    auto middleSum1 = SyntaxTree::AddNode(ast1.Root(), plusTerm);
    auto rightTerm1 = SyntaxTree::AddNode(ast1.Root(), termNonTerm, rule3);

    auto leftTerm1 = SyntaxTree::AddNode(leftExpr1, termNonTerm, rule4);
    auto leftFactor1 = SyntaxTree::AddNode(leftTerm1, factorNonTerm, rule6);
    auto leftVar1 = SyntaxTree::AddNode(leftFactor1, varTerm, "a");

    auto rightLeftTerm1 = SyntaxTree::AddNode(rightTerm1, termNonTerm, rule4);
    auto rightMultiplication1 = SyntaxTree::AddNode(rightTerm1, timesTerm);
    auto rightRightFactor1 = SyntaxTree::AddNode(rightTerm1, factorNonTerm, rule6);

    auto rightLeftFactor1 = SyntaxTree::AddNode(rightLeftTerm1, factorNonTerm, rule6);
    auto rightLeftVar1 = SyntaxTree::AddNode(rightLeftFactor1, varTerm, "a");

    auto rightRightVar1 = SyntaxTree::AddNode(rightRightFactor1, varTerm, "a");

    // Second tree. Procedural construction.
    SyntaxTree ast2;
    ast2.SetRootRule(rule1);
    auto leftExpr2 = SyntaxTree::AddNode(ast2.Root(), exprNonTerm, rule2);
    auto middleSum2 = SyntaxTree::AddNode(ast2.Root(), plusTerm);
    auto rightTerm2 = SyntaxTree::AddNode(ast2.Root(), termNonTerm, rule3);

    auto leftTerm2 = SyntaxTree::AddNode(leftExpr2, termNonTerm, rule4);
    auto leftFactor2 = SyntaxTree::AddNode(leftTerm2, factorNonTerm, rule6);
    auto leftVar2 = SyntaxTree::AddNode(leftFactor2, varTerm, "c");

    auto rightLeftTerm2 = SyntaxTree::AddNode(rightTerm2, termNonTerm, rule4);
    auto rightMultiplication2 = SyntaxTree::AddNode(rightTerm2, timesTerm);
    auto rightRightFactor2 = SyntaxTree::AddNode(rightTerm2, factorNonTerm, rule6);

    auto rightLeftFactor2 = SyntaxTree::AddNode(rightLeftTerm2, factorNonTerm, rule6);
    auto rightLeftVar2 = SyntaxTree::AddNode(rightLeftFactor2, varTerm, "b");

    auto rightRightVar2 = SyntaxTree::AddNode(rightRightFactor2, varTerm, "b");

    // Third tree. Declarative construction.
    SyntaxTree ast3(
        TreeNode(
                rule1,
                exprNonTerm,
                {
                    TreeNode(
                        rule2,
                        exprNonTerm,
                        {
                            TreeNode(
                                rule4,
                                termNonTerm,
                                {
                                    TreeNode(
                                    rule6,
                                    factorNonTerm,
                                    {
                                        TreeNode(varTerm, "c")
                                    })
                                })
                        }),
                    TreeNode(plusTerm, "+"),
                    TreeNode(
                        rule3,
                        termNonTerm,
                        {
                            TreeNode(
                                rule4,
                                termNonTerm,
                                {
                                    TreeNode(
                                        rule6,
                                        factorNonTerm,
                                        {
                                            TreeNode(varTerm, "b")
                                        })
                                }),
                            TreeNode(timesTerm, "*"),
                            TreeNode(
                                rule6,
                                factorNonTerm,
                                {
                                    TreeNode(varTerm, "b")
                                })
                        })
                })
            );

    ast3.PrintTree();
    CHECK(ast1.SynthesizeExpression() == "a+a*a");
    CHECK(ast2.SynthesizeExpression() == "c+b*b");
    CHECK(ast3.SynthesizeExpression() == "c+b*b");

    ast1.RemoveSubtree(rightTerm1);
    SyntaxTree subtree = SyntaxTree::GetSubtree(rightTerm2);
    ast1.InsertSubtree(rightTerm1, subtree.Root());

    CHECK(ast1.SynthesizeExpression() == "a+b*b");
}

TEST_CASE("Test tree traversals")
{
    SyntaxTree ast(
        TreeNode(
            rule1,
            exprNonTerm,
            {
                TreeNode(
                    rule2,
                    exprNonTerm,
                    {
                        TreeNode(
                            rule4,
                            termNonTerm,
                            {
                                TreeNode(
                                    rule6,
                                    factorNonTerm,
                                    {
                                            TreeNode(varTerm, "c")
                                    })
                            })
                    }),
                TreeNode(plusTerm, "+"),
                TreeNode(
                    rule3,
                    termNonTerm,
                    {
                        TreeNode(
                            rule4,
                            termNonTerm,
                            {
                                TreeNode(
                                    rule6,
                                    factorNonTerm,
                                    {
                                        TreeNode(varTerm, "b")
                                    })
                            }),
                        TreeNode(timesTerm, "*"),
                        TreeNode(
                            rule6,
                            factorNonTerm,
                            {
                                TreeNode(varTerm, "b")
                            })
                    })
                })
    );

    std::vector<TreeNode*> treeTraversal = ast.DepthFirstScanPostorder();
    std::vector<TreeNode*> copyNodes;

    for (auto node : treeTraversal) copyNodes.push_back(TreeNode::ShallowCopy(node));

    SyntaxTree reconstruction = SyntaxTree::BuildFromTraversal(copyNodes);

    string originalSynth = ast.SynthesizeExpression();
    string reconstructionSynth = reconstruction.SynthesizeExpression();
    cout << "Original: " << originalSynth << endl;
    cout << "Reconstructed: " << reconstructionSynth << endl;

    CHECK(originalSynth == reconstructionSynth);
}

TEST_CASE("Test tree pruning")
{
    SyntaxTree unprunedTree(
        TreeNode(
            rule2,
            exprNonTerm,
            {
                TreeNode(
                    rule3,
                    termNonTerm,
                    {
                        TreeNode(
                            rule4,
                            termNonTerm,
                            {
                                TreeNode(
                                    rule6,
                                    factorNonTerm,
                                    {
                                        TreeNode(varTerm, "a")
                                    })
                            }),
                        TreeNode(timesTerm, "*"),
                        TreeNode(
                            rule5,
                            factorNonTerm,
                            {
                                TreeNode(leftParenthesisTerm, "("),
                                TreeNode(
                                    rule2,
                                    exprNonTerm,
                                    {
                                        TreeNode(
                                            rule4,
                                            termNonTerm,
                                            {
                                                TreeNode(
                                                    rule6,
                                                    factorNonTerm,
                                                    {
                                                        TreeNode(varTerm, "b")
                                                    })
                                            })
                                    }),
                                TreeNode(rightParenthesisTerm, ")")
                            })
                    })
            })
        );

    SyntaxTree pruneRuleFrom(
        TreeNode(
            rule5,
            factorNonTerm,
            {
                TreeNode(leftParenthesisTerm, "("),
                TreeNode(
                    rule2,
                    exprNonTerm,
                    {
                        TreeNode(
                            rule4,
                            termNonTerm,
                            {
                                TreeNode(
                                    rule6,
                                    factorNonTerm,
                                    {
                                        TreeNode(varTerm)
                                    })
                            })
                    }),
                TreeNode(rightParenthesisTerm, ")")
            })
    );

    SyntaxTree pruneRuleTo(
        TreeNode(
            rule6,
            factorNonTerm,
            {
                TreeNode(varTerm)
            })
    );

    PruneRule pruneRule(pruneRuleFrom, pruneRuleTo);
    SyntaxTree prunedTree = pruneRule.Apply(unprunedTree);

    string unprunedSynth = unprunedTree.SynthesizeExpression();
    string prunedSynth = prunedTree.SynthesizeExpression();
    cout << "Original: " << unprunedSynth << endl;
    cout << "Reconstructed: " << prunedSynth << endl;

    CHECK(unprunedSynth == "a*(b)");
    CHECK(prunedSynth == "a*b");
}