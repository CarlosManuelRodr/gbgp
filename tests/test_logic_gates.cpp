#include "doctest.h"
#include "../include/gbgp.h"
using namespace std;
using namespace gbgp;

//*****************************
//*    Evaluation context     *
//****************************/

class BooleanContext : public EvaluationContext
{
public:
    bool x0{}, x1{};
    bool y0{}, y1{};
};
//*****************************
//*     Types declaration     *
//****************************/

enum Terms
{
    Var, LogOp, NotOp, // Terminals
    Array, LogExpr // NonTerminals
};

//*****************************
//*    Grammar declaration    *
//****************************/

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "x0", "x1" });
const Terminal logOpTerm(LogOp, "logOp", { "And", "Or", "Xor" });
const Terminal notOpTerm(NotOp, "notOp", { "Not" });
const NonTerminal arrayNonTerm(Array, "ARRAY");
const NonTerminal logExprNonTerm(LogExpr, "LOG_EXPR");

// Grammar definition.
const ProductionRule rule1(
        arrayNonTerm,
        {
                ProductionElement("{"),
                ProductionElement(logExprNonTerm),
                ProductionElement(", "),
                ProductionElement(logExprNonTerm),
                ProductionElement("}")
        },
        [](EvaluationContext& ctx) {
            auto& booleanContext = dynamic_cast<BooleanContext&>(ctx);
            booleanContext.y0 = stoi(ctx.SemanticValue(1));
            booleanContext.y1 = stoi(ctx.SemanticValue(3));
            ctx.result() = "{" + to_string(booleanContext.y0) + "," + to_string(booleanContext.y1) + "}";
        }
);

const ProductionRule rule2(
        logExprNonTerm,
        {
                ProductionElement(logOpTerm),
                ProductionElement("("),
                ProductionElement(logExprNonTerm),
                ProductionElement(","),
                ProductionElement(logExprNonTerm),
                ProductionElement(")")
        },
        [](EvaluationContext& ctx) {
            string op = ctx.SemanticValue(0);
            bool a = stoi(ctx.SemanticValue(2));
            bool b = stoi(ctx.SemanticValue(4));
            bool r;
            if (op == "And")
                r = a && b;
            else if (op == "Or")
                r = a || b;
            else //if (op == "Xor")
                r = a ^ b;

            ctx.result() = to_string(r);
        }
);

const ProductionRule rule3(
        logExprNonTerm,
        {
                ProductionElement(notOpTerm),
                ProductionElement("("),
                ProductionElement(logExprNonTerm),
                ProductionElement(")")
        },
        [](EvaluationContext& ctx) {
            bool a = stoi(ctx.SemanticValue(2));
            ctx.result() = to_string(!a);
        }
);

const ProductionRule rule4(
        logExprNonTerm,
        {
                ProductionElement(varTerm),
        },
        [](EvaluationContext& ctx) {
            auto& booleanContext = dynamic_cast<BooleanContext&>(ctx);
            string var = ctx.SemanticValue(0);
            bool r;
            if (var == "x0")
                r = booleanContext.x0;
            else //if (var == "x1")
                r = booleanContext.x1;

            ctx.result() = to_string(r);
        }
);

double logic_fitness_function(SyntaxTree& solution)
{
    int correct = 0;
    for (bool x0 : { false, true })
    {
        for (bool x1  : { false, true })
        {
            bool y0, y1;
            if (x0 == false && x1 == false) { y0 = false; y1 = false; }
            else if (x0 == false && x1 == true) { y0 = false; y1 = true; }
            else if (x0 == true && x1 == false) { y0 = false; y1 = true; }
            else { y0 = true; y1 = false; }

            BooleanContext ctx;
            ctx.x0 = x0;
            ctx.x1 = x1;
            solution.Evaluate(ctx);

            if (ctx.y0 == y0 && ctx.y1 == y1)
                correct++;
        }
    }

    return (double) correct / 4.0;
}

//*****************************
//*       Test routines       *
//****************************/

TEST_CASE("Test half adder optimization")
{
    SyntaxTree doubleNotExprFrom(
            TreeNode(
            rule3,
            logExprNonTerm,
            {
                TreeNode(notOpTerm, "Not"),
                TreeNode(rule3.to[1].term, "("),
                TreeNode(
                        rule3,
                        logExprNonTerm,
                        {
                            TreeNode(notOpTerm, "Not"),
                            TreeNode(rule3.to[1].term, "("),
                            TreeNode(
                                    rule4,
                                    logExprNonTerm,
                                    {
                                            TreeNode(varTerm, 1)
                                    }),
                            TreeNode(rule3.to[3].term, ")")
                        }),
                TreeNode(rule3.to[3].term, ")")
            })
    );

    SyntaxTree doubleNotExprTo(
            TreeNode(
                    rule4,
                    logExprNonTerm,
                    {
                            TreeNode(varTerm, 1)
                    })
            );

    SyntaxTree sameArgAndFrom(
            TreeNode(
                    rule2,
                    logExprNonTerm,
                    {
                            TreeNode(logOpTerm, "And"),
                            TreeNode(rule2.to[1].term, "("),
                            TreeNode(
                                    rule4,
                                    logExprNonTerm,
                                    {
                                            TreeNode(varTerm, 1)
                                    }),
                            TreeNode(rule2.to[3].term, ","),
                            TreeNode(
                                    rule4,
                                    logExprNonTerm,
                                    {
                                            TreeNode(varTerm, 1)
                                    }),
                            TreeNode(rule2.to[5].term, ")")
                    })
    );

    SyntaxTree sameArgAndTo(
            TreeNode(
                    rule4,
                    logExprNonTerm,
                    {
                            TreeNode(varTerm, 1)
                    })
    );

    SyntaxTree sameArgOrFrom(
            TreeNode(
                    rule2,
                    logExprNonTerm,
                    {
                            TreeNode(logOpTerm, "Or"),
                            TreeNode(rule2.to[1].term, "("),
                            TreeNode(
                                    rule4,
                                    logExprNonTerm,
                                    {
                                            TreeNode(varTerm, 1)
                                    }),
                            TreeNode(rule2.to[3].term, ","),
                            TreeNode(
                                    rule4,
                                    logExprNonTerm,
                                    {
                                            TreeNode(varTerm, 1)
                                    }),
                            TreeNode(rule2.to[5].term, ")")
                    })
    );

    SyntaxTree sameArgOrTo(
            TreeNode(
                    rule4,
                    logExprNonTerm,
                    {
                            TreeNode(varTerm, 1)
                    })
    );

    PruneRule doubleNegationExpr(doubleNotExprFrom, doubleNotExprTo);
    PruneRule sameArgAnd(sameArgAndFrom, sameArgAndTo);
    PruneRule sameArgOr(sameArgOrFrom, sameArgOrTo);
    std::vector<TreeNode*> sameArgPatternVariables = sameArgAndFrom.GetTermsOfType(NodeType::Terminal);
    CHECK(sameArgPatternVariables[2]->HasCaptureID());
    CHECK(sameArgPatternVariables[4]->HasCaptureID());
    CHECK((sameArgPatternVariables[2]->captureID == sameArgPatternVariables[4]->captureID));

    SyntaxTree doubleNotX1(doubleNotExprFrom);
    std::vector<TreeNode*> doubleNotVariables = doubleNotX1.GetTermsOfType(NodeType::Terminal);
    for (TreeNode* node : doubleNotVariables)
        if (node->termInstance == varTerm)
            node->termValue = "x1";
    CHECK(doubleNegationExpr.CanBeApplied(doubleNotX1));
    doubleNegationExpr.Apply(doubleNotX1);
    CHECK((doubleNotX1.SynthesizeExpression() == "x1"));

    SyntaxTree sameX0And(sameArgAndFrom);
    std::vector<TreeNode*> sameX0Variables = sameX0And.GetTermsOfType(NodeType::Terminal);
    for (TreeNode* node : sameX0Variables)
        if (node->termInstance == varTerm)
            node->termValue = "x0";
    CHECK(sameArgAnd.CanBeApplied(sameX0And));
    sameArgAnd.Apply(sameX0And);
    CHECK((sameX0And.SynthesizeExpression() == "x0"));

    SyntaxTree differentArgAnd(sameArgAndFrom);
    std::vector<TreeNode*> differentVariables = differentArgAnd.GetTermsOfType(NodeType::Terminal);
    int variableIndex = 0;
    for (TreeNode* node : differentVariables)
    {
        if (node->termInstance == varTerm)
            node->termValue = variableIndex++ == 0 ? "x0" : "x1";
    }
    CHECK((variableIndex == 2));
    CHECK((differentVariables[2]->termValue == "x0"));
    CHECK((differentVariables[4]->termValue == "x1"));
    CHECK_FALSE(sameArgAnd.CanBeApplied(differentArgAnd));

    Grammar grammar ({ rule1, rule2, rule3, rule4 }, { doubleNegationExpr, sameArgAnd, sameArgOr });
    Environment env(grammar, logic_fitness_function, 200, 100, 5, 5, 0.4);

    cout << "Generation\t|\tScore\t|\tExpression" << endl;
    Population& lastGeneration = env.GetPopulation();
    Individual fittest = lastGeneration.GetFittestByRank(0);

    for (int i = 0; i < 50 && fittest.GetFitness() < 1; i++)
    {
        env.Optimize();
        fittest = lastGeneration.GetFittestByRank(0);
        cout << i << "\t|\t" << fittest.GetFitness() << "\t|\t" << fittest.GetExpression() << endl;
    }

    CHECK((fittest.GetFitness() == 1.0));
}
