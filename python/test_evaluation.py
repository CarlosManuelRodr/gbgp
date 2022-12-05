import random
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

test_x = 5
test_y = 8


def test_external_evaluator(expr: str) -> str:
    replaced_expr = expr.replace("x", str(test_x)).replace("y", str(test_y))
    return str(eval(replaced_expr))


class TestEvaluation(unittest.TestCase):
    def test_context_evaluation(self):
        tree = gbgp.SyntaxTree()
        grammar.CreateRandomTree(tree, 100)
        synthesis = tree.SynthesizeExpression()
        print(synthesis)

        x = random.randrange(10)
        y = random.randrange(10)

        context = ArithmeticContext(x, y)
        tree.Evaluate(context)
        result = int(context.GetResult())

        replaced_synth = synthesis.replace("x", str(x)).replace("y", str(y))
        expected_result = eval(replaced_synth)

        self.assertEqual(result, expected_result)

    def test_external_evaluation(self):
        tree = gbgp.SyntaxTree()
        grammar.CreateRandomTree(tree, 100)
        synthesis = tree.SynthesizeExpression()
        print(synthesis)

        eval_result = int(tree.ExternalEvaluate(test_external_evaluator))
        print("Evaluation result: ", eval_result)

        replaced_synth = synthesis.replace("x", str(test_x)).replace("y", str(test_y))
        expected_result = eval(replaced_synth)
        print("Expected result: ", expected_result)

        self.assertEqual(eval_result, expected_result)



if __name__ == '__main__':
    unittest.main()
