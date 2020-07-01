#version 330 core

uniform vec4 p1,d2,d3;
uniform vec4 n1,nd2,nd3;
uniform vec4 n1x,nd2x,nd3x;

uniform mat4 localTransform;
uniform mat4 worldTransform;
uniform mat4 atmoOrigin;

uniform vec3 v3CameraPos;		// The camera's current position
uniform vec3 v3LightPos;		// The direction vector to the light source
uniform vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
uniform float fCameraHeight;	// The camera's current height
uniform float fCameraHeight2;	// fCameraHeight^2
uniform float fOuterRadius;		// The outer (atmosphere) radius
uniform float fOuterRadius2;	// fOuterRadius^2
uniform float fInnerRadius;		// The inner (planetary) radius
uniform float fInnerRadius2;	// fInnerRadius^2
uniform float fKrESun;			// Kr * ESun
uniform float fKmESun;			// Km * ESun
uniform float fKr4PI;			// Kr * 4 * PI
uniform float fKm4PI;			// Km * 4 * PI
uniform float fScale;			// 1 / (fOuterRadius - fInnerRadius)
uniform float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
uniform float fScaleOverScaleDepth;	// fScale / fScaleDepth

const int nSamples = 2;
const float fSamples = 2.0;

out vec3 n;
out vec3 fragPos;
out vec3 normal0;

out vec4 frontColor;
out vec4 secondaryColor;

layout (location = 0) in vec3 aPos;

vec3 mod2893(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod2893(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute3(vec4 x) {
     return mod2893(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt3(vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v) {
  const vec2  C = vec2(1.0/6.0, 1.0/3.0);
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i = floor(v + dot(v, C.yyy) );
  vec3 x0 = v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod2893(i); 
  vec4 p = permute3( permute3( permute3( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

  //Normalize gradients
  vec4 norm = taylorInvSqrt3(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

float noise(vec3 position, int octaves, float frequency, float persistence) {
	float total = 0.0;
	float maxAmplitude = 0.0;
    float amplitude = 1.0;
	for (int i = 0; i < octaves; i++) {
		total += snoise(position * frequency) * amplitude;
		frequency *= 2.0;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}
	return total / maxAmplitude;
}

float absNoise(vec3 position, int octaves, float frequency, float persistence) {
	float total = 0.0;
	float maxAmplitude = 0.0;
    float amplitude = 1.0;
	for (int i = 0; i < octaves; i++) {
		total += abs(snoise(position * frequency)) * amplitude;
		frequency *= 2.0;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}
	return total / maxAmplitude;
}

float ridgedNoise(vec3 position, int octaves, float frequency, float persistence) {
	float total = 0.0;
	float maxAmplitude = 0.0;
    float amplitude = 1.0;
	for (int i = 0; i < octaves; i++) {
		total += ((1.0 - abs(snoise(position * frequency))) * 2.0 - 1.0) * amplitude;
		frequency *= 2.0;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}
	return total / maxAmplitude;
}

float squaredNoise(vec3 position, int octaves, float frequency, float persistence) {
	float n = noise(position, octaves, frequency, persistence);
    return n * n;
}

float cubedNoise(vec3 position, int octaves, float frequency, float persistence) {
	float n = noise(position, octaves, frequency, persistence);
    return n * n * n;
}

float scale(float fCos)
{
	float x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

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
	//if (h < 1.0) h = 1.0;
	f/=2.0f;
  }
  return 1.0-clamp(h*0.5,0.0,1.0);//-0.61;
}



float scaletest(float x)
{
	float a = 0.0;
	float b = 1.0;
	float min = 0.0;
	float max = 7500.0;

	return ((b - a)*(x - min)) / (max - min) + a;
}

void main()
{
	vec4 p = p1+(d2*aPos.x+d3*aPos.y);

	n = n1.xyz+(nd2.xyz*aPos.x+nd3.xyz*aPos.y);
	n = normalize(n);

	float h = -terrainMapH(n.xyz) - 0.0;
	//float h =  ridgedNoise(n.xyz, 11, 0.003, 0.5);//scaletest(noise(n.xyz, 11, 0.03, 0.5));
	//float h = -1.0;

	p.xyz = n.xyz * (1.0-0.006*h);
	//p.xyz = n.xyz * (1.0 - h);
	//p.w = 1.0;

	//p.xyz = p.xyz * (0.006*ridgedNoise(n.xyz, 8, 0.3, 0.5));//scaletest(noise(n.xyz, 11, 0.03, 0.5));
	p.w = 1.0;

  // Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = (atmoOrigin * p).xyz;
	vec3 v3Ray = v3Pos - v3CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = v3CameraPos;
	float fDepth = (exp((fInnerRadius - fCameraHeight) / fScaleDepth)); //helps
	float fCameraAngle = (dot(-v3Ray, v3Pos) / length(v3Pos));
	float fLightAngle = (dot(v3LightPos, v3Pos) / length(v3Pos));
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate;
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = (length(v3SamplePoint));
		float fDepth = (exp(fScaleOverScaleDepth * (fInnerRadius - fHeight)));
		float fScatter = (fDepth*fTemp - fCameraOffset);
		v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	frontColor.rgb = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);

	// Calculate the attenuation factor for the ground
	secondaryColor.rgb = v3Attenuate;

	normal0 = (localTransform * vec4(n, 0.0)).xyz;
	fragPos = (localTransform * p).xyz;

	//vec4 nx = n1x+(nd2x*aPos.x+nd3x*aPos.y);	
	//p += nx * h;

	gl_Position = worldTransform * p;
	
}
