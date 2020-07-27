/*
  Copyright 2001 The University of Texas at Austin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#pragma once

#include "unistd.h"
#include "neat.h"
#include "experiments/static/staticevaluator.h"
#include <vector>

#define DEFAULT_RNG_SEED 1
#define DEFAULT_MAX_GENS 10000

static std::vector<NEAT::Test> tests;

bool accneat(int num_runs, int pop_size, NEAT::GeneticSearchType search_type, const char* experiment_name, int rng_seed = DEFAULT_RNG_SEED, int maxgens = DEFAULT_MAX_GENS, bool force_delete = false);