vec2 v_texcoord0 : TEXCOORD0 = vec2(0.0, 0.0);
vec4 v_color      : TEXCOORD1 = vec4(1.0, 1.0, 1.0, 1.0);
vec3 v_view_pos : TEXCOORD2 = vec3(0.0,0.0,0.0);
vec3 v_view_normal : TEXCOORD3 = vec3(0.0, 0.0, 1.0);
vec3 v_test : TEXCOORD4 = vec3(0.0, 1.0, 0.0);

vec3 a_position  : POSITION;
vec3 a_normal    : NORMAL;
vec2 a_texcoord0 : TEXCOORD0;
vec3 a_texcoord1 : TEXCOORD1;
vec4 a_color0     : COLOR0;
vec3 a_texcoord2 : TEXCOORD2;
vec3 a_texcoord3 : TEXCOORD3;
vec3 a_texcoord4 : TEXCOORD4;
vec4 a_texcoord5 : TEXCOORD5;
vec4 a_texcoord6    : TEXCOORD6;


vec4 i_data0 : TEXCOORD7;
vec4 i_data1 : TEXCOORD6;
vec4 i_data2 : TEXCOORD5;
vec4 i_data3 : TEXCOORD4;
vec4 i_data4 : TEXCOORD3;
