#ifndef GLOG_PYTHON_REASONER_H
#define GLOG_PYTHON_REASONER_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/unordered_map.h>
#include <memory>
#include <string>

#include <glog/gbchase.h>

namespace nb = nanobind;

// Forward declarations
class TGWrapper;
class TupleSet;
class EDBLayerWrapper;
class ProgramWrapper;

class ReasonerWrapper {
private:
    nb::object edb_obj;     // Keep EDBLayer alive
    nb::object program_obj; // Keep Program alive
    EDBLayer* edb;
    ProgramWrapper* program;

public:
    std::shared_ptr<GBChase> sn;  // Public for TG access

    ReasonerWrapper(const std::string& typeChase,
                   EDBLayerWrapper& edb_wrapper,
                   ProgramWrapper& prog_wrapper,
                   bool queryCont = true,
                   bool edbCheck = true,
                   bool rewriteCliques = true,
                   const std::string& tgpath = "",
                   const std::string& typeProv = "NOPROV",
                   bool delProofs = true);

    void set_python_objects(nb::object edb_obj_, nb::object prog_obj_);
    std::unordered_map<std::string, int> create_model(size_t startStep = 0, size_t maxStep = ~0ul);
    TGWrapper* get_TG();  // Implementation in tg.cpp where TGWrapper is fully defined

    // For internal use
    EDBLayer* get_edb();
    ProgramWrapper* get_program();
    GBChase* get_chase();
};

#endif // GLOG_PYTHON_REASONER_H
