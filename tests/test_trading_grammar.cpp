#include "doctest.h"
#include "../include/cst_tree.h"
using namespace std;

/****************************
*    Grammar declaration    *
****************************/

// Types declaration.
enum class TermType
{
    NullTerm, LogicOp, Not, NumOp, Percentile, PriceInd, VolumeInd, SignedInd, UnsignedInd
};

enum class NonTermType
{
    NullNonTerm, Conditional, PriceExpression, VolumeExpression, SignedPercentageExpression, UnsignedPercentageExpression
};

// Term/Nonterm declaration.
const Terminal logicOpTerm(TermType::LogicOp, "logicOp", { "&&", "||" });
const Terminal notTerm(TermType::Not, "not", { "!" });
const Terminal numOpTerm(TermType::NumOp, "numOp", { ">", ">=", "<", "<=" });
const Terminal percentileTerm(TermType::Percentile, "percentile", { "0.05", "0.15", "0.25", "0.75", "0.85", "0.95" });
const Terminal priceIndTerm(TermType::PriceInd, "priceInd", { "OpenPrice", "ClosePrice", "HighPrice", "LowPrice",
                                                             "WeightedClose", "TypicalPrice", "MedianPrice", "EMA" });

const Terminal volumeIndTerm(TermType::VolumeInd, "volumeInd", { "TradingVolume" });
const Terminal signedIndTerm(TermType::SignedInd, "signedInd", { "PricePercentageChangeOpenToClose", "ExtensionRatio" });
const Terminal unsignedIndTerm(TermType::UnsignedInd, "unsignedInd", { "ClosingBias" });

const NonTerminal conditionalNonTerm(NonTermType::Conditional, "Cond");
const NonTerminal priceExpressionNonTerm(NonTermType::PriceExpression, "PriceExpression");
const NonTerminal volumeExpressionNonTerm(NonTermType::VolumeExpression, "VolumeExpression");
const NonTerminal signedPercentageExpressionNonTerm(NonTermType::SignedPercentageExpression, "SignedPercentageExpression");
const NonTerminal unsignedPercentageExpressionNonTerm(NonTermType::UnsignedPercentageExpression, "UnsignedPercentageExpression");

// Grammar definition.
const ProductionRule<TermType, NonTermType> rule1(
        conditionalNonTerm,
        {
                ProductionElement<TermType, NonTermType>(conditionalNonTerm),
                ProductionElement<TermType, NonTermType>(logicOpTerm),
                ProductionElement<TermType, NonTermType>(conditionalNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>("("),
                SemanticElement<TermType, NonTermType>(conditionalNonTerm),
                SemanticElement<TermType, NonTermType>(") "),
                SemanticElement<TermType, NonTermType>(logicOpTerm),
                SemanticElement<TermType, NonTermType>(" ("),
                SemanticElement<TermType, NonTermType>(conditionalNonTerm),
                SemanticElement<TermType, NonTermType>(")")
        }
);

const ProductionRule<TermType, NonTermType> rule2(
        conditionalNonTerm,
        {
                ProductionElement<TermType, NonTermType>(notTerm),
                ProductionElement<TermType, NonTermType>(conditionalNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(notTerm),
                SemanticElement<TermType, NonTermType>(conditionalNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule3(
        conditionalNonTerm,
        {
                ProductionElement<TermType, NonTermType>(priceExpressionNonTerm),
                ProductionElement<TermType, NonTermType>(numOpTerm),
                ProductionElement<TermType, NonTermType>(priceExpressionNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(priceExpressionNonTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(numOpTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(priceExpressionNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule4(
        conditionalNonTerm,
        {
                ProductionElement<TermType, NonTermType>(volumeExpressionNonTerm),
                ProductionElement<TermType, NonTermType>(numOpTerm),
                ProductionElement<TermType, NonTermType>(volumeExpressionNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(volumeExpressionNonTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(numOpTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(volumeExpressionNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule5(
        conditionalNonTerm,
        {
                ProductionElement<TermType, NonTermType>(signedPercentageExpressionNonTerm),
                ProductionElement<TermType, NonTermType>(numOpTerm),
                ProductionElement<TermType, NonTermType>(signedPercentageExpressionNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(signedPercentageExpressionNonTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(numOpTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(signedPercentageExpressionNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule6(
        conditionalNonTerm,
        {
                ProductionElement<TermType, NonTermType>(unsignedPercentageExpressionNonTerm),
                ProductionElement<TermType, NonTermType>(numOpTerm),
                ProductionElement<TermType, NonTermType>(unsignedPercentageExpressionNonTerm)
        },
        {
                SemanticElement<TermType, NonTermType>(unsignedPercentageExpressionNonTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(numOpTerm),
                SemanticElement<TermType, NonTermType>(" "),
                SemanticElement<TermType, NonTermType>(unsignedPercentageExpressionNonTerm)
        }
);

const ProductionRule<TermType, NonTermType> rule7(
        priceExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(priceIndTerm)
        },
        {
                SemanticElement<TermType, NonTermType>("Indicator(\""),
                SemanticElement<TermType, NonTermType>(priceIndTerm),
                SemanticElement<TermType, NonTermType>("\", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule8(
        priceExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(priceIndTerm),
                ProductionElement<TermType, NonTermType>(percentileTerm),
        },
        {
                SemanticElement<TermType, NonTermType>("IndQuantile(\""),
                SemanticElement<TermType, NonTermType>(priceIndTerm),
                SemanticElement<TermType, NonTermType>("\", "),
                SemanticElement<TermType, NonTermType>(percentileTerm),
                SemanticElement<TermType, NonTermType>(", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule9(
        volumeExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(volumeIndTerm)
        },
        {
                SemanticElement<TermType, NonTermType>("Indicator(\""),
                SemanticElement<TermType, NonTermType>(volumeIndTerm),
                SemanticElement<TermType, NonTermType>("\", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule10(
        volumeExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(volumeIndTerm),
                ProductionElement<TermType, NonTermType>(percentileTerm),
        },
        {
                SemanticElement<TermType, NonTermType>("IndQuantile(\""),
                SemanticElement<TermType, NonTermType>(volumeIndTerm),
                SemanticElement<TermType, NonTermType>("\", "),
                SemanticElement<TermType, NonTermType>(percentileTerm),
                SemanticElement<TermType, NonTermType>(", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule11(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(signedIndTerm)
        },
        {
                SemanticElement<TermType, NonTermType>("Indicator(\""),
                SemanticElement<TermType, NonTermType>(signedIndTerm),
                SemanticElement<TermType, NonTermType>("\", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule12(
        signedPercentageExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(signedIndTerm),
                ProductionElement<TermType, NonTermType>(percentileTerm),
        },
        {
                SemanticElement<TermType, NonTermType>("IndQuantile(\""),
                SemanticElement<TermType, NonTermType>(signedIndTerm),
                SemanticElement<TermType, NonTermType>("\", "),
                SemanticElement<TermType, NonTermType>(percentileTerm),
                SemanticElement<TermType, NonTermType>(", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule13(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(unsignedIndTerm)
        },
        {
                SemanticElement<TermType, NonTermType>("Indicator(\""),
                SemanticElement<TermType, NonTermType>(unsignedIndTerm),
                SemanticElement<TermType, NonTermType>("\", stock, time)")
        }
);

const ProductionRule<TermType, NonTermType> rule14(
        unsignedPercentageExpressionNonTerm,
        {
                ProductionElement<TermType, NonTermType>(unsignedIndTerm),
                ProductionElement<TermType, NonTermType>(percentileTerm),
        },
        {
                SemanticElement<TermType, NonTermType>("IndQuantile(\""),
                SemanticElement<TermType, NonTermType>(unsignedIndTerm),
                SemanticElement<TermType, NonTermType>("\", "),
                SemanticElement<TermType, NonTermType>(percentileTerm),
                SemanticElement<TermType, NonTermType>(", stock, time)")
        }
);

/****************************
*       Test routines       *
****************************/
TEST_CASE("Testing random individual generation")
{
    Grammar<TermType, NonTermType> grammar{rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8,
                                           rule9, rule10, rule11, rule12, rule13, rule14 };

    ConcreteSyntaxTree<TermType, NonTermType> cst;
    cst.CreateRandomTree(grammar, 100);
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
    CHECK(cst.SynthesizeExpression().size() > 0);
}