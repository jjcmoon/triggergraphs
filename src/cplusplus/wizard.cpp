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

#include <glog-python/wizard.h>
#include <glog-python/program.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/pair.h>

#include <vlog/wizard.h>
#include <vlog/concepts.h>

namespace nb = nanobind;


WizardWrapper::WizardWrapper() {}

nb::tuple WizardWrapper::rewrite_program(ProgramWrapper& program_wrapper, const std::string& query) {
    auto program = program_wrapper.program;

    Dictionary dv;
    auto lQuery = program->parseLiteral(query, dv);

    Wizard w;
    auto adornedProgram = w.getAdornedProgram(lQuery, *program.get());

    std::pair<PredId_t, PredId_t> ioPredIDs;
    auto newProgram = w.doMagic(lQuery, adornedProgram, ioPredIDs);

    auto newP = new ProgramWrapper(program_wrapper);
    newP->program = newProgram;

    // returns (new_program, input_pred_id, output_pred_id)
    return nb::make_tuple(newP, ioPredIDs.first, ioPredIDs.second);
}

void bind_wizard(nb::module_ &m) {
    nb::class_<WizardWrapper>(m, "Wizard", "Wizard for magic set rewriting")
        .def(nb::init<>(), "Create a new Wizard")
        .def("rewrite_program", &WizardWrapper::rewrite_program,
             nb::arg("program"),
             nb::arg("query"),
             "Rewrite a program with magic sets. Returns (new_program, input_pred_id, output_pred_id)");
}
