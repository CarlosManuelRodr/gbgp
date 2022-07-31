#include "doctest.h"
#include "../include/grammar.h"
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
int s_target_func(int x, int y)
{
    return 1 + 2*x + y*y*y;
}

double s_fitness_function(SyntaxTree& solution)
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
            int expectedValue = s_target_func(x, y);

            diff.push_back(abs(solutionValue - expectedValue));
        }
    }

    int sum = std::accumulate(diff.begin(), diff.end(), 0);
    double error = (double) sum / (double) diff.size();

    return 1.0 / (1.0 + error);
}

TEST_CASE("Test fitness function")
{
    // GP Generator grammar
    Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };
    SyntaxTree tree;
    grammar.CreateRandomTree(tree, 100);
    cout << tree.SynthesizeExpression() << endl;

    double fitness = s_fitness_function(tree);
    cout << "Fitness: " << fitness << endl;
}

TEST_CASE("Test arithmetic evaluation")
{
    // GP Generator grammar
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6 };
    SyntaxTree cst;

    ArithmeticContext arithmeticContext;
    arithmeticContext.x = 4;
    arithmeticContext.y = 5;

    grammar.CreateRandomTree(cst, 100);

    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    bool evaluationState = cst.Evaluate(arithmeticContext);
    if (evaluationState)
        cout << arithmeticContext.result() << endl;

    CHECK((evaluationState == true));
}