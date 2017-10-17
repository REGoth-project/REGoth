$input a_position, a_color0, a_normal, a_texcoord0
$output v_texcoord0, v_color, v_view_pos//, v_view_normal


#include "../common/common.sh"

uniform vec4 u_SkyColors[2];

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

    v_texcoord0 = a_texcoord0;
    v_color.rgb = mix(u_SkyColors[0].rgb, u_SkyColors[1].rgb, a_color0.a) * a_color0.a;
    v_color.a = 1.0;

    // Output viewspace position
    v_view_pos = mul(u_modelView, vec4(a_position, 1.0) ).xyz;
}

