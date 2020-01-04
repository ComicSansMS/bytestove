#ifndef FILESTOVE_INCLUDE_GUARD_EXCEPTIONS_HPP_
#define FILESTOVE_INCLUDE_GUARD_EXCEPTIONS_HPP_

#include <gbBase/Exception.hpp>

namespace filestove {

class Win32Exception: public Ghulbus::Exceptions::IOError {};

namespace Exception_Info {
namespace Tags
{
struct pdh_status { };
}
namespace Records
{
}

using pdh_status = Ghulbus::ErrorInfo<Tags::pdh_status, long>;
}

}

#endif
