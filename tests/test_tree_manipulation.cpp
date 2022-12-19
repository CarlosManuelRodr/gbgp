#include "doctest.h"
#include "../include/grammar.h"
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
TEST_CASE("Test tree node")
{
    auto termNode1 = TreeNode(varTerm, "b");
    auto termNode2 = TreeNode(varTerm, "b");
    auto termNode3 = TreeNode(varTerm);

    CHECK((termNode1 == termNode2));
    CHECK((termNode1 != termNode3));

    auto nonTermNode1 = TreeNode(factorNonTerm,{ TreeNode(varTerm, "b") });
    auto nonTermNode2 = TreeNode(factorNonTerm,{ TreeNode(varTerm, "c") });
    auto nonTermNode3 = TreeNode(factorNonTerm);
    auto nonTermNode4 = TreeNode(exprNonTerm);

    CHECK((nonTermNode1 == nonTermNode2));
    CHECK((nonTermNode1 == nonTermNode3));
    CHECK((nonTermNode2 == nonTermNode3));
    CHECK((nonTermNode4 != nonTermNode1));
}

TEST_CASE("Testing subtree insertion")
{
    // First tree. Procedural construction.
    SyntaxTree ast1;
    ast1.SetRootRule(rule1);
    auto leftExpr1 = ast1.Root()->AddChildTerm(exprNonTerm, rule2);
    auto middleSum1 = ast1.Root()->AddChildTerm(plusTerm);
    auto rightTerm1 = ast1.Root()->AddChildTerm(termNonTerm, rule3);

    auto leftTerm1 = leftExpr1->AddChildTerm(termNonTerm, rule4);
    auto leftFactor1 = leftTerm1->AddChildTerm(factorNonTerm, rule6);
    auto leftVar1 = leftFactor1->AddChildTerm(varTerm, "a");

    auto rightLeftTerm1 = rightTerm1->AddChildTerm(termNonTerm, rule4);
    auto rightMultiplication1 = rightTerm1->AddChildTerm(timesTerm);
    auto rightRightFactor1 = rightTerm1->AddChildTerm(factorNonTerm, rule6);

    auto rightLeftFactor1 = rightLeftTerm1->AddChildTerm(factorNonTerm, rule6);
    auto rightLeftVar1 = rightLeftFactor1->AddChildTerm(varTerm, "a");

    auto rightRightVar1 = rightRightFactor1->AddChildTerm(varTerm, "a");

    // Second tree. Procedural construction.
    SyntaxTree ast2;
    ast2.SetRootRule(rule1);
    auto leftExpr2 = ast2.Root()->AddChildTerm(exprNonTerm, rule2);
    auto middleSum2 = ast2.Root()->AddChildTerm(plusTerm);
    auto rightTerm2 = ast2.Root()->AddChildTerm(termNonTerm, rule3);

    auto leftTerm2 = leftExpr2->AddChildTerm(termNonTerm, rule4);
    auto leftFactor2 = leftTerm2->AddChildTerm(factorNonTerm, rule6);
    auto leftVar2 = leftFactor2->AddChildTerm(varTerm, "c");

    auto rightLeftTerm2 = rightTerm2->AddChildTerm(termNonTerm, rule4);
    auto rightMultiplication2 = rightTerm2->AddChildTerm(timesTerm);
    auto rightRightFactor2 = rightTerm2->AddChildTerm(factorNonTerm, rule6);

    auto rightLeftFactor2 = rightLeftTerm2->AddChildTerm(factorNonTerm, rule6);
    auto rightLeftVar2 = rightLeftFactor2->AddChildTerm(varTerm, "b");

    auto rightRightVar2 = rightRightFactor2->AddChildTerm(varTerm, "b");

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

    ast1.DeleteSubtree(rightTerm1);
    SyntaxTree subtree = SyntaxTree::CopySubtree(rightTerm2);
    ast1.InsertSubtree(rightTerm1, subtree);

    CHECK(ast1.SynthesizeExpression() == "a+b*b");
}

TEST_CASE("Test random subtree replacement")
{
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 });

    // Original tree.
    SyntaxTree tree;
    tree.SetRootRule(rule1);
    auto leftExpr = tree.Root()->AddChildTerm(exprNonTerm, rule2);
    auto middleSum = tree.Root()->AddChildTerm(plusTerm);
    auto rightTerm = tree.Root()->AddChildTerm(termNonTerm, rule3);

    auto leftTerm = leftExpr->AddChildTerm(termNonTerm, rule4);
    auto leftFactor = leftTerm->AddChildTerm(factorNonTerm, rule6);
    auto leftVar = leftFactor->AddChildTerm(varTerm, "a");

    auto rightLeftTerm = rightTerm->AddChildTerm(termNonTerm, rule4);
    auto rightMultiplication = rightTerm->AddChildTerm(timesTerm);
    auto rightRightFactor = rightTerm->AddChildTerm(factorNonTerm, rule6);

    auto rightLeftFactor = rightLeftTerm->AddChildTerm(factorNonTerm, rule6);
    auto rightLeftVar = rightLeftFactor->AddChildTerm(varTerm, "a");

    auto rightRightVar = rightRightFactor->AddChildTerm(varTerm, "a");

    string originalSynth = tree.SynthesizeExpression();
    cout << "Original: " << originalSynth << endl;

    // Select random branch.
    std::vector<TreeNode*> nonTerminals = tree.GetTermsOfType(TreeNodeType::NonTerminal);
    TreeNode* randomNonTerm = *random_choice(nonTerminals.begin(), nonTerminals.end());

    // Remove branch and create subtree.
    tree.DeleteSubtree(randomNonTerm);
    SyntaxTree replacement;
    grammar.CreateRandomTree(replacement, 50, randomNonTerm->generatorPR);
    tree.InsertSubtree(randomNonTerm, replacement);

    string replacedSynth = tree.SynthesizeExpression();
    cout << "Replaced: " << replacedSynth << endl;

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

    std::vector<TreeNode*> treeTraversal = ast.GetPostOrderTreeTraversal();
    std::vector<TreeNode*> copyNodes;

    for (auto node : treeTraversal) copyNodes.push_back(TreeNode::ShallowCopy(node));

    SyntaxTree reconstruction;
    SyntaxTree::BuildFromTraversal(reconstruction, copyNodes);

    string originalSynth = ast.SynthesizeExpression();
    string reconstructionSynth = reconstruction.SynthesizeExpression();
    cout << "Original: " << originalSynth << endl;
    cout << "Reconstructed: " << reconstructionSynth << endl;

    CHECK((originalSynth == reconstructionSynth));
}

TEST_CASE("Test single pass pruning")
{
    SyntaxTree tree(
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
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 }, { pruneRule });

    string unprunedSynth = tree.SynthesizeExpression();
    grammar.PruneTree(tree);
    string prunedSynth = tree.SynthesizeExpression();

    cout << "Original: " << unprunedSynth << endl;
    cout << "Reconstructed: " << prunedSynth << endl;

    CHECK((unprunedSynth == "a*(b)"));
    CHECK((prunedSynth == "a*b"));
}

TEST_CASE("Test multiple pass pruning")
{
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
    Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

    for (int rep = 0; rep <= 20; rep++)
    {
        cout << "Pruning test #" << rep << endl;
        cout << "================" << endl;

        SyntaxTree tree;
        grammar.CreateRandomTree(tree, 20);

        string unprunedSynth = tree.SynthesizeExpression();
        cout << "Expression before pruning: " << unprunedSynth << endl;

        int i = 1;
        while (pruneRule.CanBeApplied(tree)) {
            pruneRule.Apply(tree);
            cout << "Pruning iteration " << i << ": " << tree.SynthesizeExpression() << endl;
            i++;
        }

        string prunedSynth = tree.SynthesizeExpression();
        cout << "Expression after pruning: " << prunedSynth << endl;

        CHECK((prunedSynth.size() <= unprunedSynth.size()));
        cout << endl;
    }
}