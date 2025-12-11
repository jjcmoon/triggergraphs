#ifndef EDBLAYER_WRAPPER_H
#define EDBLAYER_WRAPPER_H

#include <string>
#include <vector>
#include <memory>

#include <vlog/edb.h>
#include <glog-python/pyedbtable.h>
#include <kognac/utils.h>

namespace nanobind {
    class object;
    class module_;
}
namespace nb = nanobind;

class EDBLayerWrapper {
private:
    EDBConf* conf;
    EDBLayer* edb;

public:
    explicit EDBLayerWrapper(const std::string& path = "");
    ~EDBLayerWrapper();

    EDBLayer* get_edb();

    void add_source(const std::string& predName, nb::object obj);

    void add_csv_source(
        const std::string& predName,
        const std::vector<std::vector<std::string>>& rows);

    void replace_facts_csv_source(
        const std::string& predName,
        const std::vector<std::vector<std::string>>& rows);

    nb::object get_term_id(const std::string& term);

    uint64_t get_n_terms();
    size_t get_n_predicates();
    std::vector<std::string> get_predicates();

    std::vector<std::vector<uint64_t>> get_facts(const std::string& predicateName);
};

void bind_edblayer(nb::module_& m);

#endif // EDBLAYER_WRAPPER_H
