#include "doctest.h"
#include "../include/gbgp.h"
using namespace std;
using namespace gbgp;

/****************************
*    Grammar declaration    *
****************************/

enum Terms
{
    // Terminals
    NullTerm, LogicOp, Not, NumOp, Percentile, PriceInd,
    UnsignedVolumeInd, SignedVolumeInd,SignedInd, UnsignedInd,
    SignedValue, UnsignedValue,

    // NonTerminals
    NullNonTerm, Conditional, PriceExpression, UnsignedVolumeExpression,
    SignedVolumeExpression, SignedPercentageExpression,
    UnsignedPercentageExpression
};

// Term/Nonterm declaration.
const Terminal logicOpTerm(LogicOp, "logicOp", { "&&", "||" });
const Terminal notTerm(Not, "not", { "!" });
const Terminal numOpTerm(NumOp, "numOp", { ">", ">=", "<", "<=" });
const Terminal percentileTerm(Percentile, "percentile", { "0.05", "0.15", "0.25", "0.75", "0.85", "0.95" });
const Terminal priceIndTerm(PriceInd, "priceInd", { "OpenPrice", "ClosePrice", "HighPrice", "LowPrice",
                                                    "WeightedClose", "TypicalPrice", "MedianPrice", "SMA", "EMA", "VWAP" });

const Terminal unsignedVolumeIndTerm(UnsignedVolumeInd, "unsignedVolumeInd", { "TradingVolume" });
const Terminal signedVolumeIndTerm(SignedVolumeInd, "signedVolumeInd", { "OBV" });
const Terminal signedIndTerm(SignedInd, "signedInd", { "PricePercentageChangeOpenToClose", "ExtensionRatio" });
const Terminal unsignedIndTerm(UnsignedInd, "unsignedInd", { "ClosingBias" });

const Terminal signedValue(SignedValue, "signedValue", { "-100", "-75", "-50", "-25", "0", "25", "50", "75", "100" });
const Terminal unsignedValue(UnsignedValue, "unsignedValue", { "0", "25", "50", "75", "100" });

const NonTerminal conditionalNonTerm(Conditional, "Cond");
const NonTerminal priceExpressionNonTerm(PriceExpression, "PriceExpression");
const NonTerminal unsignedVolumeExpressionNonTerm(UnsignedVolumeExpression, "UnsignedVolumeExpression");
const NonTerminal signedVolumeExpressionNonTerm(SignedVolumeExpression, "SignedVolumeExpression");
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
                ProductionElement("("),
                ProductionElement(conditionalNonTerm),
                ProductionElement(")")
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
                ProductionElement(unsignedVolumeExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(unsignedVolumeExpressionNonTerm)
        }
);

const ProductionRule rule5(
        conditionalNonTerm,
        {
                ProductionElement(signedVolumeExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(signedVolumeExpressionNonTerm)
        }
);

const ProductionRule rule6(
        conditionalNonTerm,
        {
                ProductionElement(signedPercentageExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(signedPercentageExpressionNonTerm)
        }
);

const ProductionRule rule7(
        conditionalNonTerm,
        {
                ProductionElement(unsignedPercentageExpressionNonTerm),
                ProductionElement(" "),
                ProductionElement(numOpTerm),
                ProductionElement(" "),
                ProductionElement(unsignedPercentageExpressionNonTerm)
        }
);

const ProductionRule rule8(
        priceExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(priceIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule9(
        priceExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(priceIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule10(
        unsignedVolumeExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(unsignedVolumeIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule11(
        unsignedVolumeExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(unsignedVolumeIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule12(
        signedVolumeExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(signedVolumeIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule13(
        signedVolumeExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(signedVolumeIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule14(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(signedIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule15(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(signedIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule16(signedPercentageExpressionNonTerm,{ ProductionElement(signedValue) });

const ProductionRule rule17(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement("Indicator(\""),
                ProductionElement(unsignedIndTerm),
                ProductionElement("\", stock, time)")
        }
);

const ProductionRule rule18(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement("IndQuantile(\""),
                ProductionElement(unsignedIndTerm),
                ProductionElement("\", "),
                ProductionElement(percentileTerm),
                ProductionElement(", stock, time)")
        }
);

const ProductionRule rule19(unsignedPercentageExpressionNonTerm,{ ProductionElement(unsignedValue) });

/****************************
*       Test routines       *
****************************/
TEST_CASE("Testing random individual generation")
{
    Grammar grammar{ rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8,
                     rule9, rule10, rule11, rule12, rule13, rule14, rule15,
                     rule16, rule17, rule18, rule19 };

    SyntaxTree cst;
    grammar.CreateRandomTree(cst, 100);
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    CHECK((!cst.SynthesizeExpression().empty()));
}