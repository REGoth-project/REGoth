
uniform vec4 u_SkyColors[2];

/**
 * Computes the fog-term
 */
vec3 computeLinearFog(float distance, float fogNear, float fogFar, vec3 color, vec3 fogColor)
{
	float l = saturate((distance - fogNear)/(fogFar - fogNear));

	return mix(color.rgb, fogColor, l);
}

float computeLinearFogFactor(float distance, float fogNear, float fogFar) {
    return 1.0 - saturate((distance - fogNear)/(fogFar - fogNear));
}

vec4 applySkyColor(float intensity)
{
	return mix(u_SkyColors[0], u_SkyColors[1], intensity);
}
