#include <glog-python/pyedbiterator.h>
#include <nanobind/stl/string.h>
#include <nanobind/operators.h> 

namespace nb = nanobind;

PyEDBIterator::PyEDBIterator(PredId_t predid, nb::object obj, EDBLayer *layer) 
    : predid(predid), obj(std::move(obj)), layer(layer) {}

bool PyEDBIterator::hasNext() {
    return nb::cast<bool>(obj.attr("has_next")());
}

void PyEDBIterator::next() {
    obj.attr("next")();
}

Term_t PyEDBIterator::getElementAt(const uint8_t p) {
    nb::object sTerm = obj.attr("get_term_at_pos")(p);
    std::string term_str = nb::cast<std::string>(sTerm);
    LOG(TRACEL) << "Retrieved " << term_str << " from the python layer";
    Term_t id = 0;
    bool resp = layer->getOrAddDictNumber(term_str.data(), term_str.size(), id);
    LOG(TRACEL) << "Term found? " << resp << " ID=" << id;
    assert(resp);
    return id;
}

void PyEDBIterator::clear() {
    throw 10;
}

PyEDBIterator::~PyEDBIterator() {}