#version 330 core

layout (location = 0) in vec3 aPos;

uniform float radius;
uniform float heightOfNearPlane;
uniform mat4 worldTransform;

//out vec3 fPosition;

void main()
{
    //fPosition = aPos;
    //fPosition = (worldTransform * vec4(aPos, 1.0)).xyz / radius;
    gl_Position = worldTransform * vec4(aPos, 1.0);

    float size = (heightOfNearPlane * radius) / gl_Position.w;
    //if (size < 1.5) size = 1.5;
    //if (size < 0.002) size = 0.0;
    //else if (size < 1.5) size = 1.5;

    //ignore = 0;
    //if (size >= 80) ignore = 1;
    gl_PointSize = size;
}