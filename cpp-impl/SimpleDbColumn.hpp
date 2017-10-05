#ifndef SIMPLEDB_SIMPLEDBCOLUMN_HPP
#define SIMPLEDB_SIMPLEDBCOLUMN_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

namespace simpledb {

template<typename T>
class SimpleDbColumn {
 protected:
  std::string filePath;
  std::string type;

  size_t size;
  T *memblock;
  void readValuesFromFile();

 public:
  SimpleDbColumn(std::string columnPath, std::string columnType);
  ~SimpleDbColumn();

  const T *getValues();
  size_t getSize();
};

template<typename T>
SimpleDbColumn<T>::SimpleDbColumn(std::string columnPath, std::string columnType)
    : filePath(columnPath),
      type(columnType) {
  readValuesFromFile();
}

template<typename T>
SimpleDbColumn<T>::~SimpleDbColumn() {
  munmap(memblock, size * sizeof(T));
}

template<typename T>
void SimpleDbColumn<T>::readValuesFromFile() {
  struct stat st;
  stat(filePath.data(), &st);

  int fd = open(filePath.data(), O_RDONLY, 0);
  if (fd != -1) {
    memblock = (int *) mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (memblock != MAP_FAILED) {
      size = st.st_size / sizeof(T);
    }
    close(fd);
  }
}

template<typename T>
size_t SimpleDbColumn<T>::getSize() {
  return size;
}

template<typename T>
const T *SimpleDbColumn<T>::getValues() {
  return memblock;
}

}

#endif //SIMPLEDB_SIMPLEDBCOLUMN_HPP
