#include "doctest.h"
#include "../include/environment.h"
#include <numeric>
using namespace std;

//*****************************
//*    Evaluation context     *
//****************************/

class ArithmeticContext : public EvaluationContext
{
public:
    int x{}, y{};

    int GetResult()
    {
        return stoi(result());
    }
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
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(1));
            ctx->result() = std::to_string(n1 + n2);
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
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
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
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(1));
            ctx->result() = std::to_string(n1 * n2);
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
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
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
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
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
        [](EvaluationContext* ctx) {
            auto* arithmeticContext = dynamic_cast<ArithmeticContext*>(ctx);
            string var = ctx->SemanticValue(0);
            int varValue = var == "x" ? arithmeticContext->x : arithmeticContext->y;
            ctx->result() = to_string(varValue);
        }
);

//*****************************
//*       Test routines       *
//****************************/
int target_func(int x, int y)
{
    return 1 + 2*x + y*y*y;
}

double fitness_function(SyntaxTree& solution)
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

            int solutionValue = arithmeticContext.GetResult();
            int expectedValue = target_func(x, y);

            diff.push_back(abs(solutionValue - expectedValue));
        }
    }

    int sum = std::accumulate(diff.begin(), diff.end(), 0);
    double error = (double) sum / (double) diff.size();

    return 1.0 / (1.0 + error);
}

TEST_CASE("Test population optimization")
{
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    Environment env(grammar, fitness_function, 200, 100, 5, 0.4);

    cout << "Generation\t|\tScore\t|\tExpression" << endl;
    Population& lastGeneration = env.GetPopulation();
    for (int i = 0; i < 30; i++)
    {
        env.Optimize();
        Individual ind = lastGeneration.GetFittestByRank(0);
        cout << i << "\t|\t" << ind.GetFitness() << "\t|\t" << ind.GetExpression() << endl;
    }
}