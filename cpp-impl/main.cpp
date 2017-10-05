#include <iostream>
#include <iomanip>
#include <cmath>

#include "SimpleDbColumn.hpp"
#include "SumOperator.hpp"


std::pair<int64_t, double> nativePass(const int32_t *values, size_t size);
std::pair<int64_t, double> codeGenPass(const int32_t *values, size_t size);

int main(int argc, char* argv[]) {
  std::vector<std::string> files;
  if (argc < 2) {
    std::cout << "Usage: codegen <int32-column-file>\n";
    exit(0);
  }

  std::string filePath = argv[1];

  for (size_t i = 0; i < 10; i++) {
    // Load column in memory
    clock_t startTimeImport = clock();
    simpledb::SimpleDbColumn<int32_t> column(filePath, "int32");
    const int32_t *values = column.getValues();
    size_t size = column.getSize();
    size_t length = size * sizeof(int32_t);
    double importDuration = (double(clock() - startTimeImport) / (double) CLOCKS_PER_SEC) * 1000;

    std::pair<int64_t, double> nativeResult = nativePass(values, size);
    std::pair<int64_t, double> codeGenResult = codeGenPass(values, size);

    double durationDiff = fabs(nativeResult.second - codeGenResult.second);
    double perceivedDiff = (durationDiff / ((codeGenResult.second + nativeResult.second)/ 2)) * 100;

    std::cout << "[" << i << "] Rows(" << length << ") Bytes(" << size << ") :: "
              << "Import Duration: " << std::setw(6) << importDuration << "ms :: "
              << "Native[" << nativeResult.first << "] "
              << "Duration: " << std::setw(6) << nativeResult.second << "ms :: "
              << "Codegen[" << codeGenResult.first << "] "
              << "Duration: " << std::setw(6) << codeGenResult.second << "ms :: "
              << "Diff: " << std::setw(6) << durationDiff << "ms - "
              << std::setw(6) << perceivedDiff << "%"
              << std::endl;
  }
  return 0;
}

/**
 * Compute the SUM of the given values.
 *
 * @param values
 * @param size
 * @return
 */
std::pair<int64_t, double> nativePass(const int32_t *values, size_t size) {
  int64_t value = 0;
  int32_t lowest = std::numeric_limits<int32_t>::lowest();

  clock_t startTime = clock();
  int64_t i = 0;
  while (i < size) {
    int32_t v = values[i];
    if (v != lowest) {
      value = value + values[i];
    }
    i++;
  }

  double duration = (double(clock() - startTime) / (double) CLOCKS_PER_SEC) * 1000;
  return std::make_pair(value, duration);
}

/**
 * Run a Code Generated operator over the given data.
 *
 * @param values
 * @param size
 * @return
 */
std::pair<int64_t, double> codeGenPass(const int32_t *values, size_t size) {
  clock_t startTime = clock();

  // Here we call the LLVM Operator.
  simpledb::SumOperator sum;
  int64_t value = sum.run(values, size);

  double duration = (double(clock() - startTime) / (double) CLOCKS_PER_SEC) * 1000;
  return std::make_pair(value, duration);
}