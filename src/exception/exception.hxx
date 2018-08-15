#ifndef EXCEPTION_EXCEPTION_SEEN
#define EXCEPTION_EXCEPTION_SEEN

#include <sstream>
#include <stdexcept>
#include <string>

namespace nuis {
struct nuis_except : public std::exception {
  std::stringstream msgstrm;
  std::string msg;
  nuis_except() : msgstrm(), msg() {}
  nuis_except(nuis_except const &other)
      : msgstrm(), msg() {
    msgstrm << other.msg;
    msg = other.msg;
  }
  const char *what() const noexcept { return msg.c_str(); }

  template <typename T> nuis_except &operator<<(T const &obj) {
    msgstrm << obj;
    msg = msgstrm.str();
    return (*this);
  }
};

} // namespace nuis

#define NEW_NUIS_EXCEPT(EXCEPT_NAME)                                           \
  struct EXCEPT_NAME : public nuis::nuis_except {                              \
    EXCEPT_NAME() : nuis::nuis_except() {}                                     \
    EXCEPT_NAME(EXCEPT_NAME const &other) : nuis::nuis_except(other) {}        \
    template <typename T> EXCEPT_NAME &operator<<(T const &obj) {              \
      msgstrm << obj;                                                          \
      msg = msgstrm.str();                                                     \
      return (*this);                                                          \
    }                                                                          \
  }

#endif
