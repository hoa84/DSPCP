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

#include <GL/oglTexture.h>
#include <iostream>

using namespace oglWidgets;

oglTexture::oglTexture(GLenum _target){
    tid = 0xffffffff;
    target = _target;
    minfilter = GL_NEAREST;
    magfilter = GL_NEAREST;
}

oglTexture::~oglTexture(void){
    if(isTextureValid()){
        glDeleteTextures(1,&tid);
    }
}

void oglTexture::SetParameter(GLenum param, GLint val){
    if(isTextureValid()){
        Enable();
        Bind();
            glTexParameteri(target,param,val);
        Disable();
    }
}

void oglTexture::SetParameter(GLenum param, GLfloat * val){
    if(isTextureValid()){
        Enable();
        Bind();
            glTexParameterfv(target,param,val);
        Disable();
    }
}

bool oglTexture::isTextureValid() const { 
    return tid != 0xffffffff;
}

GLuint oglTexture::GetTexID(){
    if(!isTextureValid()){
        glGenTextures(1,&tid);
    }
    return tid;
}

void oglTexture::Bind() {    
    if(!isTextureValid()){
        glGenTextures(1,&tid);
    }
    glBindTexture(target,tid);
}

void oglTexture::Bind() const {    
    if(isTextureValid()){
        glBindTexture(target,tid);
    }
}

void oglTexture::Enable() const {    
    glEnable(target);
}

void oglTexture::Disable() const {    
    glDisable(target);
}


bool oglTexture::SetMinFilter(GLint v){
    if(v != GL_NEAREST && v != GL_LINEAR && v != GL_NEAREST_MIPMAP_NEAREST && v != GL_LINEAR_MIPMAP_NEAREST && v != GL_NEAREST_MIPMAP_LINEAR && v != GL_LINEAR_MIPMAP_LINEAR){
        std::cerr << "oglTexture:" << "Invalid Value" << std::endl << std::flush;
        return false;
    }
    minfilter = v;
    SetParameter(GL_TEXTURE_MIN_FILTER,minfilter);
    return true;
}

bool oglTexture::SetMagFilter(GLint v){
    if(v != GL_NEAREST && v != GL_LINEAR){
        std::cerr << "oglTexture:" << "Invalid Value" << std::endl << std::flush;
        return false;
    }
    magfilter = v;
    SetParameter(GL_TEXTURE_MAG_FILTER,magfilter);
    return true;
}

bool oglTexture::SetMinMagFilter(GLint v_min, GLint v_mag){
    if(v_min!= GL_NEAREST && v_min!= GL_LINEAR && v_min!= GL_NEAREST_MIPMAP_NEAREST && v_min!= GL_LINEAR_MIPMAP_NEAREST && v_min!= GL_NEAREST_MIPMAP_LINEAR && v_min!= GL_LINEAR_MIPMAP_LINEAR){
        std::cerr << "oglTexture:" << "Invalid Min Value" << std::endl << std::flush;
        return false;
    }
    if(v_mag != GL_NEAREST && v_mag != GL_LINEAR){
        std::cerr << "oglTexture:" << "Invalid Mag Value" << std::endl << std::flush;
        return false;
    }
    minfilter = v_min;
    magfilter = v_mag;
    SetParameter(GL_TEXTURE_MIN_FILTER,minfilter);
    SetParameter(GL_TEXTURE_MAG_FILTER,magfilter);
    return true;
}



