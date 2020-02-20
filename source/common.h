// Control structure definition for HLSL

#ifdef SHADER
#define FLOAT4 float4
#define INTEGER4 int4
#define Q1 P0.x	// individual float names
#define Q2 P0.y
#define Q3 P0.z
#define Q4 P0.w

#define XSIZE			I1.x
#define YSIZE			I1.y
#define MAXSTEPS		I1.w
cbuffer Control : register(b0)

#else
// Control structure definition for C++
#pragma once
#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4305 ) // double as float
#pragma warning( disable : 4244 ) // double as float
#pragma warning( disable : 4127 ) // constexpr

void abortProgram(char* name, int line);
#define ABORT(hr) if(FAILED(hr)) { abortProgram(__FILE__,__LINE__); }

template <class T>
void SafeRelease(T** ppT) { if (*ppT) { (*ppT)->Release(); *ppT = NULL; } }

#define FLOAT4 XMFLOAT4
#define INTEGER4 XMINT4
#define Q1 control.P0.x
#define Q2 control.P0.y
#define Q3 control.P0.z
#define Q4 control.P0.w

#define XSIZE			control.I1.x
#define YSIZE			control.I1.y
#define MAXSTEPS		control.I1.w

struct Control

#endif

// Control structure definition.  ensure 16 byte alighment
{
	FLOAT4 camera;
	FLOAT4 viewVector, topVector, sideVector;
	FLOAT4 P0;

	INTEGER4 I1;
};
