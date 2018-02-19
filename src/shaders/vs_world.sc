$input a_position, a_color0, a_normal, a_texcoord0
$output v_texcoord0, v_color, v_view_pos//, v_view_normal

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh"
#include "tools.sh"

//uniform vec4 SKY_CLUT[256];
uniform vec4 u_color;

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

    v_texcoord0 = a_texcoord0;

    vec3 normalWorld = mul(u_model[0], vec4(a_normal, 0)).xyz;
    float ndl = max(0.0, dot(normalWorld, normalize(vec3(-1,1,1))));
    v_color.rgb = min(1, (ndl * u_color.r + u_color.r * 0.5)) *  a_color0.rgb;

    // Apply CLUT
    //v_color.rgb = SKY_CLUT[int(v_color.x * 255.0)].rgb;
    v_color.rgb = applySkyColor(v_color.x).rgb;
    v_color.a *= u_color.a;

    // Output viewspace position
    v_view_pos = mul(u_modelView, vec4(a_position, 1.0) ).xyz;

    //v_view_normal = mat3(u_view) * normalWorld;
}

