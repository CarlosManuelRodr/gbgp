import pickle
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
                           ProductionElement(termNonTerm),
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


class TestSerialization(unittest.TestCase):
    def test_term_pickling(self):
        pickled_object = pickle.dumps(varTerm)
        unpickled_object = pickle.loads(pickled_object)

        self.assertEqual(varTerm, unpickled_object)

    def test_nonterm_pickling(self):
        pickled_object = pickle.dumps(exprNonTerm)
        unpickled_object = pickle.loads(pickled_object)

        self.assertEqual(exprNonTerm, unpickled_object)

    def test_production_element_pickling(self):
        pe = ProductionElement(exprNonTerm)
        pickled_object = pickle.dumps(pe)
        unpickled_object = pickle.loads(pickled_object)

        self.assertEqual(pe, unpickled_object)

    def test_production_rule_pickling(self):
        pickled_object = pickle.dumps(rule1)
        unpickled_object = pickle.loads(pickled_object)

        self.assertEqual(rule1, unpickled_object)

    def test_empty_node_pickling(self):
        node = Node()
        pickled_object = pickle.dumps(node)
        unpickled_object = pickle.loads(pickled_object)

        self.assertEqual(node, unpickled_object)

    def test_node_pickling(self):
        node = Node(rule1, factorNonTerm)
        pickled_object = pickle.dumps(node)
        unpickled_object = pickle.loads(pickled_object)

        self.assertEqual(node, unpickled_object)


if __name__ == '__main__':
    unittest.main()
