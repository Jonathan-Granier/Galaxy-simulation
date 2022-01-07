#pragma once

#include <iostream>
#include <assert.h>

// #TODO Wrap error
#define VK_CHECK_RESULT(f)                                                                                  \
    {                                                                                                       \
        VkResult res = (f);                                                                                 \
        if (res != VK_SUCCESS)                                                                              \
        {                                                                                                   \
            std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ \
                      << std::endl;                                                                         \
            assert(res == VK_SUCCESS);                                                                      \
        }                                                                                                   \
    }
