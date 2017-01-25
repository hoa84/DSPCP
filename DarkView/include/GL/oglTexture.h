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


#ifndef OGLWIDGETS_OGLTEXTURE_H
#define OGLWIDGETS_OGLTEXTURE_H

#include <GL/oglCommon.h>

namespace oglWidgets {
    class oglTexture
    {
    protected:
        GLuint tid;
        GLenum target;

        GLint minfilter;
        GLint magfilter;

        void SetParameter(GLenum param, GLint val);
        void SetParameter(GLenum param, GLfloat * val);

    public:
        oglTexture(GLenum target);
        virtual ~oglTexture(void);

        virtual bool SetMinFilter(GLint v);
        virtual bool SetMagFilter(GLint v);
        virtual bool SetMinMagFilter(GLint v_min, GLint v_mag);

        virtual bool   isTextureValid() const ;

        virtual GLuint GetTexID();

        virtual void   Enable()  const;
        virtual void   Bind();
        virtual void   Bind()       const;
        virtual void   Disable() const;
    };
}

#endif // OGLWIDGETS_OGLTEXTURE_H
