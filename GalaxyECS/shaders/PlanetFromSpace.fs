#version 330 core

in vec3 n;
in vec3 fragPos;
in vec3 normal0;
in vec4 frontColor;
in vec4 secondaryColor;

uniform vec3 ambientColour;
uniform float ambientStrength;

uniform vec3 terrainColor1;
uniform vec3 terrainColor2;
uniform vec3 oceanColor;

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

vec3 terrainNormal( in vec3 pos, float t ) {
    vec2 e = vec2(1.0,-1.0)*0.000*t;

    return normalize( e.xyy*terrainMapH( pos + e.xyy ) + 
					  e.yyx*terrainMapH( pos + e.yyx ) + 
					  e.yxy*terrainMapH( pos + e.yxy ) + 
					  e.xxx*terrainMapH( pos + e.xxx ) );
}

void main()
{
	vec4 c=vec4(n.xyz,1.0);

	float h = 1.0 - terrainMapH(n);	
	vec3 nrm = terrainCalcNormalHigh(n,0.003);
	c=vec4(1.0,1.0,1.0,1.0);
    if (h > 0.75) c.xyz = terrainColor1 * (nrm.x * 0.5 + 0.5);//c.xyz = c.xyz*(nrm.x*0.5+0.5);
	if (h < 0.75)
    {
        c=vec4(h,h,h*0.1,1.0);
        //c.xyz = c.xyz*(nrm.x*0.5+0.5);
        c.xyz = terrainColor2 * (nrm.x * 0.5 + 0.5);
    }
    if (h < 0.50)
    {
        //c=vec4(1.0, 1.0, 1.0, 1.0);
        //nrm = terrainCalcNormalHigh(vec3(n.x, n.y, n.z), 0.001);
        //c=vec4(h*1.1,h*1.1,h*0.1,1.0);
        c=vec4(h*0.4, h*0.4, h*0.7, 1.0);
        c = c * 0.9;
        //c=vec4(0.0,0.0,h * 0.2,1.0);
        //c.xyz = c.xyz * (nrm.x * 0.5 + 0.5);//
        c.xyz = oceanColor * (nrm.x * 0.5 + 0.5);
    }
	//if (h<0.5)    c=vec4(0.0,h,0.0,1.0);
	//if (h<0.505 ) c=vec4(h*3.0,h*2.0,h/2.0,1.0);
    //if (h<0.50)    c=vec4(0.0,h,0.0,1.0);
	//if (h<0.50)    c=vec4(0.0,0.0,h,1.0);
	//if (h < 0.50) c.xyz = c.xyz;//*(nrm.x*0.5+0.5);
    
    //vec3 ambientLight = ambientStrength * ambientColour;

    int shininess = 16;
    float specularStrength = 0.1;
    lightColour = vec3(1, 1, 1); // remove it after implementing lightColour uniform

    vec3 ambient = ambientStrength * ambientColour;
    
    vec3 viewPos = vec3(0, 0, 0); // camera position
    vec3 lightDir = normalize(lightPos - fragPos);// -normalize(lightDirection); // same effect

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(normal0));

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * vec3(1, 1, 1);//lightColour;

    float diff = max(dot(normalize(normal0), lightDir), 0.0);
	vec3 diffuseSh = diff * lightColour;

    vec3 result = (ambient + diffuseSh) * c.xyz;

    vec4 resultScatter = frontColor + c * secondaryColor;
    //vec4 resultScatter = (frontColor + c * c * secondaryColor);
    resultScatter.a = 1.0;
    
    FragColour = resultScatter;
    float brightness = dot(resultScatter.xyz, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) BrightColour = resultScatter;
    else BrightColour = vec4(0.0, 0.0, 0.0, 1.0);

    

}
