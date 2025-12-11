#ifndef GLOG_PYTHON_TG_H
#define GLOG_PYTHON_TG_H

#include <nanobind/nanobind.h>
#include <memory>
#include <string>
#include <vector>

#include <glog/gbgraph.h>
#include <glog/gbchase.h>

namespace nb = nanobind;

// Forward declaration
class ReasonerWrapper;

class TGWrapper {
private:
    nb::object reasoner_obj;  // Keep Reasoner alive
    std::shared_ptr<GBChase> sn;

public:
    GBGraph* g;  // Public for Querier access
    ReasonerWrapper* reasoner;  // Public for Querier access

    TGWrapper(ReasonerWrapper& reasoner_ref);

    void set_reasoner_object(nb::object obj);
    void add_node(uint32_t predid, size_t step,
                  const std::vector<std::vector<std::string>>& facts);
    size_t get_n_nodes();
    size_t get_n_edges();
    size_t get_n_facts();
    size_t get_node_size(size_t nodeId);
    void dump_files(const std::string& path = "");
};

#endif // GLOG_PYTHON_TG_H
