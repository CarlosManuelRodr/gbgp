#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/gbgp.h"
namespace py = pybind11;
using namespace std;

//*****************************
//*        Utilities          *
//****************************/

string to_string(const vector<string>& v)
{
    string s = "[";
    for (auto& elem : v)
    {
        size_t i = &elem - &v[0];
        s += "'" + elem + "'";
        if (i != v.size() - 1)
            s += ",";
    }
    s += "]";
    return s;
}

//*****************************
//*         Bindings          *
//****************************/

PYBIND11_MODULE(gbgp, m) {
    m.doc() = "gbgp python bindings"; // optional module docstring

    py::class_<Terminal>(m, "Terminal")
            .def(py::init<>())
            .def(py::init<int, const std::string&>())
            .def(py::init<int, const std::string&, const std::vector<std::string>&>())
            .def("IsMutable", &Terminal::IsMutable)
            .def("GetRandomValue", &Terminal::GetRandomValue)
            .def_readwrite("id", &Terminal::id)
            .def_readwrite("label", &Terminal::label)
            .def_readwrite("values", &Terminal::values)
            .def("__repr__",
                 [](const Terminal &t) {
                     return "<gbgp.Terminal id='" + to_string(t.id) +
                     "', label='" + t.label + "', values='" + to_string(t.values) + "'>";
                 }
            );

    py::class_<NonTerminal>(m, "NonTerminal")
            .def(py::init<>())
            .def(py::init<int, const std::string&>())
            .def_readwrite("id", &NonTerminal::id)
            .def_readwrite("label", &NonTerminal::label)
            .def("__repr__",
                 [](const NonTerminal &nt) {
                     return "<gbgp.NonTerminal id='" + to_string(nt.id) +
                            "', label='" + nt.label + "'>";
                 }
            );

    py::class_<EvaluationContext>(m, "EvaluationContext")
            .def(py::init<>())
            .def("GetResult", &EvaluationContext::GetResult)
            .def("SetResult", &EvaluationContext::SetResult)
            .def("SemanticValue", &EvaluationContext::SemanticValue)
            .def("GetSemanticValues", &EvaluationContext::GetSemanticValues)
            .def("PushSemanticValue", &EvaluationContext::PushSemanticValue)
            .def("NumberOfSemanticValues", &EvaluationContext::NumberOfSemanticValues)
            .def("__repr__",
                 [](const EvaluationContext &ctx) {
                     return "<gbgp.EvaluationContext result='" + ctx.result() +
                            "', semanticValues='" + to_string(ctx.GetSemanticValues()) + "'>";
                 }
            );

    py::enum_<ProductionElementType>(m, "ProductionElementType")
            .value("Unassigned", ProductionElementType::Unassigned)
            .value("NonTerminal", ProductionElementType::NonTerminal)
            .value("Terminal", ProductionElementType::Terminal);

    py::class_<ProductionElement>(m, "ProductionElement")
            .def(py::init<const std::string&>())
            .def(py::init<const Terminal&>())
            .def(py::init<const NonTerminal&>())
            .def(py::init<int, const std::string&, const std::vector<std::string>&>())
            .def("GetType", &ProductionElement::GetType)
            .def("GetTypeStr", &ProductionElement::GetTypeStr)
            .def("GetValue", &ProductionElement::GetValue)
            .def("__repr__",
                 [](const ProductionElement &pe) {
                     return "<gbgp.ProductionElement type='" + pe.GetTypeStr() +
                            "', value='" + pe.GetValue() + "'>";
                 }
            );

    py::class_<ProductionRule>(m, "ProductionRule")
            .def(py::init<>())
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&>())
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, int>())
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, std::function<void(EvaluationContext&)>>())
            .def("NumberOfProductionElements", &ProductionRule::NumberOfProductionElements)
            .def("ToString", &ProductionRule::ToString)
            .def("GetFrom", &ProductionRule::GetFrom)
            .def("GetTo", &ProductionRule::GetTo)
            .def("GetSemanticAction", &ProductionRule::GetSemanticAction)
            .def("__repr__",
                 [](const ProductionRule &pr) {
                     return "<gbgp.ProductionRule value='" + pr.ToString() + "'>";
                 }
            );

    py::enum_<TreeNodeType>(m, "TreeNodeType")
            .value("None", TreeNodeType::None)
            .value("NonTerminal", TreeNodeType::NonTerminal)
            .value("Terminal", TreeNodeType::Terminal);

    py::class_<TreeNode>(m, "TreeNode")
            .def(py::init<>())
            .def(py::init<const TreeNode&>())
            .def(py::init<const Terminal&>())
            .def(py::init<const Terminal&, const std::string&>())
            .def(py::init<const NonTerminal&>())
            .def(py::init<const NonTerminal&, const std::vector<TreeNode>&>())
            .def(py::init<const ProductionRule&, const NonTerminal&>())
            .def(py::init<const ProductionRule&, const NonTerminal&, const std::vector<TreeNode>&>())
            //.def("GetSemanticAction", &ProductionRule::GetSemanticAction)
            .def("__repr__",
                 [](const TreeNode &node) {
                     return "<gbgp.TreeNode value='" + node.ToString() + "'>";
                 }
            );
}