/**	@file d_camera.h
 *	@brief Contains functions and values that control how the camera behaves.
 *
 *	@author Lunar Soap
 *	@bug No known bugs.
 */
#ifndef TP_D_CAMERA_H
#define TP_D_CAMERA_H

#include <cstdint>

#include "tp/d_a_alink.h"

namespace libtp::tp::d_camera
{
    extern "C"
    {
        /**
         *  @brief Checks to see if Link is riding something (Spinner/Boar/Snowboard/etc.).
         *
         *  @param d_a_alink  A pointer to Link's actor
         *
         *  @return Bool returns True if Link is riding something, otherwise returns False.
         */
        bool checkRide(d_a_alink::daAlink* d_a_alink);
    }
} // namespace libtp::tp::d_camera
#endif