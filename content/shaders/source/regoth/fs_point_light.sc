$input v_texcoord0, v_color, v_view_pos, v_view_normal
 
#include "../common/common.sh"
#include "tools.sh"

SAMPLER2D(s_texColor, 0);

uniform vec4 u_LightPosition_Radius; 
uniform vec4 u_LightColor;


uniform vec4 u_FogColor;
uniform vec4 u_FogNearFar;

vec4 diffuseColor()
{
	vec4 diffuse = texture2D(s_texColor, v_texcoord0);
	vec4 color = diffuse;
	
	if(color.a < 0.6)
        discard;
        
    // Apply linear fog
    color.rgb = computeLinearFog(length(v_view_pos), u_FogNearFar.x, u_FogNearFar.y, color.rgb, u_FogColor.xyz);
    color.a = 1.0f;
    
    return color;
}

void main()
{    
    float radius = u_LightPosition_Radius.w * 5.0;
    vec3 lightDir = normalize(u_LightPosition_Radius.xyz - v_view_pos);
    float intensity = max(dot(v_view_normal, lightDir), 0.0);
    float dist = length(u_LightPosition_Radius.xyz - v_view_pos);
    
    float att = clamp(1.0 - dist*dist/(radius*radius), 0.0, 1.0); att *= att;
    
    if(intensity > 0.0)
    {
        // Spec
    }
    
    //gl_FragColor = vec4(1,1,1,1);
    gl_FragColor = vec4(diffuseColor().rgb * u_LightColor.rgb * att * intensity, 1.0);
}
 
