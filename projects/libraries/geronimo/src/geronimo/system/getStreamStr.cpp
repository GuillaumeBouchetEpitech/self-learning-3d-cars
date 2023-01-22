
#include "getStreamStr.hpp"

#include <sstream>

namespace gero {

std::string getStreamStr(const std::ostream& stream) {
  std::string data = dynamic_cast<const std::ostringstream&>(stream).str();
  return data;
}

} // namespace gero
