
#include "getFileContent.hpp"

#include <fstream>
#include <sstream>

namespace gero {
namespace fileUtils {

bool getFileContent(const std::string& filename, std::string& outFileContent) {
  std::ifstream istr(filename);

  if (istr.fail())
    return false;

  std::stringstream sstr;
  sstr << istr.rdbuf();

  outFileContent.clear();

  outFileContent += sstr.str();

  return true;
}

} // namespace fileUtils
} // namespace gero
