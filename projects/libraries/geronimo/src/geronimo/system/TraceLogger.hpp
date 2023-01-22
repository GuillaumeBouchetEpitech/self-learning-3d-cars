
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <cstring> // <= strrchr()
#include <sstream> // <= std::stringstream

#include <cstdint>
#include <mutex>
#include <ostream>
#include <vector>

namespace gero {

class TraceLogger {
public:
  static std::string getTime();

public:
  static void log(const std::string& msg);

private:
  std::mutex _mutex;
  std::stringstream _sstr;

public:
  TraceLogger() = default;

public:
  void dump();
  void dumpErr();
  std::string getData() const;

public:
  template <typename T> TraceLogger& operator<<(T data) {
    _sstr << data;
    return *this;
  }
};

template <> TraceLogger& TraceLogger::operator<<<bool>(bool data);

template <> TraceLogger& TraceLogger::operator<<<float>(float data);

template <> TraceLogger& TraceLogger::operator<<<double>(double data);

template <> TraceLogger& TraceLogger::operator<<<glm::ivec2>(glm::ivec2 data);
template <> TraceLogger& TraceLogger::operator<<<glm::ivec3>(glm::ivec3 data);
template <> TraceLogger& TraceLogger::operator<<<glm::ivec4>(glm::ivec4 data);

template <> TraceLogger& TraceLogger::operator<<<glm::uvec2>(glm::uvec2 data);
template <> TraceLogger& TraceLogger::operator<<<glm::uvec3>(glm::uvec3 data);
template <> TraceLogger& TraceLogger::operator<<<glm::uvec4>(glm::uvec4 data);

template <> TraceLogger& TraceLogger::operator<<<glm::vec2>(glm::vec2 data);
template <> TraceLogger& TraceLogger::operator<<<glm::vec3>(glm::vec3 data);
template <> TraceLogger& TraceLogger::operator<<<glm::vec4>(glm::vec4 data);

template <> TraceLogger& TraceLogger::operator<<<glm::quat>(glm::quat data);

} // namespace gero

// this will reduce the "__FILE__" macro to it's filename -> friendlier to read
#define D_MYLOG_FILENAME                                                       \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// this is just to make the "D_MYLOG" macro source code easier to read
#define D_MYLOG_STACK D_MYLOG_FILENAME << "|" << __func__ << "|" << __LINE__

#define D_MYLOG_PREFIX                                                         \
  "[" << gero::TraceLogger::getTime() << "] (" << D_MYLOG_STACK << ") -> "

// one line logging macro
#define D_MYLOG(streamMsg)                                                     \
  {                                                                            \
    gero::TraceLogger logger;                                                 \
    logger << "MYLOG " << D_MYLOG_PREFIX << streamMsg;                         \
    logger.dump();                                                             \
  }

// one line logging macro
#define D_MYERR(streamMsg)                                                     \
  {                                                                            \
    gero::TraceLogger logger;                                                 \
    logger << "MYERR " << D_MYLOG_PREFIX << streamMsg;                         \
    logger.dumpErr();                                                          \
  }
