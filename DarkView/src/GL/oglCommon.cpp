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

#include <GL/oglCommon.h>

#include <string.h>

const char * oglWidgets::glErrorString( int errCode ){
    switch (errCode) {
        case GL_INVALID_ENUM:        return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:       return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:   return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:       return "GL_OUT_OF_MEMORY";
        #if !defined(__ANDROID__)
            case GL_STACK_OVERFLOW:  return "GL_STACK_OVERFLOW";
            case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        #endif
    }
    return "";
}

bool oglWidgets::glExtensionSupported(const char* extName){
    return (strstr((const char*)glGetString(GL_EXTENSIONS),extName) != 0);
}

GLint oglWidgets::glGetInteger(GLenum pname){
    GLint ret;
    glGetIntegerv(pname, &ret);
    return ret;
}


void oglWidgets::checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        printf("after %s glError -- %s (0x%x)\n", op, glErrorString(error), error);
    }
}
