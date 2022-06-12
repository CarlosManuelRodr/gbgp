#include <iostream>
#include <gbgp.h>
using namespace std;

/****************************
*                           *
*    Grammar declaration    *
*                           *
****************************/

// Types declaration.
enum class TradingTerm
{
    NullTerm, LogicOp, Not, NumOp, Percentile, PriceInd, VolumeInd, SignedInd, UnsignedInd
};

enum class TradingNonTerm
{
    NullNonTerm, Conditional, PriceExpression, VolumeExpression, SignedPercentageExpression, UnsignedPercentageExpression
};

// Term/Nonterm declaration.
const Terminal<TradingTerm> logicOpTerm(TradingTerm::LogicOp, "logicOp", { "&&", "||" });
const Terminal<TradingTerm> notTerm(TradingTerm::Not, "not", { "!" });
const Terminal<TradingTerm> numOpTerm(TradingTerm::NumOp, "numOp", { ">", ">=", "<", "<=" });
const Terminal<TradingTerm> percentileTerm(TradingTerm::Percentile, "percentile", { "0.05", "0.15", "0.25", "0.75", "0.85", "0.95" });
const Terminal<TradingTerm> priceIndTerm(TradingTerm::PriceInd, "priceInd", { "OpenPrice", "ClosePrice", "HighPrice", "LowPrice",
                                                                              "WeightedClose", "TypicalPrice", "MedianPrice", "EMA" });

const Terminal<TradingTerm> volumeIndTerm(TradingTerm::VolumeInd, "volumeInd", { "TradingVolume" });
const Terminal<TradingTerm> signedIndTerm(TradingTerm::SignedInd, "signedInd", { "PricePercentangeChangeOpenToClose", "ExtensionRatio" });
const Terminal<TradingTerm> unsignedIndTerm(TradingTerm::UnsignedInd, "unsignedInd", { "ClosingBias" });

const NonTerminal<TradingNonTerm> conditionalNonTerm(TradingNonTerm::Conditional, "Cond");
const NonTerminal<TradingNonTerm> priceExpressionNonTerm(TradingNonTerm::PriceExpression, "PriceExpression");
const NonTerminal<TradingNonTerm> volumeExpressionNonTerm(TradingNonTerm::VolumeExpression, "VolumeExpression");
const NonTerminal<TradingNonTerm> signedPercentageExpressionNonTerm(TradingNonTerm::SignedPercentageExpression, "SignedPercentageExpression");
const NonTerminal<TradingNonTerm> unsignedPercentageExpressionNonTerm(TradingNonTerm::UnsignedPercentageExpression, "UnsignedPercentageExpression");

// Grammar definition.
const ProductionRule<TradingTerm, TradingNonTerm> rule1(
    conditionalNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(conditionalNonTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(logicOpTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(conditionalNonTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("("),
        SemanticElement<TradingTerm, TradingNonTerm>(conditionalNonTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(") "),
        SemanticElement<TradingTerm, TradingNonTerm>(logicOpTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" ("),
        SemanticElement<TradingTerm, TradingNonTerm>(conditionalNonTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(")")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule2(
    conditionalNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(notTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(conditionalNonTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>(notTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(conditionalNonTerm)
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule3(
    conditionalNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(priceExpressionNonTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(numOpTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(priceExpressionNonTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>(priceExpressionNonTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(numOpTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(priceExpressionNonTerm)
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule4(
    conditionalNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(volumeExpressionNonTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(numOpTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(volumeExpressionNonTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>(volumeExpressionNonTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(numOpTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(volumeExpressionNonTerm)
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule5(
    conditionalNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(signedPercentageExpressionNonTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(numOpTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(signedPercentageExpressionNonTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>(signedPercentageExpressionNonTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(numOpTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(signedPercentageExpressionNonTerm)
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule6(
    conditionalNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(unsignedPercentageExpressionNonTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(numOpTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(unsignedPercentageExpressionNonTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>(unsignedPercentageExpressionNonTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(numOpTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(" "),
        SemanticElement<TradingTerm, TradingNonTerm>(unsignedPercentageExpressionNonTerm)
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule7(
    priceExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(priceIndTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("Indicator(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(priceIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule8(
    priceExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(priceIndTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(percentileTerm),
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("IndQuantile(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(priceIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", "),
        SemanticElement<TradingTerm, TradingNonTerm>(percentileTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule9(
    volumeExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(volumeIndTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("Indicator(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(volumeIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule10(
    volumeExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(volumeIndTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(percentileTerm),
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("IndQuantile(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(volumeIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", "),
        SemanticElement<TradingTerm, TradingNonTerm>(percentileTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule11(
    signedPercentageExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(signedIndTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("Indicator(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(signedIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule12(
    signedPercentageExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(signedIndTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(percentileTerm),
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("IndQuantile(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(signedIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", "),
        SemanticElement<TradingTerm, TradingNonTerm>(percentileTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule13(
    unsignedPercentageExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(unsignedIndTerm)
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("Indicator(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(unsignedIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", stock, time)")
    }
    );

const ProductionRule<TradingTerm, TradingNonTerm> rule14(
    unsignedPercentageExpressionNonTerm,
    {
        ProductionElement<TradingTerm, TradingNonTerm>(unsignedIndTerm),
        ProductionElement<TradingTerm, TradingNonTerm>(percentileTerm),
    },
    {
        SemanticElement<TradingTerm, TradingNonTerm>("IndQuantile(\""),
        SemanticElement<TradingTerm, TradingNonTerm>(unsignedIndTerm),
        SemanticElement<TradingTerm, TradingNonTerm>("\", "),
        SemanticElement<TradingTerm, TradingNonTerm>(percentileTerm),
        SemanticElement<TradingTerm, TradingNonTerm>(", stock, time)")
    }
    );


Grammar<TradingTerm, TradingNonTerm> grammar{ rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8,
                                              rule9, rule10, rule11, rule12, rule13, rule14 };


int main()
{
    ConcreteSyntaxTree<TradingTerm, TradingNonTerm> cst(grammar);
    cst.CreateRandomTreeSafe(100);
    cst.PrintTree();
    cout << cst.SynthesizeExpression() << endl;
}