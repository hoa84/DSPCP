/*
**  Common Data Library
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

#ifndef DATA_DENSEMULTIDIMENSIONALDATA_H
#define DATA_DENSEMULTIDIMENSIONALDATA_H

#include <Data/MultiDimensionalData.h>

namespace Data {

    // Base class for volume holding data containers
    class DenseMultiDimensionalData : public MultiDimensionalData {
    public:
        DenseMultiDimensionalData( int _elemN = 0, int _dimN = 0 );

        // Get a rough estimate of the size of the data contained in the class
        virtual int GetDataSize() const ;

        virtual void Resize( int _elemN, int _dimN );

        // Various functions for setting values
        virtual void SetElement( int elem_id, const std::vector<float> & val );
        virtual void SetElement( int elem_id, int dim, float val );
        virtual void SetElement( int elem_id, const float  * val );
        virtual void SetElement( int elem_id, const double * val );

        // Various functions for getting values
        virtual SCI::VexN GetElement( int elem_id )                 const ;
        virtual float     GetElement( int elem_id, int dim )        const ;
        virtual void      GetElement( int elem_id, float  * space ) const ;
        virtual void      GetElement( int elem_id, double * space ) const ;

        virtual float              GetMaximumValue( int dim = -1 ) ;
        virtual float              GetMinimumValue( int dim = -1 ) ;

    protected:
        std::vector<float> data;

        void RecalculateMinumumAndMaximum();

    };



}


#endif // DATA_DENSEMULTIDIMENSIONALDATA_H
