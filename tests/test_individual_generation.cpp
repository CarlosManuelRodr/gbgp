#include "doctest.h"
#include "../util/arithmetic_parser.h"
#include "../include/individual.h"
using namespace std;

//*****************************
//*     Types declaration     *
//****************************/

enum Terms
{
    Var, Plus, Times, LeftParenthesis, RightParenthesis, // Terminals
    Expr, Term, Factor // NonTerminals
};

//*****************************
//*    Grammar declaration    *
//****************************/

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "1", "2", "3" });
const Terminal plusTerm(Plus, "Plus", { "+" });
const Terminal timesTerm(Times, "Times", { "*" });
const Terminal leftParenthesisTerm(LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal rightParenthesisTerm(RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal exprNonTerm(Expr, "EXPR");
const NonTerminal termNonTerm(Term, "TERM");
const NonTerminal factorNonTerm(Factor, "FACTOR");

// Grammar definition.
const ProductionRule rule1(
        exprNonTerm,
        { ProductionElement(exprNonTerm), ProductionElement(plusTerm), ProductionElement(termNonTerm) },
        [](EvaluationContext& ctx) {
            int n1 = stoi(ctx.SemanticValue(0));
            int n2 = stoi(ctx.SemanticValue(2));
            ctx.result() = std::to_string(n1 + n2);
        }
);

const ProductionRule rule2(
        exprNonTerm,
        { ProductionElement(termNonTerm) }
);

const ProductionRule rule3(
        termNonTerm,
        { ProductionElement(termNonTerm), ProductionElement(timesTerm), ProductionElement(factorNonTerm) },
        [](EvaluationContext& ctx) {
            int n1 = stoi(ctx.SemanticValue(0));
            int n2 = stoi(ctx.SemanticValue(2));
            ctx.result() = std::to_string(n1 * n2);
        }
);

const ProductionRule rule4(
        termNonTerm,
        { ProductionElement(factorNonTerm) }
);

const ProductionRule rule5(
        factorNonTerm,
        { ProductionElement(leftParenthesisTerm), ProductionElement(exprNonTerm), ProductionElement(rightParenthesisTerm) },
        1
);

const ProductionRule rule6(
        factorNonTerm,
        { ProductionElement(varTerm) }
);

//*****************************
//*       Test routines       *
//****************************/
TEST_CASE("Test individual evaluation")
{
    // GP Generator grammar
    SyntaxTree cst;
    EvaluationContext evaluationContext;
    Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

    grammar.CreateRandomTree(cst, 100);
    cst.PrintTree();

    cout << cst.SynthesizeExpression() << endl;

    bool evaluationState = cst.Evaluate(evaluationContext);
    if (evaluationState)
        cout << evaluationContext.result() << endl;

    CHECK((evaluationState == true));
}


TEST_CASE("Test individual generation")
{
    initialize_arithmetic_parser();

    // GP Generator grammar
    Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

    cout << "Testing random Individual generation" << endl;
    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual();
        ind.CreateRandom(grammar);

        EvaluationContext context;
        ind.GetTree().Evaluate(context);

        string expression = ind.GetExpression();
        string evaluationResult = context.result();
        string parserResult = std::to_string(evaluate_arithmetic_expression(expression));
        cout << "Generated expression: " << expression << endl;
        cout << "Evaluation: " << evaluationResult << endl;

        CHECK((evaluationResult == parserResult));
    }
}
