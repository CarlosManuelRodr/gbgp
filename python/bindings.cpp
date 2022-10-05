#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include "../include/gbgp.h"
namespace py = pybind11;
using namespace std;

//*****************************
//*         Bindings          *
//****************************/

struct Pet {
    Pet(const std::string& name, int age) : name(name), age(age) { }
    bool set(int& age_) { age = age_; return true; }
    bool set(EvaluationContext& ctx) const { return true; }
    bool set(const std::string& name_) { name = name_; return true;  }
    bool Evaluate(EvaluationContext& ctx) const { return true; }
    std::string name;
    int age;
};

PYBIND11_MODULE(gbgp, m) {
    m.doc() = "gbgp python bindings"; // optional module docstring

    py::class_<Pet>(m, "Pet")
            .def(py::init<const std::string &, int>())
            .def("set", py::overload_cast<int&>(&Pet::set), "Set the pet's age")
            .def("set", py::overload_cast<EvaluationContext&>(&Pet::set, py::const_), "Set the pet's age")
            .def("set", static_cast<bool (Pet::*)(const std::string &)>(&Pet::set), "Set the pets name");

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
            .def(py::init<const TreeNode&>(), "Copy constructor that copies all linked nodes.", py::arg("other"))
            .def(py::init<const Terminal&>(), "Terminal node constructor.", py::arg("t"))
            .def(py::init<const Terminal&, const std::string&>(), "Terminal node with value constructor.", py::arg("t"), py::arg("value"))
            .def(py::init<const NonTerminal&>(), "NonTerminal node constructor.", py::arg("nt"))
            .def(py::init<const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal node constructor with explicit children instances.", py::arg("nt"), py::arg("children"))
            .def(py::init<const ProductionRule&, const NonTerminal&>(), "NonTerminal node constructor with generator production rule.", py::arg("productionRule"), py::arg("nt"))
            .def(py::init<const ProductionRule&, const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal node constructor that takes TreeNode instances as children and uses them as a blueprint to construct its own children nodes.",py::arg("productionRule"), py::arg("nt"), py::arg("children"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("ClearSynthesis", &TreeNode::ClearSynthesis, "Reset the synthesis of this node.")
            .def("IsSynthesized", &TreeNode::IsSynthesized, "Check if this node has been synthesized.")
            .def("ClearEvaluation", &TreeNode::ClearEvaluation, "Reset the evaluation of this node.")
            .def("IsEvaluated", &TreeNode::IsEvaluated, "Check if this node has been evaluated.")
            .def("HasChildren", &TreeNode::HasChildren, "Check if this node has children.")
            .def("GetValue", &TreeNode::GetValue, "Returns the value of the node.")
            .def("GetLabel", &TreeNode::GetLabel, "Returns a formatted label of the node.")
            .def("ToString", &TreeNode::ToString, "Get node representation as string.")
            .def("__repr__",
                 [](const TreeNode &node) {
                     return "<gbgp.TreeNode '" + node.ToString() + "'>";
                 }
            );

    py::class_<SyntaxTree>(m, "SyntaxTree")
            .def(py::init<>(), "Creates an empty SyntaxTree.")
            .def(py::init<const TreeNode&>(), "Builds a tree from a root node.", py::arg("root"))
            .def(py::init<const SyntaxTree&>(), "Copy constructor.", py::arg("other"))
            .def("Root", &SyntaxTree::Root, "Returns a reference to the root.")
            .def("SetRoot", &SyntaxTree::SetRoot, "Set the root node.", py::arg("rootNode"))
            .def("IsEmpty", &SyntaxTree::IsEmpty, "Check if the tree is empty.")
            .def("SetRootRule", &SyntaxTree::SetRootRule, "Set the production rule of the root node.", py::arg("startRule"))
            .def("ToString", &SyntaxTree::ToString, "Get string representation.")
            .def("GetTreeTraversal", &SyntaxTree::GetTreeTraversal, "Traverses the tree in a depth first post-order.")
            .def("SynthesizeExpression", &SyntaxTree::SynthesizeExpression, "Synthesizes the tree into an expression using the production rules of the grammar.")
            .def("Evaluate", py::overload_cast<EvaluationContext&>(&SyntaxTree::Evaluate, py::const_), "Evaluates the tree using the semantic actions of the grammar.", py::arg("ctx"))
            .def("ExternalEvaluate", py::overload_cast<function<string(string)>, string&>(&SyntaxTree::ExternalEvaluate<string>, py::const_), "Evaluates the tree using an external evaluator.", py::arg("evaluator"), py::arg("result"))
            .def("__repr__",
                 [](const SyntaxTree &tree) {
                     return "<gbgp.SyntaxTree '" + tree.ToString() + "'>";
                 }
            );

    py::class_<PruneRule>(m, "PruneRule")
            .def(py::init<const SyntaxTree&, const SyntaxTree&>(), "Constructor by SyntaxTree.")
            .def("CanBeApplied", &PruneRule::CanBeApplied, "Does the target tree can be simplified further with this rule?", py::arg("target"))
            .def("Apply", &PruneRule::Apply, "Create a new SyntaxTree where the prune rule has been applied.")
            .def("ToString", &PruneRule::ToString, "Get string representation.")
            .def("__repr__",
                 [](const PruneRule &pruneRule) {
                     return "<gbgp.PruneRule '" + pruneRule.ToString() + "'>";
                 }
            );
}