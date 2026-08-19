#include <monitor.h>
#include <config.h>

static uint32_t currentIterationCounter = 0;

bool printIfNthIteration() {
    bool shouldPrint = false;
    if (currentIterationCounter == Config::NTH_ITERATION_PRINT) {
        shouldPrint = true;
        currentIterationCounter = 0;   
    }
    currentIterationCounter++;
    return shouldPrint;
}