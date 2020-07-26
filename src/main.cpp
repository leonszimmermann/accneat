#include <iostream>
#include "accneat.h"

using namespace NEAT;
using namespace std;

int main(int argc, char *argv[]) {

    //Usage Example
    if(!accneat(10, 5000, GeneticSearchType::COMPLEXIFY, "xor", DEFAULT_RNG_SEED, DEFAULT_MAX_GENS, true)) {
        cerr << "Something went wrong..." << endl;
    }

    return(0);
}

