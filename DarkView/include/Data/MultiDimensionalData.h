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

#ifndef DATA_MULTIDIMENSIONALDATA_H
#define DATA_MULTIDIMENSIONALDATA_H

#include <SCI/VexN.h>

namespace Data {

    // Base class for volume holding data containers
    class MultiDimensionalData {
    public:
        MultiDimensionalData( int _elemN, int _dimN );
        virtual ~MultiDimensionalData( );

        // Dimension of each data point
        virtual int GetDimension() const ;

        virtual void Resize( int _elemN, int _dimN );

        // Total number of elements
        virtual int GetElementCount() const ;

        // Get a rough estimate of the size of the data contained in the class
        virtual int GetDataSize() const = 0;

        // Various functions for setting values
        virtual void SetElement( int elem_id, const std::vector<float> & val ) = 0;
        virtual void SetElement( int elem_id, int dim, float val )       = 0;
        virtual void SetElement( int elem_id, const float  * val )             = 0;
        virtual void SetElement( int elem_id, const double * val )             = 0;

        // Various functions for getting values
        virtual SCI::VexN GetElement( int elem_id )                 const = 0;
        virtual float     GetElement( int elem_id, int dim )        const = 0;
        virtual void      GetElement( int elem_id, float  * space ) const = 0;
        virtual void      GetElement( int elem_id, double * space ) const = 0;

        virtual float     GetMaximumValue( int dim = -1 ) const ;
        virtual float     GetMinimumValue( int dim = -1 ) const ;

    protected:
        int     dimN;
        int     elemN;
        std::vector<float> min_dval;
        std::vector<float> max_dval;
        float min_val, max_val;

    };
}

#endif // DATA_MULTIDIMENSIONALDATA_H
