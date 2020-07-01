#version 330 core

in vec3 n;
in vec3 fragPos;
in vec3 normal0;
in vec4 frontColor;
in vec4 secondaryColor;
in vec4 pColor;

uniform vec3 ambientColour;
uniform float ambientStrength;

uniform vec3 sunColour;
uniform float sunStrength;

uniform vec3 lightDirection;
uniform vec3 lightPos;
/*uniform*/ vec3 lightColour;

uniform mat4 worldTransform;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;

float hash( const in float n ) {
    return fract(sin(n)*43758.5453123);
}
float noise( const in  vec3 x ) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
float terrainMapH(vec3 n)
{
  float h=0.0,f=640000.0;
  while(f>40.0f)	
  {
	h=h*0.5+ noise(n*f);
    //if (h < 0.50) h = 0.50;
	f/=2.0;
  }
  return clamp(h*0.5,0.0,1.0);//-0.61;
}
vec3 terrainCalcNormalHigh( in vec3 pos, float t ) {
    vec2 e = vec2(1.0,-1.0)*0.003*t;

    return normalize( e.xyy*terrainMapH( pos + e.xyy ) + 
					  e.yyx*terrainMapH( pos + e.yyx ) + 
					  e.yxy*terrainMapH( pos + e.yxy ) + 
					  e.xxx*terrainMapH( pos + e.xxx ) );
}

void main(void)
{
	vec4 c=vec4(n.xyz,1.0);

	float h = terrainMapH(n);	
	vec3 nrm = terrainCalcNormalHigh(n,0.003);

    c = vec4(sunColour, 1.0);
    c.xyz = c.xyz * (nrm.x * 0.2 + 0.9);
    c = c * sunStrength;

    //c.w = 1.0;
    //vec4 resultScatter = frontColor + c * secondaryColor;
    //vec4 resultScatter = frontColor + pColor * secondaryColor;
    vec4 resultScatter = c + pColor;
    resultScatter.a = 1.0;

    FragColour = resultScatter;

    float brightness = dot(resultScatter.xyz, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) BrightColour = resultScatter;
    else BrightColour = vec4(0.0, 0.0, 0.0, 1.0);

    

}
