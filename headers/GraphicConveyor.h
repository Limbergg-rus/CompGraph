//
// Created by Ренат Асланов on 23.10.2024.
//

#ifndef GRAPHICCONVEYOR_H
#define GRAPHICCONVEYOR_H

#include "Vector3D.h"
#include "Matrix4D.h"

class GraphicConveyor
{
public:
    static Matrix4D rotate_scale_translate();

    static Matrix4D lookAt(Vector3D eye, Vector3D target);

    static Matrix4D lookAt(Vector3D eye, Vector3D target, Vector3D up);

    static Matrix4D perspective(const float fov,
            const float aspectRatio,
            const float nearPlane,
            const float farPlane);
};

#endif //GRAPHICCONVEYOR_H