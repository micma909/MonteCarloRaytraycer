#ifndef CIE_H
#define CIE_H

#include "Util/Vec.h"
#include "Definitions.h"
#include <algorithm>

/* The number of wavelengths used per pixel sample. */
#define WAVELENGTHS (1 + 400 / RESOLUTION)

/* These are some scene file definitions for color systems. */
#define ID_EBU 0
#define ID_SMPTE 1
#define ID_HDTV 2
#define ID_REC709 3
#define ID_NTSC 4
#define ID_CIE 5

/* This is a color system. */
typedef struct ColorSystem{
    double xRed, yRed;              /* Red x, y */
    double xGreen, yGreen;          /* Green x, y */
    double xBlue, yBlue;            /* Blue x, y */
    double xWhite, yWhite;          /* White point x, y */
    double gamma;                   /* Gamma correction for system */
} ColorSystem;

/* These are some relatively common illuminants (white points). */
#define IlluminantC     0.3101, 0.3162          /* For NTSC television */
#define IlluminantD65   0.3127, 0.3291          /* For EBU and SMPTE */
#define IlluminantE     0.33333333, 0.33333333  /* CIE equal-energy illuminant */

/* 0 represents a special gamma function. */
#define GAMMA_REC709 0

/* These are some standard color systems. */
const ColorSystem /* xRed    yRed    xGreen  yGreen  xBlue  yBlue    White point        Gamma   */
    EBUSystem    =  {0.64,   0.33,   0.29,   0.60,   0.15,   0.06,   IlluminantD65,  GAMMA_REC709},
    SMPTESystem  =  {0.630,  0.340,  0.310,  0.595,  0.155,  0.070,  IlluminantD65,  GAMMA_REC709},
    HDTVSystem   =  {0.670,  0.330,  0.210,  0.710,  0.150,  0.060,  IlluminantD65,  GAMMA_REC709},
    Rec709System =  {0.64,   0.33,   0.30,   0.60,   0.15,   0.06,   IlluminantD65,  GAMMA_REC709},
    NTSCSystem   =  {0.67,   0.33,   0.21,   0.71,   0.14,   0.08,   IlluminantC,    GAMMA_REC709},
    CIESystem    =  {0.7355, 0.2645, 0.2658, 0.7243, 0.1669, 0.0085, IlluminantE,    GAMMA_REC709};

/* A static array of standard color systems. */
const ColorSystem ColorSystems[6] = {EBUSystem, SMPTESystem, HDTVSystem, Rec709System, NTSCSystem, CIESystem};

/* Converts a spectral radiance distribution to an RGB color. */
Vec3f SpectrumToRGB(float spectralRadiance[WAVELENGTHS], ColorSystem colorSystem);

/* Returns the luminance of an RGB color according to a given color system. */
float Luminance(Vec3f rgb, ColorSystem colorSystem);

/* Gamma corrects an RGB color according to a given color system. */
Vec3f GammaCorrect(Vec3f rgb, ColorSystem colorSystem);

///-----

/* The CIE color matching curve, at 5nm intervals. */
static Vec3f ColorMatchingCurve[81] = {
    Vec3f(0.0014,0.0000,0.0065), Vec3f(0.0022,0.0001,0.0105), Vec3f(0.0042,0.0001,0.0201),
    Vec3f(0.0076,0.0002,0.0362), Vec3f(0.0143,0.0004,0.0679), Vec3f(0.0232,0.0006,0.1102),
    Vec3f(0.0435,0.0012,0.2074), Vec3f(0.0776,0.0022,0.3713), Vec3f(0.1344,0.0040,0.6456),
    Vec3f(0.2148,0.0073,1.0391), Vec3f(0.2839,0.0116,1.3856), Vec3f(0.3285,0.0168,1.6230),
    Vec3f(0.3483,0.0230,1.7471), Vec3f(0.3481,0.0298,1.7826), Vec3f(0.3362,0.0380,1.7721),
    Vec3f(0.3187,0.0480,1.7441), Vec3f(0.2908,0.0600,1.6692), Vec3f(0.2511,0.0739,1.5281),
    Vec3f(0.1954,0.0910,1.2876), Vec3f(0.1421,0.1126,1.0419), Vec3f(0.0956,0.1390,0.8130),
    Vec3f(0.0580,0.1693,0.6162), Vec3f(0.0320,0.2080,0.4652), Vec3f(0.0147,0.2586,0.3533),
    Vec3f(0.0049,0.3230,0.2720), Vec3f(0.0024,0.4073,0.2123), Vec3f(0.0093,0.5030,0.1582),
    Vec3f(0.0291,0.6082,0.1117), Vec3f(0.0633,0.7100,0.0782), Vec3f(0.1096,0.7932,0.0573),
    Vec3f(0.1655,0.8620,0.0422), Vec3f(0.2257,0.9149,0.0298), Vec3f(0.2904,0.9540,0.0203),
    Vec3f(0.3597,0.9803,0.0134), Vec3f(0.4334,0.9950,0.0087), Vec3f(0.5121,1.0000,0.0057),
    Vec3f(0.5945,0.9950,0.0039), Vec3f(0.6784,0.9786,0.0027), Vec3f(0.7621,0.9520,0.0021),
    Vec3f(0.8425,0.9154,0.0018), Vec3f(0.9163,0.8700,0.0017), Vec3f(0.9786,0.8163,0.0014),
    Vec3f(1.0263,0.7570,0.0011), Vec3f(1.0567,0.6949,0.0010), Vec3f(1.0622,0.6310,0.0008),
    Vec3f(1.0456,0.5668,0.0006), Vec3f(1.0026,0.5030,0.0003), Vec3f(0.9384,0.4412,0.0002),
    Vec3f(0.8544,0.3810,0.0002), Vec3f(0.7514,0.3210,0.0001), Vec3f(0.6424,0.2650,0.0000),
    Vec3f(0.5419,0.2170,0.0000), Vec3f(0.4479,0.1750,0.0000), Vec3f(0.3608,0.1382,0.0000),
    Vec3f(0.2835,0.1070,0.0000), Vec3f(0.2187,0.0816,0.0000), Vec3f(0.1649,0.0610,0.0000),
    Vec3f(0.1212,0.0446,0.0000), Vec3f(0.0874,0.0320,0.0000), Vec3f(0.0636,0.0232,0.0000),
    Vec3f(0.0468,0.0170,0.0000), Vec3f(0.0329,0.0119,0.0000), Vec3f(0.0227,0.0082,0.0000),
    Vec3f(0.0158,0.0057,0.0000), Vec3f(0.0114,0.0041,0.0000), Vec3f(0.0081,0.0029,0.0000),
    Vec3f(0.0058,0.0021,0.0000), Vec3f(0.0041,0.0015,0.0000), Vec3f(0.0029,0.0010,0.0000),
    Vec3f(0.0020,0.0007,0.0000), Vec3f(0.0014,0.0005,0.0000), Vec3f(0.0010,0.0004,0.0000),
    Vec3f(0.0007,0.0002,0.0000), Vec3f(0.0005,0.0002,0.0000), Vec3f(0.0003,0.0001,0.0000),
    Vec3f(0.0002,0.0001,0.0000), Vec3f(0.0002,0.0001,0.0000), Vec3f(0.0001,0.0000,0.0000),
    Vec3f(0.0001,0.0000,0.0000), Vec3f(0.0001,0.0000,0.0000), Vec3f(0.0000,0.0000,0.0000)};

/* Converts a spectral radiance distribution to an RGB color. */
Vec3f SpectrumToRGB(float spectralRadiance[WAVELENGTHS], ColorSystem colorSystem)
{
    /* Simply integrate the color-matching curve. */
    float radiance = 0;
    Vec3f color(0.f);
    for (int w = 0; w < WAVELENGTHS; w++)
    {
        
        color = color + ColorMatchingCurve[w * (RESOLUTION / 5)] * spectralRadiance[w];
        radiance += spectralRadiance[w];
    }


    /* Normalize the XYZ color. */
    float sum = color[0] + color[1] + color[2];
    if (sum > EPSILON) color = color * (1.f/sum);

    /* Decode the color system. */
    float xr = colorSystem.xRed;   float yr = colorSystem.yRed;   float zr = 1 - (xr + yr);
    float xg = colorSystem.xGreen; float yg = colorSystem.yGreen; float zg = 1 - (xg + yg);
    float xb = colorSystem.xBlue;  float yb = colorSystem.yBlue;  float zb = 1 - (xb + yb);
    float xw = colorSystem.xWhite; float yw = colorSystem.yWhite; float zw = 1 - (xw + yw);

    /* Compute the XYZ to RGB matrix. */
    float rx = (yg * zb) - (yb * zg);
    float ry = (xb * zg) - (xg * zb);
    float rz = (xg * yb) - (xb * yg);
    float gx = (yb * zr) - (yr * zb);
    float gy = (xr * zb) - (xb * zr);
    float gz = (xb * yr) - (xr * yb);
    float bx = (yr * zg) - (yg * zr);
    float by = (xg * zr) - (xr * zg);
    float bz = (xr * yg) - (xg * yr);

    /* Compute the RGB luminance scaling factor. */
    float rw = ((rx * xw) + (ry * yw) + (rz * zw)) / yw;
    float gw = ((gx * xw) + (gy * yw) + (gz * zw)) / yw;
    float bw = ((bx * xw) + (by * yw) + (bz * zw)) / yw;

    /* Scale the XYZ to RGB matrix to white. */
    rx = rx / rw;  ry = ry / rw;  rz = rz / rw;
    gx = gx / gw;  gy = gy / gw;  gz = gz / gw;
    bx = bx / bw;  by = by / bw;  bz = bz / bw;

    /* Calculate the desired RGB.
    Vec3f rgb((rx * color[0]) + (ry * color[1]) + (rz * color[2]),
                        (gx * color[0]) + (gy * color[1]) + (gz * color[2]),
                        (bx * color[0]) + (by * color[1]) + (bz * color[2])); */
    Vec3f rgb;
    rgb[0] = ((rx * color[0]) + (ry * color[1]) + (rz * color[2]));
    rgb[1] = ((gx * color[0]) + (gy * color[1]) + (gz * color[2]));
    rgb[2] = ((bx * color[0]) + (by * color[1]) + (bz * color[2]));


    /* Constrain the RGB color within the RGB gamut. */
    float w = std::min(0.0f, std::min(rgb[0], std::min(rgb[1], rgb[2])));
    rgb = rgb - Vec3f(w, w, w);
    
    /* Multiply the final RGB color by the pixel's radiance. */
   return rgb * radiance;
    
}

/* Returns the luminance of an RGB color according to a given color system. */
float Luminance(Vec3f rgb, ColorSystem colorSystem)
{
    return rgb[0] * colorSystem.yRed + rgb[1] * colorSystem.yGreen + rgb[2] * colorSystem.yBlue;
}

/* Gamma corrects an RGB color according to a given color system. */
Vec3f GammaCorrect(Vec3f rgb, ColorSystem colorSystem)
{
    /* If we're using the special REC709 gamma formula... */
    if (colorSystem.gamma == GAMMA_REC709)
    {
        /* Use the special piecewise formula. */
        if (rgb[0] >= 0.018) rgb[0]= (1.099 * pow(rgb[0], 0.45)) - 0.099;
        else rgb[0] *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
        if (rgb[1] >= 0.018) rgb[1] = (1.099 * pow(rgb[1], 0.45)) - 0.099;
        else rgb[1] *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
        if (rgb[2] >= 0.018) rgb[2] = (1.099 * pow(rgb[2], 0.45)) - 0.099;
        else rgb[2] *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;

        /* Return the gamma-corrected color. */
        return rgb;
    }
    else
    {
        /* Otherwise, use a standard gamma power curve. */
        float power = 1.0f / colorSystem.gamma;
        return Vec3f(powf(rgb[0], power), powf(rgb[1], power), powf(rgb[2], power));
    }
}
#endif