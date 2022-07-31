#include "doctest.h"
#include "../include/grammar.h"
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
                ProductionElement(conditionalNonTerm),
                ProductionElement(logicOpTerm),
                ProductionElement(conditionalNonTerm)
        },
        {
                SemanticElement("("),
                SemanticElement(conditionalNonTerm),
                SemanticElement(") "),
                SemanticElement(logicOpTerm),
                SemanticElement(" ("),
                SemanticElement(conditionalNonTerm),
                SemanticElement(")")
        }
);

const ProductionRule rule2(
        conditionalNonTerm,
        {
                ProductionElement(notTerm),
                ProductionElement(conditionalNonTerm)
        },
        {
                SemanticElement(notTerm),
                SemanticElement(conditionalNonTerm)
        }
);

const ProductionRule rule3(
        conditionalNonTerm,
        {
                ProductionElement(priceExpressionNonTerm),
                ProductionElement(numOpTerm),
                ProductionElement(priceExpressionNonTerm)
        },
        {
                SemanticElement(priceExpressionNonTerm),
                SemanticElement(" "),
                SemanticElement(numOpTerm),
                SemanticElement(" "),
                SemanticElement(priceExpressionNonTerm)
        }
);

const ProductionRule rule4(
        conditionalNonTerm,
        {
                ProductionElement(volumeExpressionNonTerm),
                ProductionElement(numOpTerm),
                ProductionElement(volumeExpressionNonTerm)
        },
        {
                SemanticElement(volumeExpressionNonTerm),
                SemanticElement(" "),
                SemanticElement(numOpTerm),
                SemanticElement(" "),
                SemanticElement(volumeExpressionNonTerm)
        }
);

const ProductionRule rule5(
        conditionalNonTerm,
        {
                ProductionElement(signedPercentageExpressionNonTerm),
                ProductionElement(numOpTerm),
                ProductionElement(signedPercentageExpressionNonTerm)
        },
        {
                SemanticElement(signedPercentageExpressionNonTerm),
                SemanticElement(" "),
                SemanticElement(numOpTerm),
                SemanticElement(" "),
                SemanticElement(signedPercentageExpressionNonTerm)
        }
);

const ProductionRule rule6(
        conditionalNonTerm,
        {
                ProductionElement(unsignedPercentageExpressionNonTerm),
                ProductionElement(numOpTerm),
                ProductionElement(unsignedPercentageExpressionNonTerm)
        },
        {
                SemanticElement(unsignedPercentageExpressionNonTerm),
                SemanticElement(" "),
                SemanticElement(numOpTerm),
                SemanticElement(" "),
                SemanticElement(unsignedPercentageExpressionNonTerm)
        }
);

const ProductionRule rule7(
        priceExpressionNonTerm,
        {
                ProductionElement(priceIndTerm)
        },
        {
                SemanticElement("Indicator(\""),
                SemanticElement(priceIndTerm),
                SemanticElement("\", stock, time)")
        }
);

const ProductionRule rule8(
        priceExpressionNonTerm,
        {
                ProductionElement(priceIndTerm),
                ProductionElement(percentileTerm),
        },
        {
                SemanticElement("IndQuantile(\""),
                SemanticElement(priceIndTerm),
                SemanticElement("\", "),
                SemanticElement(percentileTerm),
                SemanticElement(", stock, time)")
        }
);

const ProductionRule rule9(
        volumeExpressionNonTerm,
        {
                ProductionElement(volumeIndTerm)
        },
        {
                SemanticElement("Indicator(\""),
                SemanticElement(volumeIndTerm),
                SemanticElement("\", stock, time)")
        }
);

const ProductionRule rule10(
        volumeExpressionNonTerm,
        {
                ProductionElement(volumeIndTerm),
                ProductionElement(percentileTerm),
        },
        {
                SemanticElement("IndQuantile(\""),
                SemanticElement(volumeIndTerm),
                SemanticElement("\", "),
                SemanticElement(percentileTerm),
                SemanticElement(", stock, time)")
        }
);

const ProductionRule rule11(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement(signedIndTerm)
        },
        {
                SemanticElement("Indicator(\""),
                SemanticElement(signedIndTerm),
                SemanticElement("\", stock, time)")
        }
);

const ProductionRule rule12(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement(signedIndTerm),
                ProductionElement(percentileTerm),
        },
        {
                SemanticElement("IndQuantile(\""),
                SemanticElement(signedIndTerm),
                SemanticElement("\", "),
                SemanticElement(percentileTerm),
                SemanticElement(", stock, time)")
        }
);

const ProductionRule rule13(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement(unsignedIndTerm)
        },
        {
                SemanticElement("Indicator(\""),
                SemanticElement(unsignedIndTerm),
                SemanticElement("\", stock, time)")
        }
);

const ProductionRule rule14(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement(unsignedIndTerm),
                ProductionElement(percentileTerm),
        },
        {
                SemanticElement("IndQuantile(\""),
                SemanticElement(unsignedIndTerm),
                SemanticElement("\", "),
                SemanticElement(percentileTerm),
                SemanticElement(", stock, time)")
        }
);

/****************************
*       Test routines       *
****************************/
TEST_CASE("Testing random individual generation")
{
    Grammar grammar{rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8,
                    rule9, rule10, rule11, rule12, rule13, rule14 };

    SyntaxTree cst;
    grammar.CreateRandomTree(cst, 100);
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    CHECK((!cst.SynthesizeExpression().empty()));
}