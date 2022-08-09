//
// Created by 玥辰 on 2021/10/1.
//

#ifndef INV_SEARCH_SATDEFS_H
#define INV_SEARCH_SATDEFS_H

#include <memory>

namespace sat {

    class AbsSatSolver;
    using SatSolver = std::shared_ptr<AbsSatSolver>;
}

#endif //INV_SEARCH_SATDEFS_H
