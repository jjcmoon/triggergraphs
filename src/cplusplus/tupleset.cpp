/*
 * Copyright 2025 Jacopo Urbani, Jaron Maene
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

#include <glog-python/tupleset.h>

#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;


TupleSet::TupleSet()
    : nodeId(0),
      ruleIdx(0),
      step(0),
      predId(0) {}

size_t TupleSet::get_n_facts() const {
    if (data) {
        return data->getNRows();
    }
    return 0;
}

void bind_tupleset(nb::module_ &m) {
    nb::class_<TupleSet>(m, "TupleSet", "TupleSet")
        .def(nb::init<>())
        .def("get_n_facts",
             &TupleSet::get_n_facts,
             "Get number of facts");
}
