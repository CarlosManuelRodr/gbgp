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
    print(ctx.SemanticValue(0))
    print(ctx.SemanticValue(2))
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
    print(ctx.SemanticValue(0))
    print(ctx.SemanticValue(2))
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
                             gbgp.ProductionElement(rightParenthesisTerm)])


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

print(rule1)
print(rule2)
print(rule3)
print(rule4)
print(rule5)
print(rule6)

grammar = gbgp.Grammar([rule1, rule2, rule3, rule4, rule5, rule6])

print(grammar)

tree = gbgp.SyntaxTree()
grammar.CreateRandomTree(tree, 100)

print(tree)
print(tree.SynthesizeExpression())

context = ArithmeticContext(2, 5)
tree.Evaluate(context)

print(context.GetResult())
