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


class TestTreeManipulation(unittest.TestCase):
    def test_tree_node(self):
        term_node1 = TreeNode(varTerm, "b")
        term_node2 = TreeNode(varTerm, "b")
        term_node3 = TreeNode(varTerm)

        self.assertEqual(term_node1, term_node2)
        self.assertNotEqual(term_node1, term_node3)
        self.assertNotEqual(term_node2, term_node3)

        non_term_node1 = TreeNode(factorNonTerm, [TreeNode(varTerm, "b")])
        non_term_node2 = TreeNode(factorNonTerm, [TreeNode(varTerm, "c")])
        non_term_node3 = TreeNode(factorNonTerm)
        non_term_node4 = TreeNode(exprNonTerm)

        self.assertEqual(non_term_node1, non_term_node2)
        self.assertEqual(non_term_node1, non_term_node3)
        self.assertEqual(non_term_node2, non_term_node3)
        self.assertNotEqual(non_term_node1, non_term_node4)

        rule_node = Node(rule2, exprNonTerm)
        node_as_str = rule_node.ToString()
        self.assertEqual("type=NonTerminal, label=EXPR , generatorPR=EXPR -> TERM", node_as_str)


if __name__ == '__main__':
    unittest.main()
