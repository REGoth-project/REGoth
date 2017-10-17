$input v_texcoord0, v_color, v_view_pos//, v_view_normal

/*
 *
 */

 
#include "../common/common.sh"
#include "tools.sh"

SAMPLER2D(s_texColor, 0);

uniform vec4 u_FogColor;
uniform vec4 u_FogNearFar;

void main()
{
	vec4 diffuse = texture2D(s_texColor, v_texcoord0);
	vec4 color = diffuse * v_color;
	        
    // Apply linear fog
    color.rgb = computeLinearFog(length(v_view_pos), u_FogNearFar.x, u_FogNearFar.y, color.rgb, u_FogColor.xyz);
    
    gl_FragColor = color;
}
 
