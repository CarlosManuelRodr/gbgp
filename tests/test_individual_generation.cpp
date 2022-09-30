#include "doctest.h"
#include "../util/arithmetic_parser.h"
#include "../include/individual.h"
using namespace std;

//*****************************
//*    Evaluation context     *
//****************************/

class ArithmeticContext : public EvaluationContext
{
public:
    int x{}, y{};

    ArithmeticContext(int px, int py) : x(px), y(py) {}

    int GetIntSemanticValue(int index) { return stoi(SemanticValue(index)); }
    int GetIntResult() { return stoi(result()); }
    void SetIntResult(int r) { result() = to_string(r); }
};

//*****************************
//*     Types declaration     *
//****************************/

enum Terms
{
    Var, Expr, Term, Factor
};

//*****************************
//*    Grammar declaration    *
//****************************/

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "1", "2", "3" });
const NonTerminal exprNonTerm(Expr, "EXPR");
const NonTerminal termNonTerm(Term, "TERM");
const NonTerminal factorNonTerm(Factor, "FACTOR");

// Grammar definition.
const ProductionRule rule1(
        exprNonTerm,
        {
                ProductionElement(exprNonTerm),
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(exprNonTerm),
                SemanticElement("+"),
                SemanticElement(termNonTerm)
        },
        [](EvaluationContext& ctx) {
            auto& arithmeticContext = dynamic_cast<ArithmeticContext&>(ctx);
            int n1 = arithmeticContext.GetIntSemanticValue(0);
            int n2 = arithmeticContext.GetIntSemanticValue(1);
            arithmeticContext.SetIntResult(n1 + n2);
        }
);

const ProductionRule rule2(
        exprNonTerm,
        {
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(termNonTerm)
        },
        [](EvaluationContext& ctx) {
            ctx.TransferSemanticValueToResult();
        }
);

const ProductionRule rule3(
        termNonTerm,
        {
                ProductionElement(termNonTerm),
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(termNonTerm),
                SemanticElement("*"),
                SemanticElement(factorNonTerm)
        },
        [](EvaluationContext& ctx) {
            auto& arithmeticContext = dynamic_cast<ArithmeticContext&>(ctx);
            int n1 = arithmeticContext.GetIntSemanticValue(0);
            int n2 = arithmeticContext.GetIntSemanticValue(1);
            arithmeticContext.SetIntResult(n1 * n2);
        }
);

const ProductionRule rule4(
        termNonTerm,
        {
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(factorNonTerm)
        },
        [](EvaluationContext& ctx) {
            ctx.TransferSemanticValueToResult();
        }
);

const ProductionRule rule5(
        factorNonTerm,
        {
                ProductionElement(exprNonTerm),
        },
        {
                SemanticElement("("),
                SemanticElement(exprNonTerm),
                SemanticElement(")")
        },
        [](EvaluationContext& ctx) {
            ctx.TransferSemanticValueToResult();
        }
);

const ProductionRule rule6(
        factorNonTerm,
        {
                ProductionElement(varTerm),
        },
        {
                SemanticElement(varTerm)
        },
        [](EvaluationContext& ctx) {
            ctx.TransferSemanticValueToResult();
        }
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
