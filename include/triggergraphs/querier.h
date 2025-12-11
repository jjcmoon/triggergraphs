#ifndef GLOG_PYTHON_QUERIER_H
#define GLOG_PYTHON_QUERIER_H

#include <nanobind/nanobind.h>
#include <memory>
#include <string>
#include <vector>

#include <glog/gbquerier.h>

namespace nb = nanobind;

// Forward declarations
class TGWrapper;
class TupleSet;

class QuerierWrapper {
private:
    nb::object tg_obj;  // Keep TG alive
    TGWrapper* tg;
    std::shared_ptr<GBQuerier> q;

public:
    QuerierWrapper(TGWrapper& tg_ref);

    void set_tg_object(nb::object obj);
    std::string get_derivation_tree(size_t nodeId, size_t factId);
    std::string get_derivation_tree_in_TupleSet(TupleSet& ts, size_t factId);
    std::vector<std::string> get_list_predicates();
    std::string get_node_details_predicate(const std::string& predName);
    std::string get_facts_in_TG_node(size_t nodeId);
    std::vector<std::string> get_fact_in_TupleSet(TupleSet& ts, size_t factId);
    nb::list get_facts_coordinates_with_predicate(const std::string& predName);
    nb::list get_leaves(size_t nodeId, size_t factId);
    nb::dict get_all_facts();
    std::string get_predicate_name(size_t predicateId);
    std::string get_term_name(size_t termId);
};

#endif // GLOG_PYTHON_QUERIER_H
