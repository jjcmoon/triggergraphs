/*
 * Copyright 2021 Jacopo Urbani, Jaron Maene
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 **/

#include <triggergraphs/querier.h>
#include <triggergraphs/tg.h>
#include <triggergraphs/reasoner.h>
#include <triggergraphs/program.h>
#include <triggergraphs/tupleset.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/pair.h>

#include <glog/gbgraph.h>
#include <glog/gbchase.h>
#include <trident/utils/json.h>
#include <vlog/concepts.h>

#include <sstream>

namespace nb = nanobind;


QuerierWrapper::QuerierWrapper(TGWrapper& tg_ref) {
    tg = &tg_ref;
    q = tg->g->getQuerier();
}

void QuerierWrapper::set_tg_object(nb::object obj) {
    tg_obj = obj;
}

std::string QuerierWrapper::get_derivation_tree(size_t nodeId, size_t factId) {
    auto out = q->getDerivationTree(nodeId, factId);
    std::stringstream ssOut;
    JSON::write(ssOut, out);
    return ssOut.str();
}

std::string QuerierWrapper::get_derivation_tree_in_TupleSet(TupleSet& ts, size_t factId) {
    if (factId >= ts.data->getNRows()) {
        return "null";
    }
    auto& ie = ts.nodes[factId];
    DuplicateChecker checker;
    auto out = q->getDerivationTree(ts.data,
            ts.nodeId,
            factId,
            ts.predId,
            ts.ruleIdx,
            ts.step,
            ie,
            &checker);
    std::stringstream ssOut;
    JSON::write(ssOut, out);
    return ssOut.str();
}

std::vector<std::string> QuerierWrapper::get_list_predicates() {
    return q->getListPredicates();
}

std::string QuerierWrapper::get_node_details_predicate(const std::string& predName) {
    auto out = q->getNodeDetailsWithPredicate(predName);
    std::stringstream ssOut;
    JSON::write(ssOut, out);
    return ssOut.str();
}

std::string QuerierWrapper::get_facts_in_TG_node(size_t nodeId) {
    auto out = q->getNodeFacts(nodeId);
    std::stringstream ssOut;
    JSON::write(ssOut, out);
    return ssOut.str();
}

std::vector<std::string> QuerierWrapper::get_fact_in_TupleSet(TupleSet& ts, size_t factId) {
    std::vector<std::string> result;
    if (factId >= ts.data->getNRows()) {
        return result;
    }

    auto n_columns = ts.data->getNColumns();
    for(size_t i = 0; i < n_columns; ++i) {
        Term_t c = ts.data->getValueAtRow(factId, i);
        std::string sTerm = q->getTermText(c);
        result.push_back(sTerm);
    }
    return result;
}

nb::list QuerierWrapper::get_facts_coordinates_with_predicate(const std::string& predName) {
    nb::list out;
    auto pairs = q->getAllFactsPredicate(predName);
    auto card = pairs.second.size() / pairs.first.size();

    for(size_t i = 0; i < pairs.first.size(); ++i) {
        auto c = pairs.first[i];

        // Create tuple for the fact terms
        nb::list tuple_list;
        for(size_t j = 0; j < card; ++j) {
            auto term = pairs.second[i * card + j];
            tuple_list.append(term);
        }

        // Create coordinates tuple (nodeId, offset)
        nb::tuple coordinates = nb::make_tuple(c.first, c.second);

        // Add (tuple, coordinates) to output
        nb::tuple fact = nb::make_tuple(tuple_list, coordinates);
        out.append(fact);
    }
    return out;
}

nb::list QuerierWrapper::get_leaves(size_t nodeId, size_t factId) {
    std::vector<std::vector<Literal>> leaves;
    leaves.emplace_back();
    q->getLeavesInDerivationTree(nodeId, factId, leaves);

    nb::list outAll;
    for(auto& leafSet : leaves) {
        nb::list outObj;
        for(auto& leaf : leafSet) {
            nb::list tuple_list;
            tuple_list.append(leaf.getPredicate().getId());
            for(size_t j = 0; j < leaf.getTupleSize(); ++j) {
                auto t = leaf.getTermAtPos(j);
                tuple_list.append(t.getValue());
            }
            outObj.append(tuple_list);
        }
        outAll.append(outObj);
    }
    return outAll;
}

nb::dict QuerierWrapper::get_all_facts() {
    nb::dict out;
    auto facts = q->getAllFacts();

    for(auto& p : facts) {
        nb::list predFacts;
        for(auto& predFact : p.second) {
            nb::list tuple_list;
            for(auto& termFact : predFact) {
                tuple_list.append(termFact);
            }
            predFacts.append(tuple_list);
        }
        out[p.first.c_str()] = predFacts;
    }
    return out;
}

std::string QuerierWrapper::get_predicate_name(size_t predicateId) {
    return tg->reasoner->get_program()->program->getPredicateName(predicateId);
}

std::string QuerierWrapper::get_term_name(size_t termId) {
    return tg->reasoner->get_edb()->getDictText(termId);
}

void bind_querier(nb::module_ &m) {
    nb::class_<QuerierWrapper>(m, "Querier", "Querier for accessing TG facts and derivations")
        .def(nb::init<TGWrapper&>(),
             nb::arg("tg"),
             nb::keep_alive<1, 2>(),  // Keep TG alive
             "Create Querier from a TG")
        .def("get_derivation_tree", &QuerierWrapper::get_derivation_tree,
             nb::arg("nodeId"),
             nb::arg("factId"),
             "Get derivation tree of a fact")
        .def("get_derivation_tree_in_TupleSet", &QuerierWrapper::get_derivation_tree_in_TupleSet,
             nb::arg("tupleSet"),
             nb::arg("factId"),
             "Get derivation tree of a fact in a given TupleSet")
        .def("get_list_predicates", &QuerierWrapper::get_list_predicates,
             "Get list predicates stored in the TG")
        .def("get_node_details_predicate", &QuerierWrapper::get_node_details_predicate,
             nb::arg("predName"),
             "Get the nodes for a given predicate")
        .def("get_facts_in_TG_node", &QuerierWrapper::get_facts_in_TG_node,
             nb::arg("nodeId"),
             "Get the facts stored on a node in the TG")
        .def("get_fact_in_TupleSet", &QuerierWrapper::get_fact_in_TupleSet,
             nb::arg("tupleSet"),
             nb::arg("factId"),
             "Get the fact stored on a given TupleSet")
        .def("get_facts_coordinates_with_predicate", &QuerierWrapper::get_facts_coordinates_with_predicate,
             nb::arg("predName"),
             "Get all the facts with a given predicate")
        .def("get_leaves", &QuerierWrapper::get_leaves,
             nb::arg("nodeId"),
             nb::arg("factId"),
             "Get the leaves for a given fact")
        .def("get_all_facts", &QuerierWrapper::get_all_facts,
             "Get all the facts in the TG")
        .def("get_predicate_name", &QuerierWrapper::get_predicate_name,
             nb::arg("predicateId"),
             "Get predicate name")
        .def("get_term_name", &QuerierWrapper::get_term_name,
             nb::arg("termId"),
             "Get textual name of a term");
}

