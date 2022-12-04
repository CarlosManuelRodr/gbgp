import unittest
import gbgp
from enum import Enum


class ArithmeticContext(gbgp.EvaluationContext):
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


varTerm = gbgp.Terminal(Terms.Var.value, "var", ["x", "y", "1"])
plusTerm = gbgp.Terminal(Terms.Plus.value, "Plus", ["+"])
timesTerm = gbgp.Terminal(Terms.Times.value, "Times", ["*"])
leftParenthesisTerm = gbgp.Terminal(Terms.LeftParenthesis.value, "LeftParenthesis", ["("])
rightParenthesisTerm = gbgp.Terminal(Terms.RightParenthesis.value, "RightParenthesis", [")"])

exprNonTerm = gbgp.NonTerminal(Terms.Expr.value, "EXPR")
termNonTerm = gbgp.NonTerminal(Terms.Term.value, "TERM")
factorNonTerm = gbgp.NonTerminal(Terms.Factor.value, "FACTOR")


def semantic_action1(ctx: gbgp.EvaluationContext):
    n1 = int(ctx.SemanticValue(0))
    n2 = int(ctx.SemanticValue(2))
    ctx.SetResult(str(n1 + n2))


rule1 = gbgp.ProductionRule(exprNonTerm,
                            [gbgp.ProductionElement(termNonTerm),
                             gbgp.ProductionElement(plusTerm),
                             gbgp.ProductionElement(termNonTerm)],
                            semantic_action1)

rule2 = gbgp.ProductionRule(exprNonTerm, [gbgp.ProductionElement(termNonTerm)])


def semantic_action3(ctx: gbgp.EvaluationContext):
    n1 = int(ctx.SemanticValue(0))
    n2 = int(ctx.SemanticValue(2))
    ctx.SetResult(str(n1 * n2))


rule3 = gbgp.ProductionRule(termNonTerm,
                            [gbgp.ProductionElement(termNonTerm),
                             gbgp.ProductionElement(timesTerm),
                             gbgp.ProductionElement(factorNonTerm)],
                            semantic_action3)
rule4 = gbgp.ProductionRule(termNonTerm, [gbgp.ProductionElement(factorNonTerm)])
rule5 = gbgp.ProductionRule(factorNonTerm,
                            [gbgp.ProductionElement(leftParenthesisTerm),
                             gbgp.ProductionElement(exprNonTerm),
                             gbgp.ProductionElement(rightParenthesisTerm)],
                            1)


def semantic_action6(ctx: gbgp.EvaluationContext):
    var = ctx.SemanticValue(0)
    if var == "x":
        var_value = ctx.x
    elif var == "y":
        var_value = ctx.y
    else:
        var_value = 1
    ctx.SetResult(str(var_value))


rule6 = gbgp.ProductionRule(factorNonTerm, [gbgp.ProductionElement(varTerm)], semantic_action6)
grammar = gbgp.Grammar([rule1, rule2, rule3, rule4, rule5, rule6])


def target_function(x: int, y: int) -> int:
    return 1 + 2 * x + y * y * y


def fitness_function(solution: gbgp.SyntaxTree) -> float:
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
        env = gbgp.Environment(grammar, fitness_function, 200, 100, 5, 5, 0.4)
        last_generation = env.GetPopulation()
        fittest = last_generation.GetFittestByRank(0)

        i = 0
        while i < 30 and fittest.GetFitness() < 1:
            env.Optimize()
            last_generation = env.GetPopulation()
            fittest = last_generation.GetFittestByRank(0)
            print(str(i) + "\t|\t" + str(fittest.GetFitness()) + "\t|\t" + str(fittest.GetExpression()))
            i += 1

        self.assertEqual(fittest.GetFitness(), 1.0)


if __name__ == '__main__':
    unittest.main()
