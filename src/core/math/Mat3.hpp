#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include "Vec.hpp"
#include "Quat.hpp"      

namespace core::math {

/* 3×3 column-major – matches GLSL layout -----------------------------------*/
struct Mat3 {
    std::array<float,9> m{1,0,0,  0,1,0,  0,0,1};

    /* access (col,row) */
    float&       operator()(std::size_t c,std::size_t r)       noexcept { return m[c*3+r]; }
    const float& operator()(std::size_t c,std::size_t r) const noexcept { return m[c*3+r]; }

    /* matrix multiply */
    Mat3 operator*(const Mat3& b) const noexcept {
        Mat3 r{};
        for(int c=0;c<3;++c)
            for(int r_=0;r_<3;++r_)
                r(c,r_)=(*this)(0,r_)*b(c,0)+(*this)(1,r_)*b(c,1)+(*this)(2,r_)*b(c,2);
        return r;
    }
    /* vec3 */
    Vec3 operator*(const Vec3& v) const noexcept {
        return {
            v[0]*m[0] + v[1]*m[3] + v[2]*m[6],
            v[0]*m[1] + v[1]*m[4] + v[2]*m[7],
            v[0]*m[2] + v[1]*m[5] + v[2]*m[8]
        };
    }

    static Mat3 identity() { return {}; }

    static Mat3 scale(const Vec3& s) {
        Mat3 r{};
        r(0,0)=s[0]; r(1,1)=s[1]; r(2,2)=s[2];
        return r;
    }

    /* from quaternion */
    static Mat3 rotate(const Quat& q);

    /* transpose */
    Mat3 transpose() const noexcept {
        Mat3 r{};
        for(int c=0;c<3;++c) for(int r_=0;r_<3;++r_) r(c,r_)=(*this)(r_,c);
        return r;
    }

    /* determinant */
    float determinant() const noexcept {
        return  m[0]*(m[4]*m[8]-m[7]*m[5])
              - m[3]*(m[1]*m[8]-m[7]*m[2])
              + m[6]*(m[1]*m[5]-m[4]*m[2]);
    }

    /* inverse */
    Mat3 inverse() const noexcept {
        float det = determinant();
        float invDet = 1.0f / det;
        Mat3 r{};

        r(0,0) =  (m[4]*m[8]-m[7]*m[5])*invDet;
        r(0,1) = -(m[1]*m[8]-m[7]*m[2])*invDet;
        r(0,2) =  (m[1]*m[5]-m[4]*m[2])*invDet;

        r(1,0) = -(m[3]*m[8]-m[6]*m[5])*invDet;
        r(1,1) =  (m[0]*m[8]-m[6]*m[2])*invDet;
        r(1,2) = -(m[0]*m[5]-m[3]*m[2])*invDet;

        r(2,0) =  (m[3]*m[7]-m[6]*m[4])*invDet;
        r(2,1) = -(m[0]*m[7]-m[6]*m[1])*invDet;
        r(2,2) =  (m[0]*m[4]-m[3]*m[1])*invDet;
        return r;
    }
};

/* Quat→Mat3 (reuse earlier formula) */
inline Mat3 Mat3::rotate(const Quat& q) {
    const float w=q.w,x=q.x,y=q.y,z=q.z;
    const float xx=x*x,yy=y*y,zz=z*z;
    const float xy=x*y,xz=x*z,yz=y*z, wx=w*x,wy=w*y,wz=w*z;
    Mat3 r{};
    r(0,0)=1-2*(yy+zz); r(1,0)=2*(xy+wz);  r(2,0)=2*(xz-wy);
    r(0,1)=2*(xy-wz);   r(1,1)=1-2*(xx+zz);r(2,1)=2*(yz+wx);
    r(0,2)=2*(xz+wy);   r(1,2)=2*(yz-wx);  r(2,2)=1-2*(xx+yy);
    return r;
}

} // namespace core::math
