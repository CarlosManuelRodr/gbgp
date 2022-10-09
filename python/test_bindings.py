import gbgp
from enum import Enum


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

rule1 = gbgp.ProductionRule(exprNonTerm,
                            [gbgp.ProductionElement(termNonTerm),
                             gbgp.ProductionElement(plusTerm),
                             gbgp.ProductionElement(termNonTerm)])
rule2 = gbgp.ProductionRule(exprNonTerm, [gbgp.ProductionElement(termNonTerm)])
rule3 = gbgp.ProductionRule(termNonTerm,
                            [gbgp.ProductionElement(termNonTerm),
                             gbgp.ProductionElement(timesTerm),
                             gbgp.ProductionElement(factorNonTerm)])
rule4 = gbgp.ProductionRule(termNonTerm, [gbgp.ProductionElement(factorNonTerm)])
rule5 = gbgp.ProductionRule(factorNonTerm,
                            [gbgp.ProductionElement(leftParenthesisTerm),
                             gbgp.ProductionElement(exprNonTerm),
                             gbgp.ProductionElement(rightParenthesisTerm)])
rule6 = gbgp.ProductionRule(factorNonTerm, [gbgp.ProductionElement(varTerm)])

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

