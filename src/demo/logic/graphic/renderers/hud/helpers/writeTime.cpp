
#include <iomanip>
#include <sstream>
#include <string>

std::string
writeTime(unsigned int time, int32_t leftPadValue /*= 5*/) {
  std::stringstream sstr;

  sstr << std::setw(std::max(0, leftPadValue));
  sstr << std::fixed;
  sstr << std::setprecision(1);

  // if (time < 1000)
  // {
  //     sstr << time << "us";
  // }
  // else
  if (time < 1000000) {
    sstr << (float(time) / 1000) << "ms";
  } else {
    sstr << (float(time) / 1000000) << "s";
  }

  return sstr.str();
}
