#ifndef _PYEDBTABLE_H
#define _PYEDBTABLE_H

#include <nanobind/nanobind.h>

#include <vlog/column.h>
#include <vlog/edbtable.h>
#include <vlog/edbiterator.h>
#include <vlog/segment.h>

#include <unordered_map>


namespace nb = nanobind;


class PyTable : public EDBTable
{
    private:
        const PredId_t predid;
        EDBLayer *layer;

        nb::object obj;
        nb::object termClass;
        nb::object convertLiteralIntoPyTuple(const Literal &lit);

    public:
        PyTable(PredId_t predid,
                std::string predname,
                EDBLayer *layer,
                nb::object obj);

        uint8_t getArity() const override;

        void query(QSQQuery *query, TupleTable *outputTable,
                std::vector<uint8_t> *posToFilter,
                std::vector<Term_t> *valuesToFilter) override;

        size_t estimateCardinality(const Literal &query) override;

        size_t getCardinality(const Literal &query) override;

        size_t getCardinalityColumn(const Literal &query, uint8_t posColumn) override;

        bool isEmpty(const Literal &query, std::vector<uint8_t> *posToFilter,
                std::vector<Term_t> *valuesToFilter) override;

        EDBIterator *getIterator(const Literal &query) override;

        EDBIterator *getSortedIterator(const Literal &query,
                const std::vector<uint8_t> &fields) override;

        bool getDictNumber(const char *text, const size_t sizeText,
                uint64_t &id) override;

        bool getDictText(const uint64_t id, char *text) override;

        bool getDictText(const uint64_t id, std::string &text) override;

        uint64_t getNTerms() override;

        void releaseIterator(EDBIterator *itr) override;

        bool acceptQueriesWithFreeVariables() override;

        uint64_t getSize() override;

        bool canChange() override;

        bool areTermsEncoded() override;

        bool isQueryAllowed(const Literal &query) override;

        ~PyTable() = default;
};

#endif