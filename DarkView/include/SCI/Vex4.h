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

#ifndef SCI_VEX4_H
#define SCI_VEX4_H


#include <SCI/Vex3.h>

namespace SCI {

    ///////////////////////////////////////////////////////////////////
    /// \brief 4D Vector and RGBA Color Class
    ///
    /// This class provides functionality that one might need for a 4D
    /// vector or RGBA color.
    ///////////////////////////////////////////////////////////////////
    class Vex4
    {
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

                            ///////////////////////////////////////////////////////
                            /// \brief w coordinate of the vector
                            ///////////////////////////////////////////////////////
                            float w;
                    };
                    struct{
                            ///////////////////////////////////////////////////////
                            /// \brief red color
                            ///////////////////////////////////////////////////////
                            float r;

                            ///////////////////////////////////////////////////////
                            /// \brief green color
                            ///////////////////////////////////////////////////////
                            float g;

                            ///////////////////////////////////////////////////////
                            /// \brief blue color
                            ///////////////////////////////////////////////////////
                            float b;

                            ///////////////////////////////////////////////////////
                            /// \brief alpha color
                            ///////////////////////////////////////////////////////
                            float a;
                    };

                    ///////////////////////////////////////////////////////////
                    /// \brief array access to the vector values
                    ///////////////////////////////////////////////////////////
                    float data[4];
            };

    public:
        ///////////////////////////////////////////////////////////////
            /// \brief Default Constructor
            ///
            /// \param x value of x (red) coordinate
            /// \param y value of y (green) coordinate
            /// \param z value of z (blue) coordinate
            /// \param w value of w (alpha) coordinate
            ///////////////////////////////////////////////////////////////
            inline Vex4(const float _x = 0, const float _y = 0, const float _z = 0, const float _w = 0){
                    data[0] = _x;
                    data[1] = _y;
                    data[2] = _z;
                    data[3] = _w;
            }

            inline Vex4(const Vex3 _xyz, const float _w){
                    data[0] = _xyz.x;
                    data[1] = _xyz.y;
                    data[2] = _xyz.z;
                    data[3] = _w;
            }

            inline Vex4( unsigned int uint_col ){
                SetFromUInt( uint_col );
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Sets the value of the coordinates
            ///
            /// \param x value of x (red) coordinate
            /// \param y value of y (green) coordinate
            /// \param z value of z (blue) coordinate
            /// \param w value of w (alpha) coordinate
            ///////////////////////////////////////////////////////////////
            inline void Set(const float _x = 0, const float _y = 0, const float _z = 0, const float _w = 0){
                    data[0] = _x;
                    data[1] = _y;
                    data[2] = _z;
                    data[3] = _w;
            }

            ///////////////////////////////////////////////////////////////
            /// \brief Vector Output - Prints to Console
            ///////////////////////////////////////////////////////////////
            inline void Print() const {
                    //printf("< %f %f %f %f >\n",x,y,z,w);

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
                            printf("FLT_MAX, ");
                    }
                    else if(z == -FLT_MAX){
                            printf("-FLT_MAX, ");
                    }
                    else{
                            printf("%f, ",z);
                    }

                    if(w == FLT_MAX){
                            printf("FLT_MAX >\n");
                    }
                    else if(w == -FLT_MAX){
                            printf("-FLT_MAX >\n");
                    }
                    else{
                            printf("%f >\n",w);
                    }
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Scale
            ///
            /// \param op2 Scale amount
            ///
            /// \return result of the scale
            ///////////////////////////////////////////////////////////////
            inline Vex4 operator*(const float op2) const {
                    return Vex4(x*op2, y*op2, z*op2, w*op2);
            }

            inline Vex4 operator*(const Vex4& op2) const {
                    return Vex4(x*op2.x,y*op2.y,z*op2.z,w*op2.w);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Divide
            ///
            /// \param op2 Divisor amount
            ///
            /// \return result of the division
            ///////////////////////////////////////////////////////////////
            inline Vex4 operator/(const float op2) const {
                    return Vex4(x/op2, y/op2, z/op2, w/op2);
            }

            inline Vex4 & operator/=(const float op2) {
                    x/=op2;
                    y/=op2;
                    z/=op2;
                    w/=op2;
                    return (*this);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Checks if the vector has valid values
            ///
            /// \return true if valid
            ///////////////////////////////////////////////////////////////
            inline bool isValid() const {
                    return (x!=FLT_MAX) && (x!=-FLT_MAX) && (y!=FLT_MAX) && (y!=-FLT_MAX) && (z!=FLT_MAX) && (z!=-FLT_MAX) && (w!=FLT_MAX) && (w!=-FLT_MAX);
            }

            Vex4& operator+=(const Vex4& op2){
                    x+=op2.x;
                    y+=op2.y;
                    z+=op2.z;
                    w+=op2.w;
                    return (*this);
            }

            Vex4& operator-=(const Vex4& op2){
                    x-=op2.x;
                    y-=op2.y;
                    z-=op2.z;
                    w-=op2.w;
                    return (*this);
            }

            inline Vex4 operator+(const Vex4& op2) const {
                    return Vex4(x+op2.x,y+op2.y,z+op2.z,w+op2.w);
            }

            inline Vex4 operator-(const Vex4& op2) const {
                    return Vex4(x-op2.x,y-op2.y,z-op2.z,w-op2.w);
            }

            Vex4& operator+=(const float op2){
                    x+=op2;
                    y+=op2;
                    z+=op2;
                    w+=op2;
                    return (*this);
            }

            Vex4& operator-=(const float op2){
                    x-=op2;
                    y-=op2;
                    z-=op2;
                    w-=op2;
                    return (*this);
            }

            inline float & operator[](int i){
                    if(i == 1){ return y; }
                    if(i == 2){ return z; }
                    if(i == 3){ return w; }
                    return x;
            }

            inline Vex4 operator+(const float op2) const {
                    return Vex4(x+op2,y+op2,z+op2,w+op2);
            }

            inline Vex4 operator-(const float op2) const {
                    return Vex4(x-op2,y-op2,z-op2,w-op2);
            }


            inline Vex4 operator-() const {
                    return Vex4(-x,-y,-z,-w);
            }

            double* CopyTo(double vex[4]) const {
                    vex[0] = x;
                    vex[1] = y;
                    vex[2] = z;
                    vex[3] = w;
                    return vex;
            }

            inline unsigned int UIntColor() const {
                    unsigned int rgb[4];

                    rgb[0] = Clamp((int)(x*255),0,255);
                    rgb[1] = Clamp((int)(y*255),0,255);
                    rgb[2] = Clamp((int)(z*255),0,255);
                    rgb[3] = Clamp((int)(w*255),0,255);

                    return rgb[0] | (rgb[1] << 8) | (rgb[2] << 16) | (rgb[3] << 24);
            }

            inline void SetFromUInt(const unsigned int color){
                    x = ((float) ((color>>0) &0xFF)) / 255.0f;
                    y = ((float) ((color>>8) &0xFF)) / 255.0f;
                    z = ((float) ((color>>16)&0xFF)) / 255.0f;
                    w = ((float) ((color>>24)&0xFF)) / 255.0f;
            }

            inline Vex3 xyz() const { return Vex3(x,y,z); }
            inline Vex3 yzx() const { return Vex3(y,z,x); }
            inline Vex3 zxy() const { return Vex3(z,x,y); }
            inline Vex3 zyx() const { return Vex3(z,y,x); }
            inline Vex3 xxx() const { return Vex3(x,x,x); }
            inline Vex3 yyy() const { return Vex3(y,y,y); }
            inline Vex3 zzz() const { return Vex3(z,z,z); }

            inline Vex2 zw() const { return Vex2(z,w); }
            inline Vex2 yz() const { return Vex2(y,z); }
            inline Vex2 xy() const { return Vex2(x,y); }
            inline Vex2 wy() const { return Vex2(w,y); }
            inline Vex2 xz() const { return Vex2(x,z); }
            inline Vex2 wx() const { return Vex2(w,x); }
            inline Vex2 yw() const { return Vex2(y,w); }
    };


    #ifndef VEX4_INVALID
            #define VEX4_INVALID Vex4(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX)
    #endif


    inline Vex4 abs(const Vex4& v){ return Vex4(::fabsf(v.x),::fabsf(v.y),::fabsf(v.z),::fabsf(v.w)); }
    inline Vex4 tan(const Vex4& v){ return Vex4(::tanf(v.x), ::tanf(v.y), ::tanf(v.z), ::tanf(v.w) );  }
    inline Vex4 cos(const Vex4& v){ return Vex4(::cosf(v.x), ::cosf(v.y), ::cosf(v.z), ::cosf(v.w) );  }
    inline Vex4 sin(const Vex4& v){ return Vex4(::tanf(v.x), ::tanf(v.y), ::tanf(v.z), ::tanf(v.w) );  }

    inline Vex4 fabs(const Vex4& v){ return Vex4(::fabsf(v.x),::fabsf(v.y),::fabsf(v.z),::fabsf(v.w)); }
    inline Vex4 tanf(const Vex4& v){ return Vex4(::tanf(v.x), ::tanf(v.y), ::tanf(v.z), ::tanf(v.w) );  }
    inline Vex4 cosf(const Vex4& v){ return Vex4(::cosf(v.x), ::cosf(v.y), ::cosf(v.z), ::cosf(v.w) );  }
    inline Vex4 sinf(const Vex4& v){ return Vex4(::tanf(v.x), ::tanf(v.y), ::tanf(v.z), ::tanf(v.w));  }

    inline float dot(const Vex4 & v1, const Vex4 & v2){
            return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
    }

    inline float magnitude(const Vex4 & v){
            return sqrtf(dot(v,v));
    }


    inline void Swap(Vex4& v0, Vex4& v1){
            Vex4 tmp = v0;
            v0 = v1;
            v1 = tmp;
    }

    inline Vex4 lerp( const Vex4 & v0, const Vex4 & v1, float t ){
        return (v0)*(1.0f-t)+(v1)*(t);
    }
}


inline QDataStream & operator << ( QDataStream & ds, SCI::Vex4 vec ) {
    ds << vec.x << vec.y << vec.z << vec.w;
    return ds;
}

inline QDataStream & operator >> ( QDataStream & ds, SCI::Vex4 & vec ) {
    ds >> vec.x >> vec.y >> vec.z >> vec.w;
    return ds;
}

#endif // SCI_VEX4_H

