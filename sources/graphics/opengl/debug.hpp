#ifndef STRAITX_OPENGL_DEBUG_HPP
#define STRAITX_OPENGL_DEBUG_HPP

#include "core/os/opengl.hpp"
#include "core/log.hpp"
#include "core/env/compiler.hpp"

static inline void glClearErrors(){
    while(glGetError() != GL_NO_ERROR);
}

static inline void glGetErrors(int line, const char *function){
    GLenum error;
    while((error = glGetError())){
        LogError("OpenGL: %: at line % from:\n%",error, line, function);
    }
}

#define GL(x) do{x; glGetErrors(SX_THIS_LINE, SX_THIS_FUNCTION);}while(0)

#endif//STRAITX_OPENGL_DEBUG_HPP