#ifndef SIMPLEDB_SUMOPERATOR_HPP
#define SIMPLEDB_SUMOPERATOR_HPP

#include <cstdint>
#include <cstdlib>

namespace simpledb {

class SumOperator {
 public:
  SumOperator();
  int64_t run(const int32_t *values, size_t size);
};

}

#endif //SIMPLEDB_SUMOPERATOR_HPP