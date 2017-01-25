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

#include <GL/oglTexture2D.h>
#include <iostream>

using namespace oglWidgets;

oglTexture2D::oglTexture2D(void) : oglTexture(GL_TEXTURE_2D) {
    minfilter = GL_NEAREST;
    magfilter = GL_NEAREST;

    wraps = GL_CLAMP;
    wrapt = GL_CLAMP;

    autoMipmap = false;
    borderColor[0] = 0;
    borderColor[1] = 0;
    borderColor[2] = 0;
    borderColor[3] = 0;
}

oglTexture2D::~oglTexture2D(void){ }

bool oglTexture2D::SetBorderColor(float * bcolor){
    borderColor[0] = bcolor[0];
    borderColor[1] = bcolor[1];
    borderColor[2] = bcolor[2];
    borderColor[3] = bcolor[3];
    SetParameter(GL_TEXTURE_BORDER_COLOR,borderColor);
    return true;
}

bool oglTexture2D::SetWrapS(GLint v){
    if(v != GL_CLAMP && v != GL_REPEAT && v != GL_CLAMP_TO_EDGE && v != GL_CLAMP_TO_BORDER && v != GL_MIRRORED_REPEAT){
        std::cerr << "oglTexture2D:" << "Invalid Value" << std::endl << std::flush;
        return false;
    }
    wraps = v;
    SetParameter(GL_TEXTURE_WRAP_S,wraps);
    return true;
}

bool oglTexture2D::SetWrapT(GLint v){
    if(v != GL_CLAMP && v != GL_REPEAT && v != GL_CLAMP_TO_EDGE && v != GL_CLAMP_TO_BORDER && v != GL_MIRRORED_REPEAT){
        std::cerr << "oglTexture2D:" << "Invalid Value" << std::endl << std::flush;
        return false;
    }
    wrapt = v;
    SetParameter(GL_TEXTURE_WRAP_T,wrapt);
    return true;
}

bool oglTexture2D::SetWrapST(GLint v_s, GLint v_t){
    return SetWrapS(v_s) && SetWrapT(v_t);
}

bool oglTexture2D::SetAutoMipmap(bool val){
    autoMipmap = (val)?GL_TRUE:GL_FALSE;
    SetParameter(GL_GENERATE_MIPMAP,autoMipmap);
    return true;
}

bool oglTexture2D::TexImage2DMipmap(GLenum iformat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels){

#ifdef WIN32

    glGetError(); // clear errors

    Enable();
    Bind();
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,minfilter);
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,magfilter);
        glTexParameteri(target,GL_TEXTURE_WRAP_S,wraps);
        glTexParameteri(target,GL_TEXTURE_WRAP_T,wrapt);
        gluBuild2DMipmaps(target, iformat, w, h, format, type, pixels);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
#else
    return TexImage2D( iformat, w, h, format, type, pixels );
#endif

}

bool oglTexture2D::TexImage2D(GLenum iformat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels){
    glGetError(); // clear errors

    Enable();
    Bind();
        glTexParameterfv(target,GL_TEXTURE_BORDER_COLOR,borderColor);
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,minfilter);
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,magfilter);
        glTexParameteri(target,GL_TEXTURE_WRAP_S,wraps);
        glTexParameteri(target,GL_TEXTURE_WRAP_T,wrapt);
        glTexParameteri(target,GL_GENERATE_MIPMAP,autoMipmap);
        glTexImage2D(target,0,iformat,w,h,GL_FALSE,format,type,pixels);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
}

bool oglTexture2D::TexImage2DBorder(GLenum iformat, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels){
    glGetError(); // clear errors

    Enable();
    Bind();
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,minfilter);
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,magfilter);
        glTexParameteri(target,GL_TEXTURE_WRAP_S,wraps);
        glTexParameteri(target,GL_TEXTURE_WRAP_T,wrapt);
        glTexParameteri(target,GL_GENERATE_MIPMAP,autoMipmap);
        glTexImage2D(target,0,iformat,w,h,GL_TRUE,format,type,pixels);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
}

bool oglTexture2D::TexSubImage2D(GLint xoffset, GLint yoffset, GLsizei w, GLsizei h, GLenum format, GLenum type, const void *pixels){
    glGetError(); // clear errors

    Enable();
    Bind();
        glTexParameterfv(target,GL_TEXTURE_BORDER_COLOR,borderColor);
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,minfilter);
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,magfilter);
        glTexParameteri(target,GL_TEXTURE_WRAP_S,wraps);
        glTexParameteri(target,GL_TEXTURE_WRAP_T,wrapt);
        glTexParameteri(target,GL_GENERATE_MIPMAP,autoMipmap);
        glTexSubImage2D(target,0,xoffset,yoffset,w,h,format,type,pixels);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
}


bool oglTexture2D::CopyTexImage2D(GLenum iformat, GLint x, GLint y, GLsizei w, GLsizei h){
    glGetError(); // clear errors

    Enable();
    Bind();
        glTexParameterfv(target,GL_TEXTURE_BORDER_COLOR,borderColor);
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,minfilter);
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,magfilter);
        glTexParameteri(target,GL_TEXTURE_WRAP_S,wraps);
        glTexParameteri(target,GL_TEXTURE_WRAP_T,wrapt);
        glTexParameteri(target,GL_GENERATE_MIPMAP,autoMipmap);
        glCopyTexImage2D(target,0,iformat,x,y,w,h,0);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
}

bool oglTexture2D::CopyTexSubImage2D(GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei w, GLsizei h){

    glGetError(); // clear errors

    Enable();
    Bind();
        glTexParameterfv(target,GL_TEXTURE_BORDER_COLOR,borderColor);
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,minfilter);
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,magfilter);
        glTexParameteri(target,GL_TEXTURE_WRAP_S,wraps);
        glTexParameteri(target,GL_TEXTURE_WRAP_T,wrapt);
        glTexParameteri(target,GL_GENERATE_MIPMAP,autoMipmap);
        glCopyTexSubImage2D(target,0,xoffset,yoffset,x,y,w,h);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
}

bool oglTexture2D::GetTexture(GLenum format, GLenum type, void *pixels) const {

    glGetError(); // clear errors

    Enable();
    Bind();
        glGetTexImage(target,0,format,type,pixels);
    Disable();

    GLenum err = glGetError();

    if(err != 0){
        std::cerr << "oglTexture2D:" << glErrorString(err) << std::endl << std::flush;
        return false;
    }
    return true;
}

void oglTexture2D::DrawTexture() const {
    Enable();
    Bind();
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(-1,-1,0);
            glTexCoord2f(1,0); glVertex3f( 1,-1,0);
            glTexCoord2f(1,1); glVertex3f( 1, 1,0);
            glTexCoord2f(0,1); glVertex3f(-1, 1,0);
        glEnd();
    Disable();
}
