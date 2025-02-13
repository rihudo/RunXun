#include "record_manager.hpp"
#include <stdio.h>
#include "logic_manager.hpp"

int main()
{
    LogicManager lm("lhood", 8899, {8895});
    lm.start();
    getchar();
}