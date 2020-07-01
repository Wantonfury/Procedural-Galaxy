#version 330

uniform vec3 v3LightPos;
float g = -0.990;
float g2 = g * g;

in vec3 v3Direction;
in vec4 frontColor;
in vec4 secondaryColor;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;

void main (void)
{
	float fCos = dot(v3LightPos, v3Direction) / length(v3Direction);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	
	FragColour = frontColor + fMiePhase * secondaryColor;
	FragColour.a = FragColour.b;// * 1.0;
	//if (FragColour.b > 0.1) FragColour.a = 1.0;
	//else FragColour.a = FragColour.b;

	float brightness = dot(FragColour.xyz, vec3(0.2126, 0.7152, 0.0722));
    BrightColour = vec4(FragColour.xyz * min(brightness, 1.0), FragColour.a);
}
