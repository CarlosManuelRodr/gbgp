import unittest
from enum import Enum

from gbgp import *


class ArithmeticContext(EvaluationContext):
    x: int
    y: int

    def __init__(self, x: int, y: int):
        super().__init__()
        self.x = x
        self.y = y


class Terms(Enum):
    Var = 1
    Plus = 2
    Times = 3
    LeftParenthesis = 4
    RightParenthesis = 5
    Expr = 6
    Term = 7
    Factor = 8


varTerm = Terminal(Terms.Var.value, "var", ["1", "2", "3"])
plusTerm = Terminal(Terms.Plus.value, "Plus", ["+"])
timesTerm = Terminal(Terms.Times.value, "Times", ["*"])
leftParenthesisTerm = Terminal(Terms.LeftParenthesis.value, "LeftParenthesis", ["("])
rightParenthesisTerm = Terminal(Terms.RightParenthesis.value, "RightParenthesis", [")"])

exprNonTerm = NonTerminal(Terms.Expr.value, "EXPR")
termNonTerm = NonTerminal(Terms.Term.value, "TERM")
factorNonTerm = NonTerminal(Terms.Factor.value, "FACTOR")


def semantic_action1(ctx: EvaluationContext):
    n1 = int(ctx.SemanticValue(0))
    n2 = int(ctx.SemanticValue(2))
    ctx.SetResult(str(n1 + n2))


rule1 = ProductionRule(exprNonTerm,
                       [ProductionElement(exprNonTerm), ProductionElement(plusTerm), ProductionElement(termNonTerm)],
                       semantic_action1)

rule2 = ProductionRule(exprNonTerm, [ProductionElement(termNonTerm)])


def semantic_action3(ctx: EvaluationContext):
    n1 = int(ctx.SemanticValue(0))
    n2 = int(ctx.SemanticValue(2))
    ctx.SetResult(str(n1 * n2))


rule3 = ProductionRule(termNonTerm,
                       [ProductionElement(termNonTerm), ProductionElement(timesTerm), ProductionElement(factorNonTerm)],
                       semantic_action3)

rule4 = ProductionRule(termNonTerm, [ProductionElement(factorNonTerm)])
rule5 = ProductionRule(factorNonTerm,
                       [
                           ProductionElement(leftParenthesisTerm),
                           ProductionElement(exprNonTerm),
                           ProductionElement(rightParenthesisTerm)
                       ],
                       1)


def semantic_action6(ctx: EvaluationContext):
    var = ctx.SemanticValue(0)
    var_value = eval(var)
    ctx.SetResult(str(var_value))


rule6 = ProductionRule(factorNonTerm, [ProductionElement(varTerm)], semantic_action6)
grammar = Grammar([rule1, rule2, rule3, rule4, rule5, rule6])


class TestIndividual(unittest.TestCase):
    def test_evaluation(self):
        self.assertEqual(True, False)


if __name__ == '__main__':
    unittest.main()
