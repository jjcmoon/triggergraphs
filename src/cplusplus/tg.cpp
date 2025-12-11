/*
 * Copyright 2021 Jacopo Urbani
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

#include <triggergraphs/tg.h>
#include <triggergraphs/reasoner.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <vlog/exporter.h>

namespace nb = nanobind;


TGWrapper::TGWrapper(ReasonerWrapper& reasoner_ref) {
    sn = reasoner_ref.sn;
    reasoner = &reasoner_ref;
    g = &(sn->getGBGraph());
}

void TGWrapper::set_reasoner_object(nb::object obj) {
    reasoner_obj = obj;
}

void TGWrapper::add_node(uint32_t predid, size_t step,
                         const std::vector<std::vector<std::string>>& facts) {
    if (!facts.empty()) {
        auto facts_copy = facts;  // Copy needed because GBGraph::addNode expects non-const reference
        g->addNode(predid, step, facts_copy);
    }
}

size_t TGWrapper::get_n_nodes() {
    return g->getNNodes();
}

size_t TGWrapper::get_n_edges() {
    return g->getNEdges();
}

size_t TGWrapper::get_n_facts() {
    return g->getNFacts();
}

size_t TGWrapper::get_node_size(size_t nodeId) {
    return g->getNodeSize(nodeId);
}

void TGWrapper::dump_files(const std::string& path) {
    Exporter exp(sn);
    exp.storeOnFiles(path.c_str(), 1, 0, false);
}

// Implementation of ReasonerWrapper::get_TG (needs to be here where TGWrapper is fully defined)
TGWrapper* ReasonerWrapper::get_TG() {
    return new TGWrapper(*this);
}

void bind_tg(nb::module_ &m) {
    nb::class_<TGWrapper>(m, "TG", "Trigger Graph")
        .def(nb::init<ReasonerWrapper&>(),
             nb::arg("reasoner"),
             nb::keep_alive<1, 2>(),  // Keep Reasoner alive
             "Create TG from a Reasoner")
        .def("add_node", &TGWrapper::add_node,
             nb::arg("predid"),
             nb::arg("step"),
             nb::arg("facts"),
             "Add a node with provided facts")
        .def("get_n_nodes", &TGWrapper::get_n_nodes,
             "Get number of nodes in the TG")
        .def("get_n_edges", &TGWrapper::get_n_edges,
             "Get number of edges in the TG")
        .def("get_n_facts", &TGWrapper::get_n_facts,
             "Get number of facts in the TG")
        .def("get_node_size", &TGWrapper::get_node_size,
             nb::arg("nodeId"),
             "Get number of facts stored in a node")
        .def("dump_files", &TGWrapper::dump_files,
             nb::arg("path") = "",
             "Store the content of the TG into files");
}
