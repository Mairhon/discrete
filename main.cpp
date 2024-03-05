#include <iostream>
#include <fstream>
#include "truths.hpp"

auto main() -> int {

    // usage demo
    // operators: A V ! --> <=>
    // Due to the C++ grammar anaylizing rule,
    // dividing formulas with ' ' and '()' is recommended.

    // simple usage
    {
        ATOM(P);
        ATOM(Q);

        P.assign(T);
        Q.assign(F);

        std::cout << "P V Q : " << (P V Q) << std::endl;
    }

    // TruthTable
    GenTruthTable1P(P, P V (!P));
    std::cout << std::endl;

    GenTruthTable2P(P, Q, (!(P A Q))-->(P<=>Q));
    std::cout << std::endl;

    GenTruthTable3P(P, Q, R, ((!(P-->Q)) V (!R)));
    std::cout << std::endl;

    GenTruthTable4P(P, Q, R, S, (P V Q) A (R --> S));
    std::cout << std::endl;

    // redefined output stream
    std::ofstream ofs("test.txt", std::ios::app);
    TruthTableManager::getInstance()->setDebugOstream(ofs);
    GenTruthTable4P(P, Q, R, S, (P V (!Q)) A ((!R) --> (S V P)));

}










