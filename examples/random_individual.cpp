#include <iostream>
#include <vector>
#include "cst_tree.h"
#include "individual.h"
using namespace std;

/****************************
*                           *
*    Grammar declaration    *
*                           *
****************************/

// Types declaration.
enum class ArithmeticTerm
{
    NullTerm [[maybe_unused]], Variable, Plus, Times, LeftParenthesis, RightParenthesis
};

enum class ArithmeticNonTerm
{
    NullNonTerm [[maybe_unused]], Expr, Term, Factor
};

// Term/Nonterm declaration.
const Terminal<ArithmeticTerm> varTerm(ArithmeticTerm::Variable, "Var", { "a", "b" });
const Terminal<ArithmeticTerm> plusTerm(ArithmeticTerm::Plus, "Plus", { "+" });
const Terminal<ArithmeticTerm> timesTerm(ArithmeticTerm::Times, "Times", { "*" });
const Terminal<ArithmeticTerm> leftParenthesisTerm(ArithmeticTerm::LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal<ArithmeticTerm> rightParenthesisTerm(ArithmeticTerm::RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal<ArithmeticNonTerm> exprNonTerm(ArithmeticNonTerm::Expr, "EXPR");
const NonTerminal<ArithmeticNonTerm> termNonTerm(ArithmeticNonTerm::Term, "TERM");
const NonTerminal<ArithmeticNonTerm> factorNonTerm(ArithmeticNonTerm::Factor, "FACTOR");

// Grammar definition.
const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule1(
    exprNonTerm,
    {
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(plusTerm),
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
    },
    {
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(plusTerm),
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm)
    }
    );

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule2(
    exprNonTerm,
    { ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm) },
    { SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm) }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule3(
    termNonTerm,
    {
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm),
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(timesTerm),
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
    },
    {
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(termNonTerm),
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(timesTerm),
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm)
    }
    );

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule4(
    termNonTerm,
    { ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm) },
    { SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(factorNonTerm) }
);

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule5(
    factorNonTerm,
    {
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(leftParenthesisTerm),
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
        ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(rightParenthesisTerm),
    },
    {
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(leftParenthesisTerm),
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(exprNonTerm),
        SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(rightParenthesisTerm)
    }
    );

const ProductionRule<ArithmeticTerm, ArithmeticNonTerm> rule6(
    factorNonTerm,
    { ProductionElement<ArithmeticTerm, ArithmeticNonTerm>(varTerm) },
    { SemanticElement<ArithmeticTerm, ArithmeticNonTerm>(varTerm) }
);

Grammar<ArithmeticTerm, ArithmeticNonTerm> grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };


int main()
{
    ConcreteSyntaxTree<ArithmeticTerm, ArithmeticNonTerm> cst(grammar);
    cst.CreateRandomTreeSafe();
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
}