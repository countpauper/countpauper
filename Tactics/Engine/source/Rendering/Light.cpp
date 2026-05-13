#include "Rendering/Light.h"
#include <GL/gl.h>

namespace Engine
{
    Light::Light() :
        id(GL_LIGHT0),
        power(1.0),
        directional(0.0f),      // hardcoded for sun. position.w = 0.0 means parallel rays
        position(0,0,0),
        diffusePower(0.8f),     // hardcoded for sun
        diffuse(RGBA::white),
        ambientPower(0.2f),     // hardcoded for sun, strong shadows
        ambient(RGBA::white),
        specularPower(1.0f),
        specular(RGBA::white)
    {
    }
    void Light::Render() const
    {
        if (power<=0.0f)
        {
            glDisable(id);   // TODO: share light id resource only between lights that are on
        }
        else
        {
            if (directional)
            {   // sun and stuff
                glLightf(id, GL_CONSTANT_ATTENUATION, 1.0f);
                glLightf(id, GL_LINEAR_ATTENUATION, 0.0f);
                glLightf(id, GL_QUADRATIC_ATTENUATION, 0.0f);
            }
            else
            {
                glLightf(id, GL_CONSTANT_ATTENUATION, 0.1f);
                glLightf(id, GL_LINEAR_ATTENUATION, 0.05f);
                glLightf(id, GL_QUADRATIC_ATTENUATION, 0.01f);
            }

            glEnable(id);
            GLfloat light_position[] = { GLfloat(position.X()), GLfloat(position.Y()), GLfloat(position.Z()), GLfloat(directional) };
            glLightfv(id, GL_POSITION, light_position);

            GLfloat light_ambient[] = { power * ambientPower * float(ambient.r)/255.0f, power * ambientPower * float(ambient.g)/255.0f, power * ambientPower * float(ambient.b)/255.0f, float(ambient.a)/255.0f };
            glLightfv(id, GL_AMBIENT, light_ambient);

            GLfloat light_diffuse[] = { power * diffusePower * float(diffuse.r)/255.0f, power * diffusePower * float(diffuse.g)/255.0f, power * diffusePower * float(diffuse.b)/255.0f, float(diffuse.a)/255.0f };
            glLightfv(id, GL_DIFFUSE, light_diffuse);

            GLfloat light_specular[] = { power * specularPower * float(specular.r)/255.0f, power * specularPower * float(specular.g)/255.0f, power * specularPower * float(specular.b)/255.0f, float(specular.a)/255.0f };
            glLightfv(id, GL_SPECULAR, light_specular);
        }
    }
    void Light::On()
    {
        power = 1.0f;
    }
    void Light::Off()
    {
        power = 0.0f;
    }

    void Light::Move(const Coordinate& newPosition)
    {
        position = newPosition;
    }


}   // ::Engine
