/*
**  Common OpenGL Support Library
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

#include <GL/oglFont.h>
#include <GL/oglCommon.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include <QString>
#include <QFile>

using namespace oglWidgets;

oglFont::oglFont( ) {
    for(int i = 0; i < 256; i++){
        vertN[i] = 0;
        chrW[i]  = 0;
        verts[i] = 0;
    }
}

oglFont::~oglFont( ){
    for(int i = 0; i < 256; i++){
        if(verts[i]) delete verts[i];
    }
}

bool oglFont::Load( const char * fname ){
    //QFile file( QString(fname) );
    QString qfname = QString(fname);
    QFile file(qfname);

    if(!file.open( QIODevice::ReadOnly )){
        printf("oglFont: Cannot open file %s\n",fname);
        return false;
    }

    for(int i = 0; i < 256; i++){
        if(verts[i]) delete verts[i];
        vertN[i] = 0;
        chrW[i]  = 0;
        verts[i] = 0;
    }

    for(int i = 0; i < 256; i++){
        file.read( (char*)&vertN[i], sizeof(int) *  1 );
        file.read( (char*)&chrW[i],  sizeof(float)* 1 );
        verts[i] = new float[3*vertN[i]];
        file.read( (char*)verts[i],  sizeof(float) * 3*vertN[i] );
    }
    file.close();

    printf("oglFont: Loaded %s\n",fname);

    return true;

    /*
    FILE * infile = fopen(fname,"rb");
    if(!infile){
        printf("oglFont: Cannot open file %s\n",fname);
        return false;
    }

    for(int i = 0; i < 256; i++){
        if(verts[i]) delete verts[i];
        vertN[i] = 0;
        chrW[i]  = 0;
        verts[i] = 0;
    }

    for(int i = 0; i < 256; i++){
        fread( &vertN[i], sizeof(int),   1,          infile );
        fread( &chrW[i],  sizeof(float), 1,          infile );
        verts[i] = new float[3*vertN[i]];
        fread( verts[i],  sizeof(float), 3*vertN[i], infile );
    }
    fclose(infile);

    printf("oglFont: Loaded %s\n",fname);

    return true;
    */
}


void oglFont::Print( char chr ){
    int idx = chr;
    if(idx < 0){ idx += 256; }

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, verts[idx] );
    glDrawArrays( GL_TRIANGLES, 0, vertN[idx] );
    glTranslatef( chrW[idx], 0, 0 );
    glDisableClientState( GL_VERTEX_ARRAY );
}

void oglFont::Print( const char * chr ){
    for(int i = 0; chr[i] != 0; i++){
        Print(chr[i]);
    }
}

void oglFont::Printf(const char *fmt, ...)
{
    char        text[1024] = "";        // Holds Our String
    va_list        ap;                // Pointer To List Of Arguments

    if (fmt == NULL) return;                // If There's No Text

    va_start(ap, fmt);                    // Parses The String For Variables
        vsprintf(text+strlen(text), fmt, ap);        // And Converts Symbols To Actual Numbers
    va_end(ap);                        // Results Are Stored In Text

    Print( text );
}

float oglFont::GetWidth( char chr ){
    int idx = chr;
    if(idx < 0){ idx += 256; }
    return chrW[idx];
}

float oglFont::GetWidth( const char * chr ){
    float width = 0;
    for(int i = 0; chr[i] != 0; i++){
        width += GetWidth(chr[i]);
    }
    return width;
}

float oglFont::GetWidthf(const char *fmt, ...){
    char        text[1024] = "";        // Holds Our String
    va_list        ap;                // Pointer To List Of Arguments

    if (fmt == NULL) return 0;                // If There's No Text

    va_start(ap, fmt);                    // Parses The String For Variables
        vsprintf(text+strlen(text), fmt, ap);        // And Converts Symbols To Actual Numbers
    va_end(ap);                        // Results Are Stored In Text

    return GetWidth( text );
}


