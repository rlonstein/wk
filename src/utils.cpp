#include "wk.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace WK {
  namespace UTILS {
    std::string getCurrentDatetime() {
      std::time_t t = std::time(nullptr);
      std::tm tm = *std::localtime(&t);
      std::stringstream timestr;
      timestr << std::put_time(&tm, "%FT%T%z");
      return timestr.str();
    }
  }
}
