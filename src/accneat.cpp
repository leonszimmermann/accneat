#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "accneat.h"
#include "rng.h"
#include "experiments/experiment.h"
#include "util/util.h"

using namespace NEAT;
using namespace std;

bool accneat(int num_runs, int pop_size, NEAT::GeneticSearchType search_type, const char* experiment_name, int rng_seed, int maxgens, bool force_delete) {
    env->num_runs = num_runs;
    env->pop_size = pop_size;

    if(force_delete) {
        sh("rm -rf experiment_*");
    } else if(exists("experiment_1")) {
        error("Already exists: experiment_1.\nMove your experiment directories or use -f to delete them automatically.")
        return false;
    }

    env->search_type = search_type;
    if(env->search_type == GeneticSearchType::BLENDED) {
        env->mutate_delete_node_prob *= 0.1;
        env->mutate_delete_link_prob *= 0.1;
    }

    Experiment *exp = Experiment::get(experiment_name);
    if(exp == nullptr) {
        trap("No such experiment: " << experiment_name);
        return false;
    }

    rng_t rng{rng_seed};
    exp->run(rng, maxgens);

    return true;
}