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
#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <cmath>
#include <vector>
#include "innovation.h"
#include "genome.h"
#include "species.h"
#include "organism.h"

#include <assert.h>

namespace NEAT {

    class Organism;
	class Species;

	// ---------------------------------------------  
	// POPULATION CLASS:
	//   A Population is a group of Organisms   
	//   including their species                        
	// ---------------------------------------------  
	class Population {
        class OrganismsBuffer {
            size_t _n;
            std::vector<Organism> _a;
            std::vector<Organism> _b;
            std::vector<Organism> *_curr;
        public:
            OrganismsBuffer(rng_t &rng, size_t n);

            size_t size();
            std::vector<Organism> &curr();
            void swap();
        } orgs;

        Population(rng_t &rng, int size);
	public:
        std::vector<Species*> species;  // Species in the Population. Note that the species should comprise all the genomes 

		// ******* Member variables used during reproduction *******
        PopulationInnovations innovations;

		int last_species;  //The highest species number

		// ******* Fitness Statistics *******
		double mean_fitness;
		double variance;
		double standard_deviation;

		int winnergen; //An integer that when above zero tells when the first winner appeared

		// ******* When do we need to delta code? *******
		double highest_fitness;  //Stagnation detector
		int highest_last_changed; //If too high, leads to delta coding

        size_t size() {return orgs.size();}
        Organism *get(size_t i) {return &orgs.curr()[i];}

		// Separate the Organisms into species
		bool speciate();

		// Print Population to a file specified by a string 
		bool print_to_file(std::ostream& outFile);

		// Print Population to a file in speciated order with comments separating each species
		bool print_to_file_by_species(std::ostream& outFile);
		bool print_to_file_by_species(char *filename);

		// Prints the champions of each species to files starting with directory_prefix
		// The file name are as follows: [prefix]g[generation_num]cs[species_num]
		// Thus, they can be indexed by generation or species
		bool print_species_champs_tofiles(char *directory_prefix,int generation);

		// Run verify on all Genomes in this Population (Debugging)
		void verify();

		// Turnover the population to a new generation using fitness 
		// The generation argument is the next generation
		bool epoch(int generation);

		// *** Real-time methods *** 

		// Places the organisms in species in order from best to worst fitness 
		bool rank_within_species();

		// Construct off of a single spawning Genome 
		Population(rng_t &rng, Genome *g, int size);

		// Construct off of a single spawning Genome without mutation
		Population(rng_t &rng, Genome *g, int size, float power);
		
		// It can delete a Population in two ways:
		//    -delete by killing off the species
		//    -delete by killing off the organisms themselves (if not speciated)
		// It does the latter if it sees the species list is empty
		~Population();

    private:
		bool clone(Genome *g, float power);
		bool spawn(Genome *g);
	};

} // namespace NEAT

#endif
