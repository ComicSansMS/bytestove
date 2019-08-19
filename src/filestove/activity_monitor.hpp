#ifndef FILESTOVE_INCLUDE_GUARD_ACTIVITY_MONITOR_HPP_
#define FILESTOVE_INCLUDE_GUARD_ACTIVITY_MONITOR_HPP_

#include <memory>

namespace filestove {

class ActivityMonitor {
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
public:
    ActivityMonitor();
    ~ActivityMonitor();

    void collect();
};

}

#endif
