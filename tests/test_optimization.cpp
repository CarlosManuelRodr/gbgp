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

    int GetIntResult() { return stoi(result()); }
};

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
const Terminal varTerm(Var, "var", { "x", "y", "1" });
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
        {
                ProductionElement(exprNonTerm),
                ProductionElement(plusTerm),
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(exprNonTerm),
                SemanticElement(plusTerm),
                SemanticElement(termNonTerm)
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(2));
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
                ProductionElement(timesTerm),
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(termNonTerm),
                SemanticElement(timesTerm),
                SemanticElement(factorNonTerm)
        },
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(2));
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
                ProductionElement(leftParenthesisTerm),
                ProductionElement(exprNonTerm),
                ProductionElement(rightParenthesisTerm)
        },
        {
                SemanticElement(leftParenthesisTerm),
                SemanticElement(exprNonTerm),
                SemanticElement(rightParenthesisTerm)
        },
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(1);
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

            int varValue;
            if (var == "x")
                varValue = arithmeticContext->x;
            else if (var == "y")
                varValue = arithmeticContext->y;
            else
                varValue = 1;

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

            int solutionValue = arithmeticContext.GetIntResult();
            int expectedValue = target_func(x, y);

            diff.push_back(abs(solutionValue - expectedValue));
        }
    }

    int sum = std::accumulate(diff.begin(), diff.end(), 0);
    double error = (double) sum / (double) diff.size();

    return 1.0 / (1.0 + error);
}

TEST_CASE("Test individual evaluation")
{
    SyntaxTree removeParenthesisFrom(
            TreeNode(
                    rule5,
                    factorNonTerm,
                    {
                            TreeNode(leftParenthesisTerm, "("),
                            TreeNode(
                                    rule2,
                                    exprNonTerm,
                                    {
                                            TreeNode(
                                                    rule4,
                                                    termNonTerm,
                                                    {
                                                            TreeNode(
                                                                    rule6,
                                                                    factorNonTerm,
                                                                    {
                                                                            TreeNode(varTerm)
                                                                    })
                                                    })
                                    }),
                            TreeNode(rightParenthesisTerm, ")")
                    })
    );

    SyntaxTree removeParenthesisTo(
            TreeNode(
                    rule6,
                    factorNonTerm,
                    {
                            TreeNode(varTerm)
                    })
    );

    PruneRule removeParenthesis(removeParenthesisFrom, removeParenthesisTo);

    // GP Generator grammar
    SyntaxTree cst;
    EvaluationContext evaluationContext;
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 }, { removeParenthesis });

    grammar.CreateRandomTree(cst, 100);
    cst.PrintTree();

    cout << cst.SynthesizeExpression() << endl;
}

TEST_CASE("Test population optimization")
{
    SyntaxTree removeParenthesisFrom(
            TreeNode(
                    rule5,
                    factorNonTerm,
                    {
                            TreeNode(leftParenthesisTerm, "("),
                            TreeNode(
                                    rule2,
                                    exprNonTerm,
                                    {
                                            TreeNode(
                                                    rule4,
                                                    termNonTerm,
                                                    {
                                                            TreeNode(
                                                                    rule6,
                                                                    factorNonTerm,
                                                                    {
                                                                            TreeNode(varTerm)
                                                                    })
                                                    })
                                    }),
                            TreeNode(rightParenthesisTerm, ")")
                    })
    );

    SyntaxTree removeParenthesisTo(
            TreeNode(
                    rule6,
                    factorNonTerm,
                    {
                            TreeNode(varTerm)
                    })
    );

    PruneRule removeParenthesis(removeParenthesisFrom, removeParenthesisTo);
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 }, { removeParenthesis });
    Environment env(grammar, fitness_function, 200, 100, 5, 5, 0.4, RuntimeMode::MultiThread);

    cout << "Generation\t|\tScore\t|\tExpression" << endl;
    Population& lastGeneration = env.GetPopulation();
    Individual fittest = lastGeneration.GetFittestByRank(0);

    for (int i = 0; i < 30 && fittest.GetFitness() < 1; i++)
    {
        env.Optimize();
        fittest = lastGeneration.GetFittestByRank(0);
        cout << i << "\t|\t" << fittest.GetFitness() << "\t|\t" << fittest.GetExpression() << endl;
    }
}