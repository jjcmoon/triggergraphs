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

#include <triggergraphs/program.h>
#include <triggergraphs/edblayer.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/shared_ptr.h>

#include <kognac/utils.h>
#include <kognac/logs.h>

namespace nb = nanobind;


ProgramWrapper::ProgramWrapper(EDBLayerWrapper& edb_wrapper) {
    edb = edb_wrapper.get_edb();
    program = std::make_shared<Program>(edb);
}

ProgramWrapper::ProgramWrapper(const ProgramWrapper& other) {
    edb = other.edb;
    edb_obj = other.edb_obj;
    // Note: program is intentionally not copied here, it will be set by caller
    program = nullptr;
}

void ProgramWrapper::set_edb_object(nb::object obj) {
    edb_obj = obj;
}

void ProgramWrapper::load_from_file(const std::string& path) {
    if (!Utils::exists(path)) {
        LOG(ERRORL) << "File " << path << " does not exist";
        throw std::runtime_error("File " + path + " does not exist");
    }
    program->readFromFile(path);
}

size_t ProgramWrapper::get_n_rules() {
    return program->getNRules();
}

std::string ProgramWrapper::get_rule(size_t ruleIdx) {
    auto rule = program->getRule(ruleIdx);
    return rule.toprettystring(program.get(), edb);
}

size_t ProgramWrapper::add_rule(const std::string& rule_str) {
    auto ruleId = program->getNRules();
    auto out = program->parseRule(rule_str, false);
    if (!out.empty()) {
        throw std::runtime_error(out);
    }
    return ruleId;
}

std::string ProgramWrapper::get_predicate_name(size_t predId) {
    return program->getPredicateName(predId);
}

EDBLayer* ProgramWrapper::get_edb() {
    return edb;
}

Program* ProgramWrapper::get_program() {
    return program.get();
}

void bind_program(nb::module_ &m) {
    nb::class_<ProgramWrapper>(m, "Program", "Program")
        .def(nb::init<EDBLayerWrapper&>(),
             nb::arg("edb"),
             nb::keep_alive<1, 2>())
        .def("load_from_file", &ProgramWrapper::load_from_file,
             nb::arg("path"),
             "Load rules from file")
        .def("get_n_rules", &ProgramWrapper::get_n_rules,
             "Return n rules")
        .def("get_rule", &ProgramWrapper::get_rule,
             nb::arg("ruleIdx"),
             "Get rules")
        .def("add_rule", &ProgramWrapper::add_rule,
             nb::arg("rule"),
             "Add a rule")
        .def("get_predicate_name", &ProgramWrapper::get_predicate_name,
             nb::arg("predId"),
             "Get name predicate");
}
