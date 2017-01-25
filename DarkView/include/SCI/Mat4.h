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

#ifndef SCI_MAT4_H
#define SCI_MAT4_H


#include <SCI/Vex2.h>
#include <SCI/Vex3.h>
#include <SCI/Vex4.h>

//data[0] data[4] data[ 8] data[12]
//data[1] data[5] data[ 9] data[13]
//data[2] data[6] data[10] data[14]
//data[3] data[7] data[11] data[15]

namespace SCI {
    class Mat4 {
    public:
        float data[16];

        const static int MAT4_IDENTITY      = 0;
        const static int MAT4_ZERO          = 1;
        const static int MAT4_ROTATE        = 2;
        const static int MAT4_ROTATION      = 2;
        const static int MAT4_TRANSLATE     = 3;
        const static int MAT4_TRANSLATION   = 3;
        const static int MAT4_SCALE         = 4;
        const static int MAT4_COLUMN        = 5;
        const static int MAT4_COLUMNS       = 5;
        const static int MAT4_ROW           = 6;
        const static int MAT4_ROWS          = 6;

        const static int IDENTITY           = 0;
        const static int ZERO               = 1;
        const static int ROTATE             = 2;
        const static int ROTATION           = 2;
        const static int TRANSLATE          = 3;
        const static int TRANSLATION        = 3;
        const static int SCALE              = 4;
        const static int COLUMN             = 5;
        const static int COLUMNS            = 5;
        const static int ROW                = 6;
        const static int ROWS               = 6;

    public:
        Mat4(int initializer = MAT4_IDENTITY);
        Mat4(int initializer, float x, float y, float z);
        Mat4(int initializer, Vex3  v);
        Mat4(int initializer, float angle, float x, float y, float z);
        Mat4(int initializer, float angle, Vex3 v);
        Mat4(int initializer, Vex4  v0, Vex4 v1, Vex4 v2, Vex4 v3);

        void LoadIdentity();
        void LoadZero();
        void LoadRotation(float angle, float x, float y, float z);
        void LoadTranslation(float x, float y, float z);
        void LoadScale(float x, float y, float z);

        float Determinant() const ;
        Mat4 Inverse() const ;
        Mat4 Transpose() const ;

        Vex4 Row(const int i) const;
        void Row(const int i, const Vex4& val);

        Vex4 Column(const int i) const ;
        void Column(const int i, const Vex4& val);

        Mat4 operator*(const Mat4& m) const ;
        Vex3 operator*(const Vex3& v) const ;
        Vex4 operator*(const Vex4& v) const ;

        Mat4 operator+(const Mat4& m) const ;

        Mat4& operator*=(const Mat4& m);
        Mat4& operator+=(const Mat4& m);

        #ifdef WIN32
            void glReadModelview();
            void glSetModelview() const ;

            void glReadProjection();
            void glSetProjection() const ;

            void glReadTexture();
            void glSetTexture() const ;

            void glMultMatrix() const ;
        #endif

        void Print() const ;

        void Store(double mat[16]) const ;
        void Store(float  mat[16]) const ;

        void Load(const double mat[16]);
        void Load(const float  mat[16]);

    };
}


#endif // SCI_MAT4_H
