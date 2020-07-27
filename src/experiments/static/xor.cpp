#include "std.h" // Must be included first. Precompiled header with standard library includes.
#include "accneat.h"
#include "staticexperiment.h"

using namespace NEAT;
using namespace std;

static struct XorInit {
    XorInit() {
        create_static_experiment("xor", [] () {return tests;});
    }
} init;
