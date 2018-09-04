#ifndef UTILITY_TERMINALUTILITY_HXX_SEEN
#define UTILITY_TERMINALUTILITY_HXX_SEEN

#include <sys/ioctl.h>

#include <unistd.h>

namespace nuis {
namespace utility {
inline size_t GetWindowWidth() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}
} // namespace utility
} // namespace nuis

#endif
