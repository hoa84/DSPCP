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

#ifndef DATA_PHYSICSDATA_H
#define DATA_PHYSICSDATA_H

#include <vector>
#include <string>
#include <map>

#include <float.h>

#include <SCI/Utility.h>
#include <Data/DenseMultiDimensionalData.h>

namespace Data {
    class PhysicsData : public DenseMultiDimensionalData {

    public:
        PhysicsData( );
        PhysicsData( const char * fname );
        ~PhysicsData( );

        bool Load(const char * fname , int b);
        bool ReLoad( const char * fname);

        std::string GetFilename();

        void LoadMeta( );
        void SaveMeta( );
        void CopyMeta( const PhysicsData & from );

        // Dimension of each data point
        virtual int GetDim() const ;

        // Get the size
        int GetElementCount() const ;

        // Get a rough estimate of the size of the data contained in the class
        virtual int GetDataSize() const ;

        virtual void SetLabel( int dim, std::string  lbl );
        virtual std::string GetLabel( int dim ) const ;
        virtual std::string GetLabelParsed( int dim ) const ;

        virtual std::vector<float> ExtractDimension( int dim ) const ;

        float GetCorrelation( int dim_x, int dim_y );

        bool isEnabled( int dim ) const ;
        bool isDisabled( int dim ) const ;

        void Disable( int dim );
        void Enable( int dim );

    protected:
        std::map< std::pair<int,int>, float > correlation;
        std::string                           filename;
        std::vector<bool>                     dim_enabled;
        std::vector<std::string>              labels;

        void ConditionString( char * buf );
        void ParseString( char * str, std::vector<float> & vals );
        void CalculateCorrelation( );
    };
}

#endif // DATA_PHYSICSDATA_H
