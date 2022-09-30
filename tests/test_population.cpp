#include "doctest.h"
#include "../include/genetic_operators.h"
#include <numeric>
using namespace std;

//*****************************
//*    Evaluation context     *
//****************************/

class ArithmeticContext : public EvaluationContext
{
public:
    int x{}, y{};

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
const Terminal varTerm(Var, "var", { "x", "y" });
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
            auto& arithmeticContext = dynamic_cast<ArithmeticContext&>(ctx);
            string var = ctx.SemanticValue(0);
            int varValue = var == "x" ? arithmeticContext.x : arithmeticContext.y;
            ctx.result() = to_string(varValue);
        }
);

//*****************************
//*       Test routines       *
//****************************/
int target_func_pop(int x, int y)
{
    return 1 + 2*x + y*y*y;
}

double fitness_function_pop(SyntaxTree& solution)
{
    vector<int> diff;
    for (int x = 0; x <= 10; x++)
    {
        for (int y = 0; y <= 10; y++)
        {
            ArithmeticContext arithmeticContext;
            arithmeticContext.x = x;
            arithmeticContext.y = y;

            solution.Evaluate(arithmeticContext);

            int solutionValue = arithmeticContext.GetIntResult();
            int expectedValue = target_func_pop(x, y);

            diff.push_back(abs(solutionValue - expectedValue));
        }
    }

    int sum = std::accumulate(diff.begin(), diff.end(), 0);
    double error = (double) sum / (double) diff.size();

    return 1.0 / (1.0 + error);
}

TEST_CASE("Test population initialization")
{
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    Population population(grammar, fitness_function_pop);
    population.Initialize(100);

    cout << "Initial population scores" << endl;
    for (auto fitness : population.GetFitness())
        cout << fitness << endl;

    cout << "Population scores after selection" << endl;
    GeneticOperators::Selection(population, 10);
    for (auto fitness : population.GetFitness())
        cout << fitness << endl;
}