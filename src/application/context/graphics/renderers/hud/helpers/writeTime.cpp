
#include <iomanip>
#include <sstream>
#include <string>

void
writeTime(
  std::stringstream& stream, uint32_t time, int32_t leftPadValue /*= 5*/) {

  stream << std::setw(std::max(0, leftPadValue));
  stream << std::fixed;
  stream << std::setprecision(1);

  // if (time < 1000)
  // {
  //     stream << time << "us";
  // }
  // else
  if (time < 1000) {
    stream << float(time) << "ms";
  } else {
    stream << (float(time) / 1000) << "s";
  }
}
