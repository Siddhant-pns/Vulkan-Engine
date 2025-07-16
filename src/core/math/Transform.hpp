#pragma once
#include "Vec.hpp"
#include "Quat.hpp"
#include "Mat4.hpp"

namespace core::math {

/* Compose TRS in one go – far cheaper than T*S*R chained calls
   Produces column-major Mat4:  M = T * R * S
*/
inline Mat4 composeTRS(const Vec3& t,const Quat& r,const Vec3& s) noexcept {
    /* rotation part (Mat3) */
    Mat4 m = Mat4::rotate(r);

    /* scale - multiply columns */
    m(0,0)*=s[0]; m(0,1)*=s[0]; m(0,2)*=s[0];
    m(1,0)*=s[1]; m(1,1)*=s[1]; m(1,2)*=s[1];
    m(2,0)*=s[2]; m(2,1)*=s[2]; m(2,2)*=s[2];

    /* translation */
    m(3,0)=t[0]; m(3,1)=t[1]; m(3,2)=t[2];
    return m;
}

} // namespace core::math
