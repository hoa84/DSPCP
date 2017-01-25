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

#ifndef SCI_UTILITY_H
#define SCI_UTILITY_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <vector>
#include <iostream>
#include <sstream>

#ifndef WIN32
    #include <sys/types.h>
    #ifndef __APPLE__
        #include <malloc.h>
    #endif
#endif

#undef PI

namespace SCI {

    const float PI = 3.14159265f;

    typedef char                 INT8;
    typedef short                INT16;
    typedef int                  INT32;
    #ifdef WIN32
        typedef __int64             INT64;
    #else
        typedef int64_t          INT64;
    #endif

    typedef unsigned char        UINT8;
    typedef unsigned short       UINT16;
    typedef unsigned int         UINT32;
    #ifdef WIN32
        typedef unsigned __int64 UINT64;
    #else
        typedef u_int64_t        UINT64;
    #endif

    typedef float                FLOAT32;
    typedef double               FLOAT64;


    inline float Min( float v0, float v1 ){ return (v0<v1)?v0:v1;  }
    inline float Max( float v0, float v1 ){ return (v0>v1)?v0:v1;  }
    inline int   Min(   int v0,   int v1 ){ return (v0<v1)?v0:v1; }
    inline int   Max(   int v0,   int v1 ){ return (v0>v1)?v0:v1; }

    inline float Min( float v0, float v1, float v2 ){ return Min( v0, Min(v1,v2) ); }
    inline float Max( float v0, float v1, float v2 ){ return Max( v0, Max(v1,v2) ); }
    inline int   Min(   int v0,   int v1,   int v2 ){ return Min( v0, Min(v1,v2) ); }
    inline int   Max(   int v0,   int v1,   int v2 ){ return Max( v0, Max(v1,v2) ); }
    inline int   Mid(   int v0,   int v1,   int v2 ){ return ( (v0 <= v1 && v0 >= v2) || (v0 >= v1 && v0 <= v2) ) ? v0 : Mid(v1,v2,v0); }


    inline float DEG2RAD(float deg){ return ((deg)/180.0f*PI); }
    inline float RAD2DEG(float rad){ return ((rad)*180.0f/PI); }

    //inline INT8  Clamp(INT8  val, INT8  a, INT8  b){    return ((val <= a) ? a : ((val >= b) ? b : val));    }
    //inline INT16 Clamp(INT16 val, INT16 a, INT16 b){    return ((val <= a) ? a : ((val >= b) ? b : val));    }
    //inline INT64 Clamp(INT64 val, INT64 a, INT64 b){    return ((val <= a) ? a : ((val >= b) ? b : val));    }
    inline    int Clamp(    int val,    int a,    int b){ return ((val <= a) ? a : ((val >= b) ? b : val));    }
    inline  float Clamp(  float val,  float a,  float b){ return ((val <= a) ? a : ((val >= b) ? b : val));    }
    inline double Clamp( double val, double a, double b){ return ((val <= a) ? a : ((val >= b) ? b : val));    }

    #ifdef WIN32
        inline char * strdup( const char * s ){ return ::_strdup(s); }
    #else
        inline char * strdup( const char * s ){ return ::strdup(s); }
    #endif



    #ifdef _WIN32
        #include <io.h>
        #include <fcntl.h>
        inline void SET_BINARY_MODE(FILE * file){
            _setmode( _fileno( file ), _O_BINARY );
        }
    #else
        inline void SET_BINARY_MODE(FILE *){ }
    #endif


    inline float fabs(float v){ return ::fabsf(v); }
    inline float fabsf(float v){ return ::fabsf(v); }
    inline float tanf(float v){ return ::tanf(v);  }
    inline float cosf(float v){ return ::cosf(v);  }
    inline float sinf(float v){ return ::sinf(v);  }

    inline float lerp( float v0, float v1, float t ){
        return (v0)*(1.0f-t)+(v1)*(t);
    }


    inline std::vector<float> lerp( const std::vector<float> & v0, const std::vector<float> & v1, float t ){
        std::vector<float> ret;
        for(int i = 0; i < (int)v0.size() && i < (int)v1.size(); i++){
            ret.push_back( (v0[i])*(1.0f-t)+(v1[i])*(t) );
        }
        return ret;
    }


    template<class A, class B, class C, class D>
    class quadruple {
    public:
        A first;
        B second;
        C third;
        D fourth;

        quadruple(){ }
        quadruple(A _first, B _second, C _third, D _fourth) : first(_first), second(_second), third(_third), fourth(_fourth){ }

        bool operator < ( const quadruple<A,B,C,D> right ) const {
            if( first  < right.first  ) return true;
            if( first  > right.first  ) return false;
            if( second < right.second ) return true;
            if( second > right.second ) return false;
            if( third  < right.third  ) return true;
            if( third  > right.third  ) return false;
            if( fourth < right.fourth ) return true;
            return false;
        }
    };

    template<class _Ty1, class _Ty2, class _Ty3, class _Ty4>
    inline
    quadruple<_Ty1, _Ty2, _Ty3, _Ty4> make_quadruple(_Ty1 _Val1, _Ty2 _Val2, _Ty3 _Val3, _Ty4 _Val4)
    {
        return (quadruple<_Ty1, _Ty2, _Ty3, _Ty4>(_Val1, _Val2, _Val3, _Val4));
    }


    inline size_t WriteVariableLength( UINT64 x, FILE * outfile ) {
        size_t write_n = 0;
        int i, j;
        unsigned char out;

        for (i = 9; i > 0; i--) {
            if (x & 127ULL << i * 7) break;
        }

        for (j = 0; j <= i; j++){
            out = ((x >> ((i - j) * 7)) & 127) | 128;
            if(i==j) out ^= 128;
            write_n += fwrite( &out, sizeof(unsigned char), 1, outfile );
        }

        return write_n;
    }

    inline UINT64 ReadVariableLength( FILE * infile ) {
        UINT64 r = 0;
        unsigned char in;

        do {
            if( fread( &in, sizeof(unsigned char), 1, infile ) != 1 ) return 0xffffffffffffffff;
            r = (r << 7) | (UINT64)(in & 127);
        } while (in & 128);

        return r;
    }


    /*
    // variable length encoding

    void to_seq(uint64_t x, uint8_t *out)
    {
        int i, j;
        for (i = 9; i > 0; i--) {
            if (x & 127ULL << i * 7) break;
        }
        for (j = 0; j <= i; j++)
            out[j] = ((x >> ((i - j) * 7)) & 127) | 128;

        out[i] ^= 128;
    }

    uint64_t from_seq(uint8_t *in)
    {
        uint64_t r = 0;

        do {
            r = (r << 7) | (uint64_t)(*in & 127);
        } while (*in++ & 128);

        return r;
    }
    */

    inline void PrintLicense( const char * software_title, const char * copyright_holder, const char * copyright_year ){
        std::stringstream ss;
        ss << std::endl;
        ss << "****************************************************************************" << std::endl;
        ss << "**  " << software_title;
        for(int i = (int)strlen(software_title); i < 68; i++){ ss << " "; }
        ss <<                                                                         "  **" << std::endl;
        ss << "**  Copyright (C) ";
        ss << copyright_year << "  " << copyright_holder;
        for(int i = (int)strlen(copyright_year) + (int)strlen(copyright_holder); i < 52; i++){ ss << " "; }
        ss <<                                                                         "  **" << std::endl;
        ss << "**                                                                        **" << std::endl;
        ss << "**  This program is free software: you can redistribute it and/or modify  **" << std::endl;
        ss << "**  it under the terms of the GNU General Public License as published by  **" << std::endl;
        ss << "**  the Free Software Foundation, either version 3 of the License, or     **" << std::endl;
        ss << "**  (at your option) any later version.                                   **" << std::endl;
        ss << "**                                                                        **" << std::endl;
        ss << "**  This program is distributed in the hope that it will be useful,       **" << std::endl;
        ss << "**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **" << std::endl;
        ss << "**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **" << std::endl;
        ss << "**  GNU General Public License for more details.                          **" << std::endl;
        ss << "**                                                                        **" << std::endl;
        ss << "**  You should have received a copy of the GNU General Public License     **" << std::endl;
        ss << "**  along with this program.  If not, see <http://www.gnu.org/licenses/>. **" << std::endl;
        ss << "****************************************************************************" << std::endl;
        //std::cerr << ss.str() << std::endl << std::flush;
    }


    inline bool FileExists( const char * fname ){
        FILE * infile = fopen(fname,"rb");
        if(infile == 0){ return false; }
        fclose(infile);
        return true;
    }

    namespace String {

        inline char * ReplaceTagWithCharacter( char * str, const char * tag, char chr ){
            char * p = 0;
            while( (p = strstr( str, tag )) ){
                std::stringstream ss;
                *p = 0;
                ss << str << chr << (p+strlen(tag));
                strcpy( str, ss.str().c_str() );
            }
            return str;
        }

        inline std::string ReplaceTagWithCharacter( std::string str, std::string tag, char chr ){
            char * tmp = strdup( str.c_str() );
            str = ReplaceTagWithCharacter( tmp, tag.c_str(), chr );
            free(tmp);
            return str;
        }

        inline std::string ReplaceTagWithCharacter( std::string str, const char * tag, char chr ){
            return ReplaceTagWithCharacter( str, std::string(tag), chr );
        }


    }

}

#endif // SCI_UTILITY_H
