
#pragma once

#include <sstream>

namespace gero {

std::string getStreamStr(const std::ostream& stream);

#define D_SSTR(stream) getStreamStr(std::ostringstream() << stream)

} // namespace gero
