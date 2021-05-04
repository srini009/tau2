/*
 * Copyright (c) 2014-2020 Kevin Huck
 * Copyright (c) 2014-2020 University of Oregon
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include "nvml.h"
#include <set>
#include <mutex>
#include <vector>

namespace tau { namespace nvml {

class monitor {
public:
    monitor (void);
    ~monitor (void);
    void query();
    static void activateDeviceIndex(uint32_t index);
private:
    uint32_t deviceCount;
    std::vector<nvmlDevice_t> devices;
    std::vector<bool> queried_once;
    static std::set<uint32_t> activeDeviceIndices;
    static std::mutex indexMutex;
    double convertValue(nvmlFieldValue_t &value);
}; // class monitor

} // namespace nvml
} // namespace tau
