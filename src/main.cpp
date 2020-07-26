#include <iostream>
#include "accneat.h"

using namespace NEAT;
using namespace std;

int main(int argc, char *argv[]) {

    //Usage Example
    if(!accneat(false, 10, DEFAULT_RNG_SEED, 5000, DEFAULT_MAX_GENS, GeneticSearchType::COMPLEXIFY, "xor")) {
        cerr << "Something went wrong..." << endl;
    }

    return(0);
}

