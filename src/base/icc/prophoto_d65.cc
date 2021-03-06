/* 
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#include <string>
#include <iostream>
#include "../iccstore.hh"


/* ***** Make profile: Romm/Prophoto, D50, gamma=1.80 */
/* Reference Input/Output Medium Metric RGB Color Encodings (RIMM/ROMM RGB)
 * Kevin E. Spaulding, Geoffrey J. Woolfe and Edward J. Giorgianni
 * Eastman Kodak Company, Rochester, New York, U.S.A.
 * Above document is available at http://photo-lovers.org/pdf/color/romm.pdf
 * Kodak designed the Romm (ProPhoto) color gamut to include all printable
 * and most real world colors. It includes some imaginary colors and excludes
 * some of the real world blues and violet blues that can be captured by
 * digital cameras. For high bit depth image editing only.
 */
static cmsCIExyYTRIPLE romm_primaries = {
    {0.7347, 0.2653, 1.0},
    {0.1596, 0.8404, 1.0},
    {0.0366, 0.0001, 1.0}
};


/* ************************** WHITE POINTS ************************** */

/* D50 WHITE POINTS */

static cmsCIExyY d50_romm_spec= {0.3457, 0.3585, 1.0};
/* http://photo-lovers.org/pdf/color/romm.pdf */

static cmsCIExyY d50_illuminant_specs = {0.345702915, 0.358538597, 1.0};
/* calculated from D50 illuminant XYZ values in ICC specs */


/* D65 WHITE POINTS */

static cmsCIExyY  d65_srgb_adobe_specs = {0.3127, 0.3290, 1.0};
/* White point from the sRGB.icm and AdobeRGB1998 profile specs:
 * http://www.adobe.com/digitalimag/pdfs/AdobeRGB1998.pdf
 * 4.2.1 Reference Display White Point
 * The chromaticity coordinates of white displayed on
 * the reference color monitor shall be x=0.3127, y=0.3290.
 * . . . [which] correspond to CIE Standard Illuminant D65.
 *
 * Wikipedia gives this same white point for SMPTE-C.
 * This white point is also given in the sRGB color space specs.
 * It's probably correct for most or all of the standard D65 profiles.
 *
 * The D65 white point values used in the LCMS virtual sRGB profile
 * is slightly different than the D65 white point values given in the
 * sRGB color space specs, so the LCMS virtual sRGB profile
 * doesn't match sRGB profiles made using the values given in the
 * sRGB color space specs.
 *
 * */


PF::ProPhotoProfileD65::ProPhotoProfileD65(TRC_type type): ICCProfile()
{
  set_trc_type( type );

  /* ***** Make profile: ProPhoto, D50, gamma=1.8 TRC */
  /*
   * */
  cmsCIExyYTRIPLE primaries = romm_primaries;
  //cmsCIExyY whitepoint = d65_srgb_adobe_specs;
  cmsCIExyY whitepoint = d50_romm_spec;
  cmsToneCurve* tone_curve[3] = {NULL};
  switch( type ) {
  case PF::PF_TRC_STANDARD: {
    /* gamma=1.80078125 tone response curve */
    /* http://www.color.org/chardata/rgb/ROMMRGB.pdf indicates that
     * the official tone response curve for ROMM isn't a simple gamma curve
     * but rather has a linear portion in shadows, just like sRGB.
     * Most ProPhotoRGB profiles use a gamma curve equal to 1.80078125.
     * This odd value is because of hexadecimal rounding.
     * */
    cmsToneCurve *curve = cmsBuildGamma (NULL, 1.80078125);
    tone_curve[0] = tone_curve[1] = tone_curve[2] = curve;
    break;
  }
  case PF::PF_TRC_PERCEPTUAL: {
    cmsFloat64Number labl_parameters[5] =
    { 3.0, 0.862076,  0.137924, 0.110703, 0.080002 };
    cmsToneCurve *curve =
        cmsBuildParametricToneCurve(NULL, 4, labl_parameters);
    tone_curve[0] = tone_curve[1] = tone_curve[2] = curve;
    break;
  }
  case PF::PF_TRC_LINEAR: {
    cmsToneCurve *curve = cmsBuildGamma (NULL, 1.00);
    tone_curve[0] = tone_curve[1] = tone_curve[2] = curve;
    break;
  }
  case PF::PF_TRC_sRGB: {
    /* sRGB TRC */
    cmsFloat64Number srgb_parameters[5] =
    { 2.4, 1.0 / 1.055,  0.055 / 1.055, 1.0 / 12.92, 0.04045 };
    cmsToneCurve *curve = cmsBuildParametricToneCurve(NULL, 4, srgb_parameters);
    //cmsToneCurve *curve = cmsBuildTabulatedToneCurve16(NULL, dt_srgb_tone_curve_values_n, dt_srgb_tone_curve_values);
    tone_curve[0] = tone_curve[1] = tone_curve[2] = curve;
    break;
  }
  case PF::PF_TRC_GAMMA_22: {
    cmsToneCurve *curve = cmsBuildGamma (NULL, 2.20);
    tone_curve[0] = tone_curve[1] = tone_curve[2] = curve;
    break;
  }
  case PF::PF_TRC_GAMMA_18: {
    cmsToneCurve *curve = cmsBuildGamma (NULL, 1.80);
    tone_curve[0] = tone_curve[1] = tone_curve[2] = curve;
    break;
  }
  }
  cmsHPROFILE profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
  cmsMLU *copyright = cmsMLUalloc(NULL, 1);
  cmsMLUsetASCII(copyright, "en", "US", "Copyright 2015, Elle Stone (website: http://ninedegreesbelow.com/; email: ellestone@ninedegreesbelow.com). This ICC profile is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License (https://creativecommons.org/licenses/by-sa/3.0/legalcode).");
  cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
  /* V4 */
  cmsMLU *description = cmsMLUalloc(NULL, 1);
  cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V4.icc");
  cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
  //const char* filename = "sRGB-elle-V4-rec709.icc";
  //cmsSaveProfileToFile(profile, filename);
  cmsMLUfree(description);

  //std::cout<<"Initializing sRGB profile"<<std::endl;
  set_profile( profile );
}
