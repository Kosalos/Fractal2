// http://blog.hvidtfeldts.net/index.php/2011/06/distance-estimated-3d-fractals-part-i/
// https://github.com/portsmouth/snelly (under fractals: apollonian_pt.html)
// also visit: http://paulbourke.net/fractals/apollony/
// apollonian: https://www.shadertoy.com/view/4ds3zn
// apollonian2: https://www.shadertoy.com/view/llKXzh
// mandelbox : http://www.fractalforums.com/3d-fractal-generation/a-mandelbox-distance-estimate-formula/
// mandelbulb: https://github.com/jtauber/mandelbulb/blob/master/mandel8.py
// tes1.frag:  http://www.fractalforums.com/3d-fractal-generation/an-escape-tim-algorithm-for-kleinian-group-limit-sets/45/
// monster: https://www.shadertoy.com/view/4sX3R2
// tower: https://www.shadertoy.com/view/MtBGDG
// polyHedral Menger based on : https://www.shadertoy.com/view/MsGcWc
// knighty's kleinian : https://www.shadertoy.com/view/lstyR4
// EvilRyu's KIFS : https://www.shadertoy.com/view/MdlSRM
// IFS Tetrahedron : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Kaleidoscopic%20IFS/Tetrahedron.frag
// IFS Octohedron : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Kaleidoscopic%20IFS/Octahedron.frag
// IFS Dodecahedron : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Kaleidoscopic%20IFS/Dodecahedron.frag
// IFS Menger : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Kaleidoscopic%20IFS/NewMenger.frag
// IFS fractals : http://www.fractalforums.com/sierpinski-gasket/kaleidoscopic-(escape-time-ifs)/?PHPSESSID=95c58fd40b61747add7ef16564ccc048
// polychora : https://github.com/Syntopia/Fragmentarium/blob/master/Fragmentarium-Source/Examples/Knighty%20Collection/polychora-special.frag
// quadray :https://github.com/Syntopia/Fragmentarium/tree/master/Fragmentarium-Source/Examples/Knighty%20Collection/Quadray
// fragm : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Experimental/MMM.frag
// quat julia 2 : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Experimental/Stereographic4DJulia.frag
// quat mandelbrot : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Experimental/QuaternionMandelbrot4D.frag
// spudsville : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Experimental/Spudsville2.frag
// menger polyhedra : https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/Examples/Benesi/MengersmoothPolyhedra.frag
// menger helix by dr2 : https://www.shadertoy.com/view/4sVyDt
// FlowerHive: https://www.shadertoy.com/view/lt3Gz8
// Jungle : https://www.shadertoy.com/view/Wd23RD
// Prisoner : https://www.shadertoy.com/view/llVGDR
// SpiralBox : https://fractalforums.org/fragmentarium/17/last-length-increase-colouring-well-sort-of/2515
// spider : https://www.shadertoy.com/view/XtKcDm
// Aleksandrov MandelBulb : https://fractalforums.org/fractal-institute/47/formulas-of-aleksandrov/656
// Surfbox : http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/httpwww-shaperich-comproshred-elite-review/
// Twistbox: http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/twistbox-spiralling-1-scale-box-variant/
// Kali Rontgen : https://www.shadertoy.com/view/XlXcRj
// VERTEBRAE (+ equ 6) : https://fractalforums.org/code-snippets-fragments/74/logxyzsinxyz-transforms/2430
// DarkBeamSurf : https://fractalforums.org/code-snippets-fragments/74/darkbeams-surfbox/2366
// Buffalo : https://fractalforums.org/fragmentarium/17/buffalo-bulb-deltade/2313
// Ancient Temple : https://www.shadertoy.com/view/4lX3Rj
// Kali 3 : https://www.shadertoy.com/view/Xs2GDK
// Sponge : https://www.shadertoy.com/view/3dlXWn
// Floral Hybrid: https://www.shadertoy.com/view/MsS3zc
// Torus Knot : https://www.shadertoy.com/view/3dXXDN
// Donuts : https://www.shadertoy.com/view/lttcWn

#define SHADER
#include "common.h"

#define MAX_MARCHING_STEPS 255
#define MIN_DIST 0.00002 
#define MAX_DIST 60.0
#define PI 3.1415926
#define PI2 = (3.1415926 * 2) 

float3 lerp(float3 a, float3 b, float w) { return a + w * (b - a); }
float  mix(float a, float b, float w) { return b * w + a * (1.0 - w); }
float3 mix(float3 a, float3 b, float w) { return lerp(b, a, w); }

struct DEResult {
	float4 orbitTrap;
	float3 pos;
	float dist, iter, hop;
};

DEResult resultInit() {
	DEResult ans;
	ans.orbitTrap = float4(10000, 10000, 10000, 10000);
	ans.dist = 0;
	ans.iter = 0;
	ans.hop = 0;
	return ans;
}

#ifndef DEVLOPEMENT_SINGLE_FRACTAL

// 1------------------------------------------------------------
//Q1 power

DEResult DE_MANDELBULB(float3 pos) {
	DEResult result = resultInit();
	float dr = 1;
	float r, theta, phi, pwr, ss;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		r = length(pos);
		if (r > 2) break;

		theta = atan2(sqrt(pos.x * pos.x + pos.y * pos.y), pos.z);
		phi = atan2(pos.y, pos.x);
		pwr = pow(r, Q1);
		ss = sin(theta * Q1);

		pos.x += pwr * ss * cos(phi * Q1);
		pos.y += pwr * ss * sin(phi * Q1);
		pos.z += pwr * cos(theta * Q1);

		dr = (pow(r, Q1 - 1.0) * Q1 * dr) + 1.0;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = 0.5 * log(r) * r / dr;
	return result;
}

// 2-----------------------------------------------------------
//  Q1 multiplier
//  Q2 foam
//  Q3 foam2
//  Q4 bend

DEResult DE_APOLLONIAN(float3 pos) {
	DEResult result = resultInit();
	float k, t = Q3 + 0.25 * cos(Q4 * PI * Q1 * (pos.z - pos.x));
	float scale = 1;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos = -1.0 + 2.0 * frac(0.5 * pos + 0.5);
		k = Q2 * t / dot(pos, pos);
		pos *= k;
		scale *= k;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, k);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = 1.5 * (0.25 * abs(pos.y) / scale);
	return result;
}

// 3-----------------------------------------------------------
//  Q1 multiplier
//  Q2 foam
//  Q3 foam2
//  Q4 bend

DEResult DE_APOLLONIAN2(float3 pos) {
	DEResult result = resultInit();
	float t = Q3 + 0.25 * cos(Q4 * PI * Q1 * (pos.z - pos.x));
	float scale = 1;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos = -1.0 + 2.0 * frac(0.5 * pos + 0.5);
		pos -= sign(pos) * Q2 / 20;

		float k = t / dot(pos, pos);
		pos *= k;
		scale *= k;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, k);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	float d1 = sqrt(min(min(dot(pos.xy, pos.xy), dot(pos.yz, pos.yz)), dot(pos.zx, pos.zx))) - 0.02;
	float dmi = min(d1, abs(pos.y));
	result.dist = 0.5 * dmi / scale;
	return result;
}

// 4-----------------------------------------------------------
// Q1 box_size_x
// Q2 box_size_z
// Q3 KleinR
// Q4 KleinI
// Q5 Clamp_y
// Q6 Clamp_DF

float dot2(float3 z) { return dot(z, z); }

float3 wrap(float3 x, float3 a, float3 s) {
	x -= s;
	return (x - a * floor(x / a)) + s;
}

float2 wrap(float2 x, float2 a, float2 s) {
	x -= s;
	return (x - a * floor(x / a)) + s;
}

DEResult JosKleinian(float3 z) {
	DEResult result = resultInit();
	float3 lz = z + float3(1, 1, 1), llz = z + float3(-1, -1, -1);
	float DE = 1e10;
	float DF = 1.0;
	float a = Q3, b = Q4;
	float f = sign(b);
	float4 ot, trap = OTfixed;

	for (int i = 0; i < BOXITERATIONS; ++i) {
		z.x = z.x + b / a * z.y;
		if (FOURGEN != 0)
			z = wrap(z, float3(2. * Q1, a, 2. * Q2), float3(-Q1, 0., -Q2));
		else
			z.xz = wrap(z.xz, float2(2.0 * Q1, 2.0 * Q2), float2(-Q1, -Q2));
		z.x = z.x - b / a * z.y;

		//If above the separation line, rotate by 180 about (-b/2, a/2)
		if (z.y >= a * (0.5 + f * 0.25 * sign(z.x + b * 0.5) * (1. - exp(-3.2 * abs(z.x + b * 0.5)))))
			z = float3(-b, a, 0.) - z;

		//Apply transformation a
		float iR = 1. / dot2(z);
		z *= -iR;
		z.x = -b - z.x;
		z.y = a + z.y;
		DF *= iR;

		//If the iterated points enters a 2-cycle , bail out.
		if (dot2(z - llz) < 1e-12) {
			break;
		}

		//Store previous iterates
		llz = lz; lz = z;

		ot.xyz = z;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, length(llz));
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	//WIP: Push the iterated point left or right depending on the sign of KleinI
	for (int j = 0; j < FINALITERATIONS; ++j) {
		float y = SHOWBALLS != 0 ? min(z.y, a - z.y) : z.y;
		DE = min(DE, min(y, Q5) / max(DF, Q6));

		//Apply transformation a
		float iR = 1. / dot2(z);
		z *= -iR;
		z.x = -b - z.x;
		z.y = a + z.y;
		DF *= iR;
	}

	float y = SHOWBALLS != 0 ? min(z.y, a - z.y) : z.y;
	result.dist = min(DE, min(y, Q5) / max(DF, Q6));

	return result;
}

DEResult DE_KLEINIAN(float3 pos) {
	DEResult result = resultInit();

	if (DOINVERSION != 0) {
		pos = pos - inv1.xyz;
		float r = length(pos);
		float r2 = r * r;
		pos = (INVERSION_RADIUS * INVERSION_RADIUS / r2) * pos + inv1.xyz;

		float an = atan2(pos.y, pos.x) + INVERSION_ANGLE;
		float ra = sqrt(pos.y * pos.y + pos.x * pos.x);
		pos.x = cos(an) * ra;
		pos.y = sin(an) * ra;
		float de = JosKleinian(pos).dist;
		de = r2 * de / (INVERSION_RADIUS * INVERSION_RADIUS + r * de);
		result.dist = de;
	}
	else
	result = JosKleinian(pos);

	return result;
}

// 5-----------------------------------------------------------
// Q1 Scale Factor
// Q2 Box
// Q3 Sphere 1
// Q4 Sphere 2

float boxFold(float v, float fold) { return abs(v + fold) - abs(v - fold) - v; }

float boxFold2(float v, float fold) { // http://www.fractalforums.com/new-theories-and-research/mandelbox-variant-21/
	if (v < -fold) v = -2 * fold - v;
	return v;
}

DEResult DE_MANDELBOX(float3 pos) {
	DEResult result = resultInit();
	float3 c = JULIAMODE != 0 ? julia.xyz : pos;
	float r2, dr = Q1;

	float fR2 = Q3 * Q3;
	float mR2 = Q4 * Q4;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		if (DOINVERSION != 0) {
			pos.x = boxFold(pos.x, Q2);
			pos.y = boxFold(pos.y, Q2);
			pos.z = boxFold(pos.z, Q2);
		}
		else {
			pos.x = boxFold2(pos.x, Q2);
			pos.y = boxFold2(pos.y, Q2);
			pos.z = boxFold2(pos.z, Q2);
		}

		r2 = pos.x * pos.x + pos.y * pos.y + pos.z * pos.z;

		if (r2 < mR2) {
			float temp = fR2 / mR2;
			pos *= temp;
			dr *= temp;
		}
		else if (r2 < fR2) {
			float temp = fR2 / r2;
			pos *= temp;
			dr *= temp;
		}

		pos = pos * Q1 + c;
		dr *= Q1;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, dr);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
		result.orbitTrap = min(result.orbitTrap, float4(abs(pos), dot(pos, pos)));
	}

	result.dist = length(pos) / abs(dr);
	return result;
}

// 6-----------------------------------------------------------
// Q1 X
// Q2 Y
// Q3 Z
// Q4 W

DEResult DE_QUATJULIA(float3 pos) {
	DEResult result = resultInit();
	float4 c = 0.5 * P0;
	float4 nz;
	float md2 = 1.0;
	float4 z = float4(pos, 0);
	float mz2 = dot(z, z);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		md2 *= 4.0 * mz2;
		nz.x = z.x * z.x - dot(z.yzw, z.yzw);
		nz.yzw = 2.0 * z.x * z.yzw;
		z = nz + c;

		mz2 = dot(z, z);
		if (mz2 > 12.0) break;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, mz2);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = 0.3 * sqrt(mz2 / md2) * log(mz2);
	return result;
}

// 9-----------------------------------------------------------
// Q1 Twist
// Q2 Menger
// Q3 Stretch
// Q4 Spin
// Q5 Shape
#define csD  P3
#define csD2 P4

float2 Rot2D(float2 q, float a) {
	float2 cs = sin(a + float2 (0.5 * PI, 0));
	return float2(dot(q, float2(cs.x, -cs.y)), dot(q.yx, cs));
}

float2 Rot2Cs(float2 q, float2 cs) {
	return float2(dot(q, float2(cs.x, -cs.y)), dot(q.yx, cs));
}

float PrBoxDf(float3 p, float3 b) {
	float3 d = abs(p) - b;
	return min(max(d.x, max(d.y, d.z)), 0.) + length(max(d, 0));
}

float3 DodecSym(float3 pos) {
	float a, w;
	w = 2. * PI / Q1;
	pos.xz = Rot2Cs(float2(pos.x, abs(pos.z)), float2(csD.x, -csD.y));
	pos.xy = Rot2D(pos.xy, -0.25 * w);
	pos.x = -abs(pos.x);

	float2 ccsD = float2(Q9, QA);
	float2 ccsD2 = float2(QB, QC);
	for (int k = 0; k < 3; ++k) {
		if (dot(pos.yz, ccsD.xy) > 0.) pos.zy = Rot2Cs(pos.zy, ccsD2.xy) * float2 (1., -1.);
		pos.xy = Rot2D(pos.xy, -w);
	}

	if (dot(pos.yz, ccsD.xy) > 0.) pos.zy = Rot2Cs(pos.zy, ccsD2.xy) * float2(1., -1.);
	a = fmod(atan2(pos.x, pos.y) + 0.5 * w, w) - 0.5 * w;
	a *= Q4;

	pos.yx = float2(cos(a), sin(a)) * length(pos.xy);
	pos.xz = -float2(abs(pos.x), pos.z);
	return pos;
}

DEResult DE_POLY_MENGER(float3 pos) {
	DEResult result = resultInit();
	float nIt = 9.69;
	float sclFac = Q2;
	float3 b = (sclFac - 1) * float3(0.8, 1., 0.5) * (1. + 0.03 * sin(float3(1.23, 1., 1.43)));
	pos = DodecSym(pos);
	pos.z += 0.6 * (1. + b.z);
	pos.xy /= 1. - Q3 * pos.z;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (float n = 0.; n < nIt; n++) {
		pos = abs(pos);
		pos.xy = (pos.x > pos.y) ? pos.xy : pos.yx;
		pos.xz = (pos.x > pos.z) ? pos.xz : pos.zx;
		pos.yz = (pos.y > pos.z) ? pos.yz : pos.zy;
		pos = sclFac * pos - b;
		pos.z += b.z * step(pos.z, -0.5 * b.z);

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, length(pos));
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = 0.8 * PrBoxDf(pos, float3(1, 1, 1)) / pow(abs(sclFac), nIt);
	return result;
}

// 10-----------------------------------------------------------
// Q1 T
// Q2 U
// Q3 V
// Q4 W
// Q5 X
// Q6 Y
// Q7 Z

DEResult DE_GOLD(float3 pos) {
	DEResult result = resultInit();
	pos.xz = fmod(pos.xz + 1.0, 2.0) - 1.0;
	float4 q = float4(pos, 1);
	float3 offset1 = float3(Q1, Q2, Q3);
	float4 offset2 = float4(Q4, Q5, Q6, Q7);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int n = 0; n < MAXSTEPS; ++n) {
		q.xyz = abs(q.xyz) - offset1;
		q = 2.0 * q / clamp(dot(q.xyz, q.xyz), 0.4, 1.0) - offset2;

		ot = q;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = length(q.xyz) / q.w;
	return result;
}

// 11-----------------------------------------------------------
// Q1 X
// Q2 Y
// Q3 Z

float3 rotatePosition(float3 pos, int axis, float angle) {
	float ss = sin(angle);
	float cc = cos(angle);
	float qt;

	switch (axis) {
	case 0:    // XY
		qt = pos.x;
		pos.x = pos.x * cc - pos.y * ss;
		pos.y = qt * ss + pos.y * cc;
		break;
	case 1:    // XZ
		qt = pos.x;
		pos.x = pos.x * cc - pos.z * ss;
		pos.z = qt * ss + pos.z * cc;
		break;
	case 2:    // YZ
		qt = pos.y;
		pos.y = pos.y * cc - pos.z * ss;
		pos.z = qt * ss + pos.z * cc;
		break;
	}
	return pos;
}

float smax(float a, float b, float s) { // iq smin
	float h = clamp(0.5 + 0.5 * (a - b) / s, 0.0, 1.0);
	return mix(b, a, h) + h * (1.0 - h) * s;
}

DEResult DE_SPIDER(float3 pos) {
	DEResult result = resultInit();
	float q = sin(pos.z * Q1) * Q2 * 10 + 0.6;
	float t = length(pos.xy);
	float s = 1.0;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < 2; ++i) {
		float m = dot(pos, pos);
		pos = abs(frac(pos / m) - 0.5);
		pos = rotatePosition(pos, 0, q);
		s *= m;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, s);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	float d = (length(pos.xz) - Q3) * s;

	result.dist = smax(d, -t, 0.3);
	return result;
}

// 12-----------------------------------------------------------
// Q9 Shape
// P3 = min3 (Q1,2,3,4)
// P4 = maxs (Q5,6,7,8)

DEResult DE_KLEINIAN2(float3 pos) {
	DEResult result = resultInit();
	float k, scale = 1;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < 7; ++i) {
		pos = 2 * clamp(pos, P3.xyz, P4.xyz) - pos;
		k = max(P3.w / dot(pos, pos), Q9);
		pos *= k;
		scale *= k;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, k);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	float rxy = length(pos.xy);
	result.dist = 0.7 * max(rxy - P4.w, rxy * pos.z / length(pos)) / scale;
	return result;
}

// 18-----------------------------------------------------------
// Q1 Scale
// Q2 Y
// Q3 Angle1
// Q4 Angle2
// P3 = n1

DEResult DE_SIERPINSKI_T(float3 pos) {
	DEResult result = resultInit();
	int i;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (i = 0; i < MAXSTEPS; ++i) {
		pos = rotatePosition(pos, 0, Q3);

		if (pos.x + pos.y < 0.0) pos.xy = -pos.yx;
		if (pos.x + pos.z < 0.0) pos.xz = -pos.zx;
		if (pos.y + pos.z < 0.0) pos.zy = -pos.yz;

		pos = rotatePosition(pos, 1, Q4);
		pos = pos * Q1 - P3.xyz * (Q1 - 1.0);
		if (length(pos) > 4) break;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, length(pos));
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = (length(pos) - 2) * pow(abs(Q1), -float(i));
	return result;
}

// 19-----------------------------------------------------------
// Q1 Scale
// Q2 Y
// Q3 Angle1
// Q4 Angle2
// P3 = n1

DEResult DE_HALF_TETRA(float3 pos) {
	DEResult result = resultInit();
	int i;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (i = 0; i < MAXSTEPS; ++i) {
		pos = rotatePosition(pos, 0, Q3);

		if (pos.x - pos.y < 0.0) pos.xy = pos.yx;
		if (pos.x - pos.z < 0.0) pos.xz = pos.zx;
		if (pos.y - pos.z < 0.0) pos.zy = pos.yz;

		pos = rotatePosition(pos, 2, Q4);
		pos = pos * Q1 - P3.xyz * (Q1 - 1.0);
		if (length(pos) > 4) break;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, length(pos));
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = (length(pos) - 2) * pow(abs(Q1), -float(i));
	return result;
}

// 24-----------------------------------------------------------
// Q1 Scale
// Q2 Y
// Q3 Z
// Q4 Angle1
// Q5 Angle2
// P3 = n1

DEResult DE_KALEIDO(float3 pos) {
	DEResult result = resultInit();
	int i;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (i = 0; i < MAXSTEPS; ++i) {
		pos = rotatePosition(pos, 0, Q4);

		pos = abs(pos);
		if (pos.x - pos.y < 0.0) pos.xy = pos.yx;
		if (pos.x - pos.z < 0.0) pos.xz = pos.zx;
		if (pos.y - pos.z < 0.0) pos.zy = pos.yz;

		pos -= 0.5 * Q3 * (Q1 - 1) / Q1;
		pos = -abs(-pos);
		pos += 0.5 * Q3 * (Q1 - 1) / Q1;

		pos = rotatePosition(pos, 1, Q5);
		pos = pos * Q1 - P3.xyz * (Q1 - 1.0);
		if (length(pos) > 4) break;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, length(pos));
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = (length(pos) - 2) * pow(abs(Q1), -float(i));
	return result;
}

// 25-----------------------------------------------------------
// P4 p
// P5 nd
// Q9 cVR
// QA sVR
// QB cSR
// QC sSR
// QD cRA
// QE sRA

float4 Rotate(float4 pos) {
	//this is a rotation on the plane defined by RotVector and w axis
	//We do not need more because the remaining 3 rotation are in our 3D space
	//That would be redundant.
	//This rotation is equivalent to translation inside the hypersphere when the camera is at 0,0,0
	float4 p1 = pos;
	float3 rv = normalize(sideVector).xyz;
	float vp = dot(rv, pos.xyz);
	p1.xyz += rv * (vp * (QD - 1.) - pos.w * QE);
	p1.w += vp * QE + pos.w * (QD - 1.);
	return p1;
}

float4 fold(float4 pos) {
	for (int i = 0; i < 3; ++i) {
		pos.xyz = abs(pos.xyz);
		float t = -2. * min(0., dot(pos, P5));
		pos += t * P5;
	}
	return pos;
}

float DDV(float ca, float sa, float r) {
	//magic formula to convert from spherical distance to planar distance.
	//involves transforming from 3-plane to 3-sphere, getting the distance
	//on the sphere (which is an angle -read: sa==sin(a) and ca==cos(a))
	//then going back to the 3-plane.
	//return r-(2.*r*ca-(1.-r*r)*sa)/((1.-r*r)*ca+2.*r*sa+1.+r*r);
	return (2. * r * Q9 - (1. - r * r) * QA) / ((1. - r * r) * Q9 + 2. * r * QA + 1. + r * r) -
		(2. * r * ca - (1. - r * r) * sa) / ((1. - r * r) * ca + 2. * r * sa + 1. + r * r);
}

float DDS(float ca, float sa, float r) {
	return (2. * r * QB - (1. - r * r) * QC) / ((1. - r * r) * QB + 2. * r * QC + 1. + r * r) -
		(2. * r * ca - (1. - r * r) * sa) / ((1. - r * r) * ca + 2. * r * sa + 1. + r * r);
}

float dist2Vertex(float4 z, float r) {
	float ca = dot(z, P4);
	float sa = 0.5 * length(P4 - z) * length(P4 + z);
	return DDV(ca, sa, r);
}

float dist2Segment(float4 z, float4 n, float r) {
	//pmin is the orthogonal projection of z onto the plane defined by p and n
	//then pmin is projected onto the unit sphere
	float zn = dot(z, n);
	float zp = dot(z, P4);
	float np = dot(n, P4);
	float alpha = zp - zn * np;
	float beta = zn - zp * np;
	float4 pmin = normalize(alpha * P4 + min(0., beta) * n);
	//ca and sa are the cosine and sine of the angle between z and pmin. This is the spherical distance.
	float ca = dot(z, pmin);
	float sa = 0.5 * length(pmin - z) * length(pmin + z); //sqrt(1.-ca*ca);//
	return DDS(ca, sa, r); //-SRadius;
}

float dist2Segments(float4 z, float r) {
	float da = dist2Segment(z, float4(1, 0, 0, 0), r);
	float db = dist2Segment(z, float4(0, 1, 0, 0), r);
	float dc = dist2Segment(z, float4(0, 0, 1, 0), r);
	float dd = dist2Segment(z, P5, r);

	return min(min(da, db), min(dc, dd));
}

DEResult DE_POLYCHORA(float3 pos) {
	DEResult result = resultInit();
	float r = length(pos);
	float4 z4 = float4(2. * pos, 1. - r * r) * 1. / (1. + r * r); //Inverse stereographic projection of pos: z4 lies onto the unit 3-sphere centered at 0.
	z4 = Rotate(z4);  //z4.xyw=rot*z4.xyw;
	z4 = fold(z4);  //fold it

	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	ot = z4;
	if (OTstyle > 0) ot -= trap;
	float4 hk = float4(ot.xyz, length(ot.xyz));
	result.orbitTrap = min(result.orbitTrap, dot(ot,ot));

	result.dist = min(dist2Vertex(z4, r), dist2Segments(z4, r));
	return result;
}

// 30-----------------------------------------------------------
// Q1 Scale
// Q2 MinRad2
// Q5 Trans X
// Q6 Trans Y
// Q7 Trans Z
// Q9 Angle
// QC absScalem1
// QD AbsScaleRaisedTo1mIters
// P4 n1
// P5 mins

DEResult DE_KALIBOX(float3 pos) {
	DEResult result = resultInit();
	float4 p = float4(pos, 1);
	float4 p0 = float4(julia.xyz, 1);  // p.w is the distance estimate
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {

		// p.xyz*=rot;
		p.xyz = rotatePosition(p.xyz, 0, Q9);
		p.xyz = rotatePosition(p.xyz, 1, Q9);

		p.xyz = abs(p.xyz) + P4.xyz;
		float r2 = dot(p.xyz, p.xyz);
		p *= clamp(max(Q2 / r2, Q2), 0.0, 1.0);  // dp3,div,max.sat,mul
		p = p * P5 + (JULIAMODE != 0 ? p0 : float4(0, 0, 0, 0));

		ot = p;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = ((length(p.xyz) - QC) / p.w - QD);
	return result;
}

// 34-----------------------------------------------------------
// Q1 Scale

DEResult DE_FLOWER(float3 pos) {
	DEResult result = resultInit();
	float4 q = float4(pos, 1);
	float4 juliaOffset = float4(julia.xyz, 0);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) { //kaliset fractal with no mirroring offset
		q.xyz = abs(q.xyz);
		float r = dot(q.xyz, q.xyz);
		q /= clamp(r, 0.0, Q1);

		q = 2.0 * q - juliaOffset;

		ot = q;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, length(ot.xyz));
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = (length(q.xy) / q.w - 0.003); // cylinder primative instead of a sphere primative.
	return result;
}

// 38-----------------------------------------------------------
// Q1 power

DEResult DE_ALEK_BULB(float3 pos) {
	DEResult result = resultInit();
	float dr = 1;
	float r, mcangle, theta, pwr;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		r = length(pos);
		if (r > 8) break;

		mcangle = abs(pos.y) / pos.x;
		mcangle = atan(mcangle);
		mcangle = (PI * 0.5 - PI * abs(pos.x) * 0.5 / pos.x + mcangle) * Q1;

		theta = acos(pos.z / r) * Q1;

		pwr = pow(r, Q1);
		pos.x = pwr * cos(mcangle) * sin(theta);
		pos.y = pwr * sin(mcangle) * sin(theta) * abs(pos.y) / pos.y;
		pos.z = pwr * cos(theta);

		pos += julia.xyz;

		dr = (pow(r, Q1 - 1.0) * Q1 * dr) + 1.0;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, theta);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = 0.5 * log(r) * r / dr;
	return result;
}

// 39-----------------------------------------------------------
// Q6 Scale Factor
// Q1 Box 1
// Q2 Box 2
// Q3 Sphere 1
// Q4 Sphere 2

float surfBoxFold(float v, float fold, float foldModX) {
	float sg = sign(v);
	float folder = sg * v - fold; // fold is Tglad's
	folder += abs(folder);
	folder = min(folder, foldModX); // and Y,Z,W
	v -= sg * folder;

	return v;
}

DEResult DE_SURFBOX(float3 pos) {
	DEResult result = resultInit();
	float3 c = JULIAMODE != 0 ? julia.xyz : pos;
	float r2, dr = Q6;
	float fR2 = Q3 * Q3;
	float mR2 = Q4 * Q4;
	float foldMod = Q1 * Q2;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos.x = surfBoxFold(pos.x, Q1, foldMod);
		pos.y = surfBoxFold(pos.y, Q1, foldMod);
		pos.z = surfBoxFold(pos.z, Q1, foldMod);

		r2 = pos.x * pos.x + pos.y * pos.y + pos.z * pos.z;

		if (r2 < mR2) {
			float temp = fR2 / mR2;
			pos *= temp;
			dr *= temp;
		}
		else if (r2 < fR2) {
			float temp = fR2 / r2;
			pos *= temp;
			dr *= temp;
		}

		pos = pos * Q6 + c;
		dr *= Q6;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, dr);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = length(pos) / abs(dr);
	return result;
}

// 47-----------------------------------------------------------
// Q1 minX
// Q2 minY
// Q3 minZ
// Q4 minW
// Q5 maxX
// Q6 maxY
// Q7 maxZ
// Q8 maxW
// Q9 Scale
// QA Shape
// P3 = min3 (Q1,2,3,4)
// P4 = maxs (Q5,6,7,8)

float sdSponge(float3 z) {
	z *= QA;

	//folding
	for (int i = 0; i < 4; ++i) {
		z = abs(z);
		z.xy = (z.x < z.y) ? z.yx : z.xy;
		z.xz = (z.x < z.z) ? z.zx : z.xz;
		z.zy = (z.y < z.z) ? z.yz : z.zy;
		z = z * 3.0 - 2.0;
		z.z += (z.z < -1.0) ? 2.0 : 0.0;
	}

	//distance to cube
	z = abs(z) - float3(1, 1, 1);
	float dis = min(max(z.x, max(z.y, z.z)), 0.0) + length(max(z, 0.0));
	//scale cube size to iterations
	return dis * 0.2 * pow(3.0, -3.0);
}

DEResult DE_SPONGE(float3 pos) {
	DEResult result = resultInit();
	float k, r2;
	float scale = 1.0;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos = 2.0 * clamp(pos, P3.xyz, P4.xyz) - pos;
		r2 = dot(pos, pos);
		k = max(P3.w / r2, 1.0);
		pos *= k;
		scale *= k;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, k);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	pos /= scale;
	pos *= P4.w * Q9;
	result.dist = float(0.1 * sdSponge(pos) / (P4.w * Q9));
	return result;
}

// 20-----------------------------------------------------------
DEResult DE_FULL_TETRA(float3 pos) {
	DEResult result = resultInit();
	int i;
	float len;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (i = 0; i < MAXSTEPS; ++i) {
		pos = rotatePosition(pos, 0, Q3);

		if (pos.x - pos.y < 0.0) pos.xy = pos.yx;
		if (pos.x - pos.z < 0.0) pos.xz = pos.zx;
		if (pos.y - pos.z < 0.0) pos.zy = pos.yz;
		if (pos.x + pos.y < 0.0) pos.xy = -pos.yx;
		if (pos.x + pos.z < 0.0) pos.xz = -pos.zx;
		if (pos.y + pos.z < 0.0) pos.zy = -pos.yz;

		pos = rotatePosition(pos, 2, Q4);
		pos = pos * Q1 - P4.xyz * (Q1 - 1.0);
		len = length(pos);
		if (len > 4) break;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, len);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = (length(pos) - 2) * pow(Q1, -float(i));
	return result;
}

// 28-----------------------------------------------------------
float4 stereographic3Sphere(float3 pos) {
	float n = dot(pos, pos) + 1.;
	return float4(Q1 * pos, n - 2.) / n;
}

float2 complexMul(float2 a, float2 b) { return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x); }

DEResult DE_QUATJULIA2(float3 pos) {
	DEResult result = resultInit();
	float4 p4 = stereographic3Sphere(pos);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	p4.xyz += julia.xyz;    // "offset"

	float2 p = p4.xy;
	float2 c = p4.zw;
	float d, dp = 1.0;

	for (int i = 0; i < MAXSTEPS; ++i) {
		dp = 2.0 * length(p) * dp + 1.0;
		p = complexMul(p, p) + c;

		d = dot(p, p);
		if (d > 10) break;

		ot = float4(p, c);
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, d);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	float r = length(p);
	result.dist = r * log(r) / abs(dp);
	return result;
}

// 29 ---------------------------------------------------------

DEResult DE_MBROT(float3 pos) {
	DEResult result = resultInit();
	float4 p = float4(pos, Q1);
	float4 dp = float4(1.0, 0.0, 0.0, 0.0);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	p.yzw = rotatePosition(p.yzw, 0, julia.x);
	p.yzw = rotatePosition(p.yzw, 1, julia.y);
	p.yzw = rotatePosition(p.yzw, 2, julia.z);

	for (int i = 0; i < MAXSTEPS; ++i) {
		dp = 2.0 * float4(p.x * dp.x - dot(p.yzw, dp.yzw), p.x * dp.yzw + dp.x * p.yzw + cross(p.yzw, dp.yzw));
		p = float4(p.x * p.x - dot(p.yzw, p.yzw), float3(2.0 * p.x * p.yzw)) + float4(pos, 0.0);

		float p2 = dot(p, p);
		if (p2 > 10) break;

		ot = p;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	float r = length(p);
	result.dist = 0.2 * r * log(r) / length(dp);
	return result;
}

// 31 ---------------------------------------------------------
float3 boxFold(float3 pos, float fold) {
	float3 ans;
	ans.x = clamp(pos.x, -fold, fold) * 2.0 - pos.x;
	ans.y = clamp(pos.y, -fold, fold) * 2.0 - pos.y;
	ans.z = clamp(pos.z, -fold, fold) * 2.0 - pos.z;
	return ans;
}

DEResult DE_SPUDS(float3 pos) {
	DEResult result = resultInit();
	int n = 0;
	float dz = 1.0;
	float r2, r = length(pos);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	while (r < 10 && n < MAXSTEPS) {
		if (n < 6) {
			pos = boxFold(pos, Q3);

			r2 = dot(pos, pos);
			if (r2 < Q1) {
				float temp = (Q2 / Q1);
				pos *= temp;
				dz *= temp;
			}
			else if (r2 < Q2) {
				float temp = Q2 / r2;
				pos *= temp;
				dz *= temp;
			}

			pos *= Q7;
			dz *= abs(Q7);
		}
		else {
			pos = boxFold(pos, Q4);
			r = length(pos);

			float zo0 = asin(pos.z / r);
			float zi0 = atan2(pos.y, pos.x);
			float zr = pow(r, Q5 - 1.0);
			float zo = zo0 * Q5;
			float zi = zi0 * Q5;
			dz = zr * dz * Q5 * abs(length(float3(1.0, 1.0, Q6) / sqrt(3.0))) + 1.0;
			zr *= r;
			pos = zr * float3(cos(zo) * cos(zi), cos(zo) * sin(zi), Q6 * sin(zo));

			pos = Q8 * pos;
			dz *= abs(Q8);
		}

		r = length(pos);
		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));

		n++;
	}

	result.dist = r * log(r) / dz;
	return result;
}

// 37 ---------------------------------------------------------

DEResult DE_SPIRALBOX(float3 pos) {
	DEResult result = resultInit();
	float3 z = pos;
	float r, DF = 1.0;
	float3 offset = (JULIAMODE != 0 ? julia.xyz / 10 : pos);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		z.xyz = boxFold(z.xyz, Q1);

		r = dot(z, z);
		if (r < 0.001 || r> 1000.) break;

		z /= -r;
		DF /= r;
		z.xz *= -1.;
		z += offset;

		ot.xyz = z;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	r = length(z);
	result.dist = 0.5 * sqrt(0.1 * r) / (abs(DF) + 1);
	return result;
}

// 40 ---------------------------------------------------------

DEResult DE_TWISTBOX(float3 pos) {
	DEResult result = resultInit();
	float3 c = JULIAMODE != 0 ? julia.xyz : pos;
	float r, DF = Q2;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos = boxFold(pos, Q1);

		r = dot(pos, pos);
		if (r < 0.001 || r> 1000.) break;
		pos /= -r;
		DF /= r;
		pos.xz *= -1;
		pos += c;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	r = length(pos);
	result.dist = 0.5 * sqrt(r) / (abs(DF) + 1);
	return result;
}

// 41-----------------------------------------------------------
// Q1 X
// Q2 Y
// Q3 Z
// Q4 Angle

DEResult DE_KALI_RONTGEN(float3 pos) {
	DEResult result = resultInit();
	float d = 10000.;
	float4 p = float4(pos, 1.);
	float3 param = float3(Q1, Q2, Q3);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		p = abs(p) / dot(p.xyz, p.xyz);

		d = min(d, (length(p.xy - float2(0, .01)) - .03) / p.w);
		if (d > 4) {
			d = 0;
			break;
		}

		p.xyz -= param;
		p.xyz = rotatePosition(p.xyz, 1, Q4);

		ot = p;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = d;
	return result;
}

// 42-----------------------------------------------------------
DEResult DE_VERTEBRAE(float3 pos) {
	DEResult result = resultInit();
#define scalelogx  Q5
#define scalelogy  Q6
#define scalelogz  Q7
#define scalesinx  Q8
#define scalesiny  Q9
#define scalesinz  QA
#define offsetsinx QB
#define offsetsiny QC
#define offsetsinz QD
#define slopesinx  QE
#define slopesiny  QF
#define slopesinz  QG
	float4 c = 0.5 * float4(Q1, Q2, Q3, Q4);
	float4 nz;
	float md2 = 1.0;
	float4 z = float4(pos, 0);
	float mz2 = dot(z, z);
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		md2 *= 4.0 * mz2;
		nz.x = z.x * z.x - dot(z.yzw, z.yzw);
		nz.yzw = 2.0 * z.x * z.yzw;
		z = nz + c;

		z.x = scalelogx * log(z.x + sqrt(z.x * z.x + 1.));
		z.y = scalelogy * log(z.y + sqrt(z.y * z.y + 1.));
		z.z = scalelogz * log(z.z + sqrt(z.z * z.z + 1.));
		z.x = scalesinx * sin(z.x + offsetsinx) + (z.x * slopesinx);
		z.y = scalesiny * sin(z.y + offsetsiny) + (z.y * slopesiny);
		z.z = scalesinz * sin(z.z + offsetsinz) + (z.z * slopesinz);

		mz2 = dot(z, z);
		if (mz2 > 12.0) break;

		ot = z;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = 0.3 * sqrt(mz2 / md2) * log(mz2);
	return result;
}

// 43 ---------------------------------------------------------

DEResult DE_DARKSURF(float3 pos) {
	DEResult result = resultInit();
	float absScalem1 = abs(Q3 - 1.0);
	float AbsScaleRaisedTo1mIters = pow(absScalem1, float(1 - MAXSTEPS));

	float4 p = float4(pos, 1), p0 = p;  // p.w is the distance estimate
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		p.xyz = rotatePosition(p.xyz, 0, Q4);
		p.xyz = rotatePosition(p.xyz, 1, Q4);

		//dark-beam's surfboxfold ported by mclarekin----------
		float3 sg = p.xyz; // or 0,0,0
		sg.x = sign(p.x);
		sg.y = sign(p.y);
		sg.z = sign(p.z);

		float3 folder = p.xyz; // or 0,0,0
		float3 Tglad = abs(p.xyz + P1.xyz) - abs(p.xyz - P1.xyz) - p.xyz;

		folder.x = sg.x * (p.x - Tglad.x);
		folder.y = sg.y * (p.y - Tglad.y);
		folder.z = sg.z * (p.z - Tglad.z);

		folder = abs(folder);

		folder.x = min(folder.x, P2.x);
		folder.y = min(folder.y, P2.y);
		folder.z = min(folder.z, P2.z);

		p.x -= sg.x * folder.x;
		p.y -= sg.y * folder.y;
		p.z -= sg.z * folder.z;

		float r2 = dot(p.xyz, p.xyz);
		p *= clamp(max(Q2 / r2, Q2), 0.0, 1.0);
		p = p * Q1 + p0;
		if (r2 > 1000.0) break;

		ot = p;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = ((length(p.xyz) - absScalem1) / p.w - AbsScaleRaisedTo1mIters);
	return result;
}

// 44-----------------------------------------------------------
// Q1 power
// Q2 angle
// Q3 deltaDE

float3 BuffaloIteration(float3 z) {
	if (PREABSX) z.x = abs(z.x);
	if (PREABSY) z.y = abs(z.y);
	if (PREABSZ) z.z = abs(z.z);

	float x2 = z.x * z.x;
	float y2 = z.y * z.y;
	float z2 = z.z * z.z;
	float temp = 1.0 - (z2 / (x2 + y2));
	float newx = (x2 - y2) * temp;
	float newy = Q1 * z.x * z.y * temp;
	float newz = -Q1 * z.z * sqrt(x2 + y2);

	z.x = ABSX ? abs(newx) : newx;
	z.y = ABSY ? abs(newy) : newy;
	z.z = ABSZ ? abs(newz) : newz;
	return z;
}

#define Bailout 4.0

// Compute the distance from `pos` to the bulb.
DEResult DE1(float3 pos) {
	DEResult result = resultInit();
	float3 z = pos;
	float r = length(z);
	float dr = 1.0;
	int i = 0;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	while (r < Bailout && (i < MAXSTEPS)) {
		dr = dr * 2 * r;
		z = BuffaloIteration(z);
		z += (JULIAMODE != 0 ? julia.xyz : pos);
		r = length(z);

		z = rotatePosition(z, 1, Q2);
		z = rotatePosition(z, 2, Q2);

		ot.xyz = z;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));

		++i;
	}

	result.pos = z;
	return result;
}

DEResult DE_BUFFALO(float3 pos) {
	DEResult t1, t2, result = resultInit();
	float3 z = pos;
	float dist = 0;

	if (USEDELTADE != 0) {
		// Author: Krzysztof Marczak (buddhi1980@gmail.com) from  MandelbulberV2
		float deltavalue = max(length(z) * 0.000001, Q3);
		float3 deltaX = float3 (deltavalue, 0.0, 0.0);
		float3 deltaY = float3 (0.0, deltavalue, 0.0);
		float3 deltaZ = float3 (0.0, 0.0, deltavalue);

		t1 = DE1(z);
		float3 zCenter = t1.pos;
		float r = length(zCenter);

		float3 d;
		t1 = DE1(z + deltaX);
		t2 = DE1(z - deltaX);
		float3 zx1 = t1.pos;
		float3 zx2 = t2.pos;
		d.x = min(abs(length(zx1) - r), abs(length(zx2) - r)) / deltavalue;

		t1 = DE1(z + deltaY);
		float3 zy1 = t1.pos;
		t2 = DE1(z - deltaY);
		float3 zy2 = t2.pos;
		d.y = min(abs(length(zy1) - r), abs(length(zy2) - r)) / deltavalue;

		t1 = DE1(z + deltaZ);
		float3 zz1 = t1.pos;
		t2 = DE1(z - deltaZ);
		float3 zz2 = t2.pos;
		d.z = min(abs(length(zz1) - r), abs(length(zz2) - r)) / deltavalue;

		float dr = length(d);

		result.dist = 0.5 * r * log(r) / dr;
	}
	else {
		float r = length(z);
		float dr = 1.0;
		int i = 0;
		float4 ot, trap = OTfixed;
		if (OTstyle == 2) trap.xyz -= pos;

		while (r < Bailout && (i < MAXSTEPS)) {
			dr = dr * 2 * r;
			z = BuffaloIteration(z);
			z += (JULIAMODE != 0 ? julia.xyz : pos);
			r = length(z);

			z = rotatePosition(z, 1, Q2);
			z = rotatePosition(z, 2, Q2);

			ot.xyz = z;
			if (OTstyle > 0) ot -= trap;
			float4 hk = float4(ot.xyz, r);
			result.orbitTrap = min(result.orbitTrap, dot(hk, hk));

			++i;
		}

		result.dist = 0.5 * log(r) * r / dr;
	}

	return result;
}

// 45 ---------------------------------------------------------

DEResult DE_TEMPLE(float3 pos) {
	DEResult result = resultInit();
	float3 p = pos;
	p.xz = abs(.5 - fmod(pos.xz, 1.)) + .01;
	float DEfactor = 1.;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; i++) {
		p = abs(p) - float3(0., Q2, 0.);
		float r2 = dot(p, p);
		float sc = Q1 / clamp(r2 * Q6, Q5, 1.);
		p *= sc;
		DEfactor *= sc;
		p = p - float3(0.5, 1., 0.5);

		p = rotatePosition(p, 0, Q7);
		p = rotatePosition(p, 1, Q8);

		ot.xyz = p;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r2);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	float rr = length(pos + float3(0., -3.03, 1.85 - Q2)) - .017;
	float fl = pos.y - Q3;
	float d = min(fl, length(p) / DEfactor - .0005);
	d = min(d, -pos.y + Q4);
	d = min(d, rr);

	result.dist = d;
	return result;
}

// 46 --------------------------------------------------------

DEResult DE_KALI3(float3 pos) {
	DEResult result = resultInit();
	float r2, dr = 1.0;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		r2 = dot(pos, pos);
		if (r2 > 100.)continue;

		pos = abs(pos) / r2 * Q1 - julia.xyz;
		dr = dr / r2 * Q1;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, r2);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = .15 * (length(pos.xz)) * length(pos.xy) / dr;
	return result;
}

// 50 --------------------------------------------------------

DEResult DE_DONUTS(float3 pos) {
	DEResult result = resultInit();
	float3 n = -float3(Q4, 0, Q5);
	float dis = 1e20;
	float newdis, s = 1.;
	float2 q;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		q = float2(length(pos.xz) - Q1, pos.y);

		newdis = (length(q) - Q2) * s;
		if (newdis < dis) dis = newdis;

		//folding
		pos.xz = abs(pos.xz);//fold to positive quadrant
		if (pos.x < pos.z) pos.xz = pos.zx;//fold 45 degrees
		pos -= 2. * min(0., dot(pos, n)) * n;//fold 22.5 degrees

		//rotation
		pos.yz = float2(-pos.z, pos.y);

		//offset
		pos.x -= Q1;

		//scaling
		pos *= Q3;
		s /= Q3;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		result.orbitTrap = min(result.orbitTrap, dot(ot,ot));
	}

	result.dist = dis;
	return result;
}

// distance estimate -----------------------------------------
DEResult DE_Inner(float3 pos) {
	DEResult result = resultInit();

	switch (EQUATION) {
	case EQU_01_MANDELBULB:		result = DE_MANDELBULB(pos); break;
	case EQU_02_APOLLONIAN:		result = DE_APOLLONIAN(pos); break;
	case EQU_03_APOLLONIAN2:	result = DE_APOLLONIAN2(pos); break;
	case EQU_04_KLEINIAN:		result = DE_KLEINIAN(pos); break;
	case EQU_05_MANDELBOX:		result = DE_MANDELBOX(pos); break;
	case EQU_06_QUATJULIA:		result = DE_QUATJULIA(pos); break;
	case EQU_09_POLY_MENGER:	result = DE_POLY_MENGER(pos); break;
	case EQU_10_GOLD:			result = DE_GOLD(pos); break;
	case EQU_11_SPIDER:			result = DE_SPIDER(pos); break;
	case EQU_12_KLEINIAN2:		result = DE_KLEINIAN2(pos); break;
	case EQU_18_SIERPINSKI:		result = DE_SIERPINSKI_T(pos); break;
	case EQU_19_HALF_TETRA:		result = DE_HALF_TETRA(pos); break;
	case EQU_24_KALEIDO:		result = DE_KALEIDO(pos); break;
	case EQU_25_POLYCHORA:		result = DE_POLYCHORA(pos); break;
	case EQU_30_KALIBOX:		result = DE_KALIBOX(pos); break;
	case EQU_34_FLOWER:			result = DE_FLOWER(pos); break;
	case EQU_38_ALEK_BULB:		result = DE_ALEK_BULB(pos); break;
	case EQU_39_SURFBOX:		result = DE_SURFBOX(pos); break;
	case EQU_40_TWISTBOX:		result = DE_TWISTBOX(pos); break;
	case EQU_41_KALI_RONTGEN:	result = DE_KALI_RONTGEN(pos); break;
	case EQU_42_VERTEBRAE:		result = DE_VERTEBRAE(pos); break;
	case EQU_44_BUFFALO:		result = DE_BUFFALO(pos); break;
	case EQU_47_SPONGE:			result = DE_SPONGE(pos);  break;
	case EQU_20_FULL_TETRA:		result = DE_FULL_TETRA(pos);  break;
	case EQU_28_QUATJULIA2:		result = DE_QUATJULIA2(pos); break;
	case EQU_29_MBROT:			result = DE_MBROT(pos); break;
	case EQU_31_SPUDS:			result = DE_SPUDS(pos); break;
	case EQU_37_SPIRALBOX:		result = DE_SPIRALBOX(pos); break;
	case EQU_43_DARKSURF:		result = DE_DARKSURF(pos); break;
	case EQU_45_TEMPLE:			result = DE_TEMPLE(pos); break;
	case EQU_46_KALI3:			result = DE_KALI3(pos); break;
	case EQU_50_DONUTS:			result = DE_DONUTS(pos); break;
	}

	return result;
}

#else  // DEVLOPEMENT_SINGLE_FRACTAL

DEResult DE_Inner(float3 pos) {
	DEResult result = resultInit();
	float k, t = Q3 + 0.25 * cos(Q4 * PI * Q1 * (pos.z - pos.x));
	float scale = 1;
	float4 ot, trap = OTfixed;
	if (OTstyle == 2) trap.xyz -= pos;

	for (int i = 0; i < MAXSTEPS; ++i) {
		pos = -1.0 + 2.0 * frac(0.5 * pos + 0.5);
		k = Q2 * t / dot(pos, pos);
		pos *= k;
		scale *= k;

		ot.xyz = pos;
		if (OTstyle > 0) ot -= trap;
		float4 hk = float4(ot.xyz, k);
		result.orbitTrap = min(result.orbitTrap, dot(hk, hk));
	}

	result.dist = 1.5 * (0.25 * abs(pos.y) / scale);
	return result;
}

#endif  // DEVLOPEMENT_SINGLE_FRACTAL

DEResult DE(float3 pos) {
	DEResult result;

	if (DOINVERSION) {
		pos = pos - inv1.xyz;
		float r = length(pos);
		float r2 = r * r;
		pos = (INVERSION_RADIUS * INVERSION_RADIUS / r2) * pos + inv1.xyz;

		float an = atan2(pos.y, pos.x) + INVERSION_ANGLE;
		float ra = sqrt(pos.y * pos.y + pos.x * pos.x);
		pos.x = cos(an) * ra;
		pos.y = sin(an) * ra;

		result = DE_Inner(pos);
		float de = result.dist;
		de = r2 * de / (INVERSION_RADIUS * INVERSION_RADIUS + r * de);
		result.dist = de;
	}
	else
		result = DE_Inner(pos);

	return result;
}

// -----------------------------------------------------------
// x = distance, y = iteration count, z = average distance hop
DEResult shortest_dist(float3 eye, float3 dir) {
	DEResult result;
	float hop = 0;
	float dist = MIN_DIST;
	int i = 0;
	float secondSurface = SECONDSURFACE;

	for (; i < MAX_MARCHING_STEPS; ++i) {
		result = DE(eye + dist * dir);
		if (result.dist < MIN_DIST) {
			if (secondSurface == 0.0) break;    // secondSurface is disabled (equals 0), or has already been used
			dist += secondSurface;				// move along ray, and start looking for 2nd surface
			secondSurface = 0;                  // set to zero as 'already been used' marker
		}

		dist += result.dist;
		if (dist >= MAX_DIST) break;

		// don't let average distance be driven into the dirt
		if (result.dist >= 0.0001) hop = hop * 0.95 + result.dist * 0.5;
	}

	result.dist = dist;
	result.iter = float(i);
	result.hop = hop;

	return result;
}

float3 calcNormal(float3 pos) {
	float2 e = float2(1.0, -1.0) * 0.057;
	return normalize(
		e.xyy * DE(pos + e.xyy).dist +
		e.yyx * DE(pos + e.yyx).dist +
		e.yxy * DE(pos + e.yxy).dist +
		e.xxx * DE(pos + e.xxx).dist);
}

//MARK: -
// boxplorer's method
// Harry: this is broken, but stll getting interesting effects from calling it
float3 getBlinnShading(float3 normal, float3 direction, float3 light) {
	float3 halfLV = normalize(light + direction);
	float spe = pow(dot(normal, halfLV), 2);
	float dif = dot(normal, light);
	return dif + spe * SPECULAR;
}

float3 hsv2rgb(float3 c) {
	return lerp(saturate((abs(frac(c.x + float3(1, 2, 3) / 3) * 6 - 3) - 1)), 1, c.y) * c.z;
}

/// Implementation based on: http://en.wikipedia.org/wiki/HSV_color_space
float3 HSVtoRGB(float3 hsv) {
	float3 result = float3(0, 0, 0);

	hsv.x = fmod(hsv.x, 2. * PI);
	int Hi = int(fmod(hsv.x / (2. * PI / 6.), 6.));
	float f = (hsv.x / (2. * PI / 6.)) - float(Hi);
	float p = hsv.z * (1. - hsv.y);
	float q = hsv.z * (1. - f * hsv.y);
	float t = hsv.z * (1. - (1. - f) * hsv.y);
	if (Hi == 0) { result = float3(hsv.z, t, p); }
	if (Hi == 1) { result = float3(q, hsv.z, p); }
	if (Hi == 2) { result = float3(p, hsv.z, t); }
	if (Hi == 3) { result = float3(p, q, hsv.z); }
	if (Hi == 4) { result = float3(t, p, hsv.z); }
	if (Hi == 5) { result = float3(hsv.z, p, q); }
	return result;
}

// -----------------------------------------------------------

float3 cycle(float3 c, float s) {
	float ss = s * OTcycles;
	return float3(0.5, 0.5, 0.5) + 0.5 * float3(cos(ss + c.x), cos(ss + c.y), cos(ss + c.z));
}

float3 getOrbitColor(float4 orbitTrap) {
	orbitTrap.w = sqrt(orbitTrap.w);

	float3 orbitColor;

	if (OTcycles > 0.0) {
		orbitColor =
			cycle(OTx.xyz, orbitTrap.x) * OTx.w * orbitTrap.x +
			cycle(OTy.xyz, orbitTrap.y) * OTy.w * orbitTrap.y +
			cycle(OTz.xyz, orbitTrap.z) * OTz.w * orbitTrap.z +
			cycle(OTr.xyz, orbitTrap.r) * OTr.w * orbitTrap.r;
	}
	else {
		orbitColor =
			OTx.xyz * OTx.w * orbitTrap.x +
			OTy.xyz * OTy.w * orbitTrap.y +
			OTz.xyz * OTz.w * orbitTrap.z +
			OTr.xyz * OTr.w * orbitTrap.r;
	}

	return orbitColor;
}

// -----------------------------------------------------------
Texture2D<float4>   InputMap  : register(t0);
RWTexture2D<float4> OutputMap : register(u0);

[numthreads(12, 12, 1)]
void CSMain(uint3 p:SV_DispatchThreadID)
{
	if (p.x >= uint(XSIZE) || p.y >= uint(YSIZE)) return;

	uint3 q = p;				// copy of current pixel coordinate; x is altered for stereo
	uint xsize = uint(XSIZE);	// copy of current window size; x is altered for stereo
	float4 cam = camera;		// copy of camera position; x is altered for stereo

	if (ISSTEREO) {
		xsize /= 2;         // window x size adjusted for 2 views side by side
		float4 offset = sideVector * PARALLAX;

		if (q.x >= uint(xsize)) {   // right side of stereo pair?
			q.x -= uint(xsize);		// base 0  X coordinate
			cam -= offset;			// adjust for right side parallax
		}
		else {
			cam += offset;   // adjust for left side parallax
		}
	}

	float den = float(YSIZE);
	float dx = 1.5 * (float(q.x) / den - 0.5);
	float dy = -1.5 * (float(q.y) / den - 0.5);
	float3 direction = normalize((sideVector * dx) + (topVector * dy) + viewVector).xyz;
	DEResult result = shortest_dist(cam.xyz, direction);
	float3 color = float3(0, 0, 0);

	if (result.dist <= MAX_DIST - 0.0001) {
		float3 position = cam.xyz + result.dist * direction;
		float3 cc, normal = calcNormal(position);

		// use texture
		if (TONOFF != 0) {
			float len = length(position) / result.dist;
			float x = normal.x / len;
			float y = normal.z / len;
			float w = float(TXSIZE);
			float h = float(TYSIZE);
			float xx = w + (TCENTERX * w + x * TSCALE) * (w + len);
			float yy = h + (TCENTERY * h + y * TSCALE) * (h + len);

			uint2 pt;
			pt.x = uint(fmod(xx, w));
			pt.y = uint(TYSIZE - fmod(yy, h)); // flip Y coord

			color = InputMap[pt].xyz;
		}

		switch (COLORSCHEME) {
		case 0:
			color += float3(1 - (normal / 10 + sqrt(result.iter / 80.0)));
			break;
		case 1:
			cc = abs(1 - (normal / 3 + sqrt(result.iter / 8))) / 10;
			color += cc;
			break;
		case 2:
			cc = abs(1 - (normal + sqrt(result.iter / 20))) / 10;
			color += cc;
			cc = 0.5 + 0.5 * cos(6.2831 * position.z + float3(0.0, 1.0, 2.0));
			color = (color + cc) / 2;
			break;
		case 3:
			color += float3(abs(normal) * 0.1);
			color += HSVtoRGB(color * result.iter * 0.1);
			break;
		case 4:
			cc = abs(normal) * result.iter * 0.01;
			color += cc;
			color += hsv2rgb(color.yzx);
			break;
		case 5:
		{
			float3 nn = normal;
			nn.x += nn.z;
			nn.y += nn.z;
			color += hsv2rgb(float3(normalize(0.5 - nn)));
		}
		break;
		case 6:
		{
			float escape = result.hop * COLORPARAM;
			float co = result.iter * 0.3 - log(log(length(position)) / log(escape)) / log(3.);
			co = sqrt(co) / 3;
			cc = .5 + cos(co + float3(0, 0.3, 0.4));
			color += cc; // blue,magenta,yellow
		}
		break;
		case 7:
		{
			float escape = result.hop * COLORPARAM;
			float co = result.iter - log(log(length(position)) / log(escape)) / log(3.);
			co = sqrt(co / 3);
			co = 0.5 + sin(co) * 8;
			color += float3(co, co, co);
		}
		break;
		}

		float3 diff = viewVector.xyz * result.iter / 10.0;
		float d1 = DE(position - diff).dist;
		float d2 = DE(position + diff).dist;
		float d3 = d1 - d2;
		color *= (1 + (1 - d3) * ENHANCE);

		float3 c2 = color;
		color.x = mix(c2.x, c2.y, COLORROLL);
		color.y = mix(c2.y, c2.z, COLORROLL);
		color.z = mix(c2.z, c2.x, COLORROLL);

		color *= BRIGHT;
		color = 0.5 + (color - 0.5) * CONTRAST * 2;

		float3 zlight = getBlinnShading(normal, direction, lightPosition.xyz);
		color += zlight * SPECULAR;

		if (OTstrength > 0.0) {
			float3 oColor = getOrbitColor(result.orbitTrap);
			color = mix(3.0 * oColor, color, OTstrength);
		}
	}

	OutputMap[p.xy] = float4(color, 1);
}

/*
	if (Reflection==0. || hitNormal == vec3(0.0)) {
		return first;
	} else {
		vec3 d = reflect(dir, hitNormal);
		return mix(first,trace(hit+d*minDist,d,hit, hitNormal),Reflection);
	}
*/