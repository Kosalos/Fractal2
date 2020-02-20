// apollonian: https://www.shadertoy.com/view/4ds3zn

#define SHADER
#include "common.h"

#define MAX_MARCHING_STEPS 255
#define MIN_DIST 0.00002 
#define MAX_DIST 60.0
#define PI 3.1415926

struct DEResult {
	float3 pos;
	float dist, iter;
};

DEResult resultInit() {
	DEResult ans;
	ans.dist = 0;
	ans.iter = 0;
	return ans;
}

DEResult DE(float3 pos) {
	DEResult result = resultInit();
	float k, t = Q3 + 0.25 * cos(Q4 * PI * Q1 * (pos.z - pos.x));
	float scale = 1;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos = -1.0 + 2.0 * frac(0.5 * pos + 0.5);
		k = Q2 * t / dot(pos, pos);
		pos *= k;
		scale *= k;
	}

	result.dist = 1.5 * (0.25 * abs(pos.y) / scale);
	return result;
}

// -----------------------------------------------------------

float3 calcNormal(float3 pos) {
	float2 e = float2(1.0, -1.0) * 0.057;
	return normalize(
		e.xyy * DE(pos + e.xyy).dist +
		e.yyx * DE(pos + e.yyx).dist +
		e.yxy * DE(pos + e.yxy).dist +
		e.xxx * DE(pos + e.xxx).dist);
}

// -----------------------------------------------------------

DEResult shortest_dist(float3 eye, float3 dir) {
	DEResult result;
	float hop = 0;
	float dist = MIN_DIST;
	int i = 0;

	for (; i < MAX_MARCHING_STEPS; ++i) {
		result = DE(eye + dist * dir);

		if (result.dist < MIN_DIST) break;

		dist += result.dist;
		if (dist >= MAX_DIST) break;
	}

	result.dist = dist;
	result.iter = float(i);

	return result;
}

// -----------------------------------------------------------

Texture2D<float4>   InputMap  : register(t0);
RWTexture2D<float4> OutputMap : register(u0);

[numthreads(32, 32, 1)]
void CSMain(
	uint3 p:SV_DispatchThreadID)
{
	if (p.x >= uint(XSIZE) || p.y >= uint(YSIZE)) return;

	float den = float(YSIZE);
	float dx = 1.5 * (float(p.x) / den - 0.5);
	float dy = -1.5 * (float(p.y) / den - 0.5);
	float3 direction = normalize((sideVector * dx) + (topVector * dy) + viewVector).xyz;
	DEResult result = shortest_dist(camera.xyz, direction);
	float3 color = float3(0, 0, 0);

	if (result.dist <= MAX_DIST - 0.0001) {
		float3 position = camera.xyz + result.dist * direction;
		float3 cc, normal = calcNormal(position);

		color = float3(1 - (normal / 10 + sqrt(result.iter / 80.0)));
	}

	OutputMap[p.xy] = float4(color, 1);
}
