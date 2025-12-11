#include <triggergraphs/pyedbtable.h>
#include <triggergraphs/pyedbiterator.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <vlog/inmemory/inmemorytable.h>
#include <vlog/edb.h>
#include <vlog/chasemgmt.h>

namespace nb = nanobind;


PyTable::PyTable(PredId_t predid,
        std::string predname,
        EDBLayer *layer,
        nb::object obj)
    : predid(predid), layer(layer), obj(std::move(obj))
{
    nb::object mod = nb::module_::import_("pyterm");

    this->termClass = mod.attr("PyTerm");
}

uint8_t PyTable::getArity() const
{
    return nb::cast<uint8_t>(obj.attr("arity")());
}

void PyTable::query(QSQQuery *query, TupleTable *outputTable,
        std::vector<uint8_t> *posToFilter,
        std::vector<Term_t> *valuesToFilter)
{
    LOG(ERRORL) << "Method not implemented";
    throw 10;
}

bool PyTable::isEmpty(const Literal &query, std::vector<uint8_t> *posToFilter,
        std::vector<Term_t> *valuesToFilter)
{
    LOG(ERRORL) << "Method not implemented";
    throw 10;
}

size_t PyTable::getCardinalityColumn(const Literal &query, uint8_t posColumn)
{
    LOG(ERRORL) << "Method not implemented";
    throw 10;
}

size_t PyTable::estimateCardinality(const Literal &query)
{
    return getCardinality(query);
}

size_t PyTable::getCardinality(const Literal &query)
{
    nb::object pQuery = convertLiteralIntoPyTuple(query);
    nb::object resp = obj.attr("get_cardinality")(pQuery);
    return nb::cast<uint64_t>(resp);
}

EDBIterator *PyTable::getIterator(const Literal &query)
{
    const std::vector<uint8_t> fields;
    return getSortedIterator(query, fields);
}

EDBIterator *PyTable::getSortedIterator(const Literal &query,
        const std::vector<uint8_t> &fields)
{
    EDBIterator *itr = nullptr;
    nb::object pQuery;
    
    pQuery = convertLiteralIntoPyTuple(query);
    
    nb::object resp = obj.attr("get_iterator")(pQuery);
    
    itr = new PyEDBIterator(predid, resp, layer);

    if (itr == nullptr) {
        return new InmemoryIterator(nullptr, predid, fields);
    }

    const auto arity = getArity();
    //Create a segment and return an inmemory segment
    SegmentInserter ins(arity);
    std::unique_ptr<Term_t[]> row = std::unique_ptr<Term_t[]>(new Term_t[arity]);
    while (itr->hasNext()) {
        itr->next();
        for(size_t i = 0; i < arity; ++i) {
            row[i] = itr->getElementAt(i);
        }
        ins.addRow(row.get());
    }
    delete itr;
    
    auto seg = ins.getSegment();
    if (!seg->isEmpty()) {
        auto sortedSeg = seg->sortBy(&fields);
        return new InmemoryIterator(sortedSeg, predid, fields);
    } else {
        return new InmemoryIterator(NULL, predid, fields);
    }
}

bool PyTable::acceptQueriesWithFreeVariables()
{
    return nb::cast<bool>(obj.attr("can_accept_queries_free_variables")());
}

bool PyTable::getDictNumber(const char *text, const size_t sizeText,
        uint64_t &id)
{
    LOG(ERRORL) << "getDictNumber: This method should never be invoked";
    throw 10;
}

bool PyTable::getDictText(const uint64_t id, char *text)
{
    LOG(ERRORL) << "getDictText (1): This method should never be invoked";
    throw 10;
}

bool PyTable::getDictText(const uint64_t id, std::string &text)
{
    LOG(ERRORL) << "getDictText (2): This method should never be invoked";
    throw 10;
}

uint64_t PyTable::getNTerms()
{
    return nb::cast<uint64_t>(obj.attr("get_n_terms")());

}

void PyTable::releaseIterator(EDBIterator *itr)
{
    delete itr;
}

uint64_t PyTable::getSize()
{
    return nb::cast<uint64_t>(obj.attr("get_size")());

}

bool PyTable::isQueryAllowed(const Literal &query)
{
    nb::object pQuery = convertLiteralIntoPyTuple(query);
    nb::object resp = obj.attr("is_query_allowed")(pQuery);
    return nb::cast<bool>(resp);
}

bool PyTable::canChange()
{
    return nb::cast<bool>(obj.attr("can_change")());

}

nb::object PyTable::convertLiteralIntoPyTuple(const Literal &lit)
{
    auto arity = lit.getTupleSize();
    nb::list out;

    for(size_t i = 0; i < arity; ++i) {
        auto t = lit.getTermAtPos(i);
        nb::object arglist;

        if (t.isVariable()) {
            std::string sId = std::to_string(t.getId());
            arglist = nb::make_tuple(true, sId, 0L, false);
        } else {
            if (IS_NULLVALUE(t.getValue()))
            {
                arglist = nb::make_tuple(false, "", (long)t.getValue(), true);
            } else if (IS_UINT(t.getValue())) {
                auto intValue = GET_UINT(t.getValue());
                arglist = nb::make_tuple(false, std::to_string(intValue), (long)intValue, false);
            } else if (IS_FLOAT32(t.getValue())) {
                auto rawValue = t.getValue();
                auto floatValue = GET_FLOAT32(rawValue);
                arglist = nb::make_tuple(false, std::to_string(floatValue), floatValue, false);
            } else {
                //Get the textual term
                std::string text = layer->getDictText(t.getValue());
                arglist = nb::make_tuple(false, text, (long)t.getValue(), false);
            }
        }
        out.append(termClass(arglist));
    }
    // Convert list to tuple before returning
    return nb::tuple(out);
}


bool PyTable::areTermsEncoded()
{
    return false;
}
