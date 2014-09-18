#include "seq_experiment.h"

#include "genome.h"
#include "network.h"
#include "organism.h"
#include "population.h"
#include "timer.h"

#include <assert.h>
#include <omp.h>

#include <iostream>
#include <sstream>

using namespace NEAT;
using namespace std;

#define NACTIVATES_PER_INPUT 10

static void init_env() {
    const bool DELETE_NODES = true;
    const bool DELETE_LINKS = true;
    const bool DEMES = true;

    if(DEMES) {
        NEAT::population_type = PopulationType::DEMES;
    } else {
        NEAT::compat_threshold = 10.0;
    }

    if(DELETE_NODES) {
        NEAT::mutate_delete_node_prob = 0.001;
    }

    if(DELETE_LINKS) {
        NEAT::mutate_delete_link_prob = 0.01;

        NEAT::mutate_toggle_enable_prob = 0.0;
        NEAT::mutate_gene_reenable_prob = 0.0;
    }
}

struct Step {
    vector<real_t> input;
    vector<real_t> output;
    real_t weight;

    real_t err(Network *net,
               float **details_act,
               float **details_err) {
        real_t result = 0.0;

        for(size_t i = 0; i < output.size(); i++) {
            real_t diff = net->get_output(i) - output[i];
            real_t err = diff * diff;

            if(err < (0.05 * 0.05)) {
                err = 0.0;
            }

            result += err;

            **details_act = net->get_output(i);
            **details_err = err;

            (*details_act)++;
            (*details_err)++;
        }

        return result * weight;
    }
};

struct Test {
    vector<Step> steps;
    Test(const vector<Step> &steps_) : steps(steps_) {
        // Insert 1.0 for bias sensor
        for(auto &step: steps) {
            step.input.insert(step.input.begin(), 1.0f);
        }
    }
};

const float S = 1.0; // Signal
const float Q = 1.0; // Query
const float _ = 0.0; // Null

const float A = 0.0;
const float B = 1.0;

const real_t weight_seq = 4;
const real_t weight_delay = 25;
const real_t weight_query = 55;

vector<Test> tests = {
    Test({
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {A, A, A}, weight_query}
        }),
    Test({
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {A, A, B}, weight_query}
        }),
    Test({
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {A, B, A}, weight_query}
        }),
    Test({
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {A, B, B}, weight_query}
        }),
    Test({
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {B, A, A}, weight_query}
        }),
    Test({
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {B, A, B}, weight_query}
        }),
    Test({
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, A, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {B, B, A}, weight_query}
        }),
    Test({
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_seq},
            {{S, _, _, B, _}, {_, _, _}, weight_seq},
            {{_, _, _, _, _}, {_, _, _}, weight_delay},
            {{_, _, Q, _, _}, {B, B, B}, weight_query}
        }),
};

const size_t nsteps = []() {
    size_t n = 0;
    for(auto &test: tests) n += test.steps.size();
    return n;
}();
const size_t nouts = []() {
    return nsteps * tests[0].steps[0].output.size();
}();
const real_t max_err = []() {
    real_t total = 0.0;
    for(auto &t: tests) for(auto &s: t.steps) total += s.weight;
    return total;
}();

static float *details_act;
static float *details_err;

static real_t score(real_t errorsum) {
    return 1.0 - errorsum/max_err;
};

static void print(Population *pop, int gen) {
    char filename[1024];
    sprintf(filename, "gen_%d", gen);
    ofstream out(filename);
    pop->write(out);
}

static void evaluate(NEAT::Population *pop);

//Perform evolution on SEQ_EXPERIMENT, for gens generations
void seq_experiment(rng_t &rng, int gens) {
    init_env();

    Genome *start_genome;

    details_act = new float[NEAT::pop_size * nouts];
    details_err = new float[NEAT::pop_size * nouts];

    cout<<"START SEQ_EXPERIMENT TEST"<<endl;

    start_genome = Genome::create_seed_genome(rng,
                                              1,
                                              tests[0].steps[0].input.size() - 1,
                                              tests[0].steps[0].output.size(),
                                              3);

    int nsuccesses = 0;

    for(int expcount = 0; expcount < NEAT::num_runs; expcount++) {
        //Spawn the Population
        Population *pop = Population::create(rng, start_genome,NEAT::pop_size);
      
        bool success = false;
        int gen;
        for(gen = 1; !success && (gen <= gens); gen++) {
            cout << "Epoch " << gen << endl;	

            static Timer timer("epoch");
            timer.start();

            if(gen != 1) {
                pop->next_generation();
            }

            evaluate(pop);

            if(pop->get_fittest().fitness >= 0.9999999) {
                success = true;
                nsuccesses++;
            }

            timer.stop();
            Timer::report();

            //Don't print on success because we'll exit the loop and print then.
            if(!success && (gen % NEAT::print_every == 0))
                print(pop, gen);
        }

        print(pop, gen - 1);

        delete pop;
    }

    delete start_genome;
    
    delete [] details_err;
    delete [] details_act;

    cout << "Failures: " << (NEAT::num_runs - nsuccesses) << " out of " << NEAT::num_runs << " runs" << endl;
}

void evaluate(Population *pop) {

    auto eval_org = [] (Organism &org) {

        float *details_act = ::details_act + org.population_index * nouts;
        float *details_err = ::details_err + org.population_index * nouts;
        Network *net = &org.net;
        real_t errorsum = 0.0;

        for(auto &test: tests) {
            for(auto &step: test.steps) {
                net->load_sensors(step.input);
                for(size_t i = 0; i < NACTIVATES_PER_INPUT; i++) {
                    net->activate();
                }
                errorsum += step.err( net, &details_act, &details_err );
            }

            net->flush();
        }
 
        org.fitness = score(errorsum);
        org.error = errorsum;
    };

    Organism &best = pop->get_fittest();

    if( pop->evaluate(eval_org) ) {

        float *best_act = details_act + best.population_index * nouts;
        float *best_err = details_err + best.population_index * nouts;
        
        for(auto &test: tests) {
            for(auto &step: test.steps) {
                for(size_t i = 0; i < step.output.size(); i++) {
                    printf("%f (%f) ", *best_act++, *best_err++);
                }
                printf("\n");
            }
            printf("---\n");
        }
    }

    cout << "best: fitness=" << best.fitness << ", nnodes=" << best.genome.nodes.size() << ", nlinks=" << best.genome.links.size() << endl;

}
