// Control structure definition for HLSL
#ifdef SHADER
#define F4 float4
#define I4 int4
#define Q1 P0.x	// individual float names
#define Q2 P0.y
#define Q3 P0.z
#define Q4 P0.w
#define Q5 P1.x
#define Q6 P1.y
#define Q7 P1.z
#define Q8 P1.w
#define Q9 P2.x
#define QA P2.y	
#define QB P2.z
#define QC P2.w
#define QD P3.x
#define QE P3.y
#define QF P3.z
#define QG P3.w

#define INVERSION_X			inv1.x
#define INVERSION_Y			inv1.y
#define INVERSION_Z			inv1.z
#define INVERSION_RADIUS	inv1.w
#define INVERSION_ANGLE		inv2.x
#define COLORPARAM			julia.w
#define BRIGHT				light.x
#define CONTRAST			light.y
#define SPECULAR			light.z
#define PARALLAX			light.w

cbuffer Control : register(b0)

#else
// Control structure definition for C++
#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

#pragma warning( disable : 4305 ) // double as float
#pragma warning( disable : 4244 ) // double as float
#pragma warning( disable : 4127 ) // constexpr

void abortProgram(char* name, int line);
#define ABORT(hr) if(FAILED(hr)) { abortProgram(__FILE__,__LINE__); }

#define F4 XMFLOAT4
#define I4 XMINT4
#define Q1 control.P0.x
#define Q2 control.P0.y
#define Q3 control.P0.z
#define Q4 control.P0.w
#define Q5 control.P1.x
#define Q6 control.P1.y
#define Q7 control.P1.z
#define Q8 control.P1.w
#define Q9 control.P2.x
#define QA control.P2.y	
#define QB control.P2.z
#define QC control.P2.w
#define QD control.P3.x
#define QE control.P3.y
#define QF control.P3.z
#define QG control.P3.w

#define INVERSION_X			control.inv1.x
#define INVERSION_Y			control.inv1.y
#define INVERSION_Z			control.inv1.z
#define INVERSION_RADIUS	control.inv1.w
#define INVERSION_ANGLE		control.inv2.x
#define COLORPARAM			control.julia.w
#define BRIGHT				control.light.x
#define CONTRAST			control.light.y
#define SPECULAR			control.light.z
#define PARALLAX			control.light.w

struct Control

#endif

// Control structure definition.  ensure 16 byte alighment
{
	F4 camera;
	F4 viewVector, topVector, sideVector;
	F4 P0, P1, P2, P3, P4, P5, P6;

	F4 inv1, inv2;
	F4 julia;	
	F4 light; 
	F4 lightPosition;

	int xSize;
	int ySize;
	int equation;
	int maxSteps;
	int doInversion;
	int juliaboxMode;
	int showBalls;
	int fourGen;
	int finalIterations;
	int boxIterations;
	int colorScheme;
	int isStereo;
	int preabsx;
	int preabsy;
	int preabsz;
	int absx;
	int absy;
	int absz;
	int useDeltaDE;
	int unused;
};

#define EQU_01_MANDELBULB 1
#define EQU_02_APOLLONIAN 2
#define EQU_03_APOLLONIAN2 3
#define EQU_04_KLEINIAN 4
#define EQU_05_MANDELBOX 5
#define EQU_06_QUATJULIA 6
#define EQU_09_POLY_MENGER 7
#define EQU_10_GOLD 8
#define EQU_11_SPIDER 9
#define EQU_12_KLEINIAN2 10
#define EQU_18_SIERPINSKI 11
#define EQU_19_HALF_TETRA 12
#define EQU_24_KALEIDO 13 
#define EQU_25_POLYCHORA 14 
#define EQU_30_KALIBOX 15
#define EQU_34_FLOWER 16
#define EQU_38_ALEK_BULB 17
#define EQU_39_SURFBOX 18
#define EQU_41_KALI_RONTGEN 19
#define EQU_42_VERTEBRAE 20
#define EQU_44_BUFFALO 21
#define EQU_47_SPONGE 22
#define EQU_MAX 23
