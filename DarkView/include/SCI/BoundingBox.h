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

#ifndef SCI_BOUNDINGBOX_H
#define SCI_BOUNDINGBOX_H

#include <SCI/Vex3.h>
#include <SCI/VexN.h>

namespace SCI {

    class BoundingND {
    public:
        VexN bbmin, bbmax;

        BoundingND( UINT32 dim = 0 ){
            if( dim > 0 ){
                bbmin.resize( dim,  FLT_MAX );
                bbmax.resize( dim, -FLT_MAX );
            }
        }

        inline void Expand( const std::vector<float> & p ){
            if( (int)p.size() > GetDimension() ){
                bbmin.resize( (int)p.size(),  FLT_MAX );
                bbmax.resize( (int)p.size(), -FLT_MAX );
            }
            for(int i = 0; i < (int)p.size(); i++){
                bbmin[i] = SCI::Min( bbmin[i], p[i] );
                bbmax[i] = SCI::Max( bbmax[i], p[i] );
            }
        }

        inline void Clear( ){
            for(int i = 0; i < GetDimension(); i++ ){
                bbmin[i] =  FLT_MAX;
                bbmax[i] = -FLT_MAX;
            }
        }

        inline int  GetDimension() const { return (int)bbmin.size(); }
        inline VexN GetCenter()    const { return (bbmin+bbmax)/2.0f; }
        inline VexN GetSize()      const { return (bbmax-bbmin); }

    };

    class BoundingBox {
    public:
        Vex3 bbmin, bbmax;
        BoundingBox(){
            bbmin = VEX3_MAX;
            bbmax = VEX3_MIN;
        }
        BoundingBox(Vex3 bmin, Vex3 bmax){
            bbmin = bmin;
            bbmax = bmax;
        }
        void Expand( Vex3 p ){
            bbmin = Min(bbmin,p);
            bbmax = Max(bbmax,p);
        }
        void Reset(){
            bbmin = VEX3_MAX;
            bbmax = VEX3_MIN;
        }
        void Clear(){
            bbmin = VEX3_MAX;
            bbmax = VEX3_MIN;
        }
        Vex3 GetPoint(int i){
            Vex3 ret;
            ret.x = (((i/1)%2)==0) ? bbmin.x : bbmax.x;
            ret.y = (((i/2)%2)==0) ? bbmin.y : bbmax.y;
            ret.z = (((i/4)%2)==0) ? bbmin.z : bbmax.z;
            return ret;
        }

        void GetDistanceRange( Vex3 pnt, float & dmin, float & dmax ){
            dmin = FLT_MAX;
            dmax = 0;
            for(int i = 0; i < 8; i++){
                dmin = SCI::Min( dmin, ( pnt - GetPoint(i) ).Length() );
                dmax = SCI::Max( dmax, ( pnt - GetPoint(i) ).Length() );
            }
        }
        inline float     GetRadius() const { return (bbmax-GetCenter()).Length(); }
        inline SCI::Vex3 GetCenter() const { return (bbmin+bbmax)/2.0f; }
        inline SCI::Vex3 GetSize() const { return (bbmax-bbmin); }
        inline float     GetMaximumDimensionSize() const {
            SCI::Vex3 siz = GetSize();
            return SCI::Max( siz.x, SCI::Max(siz.y,siz.z) );
        }

    };

    class BoundingRange {
    public:
        float bbmin, bbmax;
        BoundingRange(){
            bbmin =  FLT_MAX;
            bbmax = -FLT_MAX;
        }
        BoundingRange(float bmin, float bmax){
            bbmin = bmin;
            bbmax = bmax;
        }
        void Expand( float p ){
            bbmin = Min(bbmin,p);
            bbmax = Max(bbmax,p);
        }
        void Reset(){
            bbmin =  FLT_MAX;
            bbmax = -FLT_MAX;
        }
        void Clear(){
            bbmin =  FLT_MAX;
            bbmax = -FLT_MAX;
        }
        inline float GetCenter() const { return (bbmin+bbmax)/2.0f; }
        inline float GetSize() const { return (bbmax-bbmin); }

    };

}

#endif // SCI_BOUNDINGBOX_H
