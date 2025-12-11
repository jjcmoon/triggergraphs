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

#include <glog-python/edblayer.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/shared_ptr.h>

namespace nb = nanobind;

EDBLayerWrapper::EDBLayerWrapper(const std::string& path) {
    if (!path.empty()) {
        conf = new EDBConf(path);
        conf->setRootPath(Utils::parentDir(path));
        edb = new EDBLayer(*conf, false);
    } else {
        conf = new EDBConf("");
        conf->setRootPath(".");
        edb = new EDBLayer(*conf, false);
    }
}


EDBLayer* EDBLayerWrapper::get_edb() {
    return edb;
}

void EDBLayerWrapper::add_source(const std::string& predName, nb::object obj) {
    auto predId = edb->addEDBPredicate(predName);

    std::shared_ptr<EDBTable> ptr(
        new PyTable(predId, predName, edb, obj)
    );

    edb->addEDBTable(predId, "PYTHON", ptr);
}

void EDBLayerWrapper::add_csv_source(
    const std::string& predName,
    const std::vector<std::vector<std::string>>& rows)
{
    auto predId = edb->addEDBPredicate(predName);
    auto rows_copy = rows;
    edb->addInmemoryTable(predName, predId, rows_copy);
}

void EDBLayerWrapper::replace_facts_csv_source(
    const std::string& predName,
    const std::vector<std::vector<std::string>>& rows)
{
    auto rows_copy = rows;
    edb->replaceFactsInmemoryTable(predName, rows_copy);
}

nb::object EDBLayerWrapper::get_term_id(const std::string& term) {
    nTerm value;
    bool ok = edb->getDictNumber(term.c_str(), term.size(), value);
    return ok ? nb::cast(value) : nb::none();
}

uint64_t EDBLayerWrapper::get_n_terms() {
    return edb->getNTerms();
}

size_t EDBLayerWrapper::get_n_predicates() {
    return edb->getNPredicates();
}

std::vector<std::string> EDBLayerWrapper::get_predicates() {
    auto pids = edb->getAllEDBPredicates();
    std::vector<std::string> result;
    result.reserve(pids.size());

    for (auto pid : pids) {
        result.push_back(edb->getPredName(pid));
    }
    return result;
}

std::vector<std::vector<uint64_t>>
EDBLayerWrapper::get_facts(const std::string& predicateName)
{
    auto predId = edb->getPredID(predicateName);
    auto arity = edb->getPredArity(predId);

    VTuple predTuple(arity);
    for (int i = 0; i < arity; ++i) {
        predTuple.set(VTerm(i + 1, 0), i);
    }

    auto adorn = Predicate::calculateAdornment(predTuple);
    Predicate pred(predId, adorn, EDB, arity);
    Literal lit(pred, predTuple);

    auto itr = edb->getIterator(lit);

    std::vector<std::vector<uint64_t>> rows;

    while (itr->hasNext()) {
        itr->next();
        std::vector<uint64_t> tuple;
        tuple.reserve(arity);

        for (int j = 0; j < arity; ++j) {
            tuple.push_back(itr->getElementAt(j));
        }

        rows.push_back(std::move(tuple));
    }

    edb->releaseIterator(itr);
    return rows;
}

EDBLayerWrapper::~EDBLayerWrapper() {
    if (edb) {
        delete edb;
        edb = nullptr;
    }
    if (conf) {
        delete conf;
        conf = nullptr;
    }
}


void bind_edblayer(nb::module_& m) {
    nb::class_<EDBLayerWrapper>(m, "EDBLayer", "EDB Layer")
        .def(nb::init<>())
        .def(nb::init<const std::string&>(), nb::arg("confPath"))
        .def("add_source", &EDBLayerWrapper::add_source,
             nb::arg("predname"), nb::arg("obj"),
             "Add a new source associated to an EDB predicate")
        .def("add_csv_source", &EDBLayerWrapper::add_csv_source,
             nb::arg("predname"), nb::arg("rows"),
             "Add a new CSV source associated to an EDB predicate")
        .def("replace_facts_csv_source",
             &EDBLayerWrapper::replace_facts_csv_source,
             nb::arg("predname"), nb::arg("rows"),
             "Replace the content of a CSV source")
        .def("get_term_id", &EDBLayerWrapper::get_term_id,
             nb::arg("term"),
             "Get the numerical ID associated to a term")
        .def("get_n_terms", &EDBLayerWrapper::get_n_terms,
             "Get the number of terms")
        .def("get_n_predicates", &EDBLayerWrapper::get_n_predicates,
             "Get the number of predicates")
        .def("get_predicates", &EDBLayerWrapper::get_predicates,
             "Get list of all EDB predicates")
        .def("get_facts", &EDBLayerWrapper::get_facts,
             nb::arg("predicate_name"),
             "Get facts with a given EDB predicate");
}
