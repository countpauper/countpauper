#include "Utility/Error.h"
#include <GL/gl.h>
#include <stdexcept>

namespace Engine
{
    void CheckGLError()
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: "+std::to_string(err));
    }

}
