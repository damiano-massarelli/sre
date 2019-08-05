layout (location = 0) out vec4 FragColor;

layout (std140) uniform Lights {
    int numLights;
    Light lights[10];
};

layout (std140) uniform Camera {
    vec3 cameraPosition;
    vec3 cameraDirection;
};

// This code is heavily based on learnopengl.com tutorials

const float PI = 3.14159265359;

vec3 fresnel(float HdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

// based on roughness how much 
float normalDistribution(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

// takes into account the self-shadowing effect caused by roughness
float geometry(float NdotVL, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotVL;
    float denom = NdotVL * (1.0 - k) + k;
	
    return num / denom;
}

// takes into account the self-shadowing effect caused by roughness
// 
float geometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2  = geometry(NdotV, roughness);
    float ggx1  = geometry(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 pbrComputeColor(Light light, vec3 albedo, float roughness, float metalness, float ao, vec3 fragPosition, vec3 N, vec3 cameraPosition) {
    vec3 outcolor = vec3(0.0f);

    vec3 L = light.position - fragPosition;
    vec3 V = normalize(cameraDirection - fragPosition);
    float dist = length(L);
    L /= dist; // normalize wi
    float NdotL = max(0.0, dot(N, L));
    float NdotV = max(0.0, dot(N, V));
    float attenuation = 1.0 / (light.attenuations.x + dist * light.attenuations.y + dist * dist * light.attenuations.z);

    // half way vector
    vec3 H = normalize(V + L);

    // basic reflectivity based on "metalness-workflow"
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metalness);

    // calculate reflectivity based on fresnel
    vec3 F = fresnel(max(dot(H, V), 0.0), F0);

    // calculate normal distrubution and self-shadowing
    float NDF = normalDistribution(N, H, roughness);       
    float G   = geometrySmith(NdotV, NdotL, roughness); 

    // calculate specular component of BRDF
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular     = numerator / max(denominator, 0.001); 

    // calculate lambert component of BRDF
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metalness);	 // metal (aka dialetric materials do not have refracted light)

    // radiance
    vec3 radiance = light.diffuseColor * attenuation * NdotL;

    // calculate final radiance
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    return  Lo;
}

uniform sampler2D DiffuseData;
uniform sampler2D PBRData;
uniform sampler2D PositionData;
uniform sampler2D NormalData;

uniform int lightIndex;

void main() {
    vec2 texCoord = gl_FragCoord.xy / textureSize(DiffuseData, 0);

    vec3 albedo = texture(DiffuseData, texCoord).rgb;
    vec4 data = texture(PBRData, texCoord);
    vec3 normal = texture(NormalData, texCoord).xyz;
    vec3 position = texture(PositionData, texCoord).xyz;

    vec3 color = pbrComputeColor(lights[lightIndex], albedo, data.x, data.y, data.z, position, normal, cameraPosition);

    FragColor = vec4(albedo, 1.0);
}
