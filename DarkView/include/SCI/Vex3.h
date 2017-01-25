/*
**  Common Library
**  Copyright (C) 2013  Paul Rosen
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCI_VEX3_H
#define SCI_VEX3_H



#include <math.h>
#include <stdio.h>
#include <float.h>
#ifdef WIN32
    #include <Windows.h>
#else
    #include <algorithm>
#endif
#include <SCI/Vex2.h>
#include <SCI/Utility.h>

namespace SCI {

    ///////////////////////////////////////////////////////////////////
    /// \brief 3D Vector and RGB Color Class
    ///
    /// This class provides functionality that one might need for a 3D
    /// vector or RGB color.
    ///////////////////////////////////////////////////////////////////
    class Vex3{

    public:
            union{
                    struct{
                            ///////////////////////////////////////////////////////
                            /// \brief x coordinate of the vector
                            ///////////////////////////////////////////////////////
                            float x;

                            ///////////////////////////////////////////////////////
                            /// \brief y coordinate of the vector
                            ///////////////////////////////////////////////////////
                            float y;

                            ///////////////////////////////////////////////////////
                            /// \brief z coordinate of the vector
                            ///////////////////////////////////////////////////////
                            float z;
                    };

                    struct{
                            ///////////////////////////////////////////////////////
                            /// \brief red color component
                            ///////////////////////////////////////////////////////
                            float r;

                            ///////////////////////////////////////////////////////
                            /// \brief green color component
                            ///////////////////////////////////////////////////////
                            float g;

                            ///////////////////////////////////////////////////////
                            /// \brief blue color component
                            ///////////////////////////////////////////////////////
                            float b;
                    };

                    ///////////////////////////////////////////////////////////
                    /// \brief array access to the vector values
                    ///////////////////////////////////////////////////////////
                    float data[3];
            };

    public:
        ///////////////////////////////////////////////////////////////
            /// \brief Default Constructor
            ///
            /// \param x value of x (red) coordinate
            /// \param y value of y (green) coordinate
            /// \param z value of z (blue) coordinate
            ///////////////////////////////////////////////////////////////
            inline Vex3(const float x = 0, const float y = 0, const float z = 0){
                    data[0] = x;
                    data[1] = y;
                    data[2] = z;
            }

            inline Vex3(const Vex2 & xy, const float z){
                    data[0] = xy.x;
                    data[1] = xy.y;
                    data[2] = z;
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Sets the value of the coordinates
            ///
            /// \param x value of x (red) coordinate
            /// \param y value of y (green) coordinate
            /// \param z value of z (blue) coordinate
            ///////////////////////////////////////////////////////////////
            inline void Set(const float x = 0, const float y = 0, const float z = 0){
                    data[0] = x;
                    data[1] = y;
                    data[2] = z;
            }

            ///////////////////////////////////////////////////////////////
            /// \brief Vector Output - Prints to Console
            ///////////////////////////////////////////////////////////////
            inline void Print() const {
                    if(x == FLT_MAX){
                            printf("< FLT_MAX, ");
                    }
                    else if(x == -FLT_MAX){
                            printf("< -FLT_MAX, ");
                    }
                    else{
                            printf("< %f, ",x);
                    }

                    if(y == FLT_MAX){
                            printf("FLT_MAX, ");
                    }
                    else if(y == -FLT_MAX){
                            printf("-FLT_MAX, ");
                    }
                    else{
                            printf("%f, ",y);
                    }

                    if(z == FLT_MAX){
                            printf("FLT_MAX >\n");
                    }
                    else if(z == -FLT_MAX){
                            printf("-FLT_MAX >\n");
                    }
                    else{
                            printf("%f >\n",z);
                    }
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Unit Vector
            ///
            /// \return new unit vector of currect vector
            ///////////////////////////////////////////////////////////////
            inline Vex3 UnitVector() const {
                    float length = Length();
                    if (length != 0.0f) {
                            return Vex3(x,y,z) / length;
                    }
                    return Vex3(0,0,0);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Normalize vector to magnitude of 1
            ///////////////////////////////////////////////////////////////
            inline void Normalize(){
                    float length = Length();
                    if (length != 0.0f) {
                            x /= length;
                            y /= length;
                            z /= length;
                    }
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Magnitude
            ///
            /// \return the magnitude of the vector
            ///////////////////////////////////////////////////////////////
            inline float Length() const  {
                    return sqrtf(x*x+y*y+z*z);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Magnitude
            ///
            /// \return the magnitude of the vector
            ///////////////////////////////////////////////////////////////
            inline float Magnitude() const {
                    return sqrtf(x*x+y*y+z*z);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Add
            ///
            /// \param op2 vector to add to current
            ///
            /// \return new vector with the resulting add
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator+(const Vex3& op2) const {
                    return Vex3(x+op2.x, y+op2.y, z+op2.z);
            }

            inline Vex3 operator+(const float op2) const {
                    return Vex3(x+op2, y+op2, z+op2);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Add/Set
            ///
            /// \param op2 vector to add to current
            ///
            /// \return current vector modified with the resulting add
            ///////////////////////////////////////////////////////////////
            inline Vex3& operator+=(const Vex3& op2){
                    x += op2.x;
                    y += op2.y;
                    z += op2.z;
                    return (*this);
            }

            inline Vex3& operator+=(const float op2){
                    x += op2;
                    y += op2;
                    z += op2;
                    return (*this);
            }

            inline Vex3& operator-=(const Vex3& op2){
                    x -= op2.x;
                    y -= op2.y;
                    z -= op2.z;
                    return (*this);
            }

            inline Vex3& operator-=(const float op2){
                    x -= op2;
                    y -= op2;
                    z -= op2;
                    return (*this);
            }

            inline Vex3& operator/=(const Vex3& op2){
                    x /= op2.x;
                    y /= op2.y;
                    z /= op2.z;
                    return (*this);
            }

            inline Vex3& operator/=(const float op2){
                    x /= op2;
                    y /= op2;
                    z /= op2;
                    return (*this);
            }

            inline Vex3& operator*=(const Vex3& op2){
                    x *= op2.x;
                    y *= op2.y;
                    z *= op2.z;
                    return (*this);
            }

            inline Vex3& operator*=(const float op2){
                    x *= op2;
                    y *= op2;
                    z *= op2;
                    return (*this);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Subtract
            ///
            /// \param op2 vector to subtract from current
            ///
            /// \return new vector with the resulting subtract
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator-(const Vex3& op2) const {
                    return Vex3(x-op2.x, y-op2.y, z-op2.z);
            }

            inline Vex3 operator-(const float op2) const {
                    return Vex3(x-op2, y-op2, z-op2);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Scalar Multiply
            ///
            /// \param scalar scale to multiply to vector
            ///
            /// \return new vector with the resulting scale
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator*(const float scalar) const {
              return Vex3(x*scalar, y*scalar, z*scalar);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Scalar Multiply
            ///
            /// \param scale scale each vector comonent
            ///
            /// \return new vector with the resulting scale
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator*(const Vex3& scale) const {
              return Vex3(x*scale.x, y*scale.y, z*scale.z);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Scalar Divide
            ///
            /// \param scalar scale by which to divide the vector
            ///
            /// \return new vector with the resulting divide
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator/(const float scalar) const {
                    return Vex3(x/scalar, y/scalar, z/scalar);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Scalar Divide
            ///
            /// \param scale scale by which to divide each vector comonent
            ///
            /// \return new vector with the resulting divide
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator/(const Vex3& scale) const {
              return Vex3(x/scale.x, y/scale.y, z/scale.z);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Component Access
            ///
            /// \param i which element to access
            ///
            /// \return reference to the element requested
            ///////////////////////////////////////////////////////////////
            inline float& operator[](const int i){
                    switch(i) {
                            case 0:    return x;
                            case 1: return y;
                            case 2:    return z;
                            default:
                                    printf("ERROR (Vex3::operator[]): index out of bounds\n");
                                    return x;
                    }
            }

            inline const float& operator[](const int i) const{
                    switch(i) {
                            case 0:    return x;
                            case 1: return y;
                            case 2:    return z;
                            default:
                                    printf("ERROR (Vex3::operator[]): index out of bounds\n");
                                    return x;
                    }
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Negate
            ///
            /// \return new vector that is the negative of the current
            ///////////////////////////////////////////////////////////////
            inline Vex3 operator-() const  {
                    return Vex3(-x,-y,-z);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Get the 32-bit color value of the vector
            ///
            /// \return 32-bit RGBA color
            ///////////////////////////////////////////////////////////////
            inline unsigned int UIntColor() const {
                    int rgb[3];

                    rgb[0] = Clamp((int)(x*255),0,255);
                    rgb[1] = Clamp((int)(y*255),0,255);
                    rgb[2] = Clamp((int)(z*255),0,255);

                    return (0xFF000000 + rgb[0] + (rgb[1] << 8) + (rgb[2] << 16));
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Set the vector based upon the 32-bit color value
            ///
            /// \param color 32-bit color value to set the buffer
            ///////////////////////////////////////////////////////////////
            inline void SetFromUInt(const unsigned int color){
                    x = ((float) ((color>>0) &0xFF)) / 255.0f;
                    y = ((float) ((color>>8) &0xFF)) / 255.0f;
                    z = ((float) ((color>>16)&0xFF)) / 255.0f;
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Set
            ///
            /// \param op2 vector to set the current to
            ///
            /// \return current vector modified
            ///////////////////////////////////////////////////////////////
            inline Vex3& operator=(const Vex3& op2){
                    x = op2.x;
                    y = op2.y;
                    z = op2.z;
                    return (*this);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Checks if the vector has valid values
            ///
            /// \return true if valid
            ///////////////////////////////////////////////////////////////
            inline bool isValid() const {
                    return (x!=FLT_MAX) && (x!=-FLT_MAX) && (y!=FLT_MAX) && (y!=-FLT_MAX) && (z!=FLT_MAX) && (z!=-FLT_MAX);
            }


            inline bool operator==(const Vex3& other) const {
                    return (x == other.x) && (y == other.y) && (z == other.z);
            }

            inline bool operator!=(const Vex3& other) const {
                    return (x != other.x) || (y != other.y) || (z != other.z);
            }

            inline Vex2 xx() const { return Vex2(x,x); }
            inline Vex2 xy() const { return Vex2(x,y); }
            inline Vex2 xz() const { return Vex2(x,z); }
            inline Vex2 yx() const { return Vex2(y,x); }
            inline Vex2 yy() const { return Vex2(y,y); }
            inline Vex2 yz() const { return Vex2(y,z); }
            inline Vex2 zx() const { return Vex2(z,x); }
            inline Vex2 zy() const { return Vex2(z,y); }
            inline Vex2 zz() const { return Vex2(z,z); }

            inline Vex3 xyz() const { return Vex3(x,y,z); }
            inline Vex3 yzx() const { return Vex3(y,z,x); }
            inline Vex3 zxy() const { return Vex3(z,x,y); }
            inline Vex3 xxx() const { return Vex3(x,x,x); }
            inline Vex3 yyy() const { return Vex3(y,y,y); }
            inline Vex3 zzz() const { return Vex3(z,z,z); }

            inline bool operator < (const Vex3 & right) const {
                if(x<right.x) return true;
                if(x>right.x) return false;
                if(y<right.y) return true;
                if(y>right.y) return false;
                if(z<right.z) return true;
                if(z>right.z) return false;
                return false;
            }


    };

    inline Vex3 operator*(const float f, const Vex3 & v){
            return v*f;
    }


    inline float Distance(float x1, float x2,float y1, float y2, float z1, float z2){
            float dx = x1 - x2;
            float dy = y1 - y2;
            float dz = z1 - z2;

            return sqrtf(dx*dx + dy*dy + dz*dz);
    }

    #ifndef VEX3_INVALID
            #define VEX3_INVALID Vex3(FLT_MAX,FLT_MAX,FLT_MAX)
    #endif

    #ifndef VEX3_MAX
            #define VEX3_MAX Vex3(FLT_MAX,FLT_MAX,FLT_MAX)
    #endif

    #ifndef VEX3_MIN
            #define VEX3_MIN Vex3(-FLT_MAX,-FLT_MAX,-FLT_MAX)
    #endif


    inline Vex3 abs(const Vex3& v){ return Vex3(::fabsf(v.x),::fabsf(v.y),::fabsf(v.z)); }
    inline Vex3 tan(const Vex3& v){ return Vex3(::tanf(v.x), ::tanf(v.y), ::tanf(v.z));  }
    inline Vex3 cos(const Vex3& v){ return Vex3(::cosf(v.x), ::cosf(v.y), ::cosf(v.z));  }
    inline Vex3 sin(const Vex3& v){ return Vex3(::tanf(v.x), ::tanf(v.y), ::tanf(v.z));  }

    inline Vex3 absf(const Vex3& v){ return Vex3(::fabsf(v.x),::fabsf(v.y),::fabsf(v.z)); }
    inline Vex3 tanf(const Vex3& v){ return Vex3(::tanf(v.x), ::tanf(v.y), ::tanf(v.z));  }
    inline Vex3 cosf(const Vex3& v){ return Vex3(::cosf(v.x), ::cosf(v.y), ::cosf(v.z));  }
    inline Vex3 sinf(const Vex3& v){ return Vex3(::tanf(v.x), ::tanf(v.y), ::tanf(v.z));  }

    inline Vex3 fabs(const Vex3& v){  return Vex3(::fabsf(v.x),::fabsf(v.y),::fabsf(v.z)); }
    inline Vex3 fabsf(const Vex3& v){ return Vex3(::fabsf(v.x),::fabsf(v.y),::fabsf(v.z)); }

    inline Vex3 Min(Vex3 v0, Vex3 v1){ return Vex3( SCI::Min(v0.x,v1.x), SCI::Min(v0.y,v1.y), SCI::Min(v0.z,v1.z) );  }
    inline Vex3 Max(Vex3 v0, Vex3 v1){ return Vex3( SCI::Max(v0.x,v1.x), SCI::Max(v0.y,v1.y), SCI::Max(v0.z,v1.z) );  }

    ///////////////////////////////////////////////////////////////////
    /// \brief Cross Product of 3D Vectors
    ///
    /// Inline function to calculate the cross product of 3D Vectors.
    ///
    /// \param v1 First vector in cross product
    /// \param v2 Second vector in cross product
    ///
    /// \return 3D vector cross product
    ///////////////////////////////////////////////////////////////////
    inline Vex3 cross(const Vex3& v1, const Vex3& v2){
            Vex3 ret;
            ret.x = v1.y*v2.z - v1.z*v2.y;
            ret.y = v1.z*v2.x - v1.x*v2.z;
            ret.z = v1.x*v2.y - v1.y*v2.x;
            return ret;
    }

    ///////////////////////////////////////////////////////////////////
    /// \brief Dot Product of 3D Vectors
    ///
    /// Inline function to calculate the dot product 3D Vectors.
    ///
    /// \param v1 First vector in cross product
    /// \param v2 Second vector in cross product
    ///
    /// \return Vector dot product
    ///////////////////////////////////////////////////////////////////
    inline float dot(const Vex3& v1, const Vex3& v2){
            return ((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));
    }

    ///////////////////////////////////////////////////////////////////
    /// \brief Normalize a 3D Vector
    ///
    /// Inline function to normalize a 3D Vector.
    ///
    /// \param v Vector to normailze
    ///
    /// \return Normalized 3D vector
    ///////////////////////////////////////////////////////////////////
    inline Vex3 normalize(const Vex3& v){
            return v.UnitVector();
    }



    inline float magnitude(const Vex3& v){
            return v.Magnitude();
    }

    inline Vex3 CLAMP(Vex3 v, float minv, float maxv){
            v.x = Clamp(v.x,minv,maxv);
            v.y = Clamp(v.y,minv,maxv);
            v.z = Clamp(v.z,minv,maxv);
            return v;
    }

    //inline float MIN(Vex3 a){                            return MIN(a.x,a.y,a.z);                            }
    ////inline float MINABS(Vex3 a){                            return MINABS(a.x,a.y,a.z);                            }
    //inline Vex3 MIN(Vex3 a, Vex3 b){                    return Vex3(MIN(a.x,b.x),MIN(a.y,b.y),MIN(a.z,b.z));                            }
    //inline Vex3 MIN(Vex3 a, Vex3 b, Vex3 c){            return Vex3(MIN(a.x,b.x,c.x),MIN(a.y,b.y,c.y),MIN(a.z,b.z,c.z));                }
    //inline Vex3 MIN(Vex3 a, Vex3 b, Vex3 c, Vex3 d){    return Vex3(MIN(a.x,b.x,c.x,d.x),MIN(a.y,b.y,c.y,d.y),MIN(a.z,b.z,c.z,d.z));    }
    //
    //inline float MAX(Vex3 a){                    return MAX(a.x,a.y,a.z);                            }
    //inline float MAXABS(Vex3 a){                    return MAXABS(a.x,a.y,a.z);                            }
    //inline Vex3 MAX(Vex3 a, Vex3 b){                    return Vex3(MAX(a.x,b.x),MAX(a.y,b.y),MAX(a.z,b.z));                            }
    //inline Vex3 MAX(Vex3 a, Vex3 b, Vex3 c){            return Vex3(MAX(a.x,b.x,c.x),MAX(a.y,b.y,c.y),MAX(a.z,b.z,c.z));                }
    //inline Vex3 MAX(Vex3 a, Vex3 b, Vex3 c, Vex3 d){    return Vex3(MAX(a.x,b.x,c.x,d.x),MAX(a.y,b.y,c.y,d.y),MAX(a.z,b.z,c.z,d.z));    }

    inline void Swap(Vex3& v0, Vex3& v1){
            Vex3 tmp = v0;
            v0 = v1;
            v1 = tmp;
    }

    inline Vex3 lerp( Vex3 v0, Vex3 v1, float t ){
        return (v0)*(1.0f-t)+(v1)*(t);
    }


    inline Vex3 blerp(Vex3 a, Vex3 b, Vex3 c, Vex3 d, float t0, float t1){
            return lerp(lerp(a,b,t0),lerp(c,d,t0),t1);
    }


}

inline std::ostream & operator << ( std::ostream & os, SCI::Vex3 vec ){
    os << "( " << vec.x << ", " << vec.y << ", " << vec.z << " )";
    return os;
}

inline QDataStream & operator << ( QDataStream & ds, SCI::Vex3 vec ) {
    ds << vec.x << vec.y << vec.z;
    return ds;
}

inline QDataStream & operator >> ( QDataStream & ds, SCI::Vex3 & vec ) {
    ds >> vec.x >> vec.y >> vec.z;
    return ds;
}


#endif // SCI_VEX2_H
