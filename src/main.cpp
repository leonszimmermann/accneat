#include <iostream>
#include "accneat.h"

using namespace NEAT;
using namespace std;

int main(int argc, char *argv[]) {

    //Usage Example
    const real_t T = 1.0;
    const real_t F = 0.0;
    const real_t weight = 1.0;
    
    tests = {
        {{
                {{F, F}, {F}, weight},
        }},
        {{
                {{F, T}, {T}, weight},
        }},
        {{
                {{T, F}, {T}, weight},
        }},
        {{
                {{T, T}, {F}, weight}
        }}
    };

    if(!accneat(10, 5000, GeneticSearchType::COMPLEXIFY, "xor", DEFAULT_RNG_SEED, DEFAULT_MAX_GENS, true)) {
        cerr << "Something went wrong..." << endl;
    }

    return(0);
}

