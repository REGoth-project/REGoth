$input v_texcoord0, v_color

/*
 * Copyright 2013-2014 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh"
SAMPLER2D(s_texColor, 0);

void main()
{
	vec4 diffuse = texture2D(s_texColor, v_texcoord0);
	gl_FragColor = diffuse * v_color;
}
 
