#pragma once
#include <GL/GL.h>
template<typename T> T glGet(GLenum what)
{
    static_assert(false); // invalid type
    return T();
}

template<> int glGet(GLenum what);
template<> bool glGet(GLenum what);
template<> double glGet(GLenum what);
template<> float glGet(GLenum what);

struct glTemporary
{
    GLenum option = 0;
    bool enabled;
    glTemporary(GLenum option, bool enable);
    glTemporary(const glTemporary&) = delete;
    glTemporary& operator=(const glTemporary&) = delete;
    glTemporary(glTemporary&& other);
    glTemporary& operator=(glTemporary&& other);

    static glTemporary Enable(GLenum option);
    static glTemporary Disable(GLenum option);
    ~glTemporary();
};
