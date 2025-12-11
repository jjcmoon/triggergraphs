#ifndef PROGRAM_H
#define PROGRAM_H

#include <nanobind/nanobind.h>
#include <memory>
#include <string>

#include <vlog/edb.h>
#include <vlog/concepts.h>

namespace nb = nanobind;

// Forward declaration
class EDBLayerWrapper;

class ProgramWrapper {
private:
    nb::object edb_obj;
    EDBLayer* edb;

public:
    std::shared_ptr<Program> program;

    ProgramWrapper(EDBLayerWrapper& edb_wrapper);
    ProgramWrapper(const ProgramWrapper& other);  // Copy constructor

    void set_edb_object(nb::object obj);
    void load_from_file(const std::string& path);
    size_t get_n_rules();
    std::string get_rule(size_t ruleIdx);
    size_t add_rule(const std::string& rule_str);
    std::string get_predicate_name(size_t predId);

    // For internal use by other types
    EDBLayer* get_edb();
    Program* get_program();
};

#endif // PROGRAM_H
