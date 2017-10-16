$input v_texcoord0

/*
 * Copyright 2014 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);

void main()
{
	vec4 color = texture2D(s_texColor, v_texcoord0);
	gl_FragColor = color;
}
