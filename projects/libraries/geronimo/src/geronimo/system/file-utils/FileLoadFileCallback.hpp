
#pragma once

#include <functional>
#include <string>

namespace gero {
namespace fileUtils {

using LoadCallback =
  std::function<void(const std::string& filename, std::string& source)>;

LoadCallback getDefaulCallback();

} // namespace fileUtils

} // namespace gero
