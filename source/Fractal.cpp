#include "stdafx.h"
#include <time.h>
#include "Fractal.h"
#include "View.h"
#include "SaveLoad.h"
#include "WICTextureLoader.h"
#include "ColorMap.h"
#include "common.h"

Fractal fractal;
Widget pWidget;
Widget cWidget;

extern HWND g_hWnd;

void Fractal::init() {

#ifdef DEVLOPEMENT_SINGLE_FRACTAL
	EQUATION = 2;
#else
	EQUATION = EQU_04_KLEINIAN;
#endif

	DOINVERSION = 1;
	ISSTEREO = 0;
	PARALLAX = 0.003;
	COLORPARAM = 25000;
	BRIGHT = 1;
	CONTRAST = 0.5;
	SPECULAR = 0;
	lightAngle = 0;
	jogAmount = XMFLOAT4(0, 0, 0, 0);
	alterationSpeed = 1;
	isShiftKeyPressed = false;
	isControlKeyPressed = false;

	reset();
	resetColors();
	updateWindowTitle();
	isDirty = true;
}

FLOAT length(XMFLOAT4 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

XMFLOAT4 toRectangular(XMFLOAT4 sph) {
	FLOAT ss = sph.x * sin(sph.z);
	return XMFLOAT4(
		ss * cos(sph.y),
		ss * sin(sph.y),
		sph.x * cos(sph.z),
		0);
}

XMFLOAT4 toSpherical(XMFLOAT4 rec) {
	return XMFLOAT4(
		length(rec),
		atan2(rec.y, rec.x),
		atan2(sqrtf(rec.x * rec.x + rec.y * rec.y), rec.z),
		0);
}

void Fractal::updateShaderDirectionVector(XMFLOAT4 v) {
	// control.viewVector = normalize(v)
	XMVECTOR viewVector = XMVector4Normalize(DirectX::XMLoadFloat4(&v));
	DirectX::XMStoreFloat4(&control.viewVector, viewVector);

	control.topVector = toSpherical(control.viewVector);
	control.topVector.z += 1.5708;

	// control.topVector = normalize(toRectangular(control.topVector));
	XMFLOAT4 temp = v;
	temp = toRectangular(control.topVector);
	XMVECTOR topVector = XMVector4Normalize(DirectX::XMLoadFloat4(&temp));
	DirectX::XMStoreFloat4(&control.topVector, topVector);

	XMVECTOR sideVector = XMVector3Cross(viewVector, topVector);
	sideVector = XMVector4Normalize(sideVector) * XMVector4Length(topVector);
	DirectX::XMStoreFloat4(&control.sideVector, sideVector);
}

void inversionSettings(float x, float y, float z, float radius, float angle) {
	INVERSION_X = x;
	INVERSION_Y = y;
	INVERSION_Z = z;
	INVERSION_RADIUS = radius;
	INVERSION_ANGLE = angle;
}

void Fractal::resetColors() {
	BRIGHT = 1;
	CONTRAST = 0.7;
	COLORPARAM = 4000;
	ENHANCE = 0;
	COLORROLL = 0;
	SECONDSURFACE = 0;
	lightAngle = 1.5;
	SPECULAR = 0.2;
	OTcycles = 0;
	OTstrength = 0;
	OTindexX = 10;
	OTindexY = 110;
	OTindexZ = 180;
	OTindexR = 250;
	OTcolorXW = 1;
	OTcolorYW = 1;
	OTcolorZW = 1;
	OTcolorRW = 1;
	OTstyle = 0;
	cWidget.refresh();
}

void Fractal::reset() {
	SPECULAR = 0;
	XSIZE = windowWidth;
	YSIZE = windowHeight;
	updateShaderDirectionVector(XMFLOAT4(0, 0.1, 1, 0));

	switch (EQUATION) {
	case EQU_01_MANDELBULB:
		updateShaderDirectionVector(XMFLOAT4(0.010000015, 0.41950363, 0.64503753, 0));
		control.camera = XMFLOAT4(0.038563743, -1.1381346, -1.8405379, 0);
		MAXSTEPS = 10;

		//  Q1 power
		Q1 = 8;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.138822, -1.4459486, -1.9716375, 0);
			updateShaderDirectionVector(XMFLOAT4(0.012995179, 0.54515165, 0.8382366, 0));
			inversionSettings(-0.10600001, -0.74200004, -1.3880001, 2.14, 0.9100002);
		}
		break;

	case EQU_02_APOLLONIAN:
	case EQU_03_APOLLONIAN2:
		control.camera = XMFLOAT4(0.42461035, 10.847559, 2.5749633, 0);
		MAXSTEPS = 12;

		//  Q1 multiplier
		//  Q2 foam
		//  Q3 foam2
		//  Q4 bend
		Q1 = 25.0f;
		Q2 = 1.05265248;
		Q3 = 1.06572711f;
		Q4 = 0.0202780124f;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-4.4953876, -6.3138175, -29.144863, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			Q2 = 1.0326525;
			Q3 = 0.9399999;
			Q4 = 0.01;
			inversionSettings(0.56, 0.34, 0.46, 2.7199993, 0);
		}
		break;
	case EQU_04_KLEINIAN:
		control.camera = XMFLOAT4(0.5586236, 1.1723881, -1.8257363, 0);
		MAXSTEPS = 70;
		FINALITERATIONS = 21;
		BOXITERATIONS = 17;
		SHOWBALLS = 1;
		FOURGEN = 0;

		// Q1 box_size_x
		// Q2 box_size_z
		// Q3 KleinR
		// Q4 KleinI
		// Q5 Clamp_y
		// Q6 Clamp_DF
		Q1 = 0.6318979;
		Q2 = 1.3839532;
		Q3 = 1.9324;
		Q4 = 0.04583;
		Q5 = 0.221299887;
		Q6 = 0.00999999977;
		inversionSettings(1.0517285, 0.7155759, 0.9883028, 2.06132293, 5.5392437);

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-1.5613757, -0.61350304, 0.41508165, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			Q1 = 0.6880005;
			Q2 = 0.38800019;
			Q3 = 1.97239995;
			Q4 = 0.00999999977;
			Q5 = 0.201299876;
			Q6 = 0.00999999977;
			inversionSettings(-1.67399943, -0.494000345, 0.721998572, 0.639999986, 4.15921211);
		}
		break;
	case EQU_05_MANDELBOX:
		control.camera = XMFLOAT4(-1.3771019, 0.9999971, -5.037427, 0);

		// Q1 Scale Factor
		// Q2 Box
		// Q3 Sphere 1
		// Q4 Sphere 2
		Q1 = 2.42;
		Q2 = 1.42;
		Q3 = 1.0099998;
		Q4 = 0.02;
		MAXSTEPS = 17;
		control.julia.x = 0.0;
		control.julia.y = -6.0;
		control.julia.z = -8.0;
		BRIGHT = 1.3299997;
		CONTRAST = 0.3199999;
		JULIAMODE = true;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-1.4471021, 0.23879418, -4.3080645, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950371, 0.99503714, 0));
			inversionSettings(-0.13600002, 0.30600032, 0.011999967, 0.62999976, 0.37999997);
		}
		break;
	case EQU_06_QUATJULIA:
		control.camera = XMFLOAT4(-0.010578117, -0.49170083, -2.4, 0);
		Q1 = -1.74999952;
		Q2 = -0.349999964;
		Q3 = -0.0499999635;
		Q4 = -0.0999999642;
		MAXSTEPS = 7;
		CONTRAST = 0.28;
		SPECULAR = 0.9;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.010578117, -0.49170083, -2.4, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950371, 0.99503714, 0));
			inversionSettings(0.098000005, 0.19999999, -1.0519996, 1.5200003, -0.29999992);
		}
		break;
	case EQU_09_POLY_MENGER:
		control.camera = XMFLOAT4(-0.20046826, -0.51177955, -5.087464, 0);
		Q1 = 4.7799964;
		Q2 = 2.1500008;
		Q3 = 2.899998;
		Q4 = 3.0999982;
		Q5 = 5;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.62953156, 0.51310825, -5.1899557, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.242, 0.15800002, 0.074000046, 0.31, -0.009999985);
		}
		break;
	case EQU_10_GOLD:
		control.camera = XMFLOAT4(0.038563743, -1.1381346, -1.8405379, 0);
		updateShaderDirectionVector(XMFLOAT4(0.010000015, 0.41950363, 0.64503753, 0));
		Q1 = -0.09001912;
		Q2 = 0.43999988;
		Q3 = 1.0499994;
		Q4 = 1;
		Q5 = 0;
		Q6 = 0.6;
		Q7 = 0;
		MAXSTEPS = 15;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.042072453, -0.99094355, -1.6142143, 0);
			updateShaderDirectionVector(XMFLOAT4(0.012995181, 0.54515177, 0.83823675, 0));
			inversionSettings(0.036, 0.092000015, -0.15200002, 0.17999996, -0.25999996);
		}
		break;
	case EQU_11_SPIDER:
		control.camera = XMFLOAT4(0.04676684, -0.50068825, -3.4419205, 0);
		Q1 = 0.13099998;
		Q2 = 0.21100003;
		Q3 = 0.041;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.04676684, -0.46387178, -3.0737557, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.28600028, 0.18000007, -0.07799993, 0.13, -0.079999976);
		}
		break;
	case EQU_12_KLEINIAN2:
		control.camera = XMFLOAT4(4.1487565, 2.6955016, 1.3862593, 0);
		Q1 = -0.7821867;
		Q2 = -0.5424057;
		Q3 = -0.4748369;
		Q4 = 0.7999992;
		Q5 = 0.5;
		Q6 = 1.3;
		Q7 = 1.5499997;
		Q8 = 0.9000002;
		Q9 = 1;	 // power

		if (DOINVERSION) {
			control.camera = XMFLOAT4(4.1487565, 2.6955016, 1.3862593, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(-0.092, 0.01999999, -0.47600016, 4.2999983, 0.13000003);
		}
		break;
	case EQU_18_SIERPINSKI:
		control.camera = XMFLOAT4(0.03816485, -0.08283869, -0.63742965, 0);
		Q1 = 1.3240005;
		Q2 = 1.5160003;
		Q3 = 3.1415962;
		Q4 = 1.5610005;
		MAXSTEPS = 27;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.03816485, -0.12562533, -1.0652959, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.11600001, 0.07200002, -0.00799999, 0.20999998, 0.110000014);
		}
		break;
	case EQU_19_HALF_TETRA:
		control.camera = XMFLOAT4(-0.023862544, -0.113349974, -0.90810966, 0);
		Q1 = 1.2040006;
		Q2 = 9.236022;
		Q3 = -3.9415956;
		Q4 = 0.79159856;
		MAXSTEPS = 53;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.13613744, 0.07272194, -0.85636866, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.07199999, 0.070000015, 0.037999995, 0.33999994, 0.44);
		}
		break;
	case EQU_20_FULL_TETRA:
		control.camera = XMFLOAT4(-0.018542236, -0.08817809, -0.90810966, 0);
		Q1 = 1.1280007;
		Q2 = 8.099955;
		Q3 = -1.2150029;
		Q4 = -0.018401254;
		MAXSTEPS = 71;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.018542236, -0.08817809, -0.90810966, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.0069999956, 0.03999999, 0.22400002, 0.3, 0.4399999);
		}
		break;
	case EQU_24_KALEIDO:
		control.camera = XMFLOAT4(-0.00100744, -0.1640267, -1.7581517, 0);
		Q1 = 1.1259973;
		Q2 = 0.8359996;
		Q3 = -0.016000029;
		Q4 = 1.7849922;
		Q5 = -1.2375059;
		MAXSTEPS = 35.0;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.00100744, -0.1640267, -1.7581517, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.034000028, -0.026000002, -1.082, 0.97000015, -0.17);
		}
		break;
	case EQU_25_POLYCHORA:
		control.camera = XMFLOAT4(-0.00100744, -0.16238609, -1.7581517, 0);
		Q1 = 5.0;
		Q2 = 1.3159994;
		Q3 = 2.5439987;
		Q4 = 4.5200005;
		Q5 = 0.08000006;
		Q6 = 0.008000016;
		Q7 = -1.5999997;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.54899234, -0.03701113, -0.7053995, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.5320001, 0.012000054, -0.023999948, 0.36999995, 0.15);
		}
		break;
	case EQU_28_QUATJULIA2:
		control.camera = XMFLOAT4(-0.010578117, -0.49170083, -2.4, 0);
		MAXSTEPS = 7;
		Q1 = -1.7499995;
		control.julia = XMFLOAT4(0, 0, 0, 0);

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.010578117, -0.49170083, -2.4, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.1, .006000016, -0.072, 0.51, 0.1);
		}
		break;
	case EQU_29_MBROT:
		control.camera = XMFLOAT4(-0.23955467, -0.3426069, -2.4, 0);
		MAXSTEPS = 10;
		Q1 = -9.685755e-08;
		control.julia.x = 0.39999992;
		control.julia.y = 5.399997;
		control.julia.z = -2.3;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.39044535, -0.1694704, -0.16614081, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.4520001, -0.148, 0.626, 0.16999993, 0.07000001);
		}
		break;
	case EQU_30_KALIBOX:
		control.camera = XMFLOAT4(0.32916373, -0.42756003, -3.6908724, 0);
		Q1 = 1.6500008;
		Q2 = 0.35499972;
		Q5 = -0.7600006;
		Q6 = -0.25000006;
		Q7 = -0.8000002;
		Q9 = 3.5800009; // angle1
		control.julia.x = 0;
		control.julia.y = 0;
		control.julia.z = 0;
		BRIGHT = 0.9000001;
		MAXSTEPS = 11;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.32916373, -0.42756003, -3.6908724, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.16800001, -0.080000006, -0.39400005, 0.96000016, 0.1);
		}
		break;
	case EQU_31_SPUDS:
		control.camera = XMFLOAT4(0.98336715, -1.2565054, -3.960955, 0);
		Q1 = 3.7524672;
		Q2 = 1.0099992;
		Q3 = 1.0059854;
		Q4 = 1.0534152;
		Q5 = 3.2999988;
		Q6 = 1.1883448;
		Q7 = 4.100001;
		Q8 = 3.2119942;
		MAXSTEPS = 8;
		BRIGHT = 0.92;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.18336754, -0.29131955, -4.057477, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(-0.544, -0.18200001, -0.44799998, 1.3700002, 0.1);
		}
		break;
	case EQU_34_FLOWER:
		control.camera = XMFLOAT4(-0.16991696, -2.5964863, -12.54011, 0);
		Q1 = 1.6740334;
		control.julia.x = 6.0999966;
		control.julia.y = 13.999996;
		control.julia.z = 3.0999992;
		BRIGHT = 1.5000001;
		MAXSTEPS = 10;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.16991696, -2.5964863, -12.54011, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.03800006, 0.162, 0.11799997, 0.7099998, 0.18000002);
		}
		break;
	case EQU_37_SPIRALBOX:
		control.camera = XMFLOAT4(0.047575176, -0.122939646, 1.5686907, 0);
		Q1 = 0.8810008;
		control.julia.x = 1.9000009;
		control.julia.y = 1.0999998;
		control.julia.z = 0.19999993;
		MAXSTEPS = 9;
		JULIAMODE = 1;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.047575176, -0.122939646, 1.5686907, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.1, 0.07600006, -0.46800002, 2.31, 0.1);
		}
		break;
	case EQU_38_ALEK_BULB:
		control.camera = XMFLOAT4(-0.07642456, -0.23929897, -2.1205378, 0);
		Q1 = 3.4599924; // power
		control.julia.x = 0.6000004;
		control.julia.y = 0.29999986;
		control.julia.z = 0.29999968;
		BRIGHT = 1.4000001;
		CONTRAST = 0.5;
		MAXSTEPS = 10;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.07642456, -0.23929897, -2.1205378, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.076, 0.029999996, 0.015999988, 2.01, 0.06000001);
		}
		break;
	case EQU_39_SURFBOX:
		control.camera = XMFLOAT4(-0.37710285, 0.4399976, -5.937426, 0);
		Q1 = 1.4199952;
		Q2 = 4.1000023;
		Q3 = 1.2099996;
		Q4 = 0.0;
		Q5 = 4.3978653;
		Q6 = 2.5600004; // power
		control.julia.x = -0.6800002;
		control.julia.y = -4.779989;
		control.julia.z = -7.2700005;
		BRIGHT = 1.01;
		CONTRAST = 0.5;
		MAXSTEPS = 17;
		JULIAMODE = 1;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.37710285, 0.4399976, -5.937426, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.10799999, 0.19999999, 0.1, 0.47000003, -0.15999997);
		}
		break;
	case EQU_40_TWISTBOX:
		control.camera = XMFLOAT4(0.24289839, -2.1800025, -9.257425, 0);
		MAXSTEPS = 24;
		Q1 = 1.5611011;
		Q2 = 8.21999;
		control.julia.x = 3.2779012;
		control.julia.y = -3.0104024;
		control.julia.z = -3.2913034;
		BRIGHT = 1.7;
		CONTRAST = 0.18;
		JULIAMODE = 1;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.23289838, 0.048880175, -1.2394277, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.068000056, 0.1, 0.029999983, 0.24000005, -0.7099997);
		}
		break;
	case EQU_41_KALI_RONTGEN:
		control.camera = XMFLOAT4(-0.16709971, -0.020002633, -0.9474212, 0);
		Q1 = 0.88783956;
		Q2 = 1.3439986;
		Q3 = 0.56685466;
		Q4 = 0;
		MAXSTEPS = 7;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.4029004, -0.036918215, -0.6140825, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.23400004, 0.04200006, 0.07000005, 0.22000004, 1.4000001);
		}
		break;
	case EQU_42_VERTEBRAE:
		control.camera = XMFLOAT4(0.5029001, -1.3100017, -9.947422, 0);
		Q1 = 5.599995;
		Q2 = 8.699999;
		Q3 = -3.6499987;
		Q4 = 0.089999855;
		Q5 = 1.0324188;
		Q6 = 9.1799965;
		Q7 = -0.68002427;
		Q8 = 1.439993;
		Q9 = -0.6299968;
		QA = 2.0999985;
		QB = -4.026443;
		QC = -4.6699996;
		QD = -9.259983;
		QE = 0.8925451;
		QF = -0.0112106;
		QG = 2.666039;
		MAXSTEPS = 2;
		BRIGHT = 1.47;
		CONTRAST = 0.22000006;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(1.0229, -1.1866168, -8.713577, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(-0.9600001, -0.5200006, -3.583999, 4.01, 3.1000001);
		}
		break;
	case EQU_43_DARKSURF:
		control.camera = XMFLOAT4(-0.4870995, -1.9200011, -1.7574148, 0);
		Q1 = 7.1999893;
		Q2 = 0.34999707;
		Q3 = -1;
		Q4 = -1.5399991; // angle1
		Q5 = 1.0;  // n1
		Q6 = 0.549999;
		Q7 = 0.88503367;
		Q9 = 0.99998015; // n2
		QA = 1.8999794;
		QB = 3.3499994;
		MAXSTEPS = 10;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.10709968, -0.06923248, -1.9424983, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.068000056, 0.10799999, 0.09400001, 0.13999999, -0.95000005);
		}
		break;
	case EQU_44_BUFFALO:
		control.camera = XMFLOAT4(0.008563751, -2.8381326, -0.2005394, 0);
		updateShaderDirectionVector(XMFLOAT4(-0.0045253364, 0.73382026, 0.091496624, 0));
		Q1 = 2.6300106; // power
		Q2 = 0.009998376; // angle
		Q3 = 1; // deltaDE
		MAXSTEPS = 4;
		control.julia.x = 0.6382017;
		control.julia.y = -0.4336;
		control.julia.z = -0.9396994;
		PREABSX = 1;
		PREABSY = 1;
		PREABSZ = 0;
		ABSX = 1;
		ABSY = 0;
		ABSZ = 1;
		USEDELTADE = 0;
		JULIAMODE = 1;
		BRIGHT = 1.2100002;
		CONTRAST = 0.17999999;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.033746917, -0.4353387, -0.07229346, 0);
			updateShaderDirectionVector(XMFLOAT4(-0.0061193192, 0.9922976, 0.12372495, 0));
			inversionSettings(0.016000055, 0.08400003, 5.2619725e-08, 0.3, -2.0000002);
		}
		break;
	case EQU_45_TEMPLE:
		control.camera = XMFLOAT4(1.4945942, -0.7769, -1.71, 0);
		Q1 = 1.9772799;
		Q2 = 0.3100043;
		Q3 = -1.52;
		Q4 = 0.77;
		Q5 = 0.6599997;
		Q6 = 1.1499994;
		Q7 = -3.139998;
		Q8 = 2.6600018;
		MAXSTEPS = 16;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.15459429, 0.04401703, -0.33744603, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950371, 0.99503714, 0));
			inversionSettings(-0.07599993, 0.07000005, 0.2139999, 2.29, -0.029999984);
		}
		break;
	case EQU_46_KALI3:
		control.camera = XMFLOAT4(-0.025405688, -0.418378, -3.017353, 0);
		Q1 = -0.5659971;
		control.julia.x = -0.97769934;
		control.julia.y = -0.8630977;
		control.julia.z = -0.58009946;
		MAXSTEPS = 40;
		BRIGHT = 3;
		CONTRAST = 0.1;
		JULIAMODE = 1;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.025405688, -0.12185693, -0.8561316, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.0020000527, -0.009999948, -0.158, 0.29000002, -0.82000005);
		}
		break;
	case EQU_47_SPONGE:
		control.camera = XMFLOAT4(0.7610872, -0.7994865, -3.8773263, 0);
		Q1 = -0.8064072;
		Q2 = -0.74000216;
		Q3 = -1.0899884;
		Q4 = 1.2787694;
		Q5 = 0.26409245;
		Q6 = -0.76119435;
		Q7 = 0.2899983;
		Q8 = 0.27301705;
		Q9 = 6;
		QA = -6;
		MAXSTEPS = 3;
		BRIGHT = 2.31;
		CONTRAST = 0.17999999;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(0.25108737, -0.9736173, -2.603676, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.35200006, 0.009999977, -0.092, 1.0600003, -0.019999992);
		}
		break;
	case EQU_50_DONUTS:
		control.camera = XMFLOAT4(-0.2254057, -7.728364, -19.269318, 0);
		updateShaderDirectionVector(XMFLOAT4(-2.0272768e-08, 0.46378687, 0.89157283, 0));
		Q1 = 7.9931593;
		Q2 = 0.35945648;
		Q3 = 2.8700645;
		Q4 = 0.0;
		Q5 = 0.0;
		MAXSTEPS = 4;

		if (DOINVERSION) {
			control.camera = XMFLOAT4(-0.2254057, -7.728364, -19.269318, 0);
			updateShaderDirectionVector(XMFLOAT4(-2.0172154e-08, 0.4614851, 0.8871479, 0));
			inversionSettings(-1.8719988, -4.1039987, -1.367999, 7.589995, -2.7999995);
		}
		break;
	}

	defineWidgetsForCurrentEquation(false);
}

void Fractal::juliaGroup(FLOAT range, FLOAT delta) {
	pWidget.addLegend("");
	pWidget.addBoolean("J: Julia Mode", &JULIAMODE);

	if (JULIAMODE) {
		pWidget.addEntry("  X", &control.julia.x, -range, range, delta, kfloat);
		pWidget.addEntry("  Y", &control.julia.y, -range, range, delta, kfloat);
		pWidget.addEntry("  Z", &control.julia.z, -range, range, delta, kfloat);
	}
}

// define pWidget entries for current EQUATION
void Fractal::defineWidgetsForCurrentEquation(bool resetFocus) {
	pWidget.reset();
	cWidget.reset();

	switch (EQUATION) {
	case EQU_01_MANDELBULB:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 30, 1, kinteger);
		pWidget.addEntry("Power", &Q1, 1.5, 12, 0.02, kfloat);
		break;
	case EQU_02_APOLLONIAN:
	case EQU_03_APOLLONIAN2:
		pWidget.addEntry("Iterations", &MAXSTEPS, 2, 10, 1, kinteger);
		pWidget.addEntry("Multiplier", &Q1, 10, 300, 0.2, kfloat);
		pWidget.addEntry("Foam", &Q2, 0.1, 3, 0.02, kfloat);
		pWidget.addEntry("Foam2", &Q3, 0.1, 3, 0.02, kfloat);
		pWidget.addEntry("Bend", &Q4, 0.01, 0.03, 0.0001, kfloat);
		break;
	case EQU_04_KLEINIAN:
		pWidget.addBoolean("B: ShowBalls", &SHOWBALLS);
		pWidget.addBoolean("F: FourGen", &FOURGEN);
		pWidget.addLegend(" ");
		pWidget.addEntry("Final Iterations", &FINALITERATIONS, 1, 39, 1, kinteger);
		pWidget.addEntry("Box Iterations", &BOXITERATIONS, 1, 10, 1, kinteger);
		pWidget.addEntry("Box Size X", &Q1, 0.01, 2, 0.006, kfloat);
		pWidget.addEntry("Box Size Z", &Q2, 0.01, 2, 0.006, kfloat);
		pWidget.addEntry("Klein R", &Q3, 0.01, 2.5, 0.005, kfloat);
		pWidget.addEntry("Klein I", &Q4, 0.01, 2.5, 0.005, kfloat);
		pWidget.addEntry("Clamp Y", &Q5, 0.001, 2, 0.01, kfloat);
		pWidget.addEntry("Clamp DF", &Q6, 0.001, 2, 0.03, kfloat);
		break;
	case EQU_05_MANDELBOX:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 60, 1, kinteger);
		pWidget.addEntry("Scale Factor", &Q1, 0.6, 10, 0.02, kfloat);
		pWidget.addEntry("Box", &Q2, 0, 10, 0.001, kfloat);
		pWidget.addEntry("Sphere 1", &Q3, 0, 4, 0.01, kfloat);
		pWidget.addEntry("Sphere 2", &Q4, 0, 4, 0.01, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_06_QUATJULIA:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 10, 1, kinteger);
		pWidget.addEntry("X", &Q1, -5, 5, 0.05, kfloat);
		pWidget.addEntry("Y", &Q2, -5, 5, 0.05, kfloat);
		pWidget.addEntry("Z", &Q3, -5, 5, 0.05, kfloat);
		pWidget.addEntry("W", &Q4, -5, 5, 0.05, kfloat);
		break;
	case EQU_09_POLY_MENGER:
		pWidget.addEntry("Menger", &Q2, 1.1, 2.9, 0.05, kfloat);
		pWidget.addEntry("Stretch", &Q3, 0, 10, 0.05, kfloat);
		pWidget.addEntry("Spin", &Q4, 0.1, 5, 0.05, kfloat);
		pWidget.addEntry("Twist", &Q1, 0.5, 7, 0.05, kfloat);
		pWidget.addEntry("Shape", &Q5, 0.1, 50, 0.2, kfloat);
		break;
	case EQU_10_GOLD:
		pWidget.addEntry("Iterations", &MAXSTEPS, 2, 20, 1, kinteger);
		pWidget.addEntry("T", &Q1, -5, 5, 0.01, kfloat);
		pWidget.addEntry("U", &Q2, -5, 5, 0.01, kfloat);
		pWidget.addEntry("V", &Q3, -5, 5, 0.01, kfloat);
		pWidget.addEntry("W", &Q4, -5, 5, 0.01, kfloat);
		pWidget.addEntry("X", &Q5, -5, 5, 0.01, kfloat);
		pWidget.addEntry("Y", &Q6, -5, 5, 0.01, kfloat);
		pWidget.addEntry("Z", &Q7, -5, 5, 0.01, kfloat);
		break;
	case EQU_11_SPIDER:
		pWidget.addEntry("X", &Q1, 0.001, 5, 0.01, kfloat);
		pWidget.addEntry("Y", &Q2, 0.001, 5, 0.01, kfloat);
		pWidget.addEntry("Z", &Q3, 0.001, 5, 0.01, kfloat);
		break;
	case EQU_12_KLEINIAN2:
		pWidget.addEntry("Shape", &Q9, 0.01, 2, 0.005, kfloat);
		pWidget.addEntry("minX", &Q1, -5, 5, 0.01, kfloat);
		pWidget.addEntry("minY", &Q2, -5, 5, 0.01, kfloat);
		pWidget.addEntry("minZ", &Q3, -5, 5, 0.01, kfloat);
		pWidget.addEntry("minW", &Q4, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxX", &Q5, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxY", &Q6, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxZ", &Q7, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxW", &Q8, -5, 5, 0.01, kfloat);
		break;
	case EQU_18_SIERPINSKI:
		pWidget.addEntry("Iterations", &MAXSTEPS, 11, 40, 1, kinteger);
		pWidget.addEntry("Scale", &Q1, 1.18, 1.8, 0.02, kfloat);
		pWidget.addEntry("Y", &Q2, 0.5, 3, 0.02, kfloat);
		pWidget.addEntry("Angle1", &Q3, -4, 4, 0.01, kfloat);
		pWidget.addEntry("Angle2", &Q4, -4, 4, 0.01, kfloat);
		break;
	case EQU_19_HALF_TETRA:
		pWidget.addEntry("Iterations", &MAXSTEPS, 9, 50, 1, kinteger);
		pWidget.addEntry("Scale", &Q1, 1.12, 1.5, 0.02, kfloat);
		pWidget.addEntry("Y", &Q2, 2, 10, 0.1, kfloat);
		pWidget.addEntry("Angle1", &Q3, -4, 4, 0.01, kfloat);
		pWidget.addEntry("Angle2", &Q4, -4, 4, 0.01, kfloat);
		break;
	case EQU_20_FULL_TETRA:
		pWidget.addEntry("Iterations", &MAXSTEPS, 21, 70, 1, kinteger);
		pWidget.addEntry("Scale", &Q1, 1.06, 1.16, 0.005, kfloat);
		pWidget.addEntry("Y", &Q2, 4.6, 20, 0.1, kfloat);
		pWidget.addEntry("Angle1", &Q3, -4, 4, 0.025, kfloat);
		pWidget.addEntry("Angle2", &Q4, -4, 4, 0.025, kfloat);
		break;
	case EQU_24_KALEIDO:
		pWidget.addEntry("Iterations", &MAXSTEPS, 10, 200, 1, kinteger);
		pWidget.addEntry("Scale", &Q1, 0.5, 2, 0.0005, kfloat);
		pWidget.addEntry("Y", &Q2, -5, 5, 0.004, kfloat);
		pWidget.addEntry("Z", &Q3, -5, 5, 0.004, kfloat);
		pWidget.addEntry("Angle1", &Q4, -4, 4, 0.005, kfloat);
		pWidget.addEntry("Angle2", &Q5, -4, 4, 0.005, kfloat);
		break;
	case EQU_25_POLYCHORA:
		pWidget.addEntry("Distance 1", &Q1, -2, 10, 0.1, kfloat);
		pWidget.addEntry("Distance 2", &Q2, -2, 10, 0.1, kfloat);
		pWidget.addEntry("Distance 3", &Q3, -2, 10, 0.1, kfloat);
		pWidget.addEntry("Distance 4", &Q4, -2, 10, 0.1, kfloat);
		pWidget.addEntry("Ball", &Q5, 0, 0.35, 0.02, kfloat);
		pWidget.addEntry("Stick", &Q6, 0, 0.35, 0.02, kfloat);
		pWidget.addEntry("Spin", &Q7, -15, 15, 0.05, kfloat);
		break;
	case EQU_28_QUATJULIA2:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 10, 1, kinteger);
		pWidget.addEntry("Mul", &Q1, -5, 5, 0.05, kfloat);
		pWidget.addEntry("Offset X", &control.julia.x, -15, 15, 0.1, kfloat);
		pWidget.addEntry("Offset Y", &control.julia.y, -15, 15, 0.1, kfloat);
		pWidget.addEntry("Offset Z", &control.julia.z, -15, 15, 0.1, kfloat);
		break;
	case EQU_29_MBROT:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 10, 1, kinteger);
		pWidget.addEntry("Offset", &Q1, -5, 5, 0.05, kfloat);
		pWidget.addEntry("Rotate X", &control.julia.x, -15, 15, 0.1, kfloat);
		pWidget.addEntry("Rotate Y", &control.julia.y, -15, 15, 0.1, kfloat);
		pWidget.addEntry("Rotate Z", &control.julia.z, -15, 15, 0.1, kfloat);
		break;
	case EQU_30_KALIBOX:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 30, 1, kinteger);
		pWidget.addEntry("Scale", &Q1, -5, 5, 0.05, kfloat);
		pWidget.addEntry("MinRad2", &Q2, -5, 5, 0.05, kfloat);
		pWidget.addEntry("Trans X", &Q5, -15, 15, 0.01, kfloat);
		pWidget.addEntry("Trans Y", &Q6, -15, 15, 0.01, kfloat);
		pWidget.addEntry("Trans Z", &Q7, -1, 5, 0.01, kfloat);
		pWidget.addEntry("Angle", &Q9, -4, 4, 0.02, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_31_SPUDS:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 30, 1, kinteger);
		pWidget.addEntry("Power", &Q5, 1.5, 12, 0.1, kfloat);
		pWidget.addEntry("MinRad", &Q1, 0.1, 5, 0.1, kfloat);
		pWidget.addEntry("FixedRad", &Q2, 0.1, 5, 0.02, kfloat);
		pWidget.addEntry("Fold Limit", &Q3, 0.1, 5, 0.02, kfloat);
		pWidget.addEntry("Fold Limit2", &Q4, 0.1, 5, 0.02, kfloat);
		pWidget.addEntry("ZMUL", &Q6, 0.1, 5, 0.1, kfloat);
		pWidget.addEntry("Scale", &Q7, 0.1, 5, 0.1, kfloat);
		pWidget.addEntry("Scale2", &Q8, 0.1, 5, 0.1, kfloat);
		break;
	case EQU_34_FLOWER:
		pWidget.addEntry("Iterations", &MAXSTEPS, 2, 30, 1, kinteger);
		pWidget.addEntry("Scale", &Q1, 0.5, 3, 0.01, kfloat);
		pWidget.addEntry("Offset X", &control.julia.x, -15, 15, 0.1, kfloat);
		pWidget.addEntry("Offset Y", &control.julia.y, -15, 15, 0.1, kfloat);
		pWidget.addEntry("Offset Z", &control.julia.z, -15, 15, 0.1, kfloat);
		break;
	case EQU_37_SPIRALBOX:
		pWidget.addEntry("Iterations", &MAXSTEPS, 6, 20, 1, kinteger);
		pWidget.addEntry("Fold", &Q1, 0.5, 1, 0.003, kfloat);
		juliaGroup(2, 0.1);
		break;
	case EQU_38_ALEK_BULB:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 30, 1, kinteger);
		pWidget.addEntry("Power", &Q1, 1.5, 12, 0.02, kfloat);
		juliaGroup(1.6, 0.01);
		break;
	case EQU_39_SURFBOX:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 20, 1, kinteger);
		pWidget.addEntry("Scale Factor", &Q6, 0.6, 3, 0.02, kfloat);
		pWidget.addEntry("Box 1", &Q1, 0, 3, 0.002, kfloat);
		pWidget.addEntry("Box 2", &Q2, 4, 5.6, 0.002, kfloat);
		pWidget.addEntry("Sphere 1", &Q3, 0, 4, 0.01, kfloat);
		pWidget.addEntry("Sphere 2", &Q4, 0, 4, 0.01, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_40_TWISTBOX:
		pWidget.addEntry("Iterations", &MAXSTEPS, 3, 60, 1, kinteger);
		pWidget.addEntry("Scale Factor", &Q2, 0.6, 10, 0.02, kfloat);
		pWidget.addEntry("Box", &Q1, 0, 10, 0.0001, kfloat);
		juliaGroup(10, 0.0001);
		break;
	case EQU_41_KALI_RONTGEN:
		pWidget.addEntry("Iterations", &MAXSTEPS, 1, 30, 1, kinteger);
		pWidget.addEntry("X", &Q1, -10, 10, 0.01, kfloat);
		pWidget.addEntry("Y", &Q2, -10, 10, 0.01, kfloat);
		pWidget.addEntry("Z", &Q3, -10, 10, 0.01, kfloat);
		pWidget.addEntry("Angle", &Q4, -4, 4, 0.02, kfloat);
		break;
	case EQU_42_VERTEBRAE:
		pWidget.addEntry("Iterations", &MAXSTEPS, 1, 50, 1, kinteger);
		pWidget.addEntry("X", &Q1, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Y", &Q2, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Z", &Q3, -10, 10, 0.05, kfloat);
		pWidget.addEntry("W", &Q4, -10, 10, 0.05, kfloat);
		pWidget.addEntry("ScaleX", &Q5, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Sine X", &Q8, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Offset X", &QB, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Slope X", &QE, -10, 10, 0.05, kfloat);
		pWidget.addEntry("ScaleY", &Q6, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Sine Y", &Q9, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Offset Y", &QC, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Slope Y", &QF, -10, 10, 0.05, kfloat);
		pWidget.addEntry("ScaleZ", &Q7, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Sine Z", &QA, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Offset Z", &QD, -10, 10, 0.05, kfloat);
		pWidget.addEntry("Slope Z", &QG, -10, 10, 0.05, kfloat);
		break;
	case EQU_43_DARKSURF:
		pWidget.addEntry("Iterations", &MAXSTEPS, 2, 30, 1, kinteger);
		pWidget.addEntry("scale", &Q1, -10, 10, 0.002, kfloat);
		pWidget.addEntry("MinRad", &Q2, -10, 10, 0.002, kfloat);
		pWidget.addEntry("Scale", &Q3, -10, 10, 0.02, kfloat);
		pWidget.addEntry("Fold X", &Q5, -10, 10, 0.02, kfloat); // p1
		pWidget.addEntry("Fold Y", &Q6, -10, 10, 0.002, kfloat);
		pWidget.addEntry("Fold Z", &Q7, -10, 10, 0.002, kfloat);
		pWidget.addEntry("FoldMod X", &Q9, -10, 10, 0.002, kfloat); // p2
		pWidget.addEntry("FoldMod Y", &QA, -10, 10, 0.002, kfloat);
		pWidget.addEntry("FoldMod Z", &QB, -10, 10, 0.002, kfloat);
		pWidget.addEntry("Angle", &Q4, -4, 4, 0.002, kfloat);
		break;
	case EQU_44_BUFFALO:
		pWidget.addEntry("Iterations", &MAXSTEPS, 2, 60, 1, kinteger);
		pWidget.addEntry("Power", &Q1, 0.1, 30, 0.01, kfloat);
		pWidget.addEntry("Angle", &Q2, -4, 4, 0.01, kfloat);
		pWidget.addLegend(" ");
		pWidget.addBoolean("Q: Pre Abs X", &PREABSX);
		pWidget.addBoolean("W: Pre Abs Y", &PREABSY);
		pWidget.addBoolean("E: Pre Abs Z", &PREABSZ);
		pWidget.addBoolean("R: Abs X", &ABSX);
		pWidget.addBoolean("T: Abs Y", &ABSY);
		pWidget.addBoolean("Y: Abs Z", &ABSZ);
		pWidget.addLegend(" ");
		pWidget.addBoolean("U: Delta DE", &USEDELTADE);
		if (USEDELTADE != 0)
			pWidget.addEntry("DE Scale", &Q3, 0, 2, 0.01, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_45_TEMPLE:
		pWidget.addEntry("Iterations", &MAXSTEPS, 1, 16, 1, kinteger);
		pWidget.addEntry("X", &Q1, -10, 10, 0.01, kfloat);
		pWidget.addEntry("Y", &Q2, -10, 10, 0.01, kfloat);
		pWidget.addEntry("Z", &Q5, -4, 4, 0.01, kfloat);
		pWidget.addEntry("W", &Q6, -4, 4, 0.01, kfloat);
		pWidget.addEntry("A1", &Q7, -10, 10, 0.01, kfloat);
		pWidget.addEntry("A2", &Q8, -10, 10, 0.01, kfloat);
		pWidget.addEntry("Ceiling", &Q4, -2, 1, 0.01, kfloat);
		pWidget.addEntry("Floor", &Q3, -2, 1, 0.01, kfloat);
		break;
	case EQU_46_KALI3:
		pWidget.addEntry("Iterations", &MAXSTEPS, 4, 60, 2, kinteger);
		pWidget.addEntry("Box", &Q1, -10, 10, 0.001, kfloat);
		juliaGroup(10, 0.001);
		break;
	case EQU_47_SPONGE:
		pWidget.addEntry("Iterations", &MAXSTEPS, 1, 16, 1, kinteger);
		pWidget.addEntry("minX", &Q1, -5, 5, 0.01, kfloat);
		pWidget.addEntry("minY", &Q2, -5, 5, 0.01, kfloat);
		pWidget.addEntry("minZ", &Q3, -5, 5, 0.01, kfloat);
		pWidget.addEntry("minW", &Q4, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxX", &Q5, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxY", &Q6, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxZ", &Q7, -5, 5, 0.01, kfloat);
		pWidget.addEntry("maxW", &Q8, -5, 5, 0.01, kfloat);
		pWidget.addEntry("Scale", &Q9, 1, 20, 1, kfloat);
		pWidget.addEntry("Shape", &QA, -10, 10, 0.1, kfloat);
		break;
	case EQU_50_DONUTS:
		pWidget.addEntry("Iterations", &MAXSTEPS, 1, 16, 1, kinteger);
		pWidget.addEntry("X", &Q1, 0.01, 20, 0.05, kfloat);
		pWidget.addEntry("Y", &Q2, 0.01, 20, 0.05, kfloat);
		pWidget.addEntry("Z", &Q3, 0.01, 20, 0.05, kfloat);
		pWidget.addEntry("Spread", &Q4, 0.01, 2, 0.01, kfloat);
		pWidget.addEntry("Mult", &Q5, 0.01, 2, 0.01, kfloat);
		break;
	}

	pWidget.addLegend("");
	pWidget.addBoolean("I: Spherical Inversion", &DOINVERSION);

	if (DOINVERSION) {
		pWidget.addEntry("   X", &INVERSION_X, -5, 5, 0.02, kfloat);
		pWidget.addEntry("   Y", &INVERSION_Y, -5, 5, 0.02, kfloat);
		pWidget.addEntry("   Z", &INVERSION_Z, -5, 5, 0.02, kfloat);
		pWidget.addEntry("   Radius", &INVERSION_RADIUS, 0.01, 10, 0.01, kfloat);
		pWidget.addEntry("   Angle", &INVERSION_ANGLE, -10, 10, 0.01, kfloat);
	}

	//==================================================================

	if (ISSTEREO)
		cWidget.addEntry("Parallax", &PARALLAX, 0.001, 1, 0.01, kfloat);
	cWidget.addEntry("Brightness", &BRIGHT, 0.01, 10, 0.02, kfloat);
	cWidget.addEntry("Enhance", &ENHANCE, 0, 30, 0.03, kfloat);
	cWidget.addEntry("ColorRoll", &COLORROLL, 0, 30, 0.03, kfloat);
	cWidget.addEntry("Second Surface", &SECONDSURFACE, 0, 3, 0.00001, kfloat);

	if (COLORSCHEME == 6 || COLORSCHEME == 7)
		cWidget.addEntry("Color Boost", &COLORPARAM, 1, 1200000, 200, kfloat);
	cWidget.addEntry("Contrast", &CONTRAST, 0.1, 0.7, 0.02, kfloat);
	cWidget.addEntry("Spot Light Brightness", &SPECULAR, 0, 1, 0.02, kfloat);
	cWidget.addEntry("Spot Light Position", &lightAngle, -3, 3, 0.3, kfloat);
	cWidget.addLegend(" ");

	cWidget.addLegend("        -- OrbitTrap --");
	cWidget.addEntry("Strength", &OTstrength, 0, 0.99, 0.03, kfloat);
	cWidget.addEntry("#Cycles", &OTcycles, 0, 100, 0.2, kfloat);
	cWidget.addLegend("");
	cWidget.addEntry("X Color", &OTindexX, 0, 255, 5, kinteger);
	cWidget.addEntry("Y", &OTindexY, 0, 255, 5, kinteger);
	cWidget.addEntry("Z", &OTindexZ, 0, 255, 5, kinteger);
	cWidget.addEntry("R", &OTindexR, 0, 255, 5, kinteger);
	cWidget.addLegend("");
	cWidget.addEntry("X Weight", &OTcolorXW, -5, 5, 0.1, kfloat);
	cWidget.addEntry("Y", &OTcolorYW, -5, 5, 0.1, kfloat);
	cWidget.addEntry("Z", &OTcolorZW, -5, 5, 0.1, kfloat);
	cWidget.addEntry("R", &OTcolorRW, -5, 5, 0.1, kfloat);

	cWidget.addLegend("");
	cWidget.addEntry("FixedStyle", &OTstyle, 0, 2, 1, kinteger);
	cWidget.addEntry("X Fixed", &OTfixedX, -4, 4, 0.002, kfloat);
	cWidget.addEntry("Y", &OTfixedY, -4, 4, 0.002, kfloat);
	cWidget.addEntry("Z", &OTfixedZ, -4, 4, 0.002, kfloat);

	cWidget.addLegend("");
	cWidget.addLegend("G: Select next coloring style");
	cWidget.addLegend("C: Select next OrbitTrap palette");
	cWidget.addLegend("P: Save image to BMP file");

	cWidget.addBoolean("K: Add Texture", &TONOFF);
	if (TONOFF) {
		cWidget.addEntry("   Scale", &TSCALE, 0.1, 20, 0.1, kfloat);
		cWidget.addEntry("   Center X", &TCENTERX, 0, 1, 0.01, kfloat);
		cWidget.addEntry("   Center Y", &TCENTERY, 0, 1, 0.01, kfloat);
	}
	
	if (!resetFocus) pWidget.jumpToPreviousFocus(); else cycleFocus(true);
	pWidget.refresh();
	cWidget.refresh();
}

XMFLOAT4 add4(XMFLOAT4 v1, XMFLOAT4 v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
	return v1;
}

XMFLOAT4 sub4(XMFLOAT4 v1, XMFLOAT4 v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
	return v1;
}

XMFLOAT4 mult4(XMFLOAT4 v1, float v2) {
	v1.x *= v2;
	v1.y *= v2;
	v1.z *= v2;
	v1.w *= v2;
	return v1;
}

XMFLOAT4 recipical4(XMFLOAT4 v) {
	if (v.x != 0) v.x = 1.0 / v.x;
	if (v.y != 0) v.y = 1.0 / v.y;
	if (v.z != 0) v.z = 1.0 / v.z;
	if (v.w != 0) v.w = 1.0 / v.w;
	return v;
}

XMFLOAT4 normalize4(XMFLOAT4 v) {
	float t = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	if (t != 0) {
		v.x /= t;
		v.y /= t;
		v.z /= t;
		v.w /= t;
	}

	return v;
}

void Fractal::update() {
	if (isDirty) {
		isDirty = false;
		control.lightPosition.x = sin(lightAngle);
		control.lightPosition.y = cos(lightAngle);
		control.lightPosition.z = -1;
		control.lightPosition = normalize4(control.lightPosition);

		// orbitTrap ---------------
		XMFLOAT3* cMap = colorMapList[paletteIndex];
		XMFLOAT3 color = cMap[OTindexX];
		OTcolorXR = color.x;
		OTcolorXG = color.y;
		OTcolorXB = color.z;
		color = cMap[OTindexY];
		OTcolorYR = color.x;
		OTcolorYG = color.y;
		OTcolorYB = color.z;
		color = cMap[OTindexZ];
		OTcolorZR = color.x;
		OTcolorZG = color.y;
		OTcolorZB = color.z;
		color = cMap[OTindexR];
		OTcolorRR = color.x;
		OTcolorRG = color.y;
		OTcolorRB = color.z;

		float dihedDodec;

		switch (EQUATION) {
		case EQU_09_POLY_MENGER:
			dihedDodec = 0.5 * atan(Q5);
			control.P3.x = cos(dihedDodec);	// P3 = csD
			control.P3.y = -sin(dihedDodec);
			control.P4.z = cos(2 * double(dihedDodec)); // P4 = csD2
			control.P4.w = -sin(2 * double(dihedDodec));
			break;
		case EQU_12_KLEINIAN2:
		case EQU_47_SPONGE:
			control.P3 = XMFLOAT4(Q1, Q2, Q3, Q4); // mins
			control.P4 = XMFLOAT4(Q5, Q6, Q7, Q8); // maxs
			break;
		case EQU_18_SIERPINSKI:  // P3 = n1
		case EQU_19_HALF_TETRA:
		case EQU_24_KALEIDO:
			control.P3.x = -1.0;
			control.P3.y = Q2 - 1.0;
			control.P3.z = 1.0 / Q2 - 1.0;
			control.P3.w = 0;
			control.P3 = normalize4(control.P3);
			break;
		case EQU_20_FULL_TETRA:
			XMFLOAT4 t0 = XMFLOAT4(-1.0, Q2 - 1.0, 1.0 / Q2 - 1.0, 0);
			control.P4 = normalize4(t0);
			break;
		case EQU_25_POLYCHORA:
			XMFLOAT4 pabc = XMFLOAT4(0, 0, 0, 1);
			XMFLOAT4 pbdc = recipical4(mult4(XMFLOAT4(1, 0, 0, 1), sqrt(2)));
			XMFLOAT4 pcda = recipical4(mult4(XMFLOAT4(0, 1, 0, 1), sqrt(2)));
			XMFLOAT4 pdba = recipical4(mult4(XMFLOAT4(0, 0, 1, 1), sqrt(2)));
			XMFLOAT4 aa = mult4(pabc, Q1);
			XMFLOAT4 bb = mult4(pbdc, Q2);
			XMFLOAT4 cc = mult4(pcda, Q3);
			XMFLOAT4 dd = mult4(pdba, Q4);
			XMFLOAT4 t1 = add4(aa, bb);
			XMFLOAT4 t2 = add4(cc, dd);
			XMFLOAT4 t3 = add4(t1, t2);
			control.P4 = normalize4(t3); // p
			control.P5 = XMFLOAT4(-0.5, -0.5, -0.5, 0.5); // nd
			Q9 = cos(Q5); // cVR
			QA = sin(Q5); // sVR
			QB = cos(Q6); // cSR
			QC = sin(Q6); // sSR
			QD = cos(Q7); // cRA
			QE = sin(Q7); // sRA 
			break;
		case EQU_30_KALIBOX:
			QC = abs(Q1 - 1.0); // absScalem1
			QD = pow(abs(Q1), float(1.0 - MAXSTEPS)); // AbsScaleRaisedTo1mIters
			control.P4 = XMFLOAT4(Q5, Q6, Q7, 0); // n1
			if (Q2 != 0) {
				float t = Q1 / Q2;
				control.P5 = XMFLOAT4(t, t, t, abs(Q1) / Q2); // mins
			}
			else
				control.P5 = XMFLOAT4(0, 0, 0, 0);
			break;
		}

		view.UpdateControlBuffer();
		view.Compute();

		//char str[32];
		//sprintf_s(str, 31, "spec %8.5f\n", SPECULAR);
		//OutputDebugStringA(str);
	}
}

bool alternateJogMode = false; // 'A' key pressed while mouse jogging? = XZ movement rather than XY
bool rotateMode = false; // 'Z' key pressed while jogging? = rotate camera rather than move it.

void Fractal::moveCamera(XMFLOAT4 amt) {
	if (rotateMode) {
		updateShaderDirectionVector(add4(control.viewVector, amt));
	}
	else {
		control.camera = sub4(control.camera, mult4(control.sideVector, amt.x));
		control.camera = sub4(control.camera, mult4(control.topVector, amt.y));
		control.camera = add4(control.camera, mult4(control.viewVector, amt.z));
	}
}

void Fractal::timer() {
	// mouse dragging to move/rotate camera
	mouseTimerHandler();

	// left/right arrow keys altering a parameter
	if (pWidget.isAltering())
		isDirty = true;
	else
	if (cWidget.isAltering())
		isDirty = true;

	// number keys pressed to move/rotate camera
	if (jogAmount.x != 0 || jogAmount.y != 0 || jogAmount.z != 0) {
		isDirty = true;
		moveCamera(jogAmount);
	}
}

void Fractal::updateAlterationSpeed() {
	alterationSpeed = 1.0;

	if (isShiftKeyPressed && isControlKeyPressed) alterationSpeed *= 50; else
		if (isShiftKeyPressed) alterationSpeed *= 0.1; else
			if (isControlKeyPressed) alterationSpeed *= 10;

	pWidget.alterationSpeed = alterationSpeed;
	cWidget.alterationSpeed = alterationSpeed;
}

void Fractal::jogCameraAndFocusPosition(int dx, int dy, int dz) {
	updateAlterationSpeed();
	float speed = alterationSpeed * 0.01;

	if (dx != 0) { jogAmount.x = FLOAT(dx) * speed; }
	if (dy != 0) { jogAmount.y = FLOAT(dy) * speed; }
	if (dz != 0) { jogAmount.z = FLOAT(dz) * speed; }
}

void Fractal::jogRelease(int dx, int dy, int dz) {
	if (dx != 0) { jogAmount.x = 0; }
	if (dy != 0) { jogAmount.y = 0; }
	if (dz != 0) { jogAmount.z = 0; }
}

void Fractal::refresh(bool resetFocus) {
	defineWidgetsForCurrentEquation(resetFocus);
	isDirty = true;
}

/// alter EQUATION selection, reset it's parameters, display it's widgets, calculate fractal
void Fractal::changeEquationIndex(int dir) {
	EQUATION += dir;
	if (EQUATION >= EQU_MAX) EQUATION = 1;
	else
		if (EQUATION < 1) EQUATION = EQU_MAX - 1;

	DOINVERSION = 0;
	PARALLAX = 0.003;
	COLORPARAM = 25000;
	BRIGHT = 1;
	CONTRAST = 0.5;
	SPECULAR = 0;

	reset();
	refresh(true);
	updateWindowTitle();
}

#include <iostream>
#include <tchar.h>

extern ID3D11Device* pd3dDevice;
extern ID3D11DeviceContext* pImmediateContext;
ID3D11Texture2D* srcTexture = NULL;
ID3D11ShaderResourceView* srcTextureView = NULL;

void Fractal::toggleTexture() {
	if (TONOFF != 0) {
		TONOFF = 0;
		return;
	}
	TONOFF = 0;

	char szFileName[MAX_PATH] = { 0 };

	OPENFILENAME  ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "Image Files\0*.png; *.bmp; *.jpg\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select Texture File";
	ofn.Flags = OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn))
		return;

	size_t size = strlen(szFileName) + 1;
	wchar_t wName[MAX_PATH];
	size_t outSize;
	mbstowcs_s(&outSize, wName, size, szFileName, size - 1);

	SafeRelease(&srcTexture);
	SafeRelease(&srcTextureView);

	int w, h;

	HRESULT hr = CreateWICTextureFromFile(pd3dDevice,
		pImmediateContext,
		wName,
		(ID3D11Resource * *)& srcTexture,
		&srcTextureView,
		w, h);

	if (SUCCEEDED(hr)) {
		TONOFF = 1;
		TXSIZE = w;
		TYSIZE = h;
		TSCALE = 1;
		TCENTERX = 0.5;
		TCENTERY = 0.5;
	}
}

void Fractal::keyDown(int key) {
	//char str[32];
	//sprintf_s(str,31, "KD %d %c\n", key, key);
	//OutputDebugStringA(str);

	switch (key) {
	case VK_ESCAPE:
		if (pWidget.isVisible) { // 1st <Esc> press closes child windows
			keyDown(' ');
			return;
		}

		SendMessage(g_hWnd, WM_CLOSE, 0, 0);
		SendMessage(pWidget.hWnd, WM_CLOSE, 0, 0);
		SendMessage(cWidget.hWnd, WM_CLOSE, 0, 0);
		break;
	case VK_SHIFT:
		isShiftKeyPressed = true;
		return;
	case VK_CONTROL:
		isControlKeyPressed = true;
		return;
	case VK_LEFT:
	case VK_RIGHT:
		updateAlterationSpeed();
		pWidget.keyDown(key);
		cWidget.keyDown(key);
		break;
	case VK_DOWN:
	case VK_UP:
		pWidget.keyDown(key);
		cWidget.keyDown(key);
		break;
	}

	switch (tolower(key)) {
	case ' ':
		pWidget.toggleVisible();
		cWidget.toggleVisible();
		if(!cWidget.isVisible)
			SetForegroundWindow(g_hWnd);
		break;
	case 'v' :
		cycleFocus();
		break;
	case '1':
		changeEquationIndex(-1);
		break;
	case '2':
		changeEquationIndex(+1);
		break;
	case '3':
		ISSTEREO = !ISSTEREO;
		defineWidgetsForCurrentEquation(false);
		if (ISSTEREO > 0)
			pWidget.focus = 0;
		isDirty = true;
		break;
	case '4':	jogCameraAndFocusPosition(-1, 0, 0);	break;
	case '5':	jogCameraAndFocusPosition(+1, 0, 0);	break;
	case '6':	jogCameraAndFocusPosition(0, -1, 0);	break;
	case '7':	jogCameraAndFocusPosition(0, +1, 0);	break;
	case '8':	jogCameraAndFocusPosition(0, 0, -1);	break;
	case '9':	jogCameraAndFocusPosition(0, 0, +1);	break;
	case 'i':
		DOINVERSION = 1 - DOINVERSION;
		reset();
		refresh(false);
		break;
	case 'b':
		SHOWBALLS = 1 - SHOWBALLS;
		refresh(false);
		break;
	case 'f':
		FOURGEN = !FOURGEN;
		refresh(false);
		break;
	case 'g':
		COLORSCHEME += 1;
		if (COLORSCHEME > 7) COLORSCHEME = 0;
		defineWidgetsForCurrentEquation(false);
		updateWindowTitle();
		isDirty = true;
		break;
	case 'j':
		JULIAMODE = !JULIAMODE;
		refresh(false);
		break;
	case 'a':
		alternateJogMode = true;	// XZ mouse movements rather than XY
		break;
	case 'z':
		rotateMode = true;
		break;
	case 'q':
		PREABSX = 1 - PREABSX;
		refresh(false);
		break;
	case 'w':
		PREABSY = 1 - PREABSY;
		refresh(false);
		break;
	case 'e':
		PREABSZ = 1 - PREABSZ;
		refresh(false);
		break;
	case 'r':
		ABSX = 1 - ABSX;
		refresh(false);
		break;
	case 't':
		ABSY = 1 - ABSY;
		refresh(false);
		break;
	case 'y':
		ABSZ = 1 - ABSZ;
		refresh(false);
		break;
	case 'u':
		USEDELTADE = 1 - USEDELTADE;
		refresh(false);
		break;
	case 's':
		saveLoad.saveControl();
		break;
	case 'l':
		saveLoad.launch();
		break;
	case 'p':
		saveImageToFile();
		break;
	case 'k':
		toggleTexture();
		refresh(false);
		break;
	case 'c' :
		if (++paletteIndex > 3) paletteIndex = 0;
		cWidget.refresh();
		isDirty = true;
		break;
	}
}

void Fractal::keyUp(int key) {
	//char str[32];
	//sprintf_s(str,31, "       KU %d %c\n", key, key);
	//OutputDebugStringA(str);

	switch (tolower(key)) {
	case VK_LEFT:
	case VK_RIGHT:
		pWidget.keyUp(key);
		cWidget.keyUp(key);
		break;
	case VK_SHIFT:
		isShiftKeyPressed = false;
		break;
	case VK_CONTROL:
		isControlKeyPressed = false;
		break;
	case '4':	jogRelease(-1, 0, 0);	break;
	case '5':	jogRelease(+1, 0, 0);	break;
	case '6':	jogRelease(0, -1, 0);	break;
	case '7':	jogRelease(0, +1, 0);	break;
	case '8':	jogRelease(0, 0, -1);	break;
	case '9':	jogRelease(0, 0, +1);	break;
	case 'a':
		alternateJogMode = false;
		break;
	case 'z':
		rotateMode = false;
		break;
	}
}

// ==========================================================

static int currentFocus = 0;  // 0,1 = p,c

void Fractal::cycleFocus(bool setToPwidget) {
	if (setToPwidget) currentFocus = 0;
	if (++currentFocus > 1) currentFocus = 0;

	switch (currentFocus) {
	case 0:
		pWidget.gainFocus();
		cWidget.loseFocus();
		break;
	case 1:
		pWidget.loseFocus();
		cWidget.gainFocus();
		break;
	}

	pWidget.updateWindowFocus();
	cWidget.updateWindowFocus();
}

// ==========================================================

POINTS mouseJoggingStartPos, mouseJoggingCurrentPos;
POINTS mouseAlteringStartPos, mouseAlteringCurrentPos;
bool isMouseJogging = false;

void Fractal::lButtonDown(LPARAM lParam) {
	mouseJoggingStartPos = MAKEPOINTS(lParam);
	mouseJoggingCurrentPos = mouseJoggingStartPos;
	isMouseJogging = true;
}

void Fractal::lButtonUp() {
	isMouseJogging = false; 
}

void Fractal::rButtonDown(LPARAM) {}
void Fractal::rButtonUp() {}

void Fractal::mouseMove(WPARAM wParam, LPARAM lParam) {
	if (isMouseJogging && (wParam & MK_LBUTTON))
		mouseJoggingCurrentPos = MAKEPOINTS(lParam);
	else
		isMouseJogging = false;
}

void Fractal::mouseTimerHandler() {
	if (isMouseJogging) {
		XMFLOAT4 amt = XMFLOAT4(0, 0, 0, 0);
		float scale = 0.001;
		amt.x = float(mouseJoggingCurrentPos.x - mouseJoggingStartPos.x) * scale;
		amt.y = -float(mouseJoggingCurrentPos.y - mouseJoggingStartPos.y) * scale;

		if (alternateJogMode) { // vertical mouse movements = move Z axis instead
			amt.z = amt.y;
			amt.y = 0;
		}

		moveCamera(amt);
		isDirty = true;
	}
}

// ==========================================================
const char* equationName[] = {
"unused",
"MandelBulb",
"Apollonian",
"Apollonian2",
"Jos Leys Kleinian",
"MandelBox",
"Quaternion Julia",
"Polyhedral Menger",
"Gold",
"Spider",
"Knighty's Kleinian",
"Sierpinski Tetrahedron",
"Half Tetrahedron",
"Kaleidoscopic",
"Knighty Polychora",
"Kali's MandelBox",
"Flower Hive",
"Aleksandrov MandelBulb",
"SurfBox",
"Kali Rontgen",
"Vertebrae",
"Buffalo Bulb",
"Klienian Sponge",
"Full Tetrahedron",
"3Dickulus Quaternion Julia",
"3Dickulus Quaternion Mandelbrot",
"Spudsville",
"Pupukuusikkos Spiralbox",
"TwistBox",
"DarkBeam Surfbox",
"Ancient Temple",
"Kali 3D",
"Donuts"
};

void Fractal::updateWindowTitle() {
	char str[512];
	sprintf_s(str, 511, "Fractal    Equation %d: %s,    Parameter: %s", EQUATION + 1, equationName[EQUATION], pWidget.focusString(false));
	SetWindowTextA(g_hWnd, str);
}

// ==========================================

void WriteToBmp(const char* inFilePath);

void Fractal::saveImageToFile() {
	time_t rawtime;
	struct tm timeinfo;
	static char buffer[128], str[128];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	strftime(buffer, 127, "Fractal_%y%m%d%H%M%S.bmp", &timeinfo);

	WriteToBmp(buffer);

	sprintf_s(str, 127, "Saved to: %s", buffer);
	MessageBox(NULL, str, "Image Saved", MB_ICONEXCLAMATION | MB_OK);

}
