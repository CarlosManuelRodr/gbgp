#include "doctest.h"
#include "../include/genetic_operators.h"
using namespace std;

/****************************
*     Types declaration     *
****************************/

enum Terms
{
    Var, Plus, Times, LeftParenthesis, RightParenthesis, // Terminals
    Expr, Term, Factor // NonTerminals
};

/****************************
*    Grammar declaration    *
****************************/

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "a", "b", "c" });
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
        }
);

const ProductionRule rule2(
        exprNonTerm,
        {
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(termNonTerm)
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
        }
);

const ProductionRule rule4(
        termNonTerm,
        {
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(factorNonTerm)
        }
);

const ProductionRule rule5(
        factorNonTerm,
        {
                ProductionElement(leftParenthesisTerm),
                ProductionElement(exprNonTerm),
                ProductionElement(rightParenthesisTerm),
        },
        {
                SemanticElement(leftParenthesisTerm),
                SemanticElement(exprNonTerm),
                SemanticElement(rightParenthesisTerm)
        }
);

const ProductionRule rule6(
        factorNonTerm,
        {
                ProductionElement(varTerm),
        },
        {
                SemanticElement(varTerm)
        }
);

/****************************
*       Test routines       *
****************************/
TEST_CASE("Test Term mutation")
{
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 });

    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual();
        ind.CreateRandom(grammar);

        string originalSynth = ind.GetExpression();
        cout << "Original: " << originalSynth << endl;

        GeneticOperators::MutateIndividual(ind, grammar, 0.0);

        string replacedSynth = ind.GetExpression();
        cout << "Replaced: " << replacedSynth << endl;
    }

}

TEST_CASE("Test NonTerm mutation")
{
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 });

    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual();
        ind.CreateRandom(grammar);

        string originalSynth = ind.GetExpression();
        cout << "Original: " << originalSynth << endl;

        try {
            GeneticOperators::MutateIndividual(ind, grammar, 1.0);
        }
        catch (std::runtime_error &e) {
            cout << e.what() << endl;
        }

        string replacedSynth = ind.GetExpression();
        cout << "Replaced: " << replacedSynth << endl;
    }
}

TEST_CASE("Test mutation")
{
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 });

    for (int i = 0; i < 100; i++)
    {
        auto ind = Individual();
        ind.CreateRandom(grammar);

        string originalSynth = ind.GetExpression();
        cout << "Original: " << originalSynth << endl;

        GeneticOperators::MutateIndividual(ind, grammar);

        string replacedSynth = ind.GetExpression();
        cout << "Replaced: " << replacedSynth << endl;
    }
}
