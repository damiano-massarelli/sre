// This code is heavily based on learnopengl.com tutorials

const float PI = 3.14159265359;

// return how much the microfacet normals are aligned the the half-way vector
// H based on the roughness of the material.
float normalDistribution(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

// takes into account the self-shadowing effect caused by roughness
float geometry(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

// takes into account the self-shadowing effect caused by roughness
// (both from the light perspective and from the viwer perspective)
float geometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2  = geometry(NdotV, roughness);
    float ggx1  = geometry(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnel(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

vec3 pbrComputeColor(Light light, vec3 L, float forceAttenuation, float inShadow, vec3 albedo, float roughness, float metalness, float ao, vec3 fragPosition, vec3 N, vec3 cameraPosition) 
{
    N = normalize(N);
    vec3 V            = normalize(cameraPosition - fragPosition);

    vec3 F0           = vec3(0.04); 
    F0                = mix(F0, albedo, metalness);

    float dist        = length(L); 
    L                 /= dist; // normalize
    vec3 H            = normalize(V + L);
    float attenuation = (1.0 / (light.attenuations.x + dist * light.attenuations.y + dist * dist * light.attenuations.z));
    if (forceAttenuation >= 0)
        attenuation = forceAttenuation;
    vec3 radiance     = light.diffuseColor * attenuation; 

    float NdotV       = max(dot(N, V), 0.0);
    float NdotL       = max(dot(N, L), 0.0);
    
    // cook-torrance brdf
    float NDF = normalDistribution(N, H, roughness);        
    float G   = geometrySmith(NdotV, NdotL, roughness);      
    vec3 F    = fresnel(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular     = numerator / max(denominator, 0.001);  
        
    // add to outgoing radiance Lo
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    return ao * (light.ambientColor + (1.0 - inShadow) * Lo);
}
