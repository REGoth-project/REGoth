/**
vec3 a_normal    : NORMAL;
vec2 a_texcoord0 : TEXCOORD0;
vec4 a_color     : COLOR0;
vec3 a_localPositions1 : TEXCOORD1;
vec3 a_localPositions2 : TEXCOORD2;
vec3 a_localPositions3 : TEXCOORD3;
vec3 a_localPositions4 : TEXCOORD4;
vec4 a_boneIndices     : TEXCOORD5;
vec4 a_weights         : TEXCOORD6;
**/

$input a_normal, a_texcoord0, a_color0, a_texcoord1, a_texcoord2, a_texcoord3, a_texcoord4, a_texcoord5, a_texcoord6
$output v_texcoord0, v_color, v_view_pos//, v_view_normal

#define LOCAL_POSITIONS_1 a_texcoord1
#define LOCAL_POSITIONS_2 a_texcoord2
#define LOCAL_POSITIONS_3 a_texcoord3
#define LOCAL_POSITIONS_4 a_texcoord4
#define BONE_INDICES a_texcoord5
#define WEIGHTS a_texcoord6

#define BGFX_CONFIG_MAX_BONES 100

#include "../common/common.sh"
#include "tools.sh"

uniform vec4 u_color;

vec3 MulBone4( vec4 vInputPos, int nMatrix, float fBlendWeight )
{
    return mul(mul(u_model[1+nMatrix], vInputPos), fBlendWeight).xyz;
}

void main()
{
    vec3 position = vec3(0.0,0.0,0.0);
	//position += MulBone4(vec4(a_localPositions1, 1.0f), int(a_boneIndices.x+0.5), a_weights.x);
	//position += MulBone4(vec4(a_localPositions2, 1.0f), int(a_boneIndices.y+0.5), a_weights.y);
	//position += MulBone4(vec4(a_localPositions3, 1.0f), int(a_boneIndices.z+0.5), a_weights.z);
	//position += MulBone4(vec4(a_localPositions4, 1.0f), int(a_boneIndices.w+0.5), a_weights.w);

    //position += MulBone4(vec4(a_normal, 1.0f), 10, 1.0);

    vec4 BONE_INDICES2 = BONE_INDICES + vec4(0.5, 0.5, 0.5, 0.5);

    position =  mul(u_model[1 + int(BONE_INDICES2.x)], vec4(LOCAL_POSITIONS_1, 1.0)).xyz * WEIGHTS.x;
    position += mul(u_model[1 + int(BONE_INDICES2.y)], vec4(LOCAL_POSITIONS_2, 1.0)).xyz * WEIGHTS.y;
    position += mul(u_model[1 + int(BONE_INDICES2.z)], vec4(LOCAL_POSITIONS_3, 1.0)).xyz * WEIGHTS.z;
    position += mul(u_model[1 + int(BONE_INDICES2.w)], vec4(LOCAL_POSITIONS_4, 1.0)).xyz * WEIGHTS.w;

	gl_Position = mul(u_modelViewProj, vec4(position, 1.0) );

    v_texcoord0 = a_texcoord0;

    vec3 normalWorld = mul(u_model[0], vec4(a_normal, 0)).xyz;
    float ndl = max(0.0, dot(normalWorld, normalize(vec3(-1,1,1))));
    v_color = min(1, (ndl * u_color.r + u_color.r * 0.5)) *  a_color0;

    //v_color = a_color0 * u_color;

    // Apply CLUT
    //v_color = SKY_CLUT[int(v_color * 255.0)];
    v_color = applySkyColor(v_color.x);

    // Output viewspace position
    v_view_pos = mul(u_modelView, vec4(position, 1.0) ).xyz;
    //v_view_normal = mat3(u_view) * normalWorld;
}

