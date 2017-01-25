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

#include <SCI/Mat4.h>

#include <math.h>
#include <stdio.h>
#include <GL/oglCommon.h>



using namespace SCI;

Mat4::Mat4(int initVal){
    switch(initVal){
        case MAT4_IDENTITY:        LoadIdentity();    return;
        case MAT4_ZERO:            LoadZero();        return;
        default:
            printf("ERROR (Mat4): Invalid initial value\n");
    }
}

Mat4::Mat4(int initVal, float x, float y, float z){
    switch(initVal){
        case MAT4_IDENTITY:        LoadIdentity();            return;
        case MAT4_ZERO:            LoadZero();                return;
        case MAT4_TRANSLATE:    LoadTranslation(x,y,z);    return;
        case MAT4_SCALE:        LoadScale(x,y,z);        return;
        default:
            printf("ERROR (Mat4): Invalid initial value\n");
    }
}

Mat4::Mat4(int initVal, Vex3 v){
    switch(initVal){
        case MAT4_IDENTITY:        LoadIdentity();                    return;
        case MAT4_ZERO:            LoadZero();                        return;
        case MAT4_TRANSLATE:    LoadTranslation(v.x,v.y,v.z);    return;
        case MAT4_SCALE:        LoadScale(v.x,v.y,v.z);            return;
        default:
            printf("ERROR (Mat4): Invalid initial value\n");
    }
}


Mat4::Mat4(int initVal, float angle, float x, float y, float z){
    switch(initVal){
        case MAT4_IDENTITY:        LoadIdentity();                return;
        case MAT4_ZERO:            LoadZero();                    return;
        case MAT4_ROTATE:        LoadRotation(angle,x,y,z);    return;
        default:
            printf("ERROR (Mat4): Invalid initial value\n");
    }
}

Mat4::Mat4(int initVal, float angle, Vex3 v){
    switch(initVal){
        case MAT4_IDENTITY:        LoadIdentity();                        return;
        case MAT4_ZERO:            LoadZero();                            return;
        case MAT4_ROTATE:        LoadRotation(angle,v.x,v.y,v.z);    return;
        default:
            printf("ERROR (Mat4): Invalid initial value\n");
    }
}

Mat4::Mat4(int initVal, Vex4 v0, Vex4 v1, Vex4 v2, Vex4 v3){
    switch(initVal){
        case MAT4_IDENTITY:        LoadIdentity();        return;
        case MAT4_ZERO:            LoadZero();            return;
        case MAT4_COLUMNS:    
                                Column(0,v0);
                                Column(1,v1);
                                Column(2,v2);
                                Column(3,v3);
                                return;
        case MAT4_ROWS:
                                Row(0,v0);
                                Row(1,v1);
                                Row(2,v2);
                                Row(3,v3);
                                return;
        default:
            printf("ERROR (Mat4): Invalid initial value\n");
    }
}


void Mat4::LoadZero(){
        for(int i = 0; i < 16; i++){
        data[i] = 0;
    }
}

void Mat4::LoadIdentity(){
        for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
            data[4*i+j] = ((i==j) ? 1.0f : 0.0f);
        }
    }
}

void Mat4::LoadRotation(float angle, float x, float y, float z){
        float mag = sqrtf(x*x + y*y + z*z);
    x /= mag;
    y /= mag;
    z /= mag;

        float rangle = angle / 180.0f * 3.14159265f;

        float c = ::cosf(rangle);
        float s = ::sinf(rangle);

        float xs = x*s, ys = y*s, zs = z*s;

    data[0]  = x*x*(1-c)+c;        data[4]  = x*y*(1-c)-zs;    data[8]  = x*z*(1-c)+ys;    data[12] = 0;
    data[1]  = y*x*(1-c)+zs;    data[5]  = y*y*(1-c)+c;        data[9]  = y*z*(1-c)-xs;    data[13] = 0;
    data[2]  = z*x*(1-c)-ys;    data[6]  = z*y*(1-c)+xs;    data[10] = z*z*(1-c)+c;        data[14] = 0;
    data[3]  = 0;                data[7]  = 0;                data[11] = 0;                data[15] = 1;

}

void Mat4::LoadTranslation(float x, float y, float z){
    LoadIdentity();
    data[12] = x;
    data[13] = y;
    data[14] = z;
}

void Mat4::LoadScale(float x, float y, float z){
    LoadIdentity();
    data[0]  = x;
    data[5]  = y;
    data[10] = z;
}

Mat4 Mat4::operator*(const Mat4& m) const {
    Mat4 ret;

    ret.data[0]  = data[0] * m.data[0]  + data[4] * m.data[1]  + data[8]  * m.data[2]  + data[12] * m.data[3] ;
    ret.data[1]  = data[1] * m.data[0]  + data[5] * m.data[1]  + data[9]  * m.data[2]  + data[13] * m.data[3] ;
    ret.data[2]  = data[2] * m.data[0]  + data[6] * m.data[1]  + data[10] * m.data[2]  + data[14] * m.data[3] ;
    ret.data[3]  = data[3] * m.data[0]  + data[7] * m.data[1]  + data[11] * m.data[2]  + data[15] * m.data[3] ;
    ret.data[4]  = data[0] * m.data[4]  + data[4] * m.data[5]  + data[8]  * m.data[6]  + data[12] * m.data[7] ;
    ret.data[5]  = data[1] * m.data[4]  + data[5] * m.data[5]  + data[9]  * m.data[6]  + data[13] * m.data[7] ;
    ret.data[6]  = data[2] * m.data[4]  + data[6] * m.data[5]  + data[10] * m.data[6]  + data[14] * m.data[7] ;
    ret.data[7]  = data[3] * m.data[4]  + data[7] * m.data[5]  + data[11] * m.data[6]  + data[15] * m.data[7] ;
    ret.data[8]  = data[0] * m.data[8]  + data[4] * m.data[9]  + data[8]  * m.data[10] + data[12] * m.data[11];
    ret.data[9]  = data[1] * m.data[8]  + data[5] * m.data[9]  + data[9]  * m.data[10] + data[13] * m.data[11];
    ret.data[10] = data[2] * m.data[8]  + data[6] * m.data[9]  + data[10] * m.data[10] + data[14] * m.data[11];
    ret.data[11] = data[3] * m.data[8]  + data[7] * m.data[9]  + data[11] * m.data[10] + data[15] * m.data[11];
    ret.data[12] = data[0] * m.data[12] + data[4] * m.data[13] + data[8]  * m.data[14] + data[12] * m.data[15];
    ret.data[13] = data[1] * m.data[12] + data[5] * m.data[13] + data[9]  * m.data[14] + data[13] * m.data[15];
    ret.data[14] = data[2] * m.data[12] + data[6] * m.data[13] + data[10] * m.data[14] + data[14] * m.data[15];
    ret.data[15] = data[3] * m.data[12] + data[7] * m.data[13] + data[11] * m.data[14] + data[15] * m.data[15];

    return ret;
}

Vex3 Mat4::operator*(const Vex3& v) const {
        float x = data[0] * v.data[0] + data[4] * v.data[1] + data[8]  * v.data[2] + data[12];
        float y = data[1] * v.data[0] + data[5] * v.data[1] + data[9]  * v.data[2] + data[13];
        float z = data[2] * v.data[0] + data[6] * v.data[1] + data[10] * v.data[2] + data[14];
        float w = data[3] * v.data[0] + data[7] * v.data[1] + data[11] * v.data[2] + data[15];
    return Vex3(x,y,z)/w;
}

Vex4 Mat4::operator*(const Vex4& v) const {
    Vex4 ret;
    ret.x = data[0] * v.data[0] + data[4] * v.data[1] + data[8]  * v.data[2] + data[12] * v.data[3];
    ret.y = data[1] * v.data[0] + data[5] * v.data[1] + data[9]  * v.data[2] + data[13] * v.data[3];
    ret.z = data[2] * v.data[0] + data[6] * v.data[1] + data[10] * v.data[2] + data[14] * v.data[3];
    ret.w = data[3] * v.data[0] + data[7] * v.data[1] + data[11] * v.data[2] + data[15] * v.data[3];
    return ret;
}

Mat4 Mat4::operator+(const Mat4& m) const {
    Mat4 ret;

        for(int i = 0; i < 16; i++){
        ret.data[i] = data[i] + m.data[i]; 
    }

    return ret;
}

Mat4& Mat4::operator*=(const Mat4& m){

    Mat4 tmp = (*this)*m;
        for(int i = 0; i < 16; i++){ data[i] = tmp.data[i]; }

    return (*this);

}

Mat4& Mat4::operator+=(const Mat4& m){
        for(int i = 0; i < 16; i++){
        data[i] += m.data[i]; 
    }
    return(*this);
}


Mat4 Mat4::Transpose() const {
    Mat4 ret;

        for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
            ret.data[4*i+j] = data[4*j+i];
        }
    }

    return ret;
}



float Mat4::Determinant() const {

    return 
              data[0*4+0] * data[1*4+1] * ( data[2*4+2] * data[3*4+3] - data[2*4+3] * data[3*4+2] )
            + data[0*4+0] * data[1*4+2] * ( data[2*4+3] * data[3*4+1] - data[2*4+1] * data[3*4+3] )
            + data[0*4+0] * data[1*4+3] * ( data[2*4+1] * data[3*4+2] - data[2*4+2] * data[3*4+1] )
            + data[0*4+1] * data[1*4+0] * ( data[2*4+3] * data[3*4+2] - data[2*4+2] * data[3*4+3] )
            + data[0*4+1] * data[1*4+2] * ( data[2*4+0] * data[3*4+3] - data[2*4+3] * data[3*4+0] )
            + data[0*4+1] * data[1*4+3] * ( data[2*4+2] * data[3*4+0] - data[2*4+0] * data[3*4+2] )
            + data[0*4+2] * data[1*4+0] * ( data[2*4+1] * data[3*4+3] - data[2*4+3] * data[3*4+1] )
            + data[0*4+2] * data[1*4+1] * ( data[2*4+3] * data[3*4+0] - data[2*4+0] * data[3*4+3] )
            + data[0*4+2] * data[1*4+3] * ( data[2*4+0] * data[3*4+1] - data[2*4+1] * data[3*4+0] )
            + data[0*4+3] * data[1*4+0] * ( data[2*4+2] * data[3*4+1] - data[2*4+1] * data[3*4+2] )
            + data[0*4+3] * data[1*4+1] * ( data[2*4+0] * data[3*4+2] - data[2*4+2] * data[3*4+0] )
            + data[0*4+3] * data[1*4+2] * ( data[2*4+1] * data[3*4+0] - data[2*4+0] * data[3*4+1] );
}

Mat4 Mat4::Inverse() const {
    Mat4 ret;

        float mdet = Determinant();

        if ( ::fabsf( mdet ) < 1.0e-10f ){
        printf("WARNING (Mat4): Matrix Not Invertible\n");
        Print();
        ret.LoadIdentity();
        return ret;
    }

        ret.data[0*4+0] = (data[1*4+2]*data[2*4+3]*data[3*4+1] - data[1*4+3]*data[2*4+2]*data[3*4+1] + data[1*4+3]*data[2*4+1]*data[3*4+2] - data[1*4+1]*data[2*4+3]*data[3*4+2] - data[1*4+2]*data[2*4+1]*data[3*4+3] + data[1*4+1]*data[2*4+2]*data[3*4+3]) / mdet;
        ret.data[0*4+1] = (data[0*4+3]*data[2*4+2]*data[3*4+1] - data[0*4+2]*data[2*4+3]*data[3*4+1] - data[0*4+3]*data[2*4+1]*data[3*4+2] + data[0*4+1]*data[2*4+3]*data[3*4+2] + data[0*4+2]*data[2*4+1]*data[3*4+3] - data[0*4+1]*data[2*4+2]*data[3*4+3]) / mdet;
        ret.data[0*4+2] = (data[0*4+2]*data[1*4+3]*data[3*4+1] - data[0*4+3]*data[1*4+2]*data[3*4+1] + data[0*4+3]*data[1*4+1]*data[3*4+2] - data[0*4+1]*data[1*4+3]*data[3*4+2] - data[0*4+2]*data[1*4+1]*data[3*4+3] + data[0*4+1]*data[1*4+2]*data[3*4+3]) / mdet;
        ret.data[0*4+3] = (data[0*4+3]*data[1*4+2]*data[2*4+1] - data[0*4+2]*data[1*4+3]*data[2*4+1] - data[0*4+3]*data[1*4+1]*data[2*4+2] + data[0*4+1]*data[1*4+3]*data[2*4+2] + data[0*4+2]*data[1*4+1]*data[2*4+3] - data[0*4+1]*data[1*4+2]*data[2*4+3]) / mdet;
        ret.data[1*4+0] = (data[1*4+3]*data[2*4+2]*data[3*4+0] - data[1*4+2]*data[2*4+3]*data[3*4+0] - data[1*4+3]*data[2*4+0]*data[3*4+2] + data[1*4+0]*data[2*4+3]*data[3*4+2] + data[1*4+2]*data[2*4+0]*data[3*4+3] - data[1*4+0]*data[2*4+2]*data[3*4+3]) / mdet;
        ret.data[1*4+1] = (data[0*4+2]*data[2*4+3]*data[3*4+0] - data[0*4+3]*data[2*4+2]*data[3*4+0] + data[0*4+3]*data[2*4+0]*data[3*4+2] - data[0*4+0]*data[2*4+3]*data[3*4+2] - data[0*4+2]*data[2*4+0]*data[3*4+3] + data[0*4+0]*data[2*4+2]*data[3*4+3]) / mdet;
        ret.data[1*4+2] = (data[0*4+3]*data[1*4+2]*data[3*4+0] - data[0*4+2]*data[1*4+3]*data[3*4+0] - data[0*4+3]*data[1*4+0]*data[3*4+2] + data[0*4+0]*data[1*4+3]*data[3*4+2] + data[0*4+2]*data[1*4+0]*data[3*4+3] - data[0*4+0]*data[1*4+2]*data[3*4+3]) / mdet;
        ret.data[1*4+3] = (data[0*4+2]*data[1*4+3]*data[2*4+0] - data[0*4+3]*data[1*4+2]*data[2*4+0] + data[0*4+3]*data[1*4+0]*data[2*4+2] - data[0*4+0]*data[1*4+3]*data[2*4+2] - data[0*4+2]*data[1*4+0]*data[2*4+3] + data[0*4+0]*data[1*4+2]*data[2*4+3]) / mdet;
        ret.data[2*4+0] = (data[1*4+1]*data[2*4+3]*data[3*4+0] - data[1*4+3]*data[2*4+1]*data[3*4+0] + data[1*4+3]*data[2*4+0]*data[3*4+1] - data[1*4+0]*data[2*4+3]*data[3*4+1] - data[1*4+1]*data[2*4+0]*data[3*4+3] + data[1*4+0]*data[2*4+1]*data[3*4+3]) / mdet;
        ret.data[2*4+1] = (data[0*4+3]*data[2*4+1]*data[3*4+0] - data[0*4+1]*data[2*4+3]*data[3*4+0] - data[0*4+3]*data[2*4+0]*data[3*4+1] + data[0*4+0]*data[2*4+3]*data[3*4+1] + data[0*4+1]*data[2*4+0]*data[3*4+3] - data[0*4+0]*data[2*4+1]*data[3*4+3]) / mdet;
        ret.data[2*4+2] = (data[0*4+1]*data[1*4+3]*data[3*4+0] - data[0*4+3]*data[1*4+1]*data[3*4+0] + data[0*4+3]*data[1*4+0]*data[3*4+1] - data[0*4+0]*data[1*4+3]*data[3*4+1] - data[0*4+1]*data[1*4+0]*data[3*4+3] + data[0*4+0]*data[1*4+1]*data[3*4+3]) / mdet;
        ret.data[2*4+3] = (data[0*4+3]*data[1*4+1]*data[2*4+0] - data[0*4+1]*data[1*4+3]*data[2*4+0] - data[0*4+3]*data[1*4+0]*data[2*4+1] + data[0*4+0]*data[1*4+3]*data[2*4+1] + data[0*4+1]*data[1*4+0]*data[2*4+3] - data[0*4+0]*data[1*4+1]*data[2*4+3]) / mdet;
        ret.data[3*4+0] = (data[1*4+2]*data[2*4+1]*data[3*4+0] - data[1*4+1]*data[2*4+2]*data[3*4+0] - data[1*4+2]*data[2*4+0]*data[3*4+1] + data[1*4+0]*data[2*4+2]*data[3*4+1] + data[1*4+1]*data[2*4+0]*data[3*4+2] - data[1*4+0]*data[2*4+1]*data[3*4+2]) / mdet;
        ret.data[3*4+1] = (data[0*4+1]*data[2*4+2]*data[3*4+0] - data[0*4+2]*data[2*4+1]*data[3*4+0] + data[0*4+2]*data[2*4+0]*data[3*4+1] - data[0*4+0]*data[2*4+2]*data[3*4+1] - data[0*4+1]*data[2*4+0]*data[3*4+2] + data[0*4+0]*data[2*4+1]*data[3*4+2]) / mdet;
        ret.data[3*4+2] = (data[0*4+2]*data[1*4+1]*data[3*4+0] - data[0*4+1]*data[1*4+2]*data[3*4+0] - data[0*4+2]*data[1*4+0]*data[3*4+1] + data[0*4+0]*data[1*4+2]*data[3*4+1] + data[0*4+1]*data[1*4+0]*data[3*4+2] - data[0*4+0]*data[1*4+1]*data[3*4+2]) / mdet;
        ret.data[3*4+3] = (data[0*4+1]*data[1*4+2]*data[2*4+0] - data[0*4+2]*data[1*4+1]*data[2*4+0] + data[0*4+2]*data[1*4+0]*data[2*4+1] - data[0*4+0]*data[1*4+2]*data[2*4+1] - data[0*4+1]*data[1*4+0]*data[2*4+2] + data[0*4+0]*data[1*4+1]*data[2*4+2]) / mdet;

    return ret;

}


Vex4 Mat4::Row(const int i) const {
    return Vex4(data[i],data[4+i],data[8+i],data[12+i]);
}

void Mat4::Row(const int i, const Vex4& val){
    data[ 0+i] = val.x;
    data[ 4+i] = val.y;
    data[ 8+i] = val.z;
    data[12+i] = val.w;
}


Vex4 Mat4::Column(const int i) const {
    return Vex4(data[4*i+0],data[4*i+1],data[4*i+2],data[4*i+3]);
}

void Mat4::Column(const int i, const Vex4& val){
    data[4*i+0] = val.x;
    data[4*i+1] = val.y;
    data[4*i+2] = val.z;
    data[4*i+3] = val.w;
}

#ifdef WIN32
    void Mat4::glMultMatrix() const {
            glMultMatrixf(data);
    }

    void Mat4::glReadModelview(){
            glGetFloatv(GL_MODELVIEW_MATRIX,data);
    }

    void Mat4::glReadProjection(){
            glGetFloatv(GL_PROJECTION_MATRIX,data);
    }

    void Mat4::glReadTexture(){
            glGetFloatv(GL_TEXTURE_MATRIX,data);
    }

    void Mat4::glSetModelview() const {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glMultMatrixf(data);
    }

    void Mat4::glSetProjection() const {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMultMatrixf(data);
    }

    void Mat4::glSetTexture() const {
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glMultMatrixf(data);
    }
#endif

void Mat4::Print() const {
        for(int i = 0; i < 4; i++){
        printf("[\t%f\t%f\t%f\t%f\t]\n",data[i],data[4+i],data[8+i],data[12+i]);
    }
}

void Mat4::Store(double mat[16]) const {
    for(int i = 0; i < 16; i++){
        mat[i] = (double)data[i];
    }
}

void Mat4::Store(float mat[16]) const {
    for(int i = 0; i < 16; i++){
        mat[i] = data[i];
    }
}

void Mat4::Load(const double mat[16]){
    for(int i = 0; i < 16; i++){
        data[i] = (float)mat[i];
    }
}

void Mat4::Load(const float mat[16]){
    for(int i = 0; i < 16; i++){
        data[i] = mat[i];
    }
}


