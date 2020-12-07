// FXAA filter based on McNopper implementation
// https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl

const vec3 FXAA_TO_LUMA = vec3(0.299, 0.587, 0.114);

uniform float uFXAAReduceMultiplier;
uniform float uFXAAReduceMin;
uniform float uFXAAMaxSpan;
uniform float uFXAALumaThreshold;

vec4 fxaa(vec4 color) {
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);

    vec3 rgbNW = textureOffset(screenTexture, texCoord, ivec2(-1, 1)).rgb;
    rgbNW = _gc_toGammaCorrectedLDR(rgbNW);
    vec3 rgbNE = textureOffset(screenTexture, texCoord, ivec2(1, 1)).rgb;
    rgbNE = _gc_toGammaCorrectedLDR(rgbNE);
    vec3 rgbSW = textureOffset(screenTexture, texCoord, ivec2(-1, -1)).rgb;
    rgbSW = _gc_toGammaCorrectedLDR(rgbSW);
    vec3 rgbSE = textureOffset(screenTexture, texCoord, ivec2(1, -1)).rgb;
    rgbSE = _gc_toGammaCorrectedLDR(rgbSE);
	
	// Convert from RGB to luma.
	float lumaNW = dot(rgbNW, FXAA_TO_LUMA);
	float lumaNE = dot(rgbNE, FXAA_TO_LUMA);
	float lumaSW = dot(rgbSW, FXAA_TO_LUMA);
	float lumaSE = dot(rgbSE, FXAA_TO_LUMA);
	float lumaM = dot(color.rgb, FXAA_TO_LUMA);

	// Gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	// If contrast is lower than a maximum threshold ...
	if (lumaMax - lumaMin <= lumaMax * uFXAALumaThreshold)
	{		
		return color;
	}

    // Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * uFXAAReduceMultiplier, uFXAAReduceMin);

    // Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-uFXAAMaxSpan), vec2(uFXAAMaxSpan)) * texelSize;
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(screenTexture, texCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(screenTexture, texCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5; 

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(screenTexture, texCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(screenTexture, texCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;

	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;
    rgbFourTab = _gc_toGammaCorrectedLDR(rgbFourTab); 
	rgbTwoTab = _gc_toGammaCorrectedLDR(rgbTwoTab);

	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, FXAA_TO_LUMA);
	
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		// ... yes, so use only two samples.
		color = vec4(rgbTwoTab, 1.0); 
	}
	else
	{
		// ... no, so use four samples. 
		color = vec4(rgbFourTab, 1.0);
	}

    return color;
}
