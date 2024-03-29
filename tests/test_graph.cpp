#include "doctest.h"
#include "../include/gbgp.h"
using namespace std;
using namespace gbgp;

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

TEST_CASE("Test Graph conversion")
{
    Grammar grammar({rule1, rule2, rule3, rule4, rule5, rule6});
    SyntaxTree tree;
    grammar.CreateRandomTree(tree, 20);

    Graph graph = tree.ToGraph();
    SyntaxTree reconstruction(graph);

    vector<TreeNode*> treeTraversal = tree.GetPreOrderTreeTraversal();
    vector<TreeNode*> reconstructionTraversal = reconstruction.GetPreOrderTreeTraversal();

    CHECK((treeTraversal.size() == reconstructionTraversal.size()));
    bool areEqual = true;
    for (int i = 0; i < treeTraversal.size(); i++)
    {
        if (*treeTraversal[i] != *reconstructionTraversal[i])
        {
            areEqual = false;
            break;
        }
    }
    CHECK(areEqual);
}

TEST_CASE("Test graph nodes and edges")
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

    Graph graph = tree.ToGraph();

    std::vector<int> nodes = graph.GetNodeIndexes();
    std::vector<std::pair<int, int>> edges = graph.GetEdges();
    std::map<int, std::string> labels = graph.GetLabels();

    for (int i = 0; i < nodes.size(); i++)
        CHECK((nodes[i] == i+1));

    CHECK((edges[4].first == 2));
    CHECK((edges[4].second == 3));

    CHECK((labels[5] == "Times [*]"));
    CHECK((labels[6] == "FACTOR"));
}