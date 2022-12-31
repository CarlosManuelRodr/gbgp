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


varTerm = Terminal(Terms.Var.value, "var", ["x", "y", "1"])
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
    if var == "x":
        var_value = ctx.x
    elif var == "y":
        var_value = ctx.y
    else:
        var_value = 1
    ctx.SetResult(str(var_value))


rule6 = ProductionRule(factorNonTerm, [ProductionElement(varTerm)], semantic_action6)
grammar = Grammar([rule1, rule2, rule3, rule4, rule5, rule6])


def target_function(x: int, y: int) -> int:
    return 1 + 2 * x + y * y * y


def fitness_function(solution: SyntaxTree) -> float:
    diff = []
    for x in range(9):
        for y in range(9):
            ctx = ArithmeticContext(x, y)
            solution.Evaluate(ctx)

            solution_value = int(ctx.GetResult())
            expected_value = target_function(x, y)
            diff.append(abs(solution_value - expected_value))

    error = sum(diff) / len(diff)
    return 1.0 / (1.0 + error)


class TestOptimization(unittest.TestCase):
    def test_population_optimization(self):
        env = Environment(grammar, fitness_function, 200, 100, 5, 5, 0.4)
        last_generation = env.GetPopulation()
        fittest = last_generation.GetFittestByRank(0)

        i = 0
        while i < 50 and fittest.GetFitness() < 1:
            env.Optimize()
            last_generation = env.GetPopulation()
            fittest = last_generation.GetFittestByRank(0)
            print(str(i) + "\t|\t" + str(fittest.GetFitness()) + "\t|\t" + str(fittest.GetExpression()))
            i += 1

        self.assertEqual(fittest.GetFitness(), 1.0)


if __name__ == '__main__':
    unittest.main()
