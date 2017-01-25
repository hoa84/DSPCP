/*
**  Common Data Library
**  Copyright (C) 2016  Hoa Nguyen, Paul Rosen
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


#include <Data/PhysicsData.h>

#include <iostream>
#include <stdlib.h>

#include <SCI/VexN.h>

using namespace Data;

PhysicsData::PhysicsData( ) : DenseMultiDimensionalData( 0, 0 ) { }

PhysicsData::PhysicsData( const char * fname ) : DenseMultiDimensionalData( 0, 0 ) {
    //Load(fname);
}

PhysicsData::~PhysicsData(){}

bool PhysicsData::Load(const char * fname, int b)
{
        data.clear();
        labels.clear();
        dim_enabled.clear();

        filename = std::string(fname);

        int dim = 0;
        std::vector<float> dtmp;
        char buf[4096];
        FILE * infile = fopen(fname, "r");

        std::cout << "Loading file: " << filename.c_str() << std::endl << std::flush;


        if(infile){
            while( fgets( buf, 4096, infile ) != 0 ){
                ParseString( buf, dtmp );
                if( dim == 0 ){ dim = (int)dtmp.size(); }
            }
            fclose(infile);

            Resize( (int)dtmp.size()/dim, dim );

            for(int i = 0; i < (int)dtmp.size(); i++)
            {
              // SetElement
                SetElement( i/dim, i%dim, dtmp[i] );
            }

            dim_enabled.resize(dimN,true);

            LoadMeta();
            CalculateCorrelation();

            return true;
        }
        return false;
}


float PhysicsData::GetCorrelation( int dim_x, int dim_y ){
    return correlation[ std::make_pair(dim_x,dim_y) ];
}

std::string PhysicsData::GetFilename(){
    return filename;
}

void PhysicsData::LoadMeta( ){
    std::string meta_fname = filename;
    meta_fname.append(std::string(".meta"));
    FILE * infile = fopen(meta_fname.c_str(), "r");
    char buf[1024];

    std::cout << "Loading meta: " << meta_fname.c_str() << std::endl << std::flush;
    if(infile){
        for(int i = 0; i < dimN && fgets( buf, 1024, infile ) != 0; i++ ){
            int j = (int)strlen(buf)-1;
            while( j > 0 && ( buf[j] == '\r' || buf[j] == '\n' || buf[j] == ' ' || buf[j] == '\t' ) ){ buf[j]=0; j--; }
            if( strncmp(buf,"true " ,5) == 0 ){ SetLabel(i,std::string(buf+5)); dim_enabled[i] = true;  }
            if( strncmp(buf,"false ",6) == 0 ){ SetLabel(i,std::string(buf+6)); dim_enabled[i] = false; }
        }
        fclose(infile);
    }
}

void PhysicsData::CopyMeta( const PhysicsData & from ){
    for(int i = 0; i < GetDim() && i < from.GetDim(); i++){
        if( from.isEnabled( i ) )
            Enable(i);
        else
            Disable(i);
        SetLabel( i, from.GetLabel( i ) );
    }
}


void PhysicsData::SaveMeta( ){
    std::string meta_fname = filename;
    meta_fname.append(std::string(".meta"));
    FILE * outfile = fopen(meta_fname.c_str(), "w");

    std::cout << "Saving meta: " << meta_fname.c_str() << std::endl << std::flush;
    if(outfile){
        for(int i = 0; i < dimN; i++ ){
            fprintf(outfile, "%s %s\n", (dim_enabled[i]?"true":"false"), GetLabel(i).c_str() );
        }
        fclose(outfile);
    }
}



void PhysicsData::CalculateCorrelation( ){
    for(int i = 0; i < GetDim(); i++){
        SCI::VexN vi = ExtractDimension( i );
        correlation[std::make_pair(i,i)] = 1.0f;
        for(int j = 0; j < GetDim(); j++){
            SCI::VexN vj = ExtractDimension( j );
            float c = (float)SCI::PearsonCorrelation( vi, vj );
            correlation[std::make_pair(i,j)] = c;
            correlation[std::make_pair(j,i)] = c;
        }
    }
}

void PhysicsData::SetLabel( int _dim, std::string  lbl ){
    if((int)labels.size()<=_dim) labels.resize(_dim+1,std::string("default label"));
    labels[_dim] = lbl;
}

std::string PhysicsData::GetLabel( int _dim ) const {
    if(_dim<(int)labels.size()) return labels[_dim];
    return std::string("default label");
}


std::string PhysicsData::GetLabelParsed( int _dim ) const {
    std::string lbl = GetLabel( _dim );
    char gamma = (char)(227);
    char sigma = (char)(243);
    char nu    = (char)(237);

    lbl = SCI::String::ReplaceTagWithCharacter( lbl, "\\gamma", gamma );
    lbl = SCI::String::ReplaceTagWithCharacter( lbl, "\\sigma", sigma );
    lbl = SCI::String::ReplaceTagWithCharacter( lbl, "\\nu",    nu    );

    return lbl;
}


bool PhysicsData::isEnabled( int dim ) const {
    if( dim < (int)dim_enabled.size() ) return dim_enabled[dim];
    return false;
}

bool PhysicsData::isDisabled( int dim ) const {
    if( dim < (int)dim_enabled.size() ) return !dim_enabled[dim];
    return true;
}

void PhysicsData::Disable( int dim ){
    if( dim < (int)dim_enabled.size() ) dim_enabled[dim] = false;
}

void PhysicsData::Enable( int dim ){
    if( dim < (int)dim_enabled.size() ) dim_enabled[dim] = true;
}


void PhysicsData::ConditionString( char * buf ){
    int j = 0;
    for(int i = 0; buf[i] != 0; i++){
        if( buf[i] == '\t' ) buf[i] = ' ';
        if( buf[i] == '\n' ) buf[i] = ' ';
        if( buf[i] == '\r' ) buf[i] = ' ';
    }
    int i = 0;
    while( buf[i] == ' ' ) i++;
     while( buf[i] != 0 ) {
        buf[j++] = buf[i];
        if( buf[i] != ' ' ){
            i++;
        }
        else{
            while( buf[i] == ' ' ) i++;
        }
    }
    buf[j] = 0;
    while( j > 0 && (buf[j] == 0 || buf[j] == ' ') ) j--;
    buf[j+1] = 0;
}

void PhysicsData::ParseString( char * str, std::vector<float> & vals ){
    ConditionString( str );
    do {
        vals.push_back((float)atof(str));
        str = strstr( str, " " );
        if(str) str++;
    } while(str);
}

// Dimension of each data point
int PhysicsData::GetDim() const {
    return dimN;
}

// Get the size
int PhysicsData::GetElementCount() const {
    return (int)data.size()/dimN;
}

// Get a rough estimate of the size of the data contained in the class
int PhysicsData::GetDataSize() const {
    return sizeof(float)*(int)data.size();
}

std::vector<float> PhysicsData::ExtractDimension( int dim ) const {
    std::vector<float> ret;
    for(int i = 0; i < GetElementCount(); i++){
        ret.push_back( GetElement(i, dim) );
    }
    return ret;
}
