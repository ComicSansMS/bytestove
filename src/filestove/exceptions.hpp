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

typedef boost::error_info<Tags::pdh_status, long> pdh_status;
}

}

#endif
