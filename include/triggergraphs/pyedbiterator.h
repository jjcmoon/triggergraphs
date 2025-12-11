#ifndef _PYEDBITR_H
#define _PYEDBITR_H

#include <nanobind/nanobind.h>

#include <vlog/column.h>
#include <vlog/edbtable.h>
#include <vlog/edbiterator.h>
#include <vlog/segment.h>

namespace nb = nanobind;

class PyEDBIterator : public EDBIterator
{
private:
    const PredId_t predid;
    nb::object obj;
    EDBLayer *layer;

public:
    PyEDBIterator(PredId_t predid, nb::object obj, EDBLayer *layer);

    bool hasNext();

    void next();

    Term_t getElementAt(const uint8_t p);

    void clear();

    PredId_t getPredicateID() {
        return predid;
    }

    void skipDuplicatedFirstColumn() {
        throw 10;
    }

    ~PyEDBIterator();
};

#endif
