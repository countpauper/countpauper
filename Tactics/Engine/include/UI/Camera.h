#pragma once
#include <cstdint>
#include "Geometry/Coordinate.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include "UI/Bus.h"

namespace Engine
{
    class Camera
    {
    public:
        Camera();

        virtual void Render() const = 0;
        void Move(const Coordinate& newPosition);
        void Move(const Vector& offset);
        void Zoom(double factor);
        void Face(Coordinate target);
        void Turn(const Vector& delta);
        Matrix Projection() const;
    protected:
        Vector vertical{0, 0, 1};
        Coordinate position{0,0,0};
        Coordinate rotation{0,0,0};
        double zoom = 1.0;
        mutable Matrix projection;
    };


    class PerspectiveCamera :
        public Camera,
        public Passenger
    {
    public:
        PerspectiveCamera();
        void Render() const override;
        float fov;
    };

    class TopCamera : public Camera
    {
    public:
        TopCamera();
        void Render() const override;
    };


    // TODO: this (and TrackingCmaera) is more like a cameraman and the camera should be injected
    // for now assume perspective
    class FreeCamera : public PerspectiveCamera
    {
    public:
        FreeCamera();
        void OnMessage(const Message& message) override;
    };

    class TrackingCamera : public PerspectiveCamera
    {
    public:
        TrackingCamera();
        void Track(const Coordinate& newPosition);
        void OnMessage(const Message& message) override;
    private:
        Coordinate target{0,0,0};
    };

}   // ::Engine
