#ifndef GLOG_PYTHON_WIZARD_H
#define GLOG_PYTHON_WIZARD_H

#include <nanobind/nanobind.h>
#include <string>

namespace nb = nanobind;

// Forward declaration
class ProgramWrapper;

class WizardWrapper {
public:
    WizardWrapper();

    nb::tuple rewrite_program(ProgramWrapper& program_wrapper, const std::string& query);
};

#endif // GLOG_PYTHON_WIZARD_H
