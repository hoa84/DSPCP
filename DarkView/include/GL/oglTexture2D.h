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


#ifndef OGLWIDGETS_OGLTEXTURE2D_H
#define OGLWIDGETS_OGLTEXTURE2D_H

#include <GL/oglCommon.h>
#include <GL/oglTexture.h>

namespace oglWidgets {

    class oglTexture2D : public oglTexture
    {
    protected:
        GLint wraps;
        GLint wrapt;
        GLint autoMipmap;
        float borderColor[4];

    public:
        oglTexture2D(void);
        virtual ~oglTexture2D(void);

        bool SetWrapS(GLint v);
        bool SetWrapT(GLint v);
        bool SetWrapST(GLint v_s, GLint v_t);

        #ifndef __ANDROID__
            bool SetAutoMipmap(bool val);
            bool SetBorderColor(float * bcolor);
        #endif

        inline GLint GetWrapS(){ return wraps; }
        inline GLint GetWrapT(){ return wrapt; }

        virtual bool TexImage2DMipmap(GLenum iformat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels);
        virtual bool TexImage2D(GLenum iformat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels);
        virtual bool TexImage2DBorder(GLenum iformat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels);
        virtual bool TexSubImage2D(GLint xoffset, GLint yoffset, GLsizei w, GLsizei h, GLenum format, GLenum type, const void *pixels);

        virtual bool CopyTexImage2D(GLenum iformat, GLint x, GLint y, GLsizei w, GLsizei h);
        virtual bool CopyTexSubImage2D(GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei w, GLsizei h);

        #ifndef __ANDROID__
            virtual bool GetTexture(GLenum format, GLenum type, void *pixels) const ;
        #endif

        #ifndef __ANDROID__
            virtual void DrawTexture() const ;
        #endif
    };
}

#endif // OGLWIDGETS_OGLTEXTURE2D_H
