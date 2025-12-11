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

#include <glog-python/reasoner.h>
#include <glog-python/edblayer.h>
#include <glog-python/program.h>
#include <glog-python/tg.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/vector.h>

#include <vlog/reasoner.h>
#include <kognac/logs.h>
#include <kognac/utils.h>
#include <trident/utils/json.h>

namespace nb = nanobind;


ReasonerWrapper::ReasonerWrapper(const std::string& typeChase,
                               EDBLayerWrapper& edb_wrapper,
                               ProgramWrapper& prog_wrapper,
                               bool queryCont,
                               bool edbCheck,
                               bool rewriteCliques,
                               const std::string& tgpath,
                               const std::string& typeProv,
                               bool delProofs) {

    edb = edb_wrapper.get_edb();
    program = &prog_wrapper;

    GBChaseAlgorithm tc = GBChaseAlgorithm::GBCHASE;
    std::string tp = typeProv;

    if (typeChase == "tgchase_static") {
        tc = GBChaseAlgorithm::TGCHASE_STATIC;
    } else if (typeChase == "tgchase") {
        if (tp == "NOPROV") {
            tc = GBChaseAlgorithm::GBCHASE;
        } else if (tp == "NODEPROV") {
            tc = GBChaseAlgorithm::TGCHASE_DYNAMIC;
        } else if (tp == "FULLPROV") {
            tc = GBChaseAlgorithm::TGCHASE_DYNAMIC_FULLPROV;
        } else {
            throw std::invalid_argument("Invalid typeProv: " + tp);
        }
    } else if (typeChase == "probtgchase") {
        tc = GBChaseAlgorithm::PROBTGCHASE;
        tp = "FULLPROV";
    }

    if (tp == "FULLPROV") {
        rewriteCliques = false;
    }

    LOG(INFOL) << "Reasoner created with parameters "
        "queryCont=" << queryCont << " "
        "edbCheck=" << edbCheck << " "
        "rewriteCliques=" << rewriteCliques << " "
        "optDelProofs=" << delProofs << " "
        "tgpath=" << tgpath << " "
        "typeProv=" << typeProv;

    if (tc != GBChaseAlgorithm::PROBTGCHASE) {
        sn = Reasoner::getGBChase(*edb, program->program.get(), tc,
                queryCont, edbCheck, rewriteCliques, tgpath);
    } else {
        sn = Reasoner::getProbTGChase(*edb, program->program.get(), delProofs);
    }
}

void ReasonerWrapper::set_python_objects(nb::object edb_obj_, nb::object prog_obj_) {
    edb_obj = edb_obj_;
    program_obj = prog_obj_;
}

std::string ReasonerWrapper::create_model(size_t startStep, size_t maxStep) {
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    sn->prepareRun(startStep, maxStep);
    sn->run();
    std::chrono::duration<double> secMat = std::chrono::system_clock::now() - start;

    LOG(INFOL) << "Runtime materialization = " << secMat.count() * 1000 << " milliseconds";
    LOG(INFOL) << "Derived tuples = " << sn->getNDerivedFacts();
    LOG(INFOL) << "N. nodes = " << sn->getNnodes();
    LOG(INFOL) << "N. edges = " << sn->getNedges();
    LOG(INFOL) << "Triggers = " << sn->getNTriggers();

    std::stringstream ssOut;
    JSON out;
    out.put("n_nodes", sn->getNnodes());
    out.put("n_edges", sn->getNedges());
    out.put("n_triggers", sn->getNTriggers());
    out.put("n_derivations", sn->getNDerivedFacts());
    out.put("steps", sn->getNSteps());
    out.put("max_mem_mb", Utils::get_max_mem());
    out.put("runtime_ms", secMat.count() * 1000);
    JSON::write(ssOut, out);
    return ssOut.str();
}

EDBLayer* ReasonerWrapper::get_edb() {
    return edb;
}

ProgramWrapper* ReasonerWrapper::get_program() {
    return program;
}

GBChase* ReasonerWrapper::get_chase() {
    return sn.get();
}

void bind_reasoner(nb::module_ &m) {
    nb::class_<ReasonerWrapper>(m, "Reasoner", "Reasoner")
        .def(nb::init<const std::string&, EDBLayerWrapper&, ProgramWrapper&,
                     bool, bool, bool, const std::string&, const std::string&, bool>(),
             nb::arg("typeChase"),
             nb::arg("edb"),
             nb::arg("program"),
             nb::arg("queryCont") = true,
             nb::arg("edbCheck") = true,
             nb::arg("rewriteCliques") = true,
             nb::arg("tgpath") = "",
             nb::arg("typeProv") = "NOPROV",
             nb::arg("delProofs") = true,
             nb::keep_alive<1, 2>(),  // Keep EDBLayer alive
             nb::keep_alive<1, 3>())  // Keep Program alive
        .def("create_model", &ReasonerWrapper::create_model,
             nb::arg("startStep") = 0,
             nb::arg("maxStep") = ~0ul,
             "Create model and return statistics as JSON string")
        .def("get_TG", &ReasonerWrapper::get_TG,
             nb::rv_policy::take_ownership,
             "Get the triggered graph");
}
