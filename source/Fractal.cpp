#include <windows.h>
#include <d3d11.h>
#include "Fractal.h"
#include "View.h"
#include "Widget.h"

#define  _CRT_SECURE_NO_WARNINGS

Fractal fractal;

XMFLOAT4 jogAmount = XMFLOAT4(0, 0, 0, 0);
FLOAT alterationSpeed = 1;
bool shiftDown = false;
bool controlDown = false;
FLOAT lightAngle = 0;

void Fractal::init() {
	control.equation = EQU_04_KLEINIAN;
	control.doInversion = 1;
	control.isStereo = 0;
	PARALLAX = 0.003;
	COLORPARAM = 25000;
	BRIGHT = 1;
	CONTRAST = 0.5;
	SPECULAR = 0;
	reset();
	updateWindowTitle();
	isDirty = true;
}

FLOAT length(XMFLOAT4 v) { 
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

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

void Fractal::reset() {
	SPECULAR = 0;
	control.xSize = windowWidth;
	control.ySize = windowHeight;
	updateShaderDirectionVector(XMFLOAT4(0, 0.1, 1, 0));

	switch (control.equation) {
	case EQU_01_MANDELBULB:
		updateShaderDirectionVector(XMFLOAT4(0.010000015, 0.41950363, 0.64503753, 0));
		control.camera = XMFLOAT4(0.038563743, -1.1381346, -1.8405379, 0);
		control.maxSteps = 10;

		//  Q1 power
		Q1 = 8;

		if (control.doInversion) {
			control.camera = XMFLOAT4(-0.138822, -1.4459486, -1.9716375, 0);
			updateShaderDirectionVector(XMFLOAT4(0.012995179, 0.54515165, 0.8382366, 0));
			inversionSettings(-0.10600001, -0.74200004, -1.3880001, 2.14, 0.9100002);
		}
		break;

	case EQU_02_APOLLONIAN:
	case EQU_03_APOLLONIAN2:
		control.camera = XMFLOAT4(0.42461035, 10.847559, 2.5749633, 0);
		control.maxSteps = 12;

		//  Q1 multiplier
		//  Q2 foam
		//  Q3 foam2
		//  Q4 bend
		Q1 = 25.0f;
		Q2 = 1.05265248;
		Q3 = 1.06572711f;
		Q4 = 0.0202780124f;

		if (control.doInversion) {
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
		control.maxSteps = 70;
		control.finalIterations = 21;
		control.boxIterations = 17;
		control.showBalls = 1;
		control.fourGen = 0;

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

		if (control.doInversion) {
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
		control.maxSteps = 17;
		control.julia.x = 0.0;
		control.julia.y = -6.0;
		control.julia.z = -8.0;
		BRIGHT = 1.3299997;
		CONTRAST = 0.3199999;
		control.juliaboxMode = true;

		if (control.doInversion) {
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
		control.maxSteps = 7;
		CONTRAST = 0.28;
		SPECULAR = 0.9;

		if (control.doInversion) {
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

		if (control.doInversion) {
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
		control.maxSteps = 15;

		if (control.doInversion) {
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

		if (control.doInversion) {
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

		if (control.doInversion) {
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
		control.maxSteps = 27;

		if (control.doInversion) {
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
		control.maxSteps = 53;

		if (control.doInversion) {
			control.camera = XMFLOAT4(0.13613744, 0.07272194, -0.85636866, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.07199999, 0.070000015, 0.037999995, 0.33999994, 0.44);
		}
		break;
	case EQU_24_KALEIDO:
		control.camera = XMFLOAT4(-0.00100744, -0.1640267, -1.7581517, 0);
		Q1 = 1.1259973;
		Q2 = 0.8359996;
		Q3 = -0.016000029;
		Q4 = 1.7849922;
		Q5 = -1.2375059;
		control.maxSteps = 35.0;

		if (control.doInversion) {
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

		if (control.doInversion) {
			control.camera = XMFLOAT4(0.54899234, -0.03701113, -0.7053995, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.5320001, 0.012000054, -0.023999948, 0.36999995, 0.15);
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
		control.maxSteps = 11;

		if (control.doInversion) {
			control.camera = XMFLOAT4(0.32916373, -0.42756003, -3.6908724, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.16800001, -0.080000006, -0.39400005, 0.96000016, 0.1);
		}
		break;
	case EQU_34_FLOWER:
		control.camera = XMFLOAT4(-0.16991696, -2.5964863, -12.54011, 0);
		Q1 = 1.6740334;
		control.julia.x = 6.0999966;
		control.julia.y = 13.999996;
		control.julia.z = 3.0999992;
		BRIGHT = 1.5000001;
		control.maxSteps = 10;

		if (control.doInversion) {
			control.camera = XMFLOAT4(-0.16991696, -2.5964863, -12.54011, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.03800006, 0.162, 0.11799997, 0.7099998, 0.18000002);
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
		control.maxSteps = 10;

		if (control.doInversion) {
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
		control.maxSteps = 17;
		control.juliaboxMode = 1;

		if (control.doInversion) {
			control.camera = XMFLOAT4(-0.37710285, 0.4399976, -5.937426, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.10799999, 0.19999999, 0.1, 0.47000003, -0.15999997);
		}
		break;
	case EQU_41_KALI_RONTGEN:
		control.camera = XMFLOAT4(-0.16709971, -0.020002633, -0.9474212, 0);
		Q1 = 0.88783956;
		Q2 = 1.3439986;
		Q3 = 0.56685466;
		Q4 = 0;
		control.maxSteps = 7;

		if (control.doInversion) {
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
		control.maxSteps = 2;
		BRIGHT = 1.47;
		CONTRAST = 0.22000006;

		if (control.doInversion) {
			control.camera = XMFLOAT4(1.0229, -1.1866168, -8.713577, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(-0.9600001, -0.5200006, -3.583999, 4.01, 3.1000001);
		}
		break;
	case EQU_44_BUFFALO:
		control.camera = XMFLOAT4(0.008563751, -2.8381326, -0.2005394, 0);
		updateShaderDirectionVector(XMFLOAT4(-0.0045253364, 0.73382026, 0.091496624, 0));
		Q1 = 2.6300106; // power
		Q2 = 0.009998376; // angle
		Q3 = 1; // deltaDE
		control.maxSteps = 4;
		control.julia.x = 0.6382017;
		control.julia.y = -0.4336;
		control.julia.z = -0.9396994;
		control.preabsx = 1;
		control.preabsy = 1;
		control.preabsz = 0;
		control.absx = 1;
		control.absy = 0;
		control.absz = 1;
		control.useDeltaDE = 0;
		control.juliaboxMode = 1;
		BRIGHT = 1.2100002;
		CONTRAST = 0.17999999;

		if (control.doInversion) {
			control.camera = XMFLOAT4(0.033746917, -0.4353387, -0.07229346, 0);
			updateShaderDirectionVector(XMFLOAT4(-0.0061193192, 0.9922976, 0.12372495, 0));
			inversionSettings(0.016000055, 0.08400003, 5.2619725e-08, 0.3, -2.0000002);
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
		control.maxSteps = 3;
		BRIGHT = 2.31;
		CONTRAST = 0.17999999;

		if(control.doInversion) {
			control.camera = XMFLOAT4(0.25108737, -0.9736173, -2.603676, 0);
			updateShaderDirectionVector(XMFLOAT4(0.0, 0.09950372, 0.9950372, 0));
			inversionSettings(0.35200006,0.009999977,-0.092,1.0600003,-0.019999992);
		}
		break;
	}

	defineWidgetsForCurrentEquation(false);
}

void Fractal::juliaGroup(FLOAT range, FLOAT delta) {
	widget.addLegend("");
	widget.addBoolean("J: Julia Mode", &control.juliaboxMode);

	if (control.juliaboxMode) {
		widget.addEntry("  X", &control.julia.x, -range, range, delta, kfloat);
		widget.addEntry("  Y", &control.julia.y, -range, range, delta, kfloat);
		widget.addEntry("  Z", &control.julia.z, -range, range, delta, kfloat);
	}
}

// define widget entries for current equation
void Fractal::defineWidgetsForCurrentEquation(bool resetFocus) {
	widget.reset();

	if (control.isStereo)
		widget.addEntry("Parallax", &PARALLAX, 0.001, 1, 0.01, kfloat);
	widget.addEntry("Brightness", &BRIGHT, 0.01, 10, 0.02, kfloat);
	if (control.colorScheme == 6 || control.colorScheme == 7)
		widget.addEntry("Color Boost", &COLORPARAM, 1, 1200000, 200, kfloat);
	widget.addEntry("Contrast", &CONTRAST, 0.1, 0.7, 0.02, kfloat);
	widget.addEntry("Spot Light Brightness", &SPECULAR, 0, 1, 0.02, kfloat);
	widget.addEntry("Spot Light Position", &lightAngle, -3, 3, 0.3, kfloat);
	widget.addLegend(" ");

	switch (control.equation) {
	case EQU_01_MANDELBULB:
		widget.addEntry("Iterations", &control.maxSteps, 3, 30, 1, kinteger);
		widget.addEntry("Power", &Q1, 1.5, 12, 0.02, kfloat);
		break;
	case EQU_02_APOLLONIAN:
	case EQU_03_APOLLONIAN2:
		widget.addEntry("Iterations", &control.maxSteps, 2, 10, 1, kinteger);
		widget.addEntry("Multiplier", &Q1, 10, 300, 0.2, kfloat);
		widget.addEntry("Foam", &Q2, 0.1, 3, 0.02, kfloat);
		widget.addEntry("Foam2", &Q3, 0.1, 3, 0.02, kfloat);
		widget.addEntry("Bend", &Q4, 0.01, 0.03, 0.0001, kfloat);
		break;
	case EQU_04_KLEINIAN:
		widget.addBoolean("B: ShowBalls", &control.showBalls);
		widget.addBoolean("F: FourGen", &control.fourGen);
		widget.addLegend(" ");
		widget.addEntry("Final Iterations", &control.finalIterations, 1, 39, 1, kinteger);
		widget.addEntry("Box Iterations", &control.boxIterations, 1, 10, 1, kinteger);
		widget.addEntry("Box Size X", &Q1, 0.01, 2, 0.006, kfloat);
		widget.addEntry("Box Size Z", &Q2, 0.01, 2, 0.006, kfloat);
		widget.addEntry("Klein R", &Q3, 0.01, 2.5, 0.005, kfloat);
		widget.addEntry("Klein I", &Q4, 0.01, 2.5, 0.005, kfloat);
		widget.addEntry("Clamp Y", &Q5, 0.001, 2, 0.01, kfloat);
		widget.addEntry("Clamp DF", &Q6, 0.001, 2, 0.03, kfloat);
		break;
	case EQU_05_MANDELBOX:
		widget.addEntry("Iterations", &control.maxSteps, 3, 60, 1, kinteger);
		widget.addEntry("Scale Factor", &Q1, 0.6, 10, 0.02, kfloat);
		widget.addEntry("Box", &Q2, 0, 10, 0.001, kfloat);
		widget.addEntry("Sphere 1", &Q3, 0, 4, 0.01, kfloat);
		widget.addEntry("Sphere 2", &Q4, 0, 4, 0.01, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_06_QUATJULIA:
		widget.addEntry("Iterations", &control.maxSteps, 3, 10, 1, kinteger);
		widget.addEntry("X", &Q1, -5, 5, 0.05, kfloat);
		widget.addEntry("Y", &Q2, -5, 5, 0.05, kfloat);
		widget.addEntry("Z", &Q3, -5, 5, 0.05, kfloat);
		widget.addEntry("W", &Q4, -5, 5, 0.05, kfloat);
		break;
	case EQU_09_POLY_MENGER:
		widget.addEntry("Menger", &Q2, 1.1, 2.9, 0.05, kfloat);
		widget.addEntry("Stretch", &Q3, 0, 10, 0.05, kfloat);
		widget.addEntry("Spin", &Q4, 0.1, 5, 0.05, kfloat);
		widget.addEntry("Twist", &Q1, 0.5, 7, 0.05, kfloat);
		widget.addEntry("Shape", &Q5, 0.1, 50, 0.2, kfloat);
		break;
	case EQU_10_GOLD:
		widget.addEntry("Iterations", &control.maxSteps, 2, 20, 1, kinteger);
		widget.addEntry("T", &Q1, -5, 5, 0.01, kfloat);
		widget.addEntry("U", &Q2, -5, 5, 0.01, kfloat);
		widget.addEntry("V", &Q3, -5, 5, 0.01, kfloat);
		widget.addEntry("W", &Q4, -5, 5, 0.01, kfloat);
		widget.addEntry("X", &Q5, -5, 5, 0.01, kfloat);
		widget.addEntry("Y", &Q6, -5, 5, 0.01, kfloat);
		widget.addEntry("Z", &Q7, -5, 5, 0.01, kfloat);
		break;
	case EQU_11_SPIDER:
		widget.addEntry("X", &Q1, 0.001, 5, 0.01, kfloat);
		widget.addEntry("Y", &Q2, 0.001, 5, 0.01, kfloat);
		widget.addEntry("Z", &Q3, 0.001, 5, 0.01, kfloat);
		break;
	case EQU_12_KLEINIAN2:
		widget.addEntry("Shape", &Q9, 0.01, 2, 0.005, kfloat);
		widget.addEntry("minX", &Q1, -5, 5, 0.01, kfloat);
		widget.addEntry("minY", &Q2, -5, 5, 0.01, kfloat);
		widget.addEntry("minZ", &Q3, -5, 5, 0.01, kfloat);
		widget.addEntry("minW", &Q4, -5, 5, 0.01, kfloat);
		widget.addEntry("maxX", &Q5, -5, 5, 0.01, kfloat);
		widget.addEntry("maxY", &Q6, -5, 5, 0.01, kfloat);
		widget.addEntry("maxZ", &Q7, -5, 5, 0.01, kfloat);
		widget.addEntry("maxW", &Q8, -5, 5, 0.01, kfloat);
		break;
	case EQU_18_SIERPINSKI:
		widget.addEntry("Iterations", &control.maxSteps, 11, 40, 1, kinteger);
		widget.addEntry("Scale", &Q1, 1.18, 1.8, 0.02, kfloat);
		widget.addEntry("Y", &Q2, 0.5, 3, 0.02, kfloat);
		widget.addEntry("Angle1", &Q3, -4, 4, 0.01, kfloat);
		widget.addEntry("Angle2", &Q4, -4, 4, 0.01, kfloat);
		break;
	case EQU_19_HALF_TETRA:
		widget.addEntry("Iterations", &control.maxSteps, 9, 50, 1, kinteger);
		widget.addEntry("Scale", &Q1, 1.12, 1.5, 0.02, kfloat);
		widget.addEntry("Y", &Q2, 2, 10, 0.1, kfloat);
		widget.addEntry("Angle1", &Q3, -4, 4, 0.01, kfloat);
		widget.addEntry("Angle2", &Q4, -4, 4, 0.01, kfloat);
		break;
	case EQU_24_KALEIDO:
		widget.addEntry("Iterations", &control.maxSteps, 10, 200, 1, kinteger);
		widget.addEntry("Scale", &Q1, 0.5, 2, 0.0005, kfloat);
		widget.addEntry("Y", &Q2, -5, 5, 0.004, kfloat);
		widget.addEntry("Z", &Q3, -5, 5, 0.004, kfloat);
		widget.addEntry("Angle1", &Q4, -4, 4, 0.005, kfloat);
		widget.addEntry("Angle2", &Q5, -4, 4, 0.005, kfloat);
		break;
	case EQU_25_POLYCHORA:
		widget.addEntry("Distance 1", &Q1, -2, 10, 0.1, kfloat);
		widget.addEntry("Distance 2", &Q2, -2, 10, 0.1, kfloat);
		widget.addEntry("Distance 3", &Q3, -2, 10, 0.1, kfloat);
		widget.addEntry("Distance 4", &Q4, -2, 10, 0.1, kfloat);
		widget.addEntry("Ball", &Q5, 0, 0.35, 0.02, kfloat);
		widget.addEntry("Stick", &Q6, 0, 0.35, 0.02, kfloat);
		widget.addEntry("Spin", &Q7, -15, 15, 0.05, kfloat);
		break;
	case EQU_30_KALIBOX:
		widget.addEntry("Iterations", &control.maxSteps, 3, 30, 1, kinteger);
		widget.addEntry("Scale", &Q1, -5, 5, 0.05, kfloat);
		widget.addEntry("MinRad2", &Q2, -5, 5, 0.05, kfloat);
		widget.addEntry("Trans X", &Q5, -15, 15, 0.01, kfloat);
		widget.addEntry("Trans Y", &Q6, -15, 15, 0.01, kfloat);
		widget.addEntry("Trans Z", &Q7, -1, 5, 0.01, kfloat);
		widget.addEntry("Angle", &Q9, -4, 4, 0.02, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_34_FLOWER:
		widget.addEntry("Iterations", &control.maxSteps, 2, 30, 1, kinteger);
		widget.addEntry("Scale", &Q1, 0.5, 3, 0.01, kfloat);
		widget.addEntry("Offset X", &control.julia.x, -15, 15, 0.1, kfloat);
		widget.addEntry("Offset Y", &control.julia.y, -15, 15, 0.1, kfloat);
		widget.addEntry("Offset Z", &control.julia.z, -15, 15, 0.1, kfloat);
		break;
	case EQU_38_ALEK_BULB:
		widget.addEntry("Iterations", &control.maxSteps, 3, 30, 1, kinteger);
		widget.addEntry("Power", &Q1, 1.5, 12, 0.02, kfloat);
		juliaGroup(1.6, 0.01);
		break;
	case EQU_39_SURFBOX:
		widget.addEntry("Iterations", &control.maxSteps, 3, 20, 1, kinteger);
		widget.addEntry("Scale Factor", &Q6, 0.6, 3, 0.02, kfloat);
		widget.addEntry("Box 1", &Q1, 0, 3, 0.002, kfloat);
		widget.addEntry("Box 2", &Q2, 4, 5.6, 0.002, kfloat);
		widget.addEntry("Sphere 1", &Q3, 0, 4, 0.01, kfloat);
		widget.addEntry("Sphere 2", &Q4, 0, 4, 0.01, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_41_KALI_RONTGEN :
		widget.addEntry("Iterations", &control.maxSteps, 1, 30, 1, kinteger);
		widget.addEntry("X", &Q1, -10, 10, 0.01, kfloat);
		widget.addEntry("Y", &Q2, -10, 10, 0.01, kfloat);
		widget.addEntry("Z", &Q3, -10, 10, 0.01, kfloat);
		widget.addEntry("Angle", &Q4, -4, 4, 0.02, kfloat);
		break;
	case EQU_42_VERTEBRAE :
		widget.addEntry("Iterations", &control.maxSteps, 1, 50, 1, kinteger);
		widget.addEntry("X", &Q1, -10, 10, 0.05, kfloat);
		widget.addEntry("Y", &Q2, -10, 10, 0.05, kfloat);
		widget.addEntry("Z", &Q3, -10, 10, 0.05, kfloat);
		widget.addEntry("W", &Q4, -10, 10, 0.05, kfloat);
		widget.addEntry("ScaleX", &Q5, -10, 10, 0.05, kfloat);
		widget.addEntry("Sine X", &Q8, -10, 10, 0.05, kfloat);
		widget.addEntry("Offset X", &QB, -10, 10, 0.05, kfloat);
		widget.addEntry("Slope X", &QE, -10, 10, 0.05, kfloat);
		widget.addEntry("ScaleY", &Q6, -10, 10, 0.05, kfloat);
		widget.addEntry("Sine Y", &Q9, -10, 10, 0.05, kfloat);
		widget.addEntry("Offset Y", &QC, -10, 10, 0.05, kfloat);
		widget.addEntry("Slope Y", &QF, -10, 10, 0.05, kfloat);
		widget.addEntry("ScaleZ", &Q7, -10, 10, 0.05, kfloat);
		widget.addEntry("Sine Z", &QA, -10, 10, 0.05, kfloat);
		widget.addEntry("Offset Z", &QD, -10, 10, 0.05, kfloat);
		widget.addEntry("Slope Z", &QG, -10, 10, 0.05, kfloat);
		break;
	case EQU_44_BUFFALO :
		widget.addEntry("Iterations", &control.maxSteps, 2, 60, 1, kinteger);
		widget.addEntry("Power", &Q1, 0.1, 30, 0.01, kfloat);
		widget.addEntry("Angle", &Q2, -4, 4, 0.01, kfloat);
		widget.addLegend(" ");
		widget.addBoolean("Q: Pre Abs X", &control.preabsx);
		widget.addBoolean("W: Pre Abs Y", &control.preabsy);
		widget.addBoolean("E: Pre Abs Z", &control.preabsz);
		widget.addBoolean("R: Abs X", &control.absx);
		widget.addBoolean("T: Abs Y", &control.absy);
		widget.addBoolean("Y: Abs Z", &control.absz);
		widget.addLegend(" ");
		widget.addBoolean("U: Delta DE", &control.useDeltaDE);
		if(control.useDeltaDE != 0)
			widget.addEntry("DE Scale",&Q3, 0,2,0.01, kfloat);
		juliaGroup(10, 0.01);
		break;
	case EQU_47_SPONGE :
		widget.addEntry("Iterations", &control.maxSteps, 1,16, 1, kinteger);
		widget.addEntry("minX", &Q1, -5, 5, 0.01, kfloat);
		widget.addEntry("minY", &Q2, -5, 5, 0.01, kfloat);
		widget.addEntry("minZ", &Q3, -5, 5, 0.01, kfloat);
		widget.addEntry("minW", &Q4, -5, 5, 0.01, kfloat);
		widget.addEntry("maxX", &Q5, -5, 5, 0.01, kfloat);
		widget.addEntry("maxY", &Q6, -5, 5, 0.01, kfloat);
		widget.addEntry("maxZ", &Q7, -5, 5, 0.01, kfloat);
		widget.addEntry("maxW", &Q8, -5, 5, 0.01, kfloat);
		widget.addEntry("Scale", &Q9, 1, 20, 1, kfloat);
		widget.addEntry("Shape", &QA, -10, 10, 0.1, kfloat);
		break;
	}

	widget.addLegend("");
	widget.addBoolean("I: Spherical Inversion", &control.doInversion);

	if (control.doInversion) {
		widget.addEntry("   X", &INVERSION_X, -5, 5, 0.002, kfloat);
		widget.addEntry("   Y", &INVERSION_Y, -5, 5, 0.002, kfloat);
		widget.addEntry("   Z", &INVERSION_Z, -5, 5, 0.002, kfloat);
		widget.addEntry("   Radius", &INVERSION_RADIUS, 0.01, 10, 0.01, kfloat);
		widget.addEntry("   Angle", &INVERSION_ANGLE, -10, 10, 0.01, kfloat);
	}

	if (!resetFocus) widget.jumpToPreviousFocus();
	widget.refresh();
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

		float dihedDodec;

		switch (control.equation) {
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
			QD = pow(abs(Q1), float(1.0 - control.maxSteps)); // AbsScaleRaisedTo1mIters
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

bool rotateMode = false;

void Fractal::timer() {
	if (widget.isAltering())
		isDirty = true;

	if (jogAmount.x != 0 || jogAmount.y != 0 || jogAmount.z != 0) {
		isDirty = true;

		if (rotateMode) {
			updateShaderDirectionVector(add4(control.viewVector, jogAmount));
		}
		else {
			control.camera = sub4(control.camera, mult4(control.sideVector, jogAmount.x));
			control.camera = sub4(control.camera, mult4(control.topVector, jogAmount.y));
			control.camera = add4(control.camera, mult4(control.viewVector, jogAmount.z));
		}
	}
}

void Fractal::updateAlterationSpeed() {
	alterationSpeed = 1.0;

	if (shiftDown && controlDown) alterationSpeed *= 50; else
		if (shiftDown) alterationSpeed *= 0.1; else
			if (controlDown) alterationSpeed *= 10;
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

/// alter equation selection, reset it's parameters, display it's widgets, calculate fractal
void Fractal::changeEquationIndex(int dir) {
	control.equation += dir;
	if (control.equation >= EQU_MAX) control.equation = 1;
	else
		if (control.equation < 1) control.equation = EQU_MAX - 1;

	reset();
	refresh(true);
	updateWindowTitle();
}

extern HWND g_hWnd;

void Fractal::keyDown(int key) {
	//char str[32];
	//sprintf_s(str,31, "KD %d %c\n", key, key);
	//OutputDebugStringA(str);

	switch (key) {
	case VK_ESCAPE:
		SendMessage(g_hWnd, WM_CLOSE, 0, 0);
		SendMessage(widget.hWnd, WM_CLOSE, 0, 0);
		break;
	case VK_SHIFT:
		shiftDown = true;
		return;
	case VK_CONTROL:
		controlDown = true;
		return;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_UP:
		widget.keyDown(key);
		break;
	}

	switch (tolower(key)) {
	case ' ':
		widget.toggleVisible();
		break;
	case '1':
		changeEquationIndex(-1);
		break;
	case '2':
		changeEquationIndex(+1);
		break;
	case '3':
		control.isStereo = !control.isStereo;
		defineWidgetsForCurrentEquation(false);
		isDirty = true;
		break;
	case '4':	jogCameraAndFocusPosition(-1, 0, 0);	break;
	case '5':	jogCameraAndFocusPosition(+1, 0, 0);	break;
	case '6':	jogCameraAndFocusPosition(0, -1, 0);	break;
	case '7':	jogCameraAndFocusPosition(0, +1, 0);	break;
	case '8':	jogCameraAndFocusPosition(0, 0, -1);	break;
	case '9':	jogCameraAndFocusPosition(0, 0, +1);	break;
	case 'i':
		control.doInversion = 1 - control.doInversion;
		reset();
		refresh(false);
		break;
	case 'b':
		control.showBalls = 1 - control.showBalls;
		refresh(false);
		break;
	case 'f':
		control.fourGen = !control.fourGen;
		refresh(false);
		break;
	case 'g':
		control.colorScheme += 1;
		if (control.colorScheme > 7) control.colorScheme = 0;
		defineWidgetsForCurrentEquation(false);
		updateWindowTitle();
		isDirty = true;
		break;
	case 'j':
		control.juliaboxMode = !control.juliaboxMode;
		refresh(false);
		break;
	case 'z':
		rotateMode = true;
		break;
	case 'q' :
		control.preabsx = 1 - control.preabsx;
		refresh(false);
		break;
	case 'w' :
		control.preabsy = 1 - control.preabsy;
		refresh(false);
		break;
	case 'e' :
		control.preabsz = 1 - control.preabsz;
		refresh(false);
		break;
	case 'r' :
		control.absx = 1 - control.absx;
		refresh(false);
		break;
	case 't' :
		control.absy = 1 - control.absy;
		refresh(false);
		break;
	case 'y' :
		control.absz = 1 - control.absz;
		refresh(false);
		break;
	case 'u' :
		control.useDeltaDE = 1 - control.useDeltaDE;
		refresh(false);
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
		widget.keyUp(key);
		break;
	case VK_SHIFT:
		shiftDown = false;
		break;
	case VK_CONTROL:
		controlDown = false;
		break;
	case '4':	jogRelease(-1, 0, 0);	break;
	case '5':	jogRelease(+1, 0, 0);	break;
	case '6':	jogRelease(0, -1, 0);	break;
	case '7':	jogRelease(0, +1, 0);	break;
	case '8':	jogRelease(0, 0, -1);	break;
	case '9':	jogRelease(0, 0, +1);	break;
	case 'z':
		rotateMode = false;
		break;
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
"Klienian Sponge"
};

void Fractal::updateWindowTitle() {
	char str[512];
	sprintf_s(str, 511, "Fractal    Equation %d: %s,    Parameter: %s", control.equation + 1, equationName[control.equation], widget.focusString());
	SetWindowTextA(g_hWnd, str);
}
