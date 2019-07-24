#include "src/packedForest.h"

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

        .def("_growForestnumpy", [](fpForest<double> &self, py::array_t<double, py::array::c_style | py::array::forcecast> X, py::array_t<int, py::array::c_style | py::array::forcecast> Y, int numObs, int numFeatures) {
            py::buffer_info Xbuf = X.request();
            const double *Xptr = (double *)Xbuf.ptr;

            py::buffer_info Ybuf = Y.request();
            const int *Yptr = (int *)Ybuf.ptr;

            self.growForest(Xptr, Yptr, numObs, numFeatures);
        })
        .def("_return_pair_mat", &fpForest<double>::returnPairMat)
        .def("_growForest", py::overload_cast<>(&fpForest<double>::growForest))
        .def("_predict", py::overload_cast<std::vector<double> &>(&fpForest<double>::predict))
        .def("_predict_numpy", [](fpForest<double> &self, py::array_t<double, py::array::c_style | py::array::forcecast> mat) {
            py::buffer_info buf = mat.request();
            double *ptr = (double *)buf.ptr;
            int numObservations = buf.shape[0];
            int numFeatures = buf.shape[1];

            std::vector<double> currObs(numFeatures);
            std::vector<int> predictions(numObservations);

            for (int i = 0; i < numObservations; i++)
            {
                for (int j = 0; j < numFeatures; j++)
                {
                    currObs[j] = ptr[i * numFeatures + j];
                }
                predictions[i] = self.predict(currObs);
            }
            return predictions;
        })

        .def("_predict_post", &fpForest<double>::predictPost, "Returns a vector representing the votes for each class.")

        .def("_predict_post_array", [](fpForest<double> &self, py::array_t<double, py::array::c_style | py::array::forcecast> mat) {
            py::buffer_info buf = mat.request();
            double *ptr = (double *)buf.ptr;
            int numObservations = buf.shape[0];
            int numFeatures = buf.shape[1];

            std::vector<double> currObs(numFeatures);
            std::vector<std::vector<int>> predict_posts(numObservations);

            for (int i = 0; i < numObservations; i++)
            {
                for (int j = 0; j < numFeatures; j++)
                {
                    currObs[j] = ptr[i * numFeatures + j];
                }
                predict_posts[i] = self.predictPost(currObs);
            }
            return predict_posts;
        },
             "Returns a vector of vectors representing the votes for each class for each observation")

        .def("_report_OOB", &fpForest<double>::reportOOB, "Returns the out of bag score on the forest.")

        .def("testAccuracy", &fpForest<double>::testAccuracy);
}

} // namespace fp
