$input a_position, a_color0, a_normal, a_texcoord0, i_data0, i_data1, i_data2, i_data3, i_data4
$output v_texcoord0, v_color, v_view_pos

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh" 

uniform vec4 SKY_CLUT[256];

void main()
{
    mat4 model;
	model[0] = i_data0;
	model[1] = i_data1;
	model[2] = i_data2;
	model[3] = i_data3;

	gl_Position = mul(u_viewProj * model, vec4(a_position, 1.0) );

    v_texcoord0 = a_texcoord0;
    
    vec3 normalWorld = mat3(model) * a_normal;
    float ndl = max(0.0, dot(normalWorld, normalize(vec3(-1,1,1))));
    v_color.rgb = min(1, (ndl * i_data4.r + i_data4.r * 0.5)) *  a_color0.rgb;
        
    // Apply CLUT
    v_color.rgb = SKY_CLUT[int(v_color * 255.0)].rgb;
    
    // Output viewspace position
    v_view_pos = mul(u_view * model, vec4(a_position, 1.0) ).xyz;
}
 
