#include "doctest.h"
#include "../include/gbgp.h"
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
        { ProductionElement(exprNonTerm), ProductionElement(plusTerm), ProductionElement(termNonTerm) }
);

const ProductionRule rule2(
        exprNonTerm,
        { ProductionElement(termNonTerm) }
);

const ProductionRule rule3(
        termNonTerm,
        { ProductionElement(termNonTerm), ProductionElement(timesTerm), ProductionElement(factorNonTerm) }
);

const ProductionRule rule4(
        termNonTerm,
        { ProductionElement(factorNonTerm) }
);

const ProductionRule rule5(
        factorNonTerm,
        { ProductionElement(leftParenthesisTerm), ProductionElement(exprNonTerm), ProductionElement(rightParenthesisTerm) }
);

const ProductionRule rule6(
        factorNonTerm,
        { ProductionElement(varTerm) }
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

        GeneticOperators::MutateIndividual(ind, grammar, 1.0);

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

TEST_CASE("Test crossover")
{
    Grammar grammar({ rule1, rule2, rule3, rule4, rule5, rule6 });

    auto parent1 = Individual();
    auto parent2 = Individual();
    parent1.CreateRandom(grammar);
    parent2.CreateRandom(grammar);

    string originalSynthParent1 = parent1.GetExpression();
    string originalSynthParent2 = parent2.GetExpression();
    cout << "Parent 1 original: " << originalSynthParent1 << endl;
    cout << "Parent 2 original: " << originalSynthParent2 << endl;

    Individual offspring = GeneticOperators::IndividualsCrossover(parent1, parent2);

    string offspringSynth = offspring.GetExpression();
    cout << "Offspring: " << offspringSynth << endl;
}
