#include "doctest.h"
#include "../include/gbgp.h"
using namespace std;

/****************************
*    Grammar declaration    *
****************************/

enum Terms
{
    NullTerm, LogicOp, Not, NumOp, Percentile, PriceInd, VolumeInd, SignedInd, UnsignedInd, // Terminals
    NullNonTerm, Conditional, PriceExpression, VolumeExpression, SignedPercentageExpression, UnsignedPercentageExpression // NonTerminals
};

// Term/Nonterm declaration.
const Terminal logicOpTerm(LogicOp, "logicOp", { "&&", "||" });
const Terminal notTerm(Not, "not", { "!" });
const Terminal numOpTerm(NumOp, "numOp", { ">", ">=", "<", "<=" });
const Terminal percentileTerm(Percentile, "percentile", { "0.05", "0.15", "0.25", "0.75", "0.85", "0.95" });
const Terminal priceIndTerm(PriceInd, "priceInd", { "OpenPrice", "ClosePrice", "HighPrice", "LowPrice",
                                                             "WeightedClose", "TypicalPrice", "MedianPrice", "EMA" });

const Terminal volumeIndTerm(VolumeInd, "volumeInd", { "TradingVolume" });
const Terminal signedIndTerm(SignedInd, "signedInd", { "PricePercentageChangeOpenToClose", "ExtensionRatio" });
const Terminal unsignedIndTerm(UnsignedInd, "unsignedInd", { "ClosingBias" });

const NonTerminal conditionalNonTerm(Conditional, "Cond");
const NonTerminal priceExpressionNonTerm(PriceExpression, "PriceExpression");
const NonTerminal volumeExpressionNonTerm(VolumeExpression, "VolumeExpression");
const NonTerminal signedPercentageExpressionNonTerm(SignedPercentageExpression, "SignedPercentageExpression");
const NonTerminal unsignedPercentageExpressionNonTerm(UnsignedPercentageExpression, "UnsignedPercentageExpression");

// Grammar definition.
const ProductionRule rule1(
        conditionalNonTerm,
        {
                ProductionElement("("),
                ProductionElement(conditionalNonTerm),
                ProductionElement(") "),
                ProductionElement(logicOpTerm),
                ProductionElement(" ("),
                ProductionElement(conditionalNonTerm),
                ProductionElement(")")
        }
);

const ProductionRule rule2(
        conditionalNonTerm,
        {
                ProductionElement(notTerm),
                ProductionElement(conditionalNonTerm)
        }
);

const ProductionRule rule3(
        conditionalNonTerm,
        {
                ProductionElement(priceExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(priceExpressionNonTerm)
        }
);

const ProductionRule rule4(
        conditionalNonTerm,
        {
                ProductionElement(volumeExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(volumeExpressionNonTerm)
        }
);

const ProductionRule rule5(
        conditionalNonTerm,
        {
                ProductionElement(signedPercentageExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(signedPercentageExpressionNonTerm)
        }
);

const ProductionRule rule6(
        conditionalNonTerm,
        {
                ProductionElement(unsignedPercentageExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(unsignedPercentageExpressionNonTerm)
        }
);

const ProductionRule rule7(
        priceExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(priceIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule8(
        priceExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(priceIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule9(
        volumeExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(volumeIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule10(
        volumeExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(volumeIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule11(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(signedIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule12(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(signedIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule13(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(unsignedIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule14(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(unsignedIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

/****************************
*       Test routines       *
****************************/
TEST_CASE("Testing random individual generation")
{
    Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8,
                     rule9, rule10, rule11, rule12, rule13, rule14 };

    SyntaxTree cst;
    grammar.CreateRandomTree(cst, 100);
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    CHECK((!cst.SynthesizeExpression().empty()));
}