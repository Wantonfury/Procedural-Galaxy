#version 330 core

uniform mat4 worldTransform;
uniform float heightOfNearPlane;

layout(location = 0) in vec3 position;
layout (location = 1) in vec4 colourFrag;

out vec4 colour;
out float fSize;

void main()
{
    gl_Position = worldTransform * vec4(position, 1.0);//vec4(position, 1.0) * worldTransform;//worldTransform * position;

    float size = (heightOfNearPlane * 1000000.0) / gl_Position.w;
    colour = colourFrag;

    float brightness = dot(colour.xyz, vec3(0.2126, 0.7152, 0.0722));
    if (brightness < 1.0 && size < 0.0015) gl_PointSize = 0.0;
    else gl_PointSize = max(size, 1.5);
}