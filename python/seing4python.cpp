#include "../src/inputs.h" 
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(seing4python, m){
    m.doc() = "Read in the option files and";
    m.def("read_prop_file", &read_prop_file);

    py::class_<fingerprintProperties>(m, "fingerprintProperties");
    /*
    py::class_<>(m, "Inputs")
        .def(py::init<>)
      */  
}
