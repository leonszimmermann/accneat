#include "std.h" // Must be included first. Precompiled header with standard library includes.
#include "accneat.h"
#include "staticexperiment.h"
#include "main.h"

using namespace NEAT;
using namespace std;

static struct XorInit {
    XorInit() {
        create_static_experiment("xor", [] () {
                // const real_t T = 1.0;
                // const real_t F = 0.0;
                // const real_t weight = 1.0;

                // vector<Test> tests = {
                //     {{
                //             {{F, F}, {F}, weight},
                //     }},
                //     {{
                //             {{F, T}, {T}, weight},
                //     }},
                //     {{
                //             {{T, F}, {T}, weight},
                //     }},
                //     {{
                //             {{T, T}, {F}, weight}
                //     }}
                // };
            
                return GetDataFromCSV("test.csv");
            });
    }
} init;
