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


class TestGrammar(unittest.TestCase):
    def test_rule_representation(self):
        rule1_string = rule1.ToString()
        rule2_string = rule2.ToString()
        rule3_string = rule3.ToString()
        rule4_string = rule4.ToString()
        rule5_string = rule5.ToString()
        rule6_string = rule6.ToString()

        self.assertEqual("EXPR -> EXPR Plus TERM", rule1_string)
        self.assertEqual("EXPR -> TERM", rule2_string)
        self.assertEqual("TERM -> TERM Times FACTOR", rule3_string)
        self.assertEqual("TERM -> FACTOR", rule4_string)
        self.assertEqual("FACTOR -> LeftParenthesis EXPR RightParenthesis", rule5_string)
        self.assertEqual("FACTOR -> var", rule6_string)


if __name__ == '__main__':
    unittest.main()
