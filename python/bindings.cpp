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
}