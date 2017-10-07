$input a_position, a_color0, a_normal, a_texcoord0
$output v_texcoord0, v_color, v_view_pos

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh" 

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

    v_texcoord0 = a_texcoord0;
    v_color = a_color0;
    
    // Output viewspace position
    v_view_pos = mul(u_modelView, vec4(a_position, 1.0) ).xyz;

}
 
