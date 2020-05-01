#ifndef FORMAT_H
#define FORMAT_H

#define seconds_hours 3600
#define seconds_minutes 60

#include <string>

namespace Format {
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
std::string kB2MB(long memory);
};                                    // namespace Format

#endif