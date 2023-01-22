
#include "FileLoadFileCallback.hpp"

#include "getFileContent.hpp"

#include "geronimo/system/ErrorHandler.hpp"

namespace gero {
namespace fileUtils {

LoadCallback getDefaulCallback() {
  return [](const std::string& filename, std::string& outFileContent) {
    outFileContent.clear();
    if (!getFileContent(filename, outFileContent))
      D_THROW(std::runtime_error,
              "ERROR: Could not load file: \"" << filename << "\"");
  };
}

} // namespace fileUtils
} // namespace gero
