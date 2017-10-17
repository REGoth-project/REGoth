$input a_position, a_color0, a_normal, a_texcoord0
$output v_texcoord0, v_color, v_view_pos//, v_view_normal


#include "../common/common.sh"
#include "tools.sh"

uniform vec4 u_color;
uniform vec4 u_skyTextureParams;

#define u_skyTextureScale u_skyTextureParams.xy
#define u_skytextureSpeed u_skyTextureParams.zw

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

    v_texcoord0 = a_texcoord0 * u_skyTextureScale + u_skytextureSpeed;
    v_color = a_color0 * u_color;

    // Output viewspace position
    v_view_pos = mul(u_modelView, vec4(a_position, 1.0) ).xyz;
}

