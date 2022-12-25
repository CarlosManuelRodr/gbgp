#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <sstream>
#include "../include/environment.h"
#include "doctest.h"
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

Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6 };

//*****************************
//*       Test routines       *
//****************************/

TEST_CASE("Test Term serialization/deserialization")
{
    std::stringstream ss;

    // Serialize
    {
        cereal::BinaryOutputArchive oArchive(ss);
        oArchive(varTerm);
        cout << "Serialized Terminal: " << varTerm.ToString() << endl;
    }

    // Deserialize
    Terminal deserialized;
    {
        cereal::BinaryInputArchive iArchive(ss);
        iArchive(deserialized);
        cout << "Deserialized Terminal: " << deserialized.ToString() << endl;
    }

    CHECK((varTerm == deserialized));
}

TEST_CASE("Test NonTerm serialization/deserialization")
{
    std::stringstream ss;

    // Serialize
    {
        cereal::BinaryOutputArchive oArchive(ss);
        oArchive(exprNonTerm);
        cout << "Serialized NonTerminal: " << exprNonTerm.ToString() << endl;
    }

    // Deserialize
    NonTerminal deserialized;
    {
        cereal::BinaryInputArchive iArchive(ss);
        iArchive(deserialized);
        cout << "Deserialized NonTerminal: " << deserialized.ToString() << endl;
    }

    CHECK((exprNonTerm == deserialized));
}

TEST_CASE("Test ProductionElement serialization/deserialization")
{
    ProductionElement el(exprNonTerm);
    std::stringstream ss;

    // Serialize
    {
        cereal::BinaryOutputArchive oArchive(ss);
        oArchive(el);
        cout << "Serialized ProductionElement: " << el.ToString() << endl;
    }

    // Deserialize
    ProductionElement deserialized;
    {
        cereal::BinaryInputArchive iArchive(ss);
        iArchive(deserialized);
        cout << "Deserialized ProductionElement: " << deserialized.ToString() << endl;
    }

    CHECK((el == deserialized));
}

TEST_CASE("Test ProductionRule serialization/deserialization")
{
    std::stringstream ss;

    // Serialize
    {
        cereal::BinaryOutputArchive oArchive(ss);
        oArchive(rule1);
        cout << "Serialized ProductionRule: " << rule1.ToString() << endl;
    }

    // Deserialize
    ProductionRule deserialized;
    {
        cereal::BinaryInputArchive iArchive(ss);
        iArchive(deserialized);
        cout << "Deserialized ProductionRule: " << deserialized.ToString() << endl;
    }
    grammar.RestoreSemanticAction(deserialized);

    CHECK((rule1 == deserialized));
}

TEST_CASE("Test node serialization/deserialization")
{
    TreeNode node = TreeNode(factorNonTerm,{ TreeNode(varTerm, "b") });
    //std::ofstream os("node.bin", std::ios::binary);
    //cereal::BinaryOutputArchive archive( os );
}

TEST_CASE("Test tree serialization/deserialization")
{
    // GP Generator grammar
    SyntaxTree cst;
    EvaluationContext evaluationContext;
    grammar.CreateRandomTree(cst, 100);

    // Get original expr.
    string originalExpr = cst.SynthesizeExpression();

    // Serialize tree.
/*    std::ofstream os("out.cereal", std::ios::binary);
    cereal::BinaryOutputArchive archive( os );
    archive(cst);*/
}