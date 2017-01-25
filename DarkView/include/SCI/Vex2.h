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

#ifndef SCI_VEX2_H
#define SCI_VEX2_H

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <QDataStream>
#ifdef WIN32
    #include <Windows.h>
#else
    #include <algorithm>
#endif
#include <SCI/Utility.h>

namespace SCI {


    ///////////////////////////////////////////////////////////////////
    /// \brief 2D Vector Class
    ///
    /// This class provides functionality that one might need for a 2D
    /// vector.
    ///////////////////////////////////////////////////////////////////
    class Vex2 {

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
            };

            ///////////////////////////////////////////////////////////
            /// \brief array access to the vector values
            ///////////////////////////////////////////////////////////
            float data[2];
        };

    public:
        ///////////////////////////////////////////////////////////////
        /// \brief Default Constructor
        ///
        /// \param x value of x coordinate
        /// \param y value of y coordinate
        ///////////////////////////////////////////////////////////////
        inline Vex2(const float _x = 0, const float _y = 0) : x(_x), y(_y) { }

        ///////////////////////////////////////////////////////////////
        /// \brief Sets the value of the coordinates
        ///
        /// \param x value of x coordinate
        /// \param y value of y coordinate
        ///////////////////////////////////////////////////////////////
        inline void Set(const float _x = 0, const float _y = 0){ x = _x; y = _y; }

        ///////////////////////////////////////////////////////////////
        /// \brief Scalar Multiply
        ///
        /// \param scalar scale to multiply to vector
        ///
        /// \return new vector with the resulting scale
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator*(const float scalar) const  { return Vex2(x*scalar, y*scalar); }

        ///////////////////////////////////////////////////////////////
        /// \brief Component-wise Multiply
        ///
        /// \param scalar scale to multiply each component
        ///
        /// \return new vector with the resulting scaling
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator*(const Vex2 & scalar) const  { return Vex2(x*scalar.x, y*scalar.y); }

        ///////////////////////////////////////////////////////////////
        /// \brief Scalar Divide
        ///
        /// \param scalar scale by which to divide the vector
        ///
        /// \return new vector with the resulting divide
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator/(const float scalar) const { return Vex2(x/scalar, y/scalar); }

        ///////////////////////////////////////////////////////////////
        /// \brief Component-wise Divide
        ///
        /// \param scalar scale by which to divide each component
        ///
        /// \return new vector with the resulting divide
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator/(const Vex2 & scalar) const { return Vex2(x/scalar.x, y/scalar.y); }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Add
        ///
        /// \param op2 vector to add to current
        ///
        /// \return new vector with the resulting add
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator+(const Vex2& op2) const { return Vex2(x+op2.x, y+op2.y); }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Add
        ///
        /// \param op2 value to add to each component
        ///
        /// \return new vector with the resulting add
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator+(const float op2) const { return Vex2(x+op2, y+op2); }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Subtract
        ///
        /// \param op2 vector to subtract from current
        ///
        /// \return new vector with the resulting subtract
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator-(const Vex2 op2) const { return Vex2(x-op2.x, y-op2.y); }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Subtract
        ///
        /// \param op2 value to subtract from each component
        ///
        /// \return new vector with the resulting subtract
        ///////////////////////////////////////////////////////////////
        inline Vex2 operator-(const float op2) const { return Vex2(x-op2, y-op2); }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Set
        ///
        /// \param op2 vector to set the current to
        ///
        /// \return current vector modified
        ///////////////////////////////////////////////////////////////
        inline Vex2& operator=(const Vex2 op2){
            x = op2.x;
            y = op2.y;
            return (*this);
        }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Add/Set
        ///
        /// \param op2 vector to add to current
        ///
        /// \return current vector modified with the resulting add
        ///////////////////////////////////////////////////////////////
        inline Vex2& operator+=(const Vex2 op2) {
            x += op2.x;
            y += op2.y;
            return (*this);
        }

        ///////////////////////////////////////////////////////////////
        /// \brief Vector Add/Set
        ///
        /// \param op2 value to add to each component
        ///
        /// \return current vector modified with the resulting add
        ///////////////////////////////////////////////////////////////
        inline Vex2& operator+=(const float op2) {
            x += op2;
            y += op2;
            return (*this);
        }

        inline Vex2& operator-=(const Vex2& op2) {
            x -= op2.x;
            y -= op2.y;
            return (*this);
        }

        inline Vex2& operator-=(const float op2) {
            x -= op2;
            y -= op2;
            return (*this);
        }

        inline Vex2& operator*=(const float op2) {
            x *= op2;
            y *= op2;
            return (*this);
        }





            inline Vex2 operator-(){
                    return Vex2(-x,-y);
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
                            printf("FLT_MAX >\n");
                    }
                    else if(y == -FLT_MAX){
                            printf("-FLT_MAX >\n");
                    }
                    else{
                            printf("%f >\n",y);
                    }
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Checks if the vector has valid values
            ///
            /// \return true if valid
            ///////////////////////////////////////////////////////////////
            inline bool isValid() const {
                    return (x!=FLT_MAX) && (x!=-FLT_MAX) && (y!=FLT_MAX) && (y!=-FLT_MAX);
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Unit Vector
            ///
            /// \return new unit vector of currect vector
            ///////////////////////////////////////////////////////////////
            inline Vex2 UnitVector() const {
                    float length = Length();
                    if (length != 0.0f) {
                            return Vex2(x,y) / length;
                    }
                    return Vex2();
            }


        ///////////////////////////////////////////////////////////////
            /// \brief Normalize vector to magnitude of 1
            ///////////////////////////////////////////////////////////////
            inline void Normalize(){
                    float length = Length();
                    if (length != 0.0f) {
                            x /= length;
                            y /= length;
                    }
            }

        ///////////////////////////////////////////////////////////////
            /// \brief Vector Magnitude
            ///
            /// \return the magnitude of the vector
            ///////////////////////////////////////////////////////////////
            inline float Length() const  {
              return sqrtf(x*x+y*y);
            }

            inline float& operator[](const int i){
                    switch(i) {
                            case 0:    return x;
                            case 1: return y;
                            default:
                                    printf("ERROR (Vex2::operator[]): index out of bounds\n");
                                    return x;
                    }
            }

            inline const float& operator[](const int i) const{
                    switch(i) {
                            case 0:    return x;
                            case 1: return y;
                            default:
                                    printf("ERROR (Vex2::operator[]): index out of bounds\n");
                                    return x;
                    }
            }

            inline Vex2 Perpendicular() const {
                    return Vex2(-y,x);
            }

    };


    #ifndef VEX2_INVALID
            #define VEX2_INVALID Vex2(FLT_MAX,FLT_MAX)
    #endif

    #ifndef VEX2_MAX
            #define VEX2_MAX Vex2(FLT_MAX,FLT_MAX)
    #endif

    #ifndef VEX2_MIN
            #define VEX2_MIN Vex2(-FLT_MAX,-FLT_MAX)
    #endif

    inline Vex2 abs(const Vex2& v){ return Vex2(::fabsf(v.x),::fabsf(v.y)); }
    inline Vex2 tan(const Vex2& v){ return Vex2(::tanf(v.x), ::tanf(v.y));  }
    inline Vex2 cos(const Vex2& v){ return Vex2(::cosf(v.x), ::cosf(v.y));  }
    inline Vex2 sin(const Vex2& v){ return Vex2(::tanf(v.x), ::tanf(v.y));  }

    inline Vex2 fabs(const Vex2& v){ return Vex2(::fabsf(v.x),::fabsf(v.y)); }
    inline Vex2 tanf(const Vex2& v){ return Vex2(::tanf(v.x), ::tanf(v.y));  }
    inline Vex2 cosf(const Vex2& v){ return Vex2(::cosf(v.x), ::cosf(v.y));  }
    inline Vex2 sinf(const Vex2& v){ return Vex2(::tanf(v.x), ::tanf(v.y));  }

    ///////////////////////////////////////////////////////////////////
    /// \brief Normalize a 2D Vector
    ///
    /// Inline function to normalize a 2D Vector.  Included for CG
    /// compatibility.
    ///
    /// \param v Vector to normailze
    ///
    /// \return Normalized 2D vector
    ///////////////////////////////////////////////////////////////////
    inline Vex2 normalize(Vex2 v){
            return v.UnitVector();
    }

    inline float cross(Vex2 v1, Vex2 v2){
            return v1.x*v2.y - v1.y*v2.x;
    }

    ///////////////////////////////////////////////////////////////
    /// \brief Dot Product
    ///
    /// \param op2 Other vector to apply dot product
    ///
    /// \return result of dot product (x0*x1 + y0*y1)
    ///////////////////////////////////////////////////////////////
    inline float dot(Vex2 v1, Vex2 v2){
            return v1.x*v2.x + v1.y*v2.y;
    }

    inline float magnitude(Vex2 v){
            return sqrtf(dot(v,v));
    }




    inline Vex2 Min(Vex2 v0, Vex2 v1){ return Vex2( SCI::Min(v0.x,v1.x), SCI::Min(v0.y,v1.y) );  }
    inline Vex2 Max(Vex2 v0, Vex2 v1){ return Vex2( SCI::Max(v0.x,v1.x), SCI::Max(v0.y,v1.y) );  }

    //inline float MIN(Vex2 a){                            return MIN(a.x,a.y);                            }
    ////inline float MINABS(Vex2 a){                            return MINABS(a.x,a.y);                            }
    //inline Vex2 MIN(Vex2 a, Vex2 b){                    return Vex2(MIN(a.x,b.x),MIN(a.y,b.y));                            }
    //inline Vex2 MIN(Vex2 a, Vex2 b, Vex2 c){            return Vex2(MIN(a.x,b.x,c.x),MIN(a.y,b.y,c.y));                }
    //inline Vex2 MIN(Vex2 a, Vex2 b, Vex2 c, Vex2 d){    return Vex2(MIN(a.x,b.x,c.x,d.x),MIN(a.y,b.y,c.y,d.y));    }
    //
    //inline float MAX(Vex2 a){                    return MAX(a.x,a.y);                            }
    //inline float MAXABS(Vex2 a){                    return MAXABS(a.x,a.y);                            }
    //inline Vex2 MAX(Vex2 a, Vex2 b){                    return Vex2(MAX(a.x,b.x),MAX(a.y,b.y));                            }
    //inline Vex2 MAX(Vex2 a, Vex2 b, Vex2 c){            return Vex2(MAX(a.x,b.x,c.x),MAX(a.y,b.y,c.y));                }
    //inline Vex2 MAX(Vex2 a, Vex2 b, Vex2 c, Vex2 d){    return Vex2(MAX(a.x,b.x,c.x,d.x),MAX(a.y,b.y,c.y,d.y));    }
    //
    //inline void Swap(Vex2& v0, Vex2& v1){
    //    Vex2 tmp = v0;
    //    v0 = v1;
    //    v1 = tmp;
    //}

    //#include "global.h"


    inline Vex2 lerp( Vex2 v0, Vex2 v1, float t ){
        return (v0)*(1.0f-t)+(v1)*(t);
    }


    inline Vex2 plerp(Vex2 cen, Vex2 p0, Vex2 p1, float t ){
        float a0 = atan2f( p0.y-cen.y, p0.x-cen.x );
        float a1 = atan2f( p1.y-cen.y, p1.x-cen.x );

        float r0 = (p0-cen).Length();
        float r1 = (p1-cen).Length();

        if(a0 <= 0){ a0 += 2.0f * PI; }
        if(a1 <= 0){ a1 += 2.0f * PI; }

    //    if( fabsf(r0-r1) > 10 && fabsf(a0-a1) < 0.5f ){
    //        a1 += 2.0f * PI;
    //    }
        if( fabsf(a0-a1) < 0.5f ){
               a1 += 2.0f * PI;
        }

        float angle  = lerp(a0,a1,t);
        float radius = lerp(r0,r1,t);

        return cen + Vex2( ::cosf( angle ), ::sinf( angle ) ) * radius;
    }

}

inline QDataStream & operator << ( QDataStream & ds, SCI::Vex2 vec ) {
    ds << vec.x << vec.y;
    return ds;
}

inline QDataStream & operator >> ( QDataStream & ds, SCI::Vex2 & vec ) {
    ds >> vec.x >> vec.y;
    return ds;
}

#endif // SCI_VEX2_H
