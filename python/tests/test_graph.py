import unittest
from enum import Enum

from gbgp import *


class Terms(Enum):
    Var = 1
    Plus = 2
    Times = 3
    LeftParenthesis = 4
    RightParenthesis = 5
    Expr = 6
    Term = 7
    Factor = 8


varTerm = Terminal(Terms.Var.value, "var", ["x", "y", "1"])
plusTerm = Terminal(Terms.Plus.value, "Plus", ["+"])
timesTerm = Terminal(Terms.Times.value, "Times", ["*"])
leftParenthesisTerm = Terminal(Terms.LeftParenthesis.value, "LeftParenthesis", ["("])
rightParenthesisTerm = Terminal(Terms.RightParenthesis.value, "RightParenthesis", [")"])

exprNonTerm = NonTerminal(Terms.Expr.value, "EXPR")
termNonTerm = NonTerminal(Terms.Term.value, "TERM")
factorNonTerm = NonTerminal(Terms.Factor.value, "FACTOR")

rule1 = ProductionRule(exprNonTerm,
                       [
                           ProductionElement(exprNonTerm),
                           ProductionElement(plusTerm),
                           ProductionElement(termNonTerm)
                       ])
rule2 = ProductionRule(exprNonTerm, [ProductionElement(termNonTerm)])
rule3 = ProductionRule(termNonTerm,
                       [
                           ProductionElement(termNonTerm),
                           ProductionElement(timesTerm),
                           ProductionElement(factorNonTerm)
                       ])
rule4 = ProductionRule(termNonTerm, [ProductionElement(factorNonTerm)])
rule5 = ProductionRule(factorNonTerm,
                       [
                           ProductionElement(leftParenthesisTerm),
                           ProductionElement(exprNonTerm),
                           ProductionElement(rightParenthesisTerm)
                       ])
rule6 = ProductionRule(factorNonTerm, [ProductionElement(varTerm)])

grammar = Grammar([rule1, rule2, rule3, rule4, rule5, rule6])


class TestGraph(unittest.TestCase):
    def test_graph_properties(self):
        tree = SyntaxTree(
            TreeNode(
                rule2,
                exprNonTerm,
                [
                    TreeNode(
                        rule3,
                        termNonTerm,
                        [
                            TreeNode(
                                rule4,
                                termNonTerm,
                                [
                                    TreeNode(
                                        rule6,
                                        factorNonTerm,
                                        [
                                            TreeNode(varTerm, "a")
                                        ])
                                ]),
                            TreeNode(timesTerm, "*"),
                            TreeNode(
                                rule5,
                                factorNonTerm,
                                [
                                    TreeNode(leftParenthesisTerm, "("),
                                    TreeNode(
                                        rule2,
                                        exprNonTerm,
                                        [
                                            TreeNode(
                                                rule4,
                                                termNonTerm,
                                                [
                                                    TreeNode(
                                                        rule6,
                                                        factorNonTerm,
                                                        [
                                                            TreeNode(varTerm, "b")
                                                        ])
                                                ])
                                        ]),
                                    TreeNode(rightParenthesisTerm, ")")
                                ])
                        ])
                ])
        )

        graph = tree.ToGraph()
        self.assertEqual(True, False)  # add assertion here


if __name__ == '__main__':
    unittest.main()
