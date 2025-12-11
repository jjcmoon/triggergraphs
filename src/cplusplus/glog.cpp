#include <nanobind/nanobind.h>
#include <kognac/logs.h>

namespace nb = nanobind;

void bind_tupleset(nb::module_ &m);
void bind_edblayer(nb::module_ &m);
void bind_program(nb::module_ &m);
void bind_reasoner(nb::module_ &m);
void bind_tg(nb::module_ &m);
void bind_querier(nb::module_ &m);

NB_MODULE(glog, m) {
    m.doc() = "Python bindings for GLog";

    m.def("set_logging_level", [](int level) {
        Logger::setMinLevel(level);
    }, nb::arg("level"),
       "Set the logging level. From 0 (trace) to 5 (error).");

    // Set default logging level to INFO
    Logger::setMinLevel(INFOL);

    bind_tupleset(m);
    bind_edblayer(m);
    bind_program(m);
    bind_reasoner(m);
    bind_tg(m);
    bind_querier(m);
}
