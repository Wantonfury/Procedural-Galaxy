#version 330 core

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;

in vec4 colour;
in float fSize;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);  //from [0,1] to [-0.5,0.5]
    
    if(length(coord) > 0.5 || colour.w == 0.0)
        discard;
    
    FragColour = colour;

    float brightness = dot(colour.xyz, vec3(0.2126, 0.7152, 0.0722));
    BrightColour = colour * min(brightness, 1.0);
    //if (brightness > 0.1) BrightColour = colour;
    //else BrightColour = vec4(0.0, 0.0, 0.0, 1.0);
}