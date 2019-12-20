#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <morphio/enums.h>
#include <morphio/types.h>

#include <morphio/errorMessages.h>
#include <morphio/mito_section.h>
#include <morphio/mitochondria.h>
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#include "bind_immutable.cpp"
#include "bind_misc.cpp"
#include "bind_mutable.cpp"
#include "bind_vasculature.cpp"
#include "bindings_utils.cpp"

namespace py = pybind11;
using namespace py::literals;


PYBIND11_MODULE(morphio, m) {
    bind_misc(m);
    bind_immutable_module(m);

    py::module mut_module = m.def_submodule("mut");
    bind_mutable_module(mut_module);

    py::module vasc_module = m.def_submodule("vasculature");
    bind_vasculature(vasc_module);
}
