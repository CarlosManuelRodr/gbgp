#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include "../include/gbgp.h"
namespace py = pybind11;
using namespace std;

//*****************************
//*         Bindings          *
//****************************/

PYBIND11_MODULE(gbgp, m) {
    m.doc() = "gbgp python bindings"; // optional module docstring

    py::class_<Terminal>(m, "Terminal")
            .def(py::init<>())
            .def(py::init<const std::string&>(), py::arg("value"))
            .def(py::init<int, const std::string&>(), py::arg("pid"), py::arg("plabel"))
            .def(py::init<int, const std::string&, const std::vector<std::string>&>(), py::arg("pid"), py::arg("plabel"), py::arg("pvalues"))
            .def("IsMutable", &Terminal::IsMutable, "Does this terminal have multiple possible values?")
            .def("GetRandomValue", &Terminal::GetRandomValue, "Get a random value of the set of possible values.")
            .def("ToString", &Terminal::ToString, "Get the string representation.")
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def_readwrite("id", &Terminal::id, "The term type.")
            .def_readwrite("label", &Terminal::label, "The label used visual representation and debugging.")
            .def_readwrite("values", &Terminal::values, "List of possible terminal values that a node of this type can contain.")
            .def("__repr__",
                 [](const Terminal &t) {
                     return "<gbgp.Terminal " + t.ToString() + ">";
                 }
            );

    py::class_<NonTerminal>(m, "NonTerminal")
            .def(py::init<>())
            .def(py::init<int, const std::string&>(), py::arg("pid"), py::arg("plabel"))
            .def("ToString", &NonTerminal::ToString, "Get the string representation.")
            .def_readwrite("id", &NonTerminal::id, "The term type.")
            .def_readwrite("label", &NonTerminal::label, "The label used visual representation and debugging.")
            .def("__repr__",
                 [](const NonTerminal &nt) {
                     return "<gbgp.NonTerminal " + nt.ToString() + ">";
                 }
            );

    py::class_<EvaluationContext>(m, "EvaluationContext")
            .def(py::init<>())
            .def("GetResult", &EvaluationContext::GetResult, "Get the string result.")
            .def("SetResult", &EvaluationContext::SetResult, "Set a string result.", py::arg("s"))
            .def("SemanticValue", &EvaluationContext::SemanticValue, "Get semantic value of the associated ProductionRule at the specified index.", py::arg("index"))
            .def("GetSemanticValues", &EvaluationContext::GetSemanticValues, "Get the list of semantic values.")
            .def("PushSemanticValue", &EvaluationContext::PushSemanticValue, "Push the evaluation result of a ProductionRule for later use inside a semantic action.", py::arg("value"))
            .def("NumberOfSemanticValues", &EvaluationContext::NumberOfSemanticValues, "Get the total number of semantic values.")
            .def("TransferSemanticValueToResult", &EvaluationContext::TransferSemanticValueToResult, "Transfer the semantic value at the specified index to the result.")
            .def("ToString", &EvaluationContext::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const EvaluationContext &ctx) {
                     return "<gbgp.EvaluationContext " + ctx.ToString() + ">";
                 }
            );

    py::enum_<ProductionElementType>(m, "ProductionElementType")
            .value("Unassigned", ProductionElementType::Unassigned)
            .value("NonTerminal", ProductionElementType::NonTerminal)
            .value("Terminal", ProductionElementType::Terminal);

    py::class_<ProductionElement>(m, "ProductionElement")
            .def(py::init<const std::string&>(), "Terminal constructor with unique value.", py::arg("value"))
            .def(py::init<const Terminal&>(), "Terminal element constructor.", py::arg("pterm"))
            .def(py::init<const NonTerminal&>(), "NonTerminal element constructor.", py::arg("pnonterm"))
            .def(py::init<int, const std::string&, const std::vector<std::string>&>(), "Terminal constructor with various possible values.", py::arg("id"), py::arg("label"), py::arg("values"))
            .def("GetType", &ProductionElement::GetType, "Get the production element type.")
            .def("GetTypeStr", &ProductionElement::GetTypeStr, "Get production element type as a string.")
            .def("GetValue", &ProductionElement::GetValue, "Get the label value of the production element.")
            .def("ToString", &ProductionElement::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const ProductionElement &pe) {
                     return "<gbgp.ProductionElement " + pe.ToString() + ">";
                 }
            );

    py::class_<ProductionRule>(m, "ProductionRule")
            .def(py::init<>(), "Empty ProductionRule constructor.")
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&>(), "Production rule with default semantic action.", py::arg("pfrom"), py::arg("pto"))
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, int>(), "Production rule with default semantic action over element at index semanticTransferIndex.", py::arg("pfrom"), py::arg("pto"), py::arg("semanticTransferIndex"))
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, std::function<void(EvaluationContext&)>>(), "Production rule with custom semantic action.", py::arg("pfrom"), py::arg("pto"), py::arg("pSemanticAction"))
            .def("NumberOfProductionElements", &ProductionRule::NumberOfProductionElements, "Returns the number of production elements.")
            .def("GetFrom", &ProductionRule::GetFrom, "Getter for the from list.")
            .def("GetTo", &ProductionRule::GetTo, "Getter for the to list.")
            .def("GetSemanticAction", &ProductionRule::GetSemanticAction, "Getter for the semantic action.")
            .def("ToString", &ProductionRule::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const ProductionRule &pr) {
                     return "<gbgp.ProductionRule '" + pr.ToString() + "'>";
                 }
            );

    py::enum_<TreeNodeType>(m, "TreeNodeType")
            .value("None", TreeNodeType::None)
            .value("NonTerminal", TreeNodeType::NonTerminal)
            .value("Terminal", TreeNodeType::Terminal);

    py::class_<TreeNode>(m, "TreeNode")
            .def(py::init<>(), "Constructor of an empty node.")
            .def(py::init<const TreeNode&>(), "Copy constructor that copies all linked nodes.")
            .def(py::init<const Terminal&>(), "Terminal node constructor.")
            .def(py::init<const Terminal&, const std::string&>(), "Terminal node with value constructor.")
            .def(py::init<const NonTerminal&>(), "NonTerminal node constructor.")
            .def(py::init<const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal node constructor with explicit children instances.")
            .def(py::init<const ProductionRule&, const NonTerminal&>(), "NonTerminal node constructor with generator production rule.")
            .def(py::init<const ProductionRule&, const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal node constructor that takes TreeNode instances as children and uses them as a blueprint to construct its own children nodes.")
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("SameID", &TreeNode::SameID)
            .def("ClearSynthesis", &TreeNode::ClearSynthesis)
            .def("IsSynthesized", &TreeNode::IsSynthesized)
            .def("ClearEvaluation", &TreeNode::ClearEvaluation)
            .def("IsEvaluated", &TreeNode::IsEvaluated)
            .def("HasChildren", &TreeNode::HasChildren)
            .def("GetValue", &TreeNode::GetValue)
            .def("GetLabel", &TreeNode::GetLabel)
            .def("ToString", &TreeNode::ToString)
            .def("__repr__",
                 [](const TreeNode &node) {
                     return "<gbgp.TreeNode value='" + node.ToString() + "'>";
                 }
            );

    py::class_<SyntaxTree>(m, "SyntaxTree")
            .def(py::init<>())
            .def(py::init<const TreeNode&>())
            .def(py::init<const SyntaxTree&>())
            .def("IsEmpty", &SyntaxTree::IsEmpty)
            .def("SetRootRule", &SyntaxTree::SetRootRule)
            .def("ToString", &SyntaxTree::ToString)
            .def("GetTreeTraversal", &SyntaxTree::GetTreeTraversal)
            .def("SynthesizeExpression", &SyntaxTree::SynthesizeExpression)
            //.def("Evaluate", &SyntaxTree::Evaluate)
            .def("__repr__",
                 [](const SyntaxTree &tree) {
                     return "<gbgp.SyntaxTree '" + tree.ToString() + "'>";
                 }
            );
}