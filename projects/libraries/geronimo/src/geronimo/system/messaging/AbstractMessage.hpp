
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <cstdint>
#include <string>

namespace gero {
namespace messaging {

class AbstractMessage {
protected:
  char* _dataPointer = nullptr;
  uint32_t _dataSize = 0;

public:
  AbstractMessage() = default;
  AbstractMessage(const char* dataPointer, uint32_t dataSize);
  virtual ~AbstractMessage() = default;

public:
  virtual void clear();

public:
  const char* getData() const;
  uint32_t getSize() const;
};

} // namespace messaging
} // namespace gero
