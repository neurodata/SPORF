#include "../src/packedForest/src/packedForest.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>

#include <string>

namespace py = pybind11;

namespace fp
{

PYBIND11_MODULE(pyfp, m)
{
    py::class_<fpForest<double>>(m, "fpForest")
        .def(py::init<>())
        .def("setParameter",
             py::overload_cast<const std::string &, const std::string &>(&fpForest<double>::setParameter),
             "sets a string parameter")
        .def("setParameter",
             py::overload_cast<const std::string &, const int>(&fpForest<double>::setParameter),
             "sets an int parameter")
        .def("setParameter",
             py::overload_cast<const std::string &, const double>(&fpForest<double>::setParameter),
             "sets a float parameter")
        .def("printParameters", &fpForest<double>::printParameters,
             py::call_guard<py::scoped_ostream_redirect,
                            py::scoped_estream_redirect>())
        .def("printForestType", &fpForest<double>::printForestType,
             py::call_guard<py::scoped_ostream_redirect,
                            py::scoped_estream_redirect>())
        .def("setNumberOfThreads", &fpForest<double>::setNumberOfThreads)
        .def("growForest",
             py::overload_cast<const py::array_t<double> *, const int *, int, int>(&fpForest<double>::growForest))
        .def("growForest",
             py::overload_cast<>(&fpForest<double>::growForest))
        .def("predict",
             py::overload_cast<std::vector<double> &>(&fpForest<double>::predict))
        .def("predict",
             py::overload_cast<const double *>(&fpForest<double>::predict))
        .def("testAccuracy", &fpForest<double>::testAccuracy);
}

} // namespace fp