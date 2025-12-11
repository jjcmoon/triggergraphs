#ifndef TUPLESET_H
#define TUPLESET_H

#include <vector>

#include <glog/gbgraph.h>

class TupleSet {
public:
    std::shared_ptr<const TGSegment> data;
    size_t nodeId;
    size_t ruleIdx;
    size_t step;
    PredId_t predId;
    std::vector<std::vector<size_t>> nodes;

    TupleSet();

    size_t get_n_facts() const;
};

#endif // TUPLESET_H
