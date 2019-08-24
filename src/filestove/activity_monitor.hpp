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

    /** Collects bytes read and written for disk I/O.
    * \return The accumulated number of bytes read and written across all disks
     *        since the last call to collect().
     */
    std::size_t collect();
};

}

#endif
