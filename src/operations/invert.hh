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

#ifndef VIPS_INVERT_H
#define VIPS_INVERT_H

#include <iostream>

#include "../base/format_info.hh"
#include "../base/pixel_processor.hh"

namespace PF 
{

  class InvertPar: public PixelProcessorPar
  {
  public:
    InvertPar();

    bool has_intensity() { return false; }
    bool has_target_channel() { return true; }
  };

  

  template < typename T, colorspace_t CS, int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc
  {
    InvertPar* par;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(T**p, const int& n, const int& first, const int& nch, int& x, const double& intensity, T* pout) {}
  };

  
  template < typename T, int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc<T, PF_COLORSPACE_GRAYSCALE, CHMIN, CHMAX, PREVIEW, OP_PAR>
  {
    InvertPar* par;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(T**p, const int& n, const int& first, const int& nch, const int& x, const double& intensity, T* pout) 
    {
      pout[x] = (T)(FormatInfo<T>::RANGE - p[first][x]); 
    }
  };

  
  template < typename T, int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc<T, PF_COLORSPACE_RGB, CHMIN, CHMAX, PREVIEW, OP_PAR>
  {
    InvertPar* par;
    int pos;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(T**p, const int& n, const int& first, const int& nch, const int& x, const double& intensity, T* pout) 
    {
      T* pp = p[first];
      pos = x;
      for(int i = CHMIN; i <= CHMAX; i++, pos++) {
        pout[pos] = FormatInfo<T>::MAX + FormatInfo<T>::MIN - pp[pos];
      }
    }
  };

  
  template < int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc<unsigned short int, PF_COLORSPACE_RGB, CHMIN, CHMAX, PREVIEW, OP_PAR>
  {
    InvertPar* par;
    int pos;
    unsigned short int* pp;
    unsigned short int temp_in, temp_out;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(unsigned short int**p, const int& n, const int& first,
        const int& nch, const int& x, const double& intensity, unsigned short int* pout)
    {
      pp = p[first];
      pos = x;
      for(int i = CHMIN; i <= CHMAX; i++, pos++) {
        pout[pos] = FormatInfo<float>::MAX - pp[pos];
        //temp_in = PF::ICCStore::Instance().get_linear2perceptual_vec()[ pp[pos] ];
        //temp_out = FormatInfo<unsigned short int>::MAX - temp_in;
        //pout[pos] = PF::ICCStore::Instance().get_perceptual2linear_vec()[temp_out];
      }
    }
  };


  template < int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc<float, PF_COLORSPACE_RGB, CHMIN, CHMAX, PREVIEW, OP_PAR>
  {
    InvertPar* par;
    int pos;
    float* pp;
    float temp_in, temp_out;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(float**p, const int& n, const int& first,
        const int& nch, const int& x, const double& intensity, float* pout)
    {
      pp = p[first];
      pos = x;
      for(int i = CHMIN; i <= CHMAX; i++, pos++) {
        pout[pos] = FormatInfo<float>::MAX - pp[pos];
        //temp_in = PF::ICCStore::Instance().linear2perceptual( pp[pos] );
        //temp_out = 1.0f - temp_in;
        //pout[pos] = PF::ICCStore::Instance().perceptual2linear( temp_out );
      }
    }
  };


  template < typename T, int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc<T, PF_COLORSPACE_LAB, CHMIN, CHMAX, PREVIEW, OP_PAR>
  {
    InvertPar* par;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(T**p, const int& n, const int& first, const int& nch, const int& x, const double& intensity, T* pout) 
    {
      /*
      int i = x;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      i += 1;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      i += 1;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      */
      /**/
      T* pp = p[first];
      for(int i = CHMIN; i <= CHMAX; i++) {
	pout[x+i] = (T)(FormatInfo<T>::RANGE - pp[x+i]); 
      }
      /**/
    }
  };

  
  template < typename T, int CHMIN, int CHMAX, bool PREVIEW, class OP_PAR >
  class InvertProc<T, PF_COLORSPACE_CMYK, CHMIN, CHMAX, PREVIEW, OP_PAR>
  {
    InvertPar* par;
  public:
    InvertProc(InvertPar* p): par(p) {}

    void process(T**p, const int& n, const int& first, const int& nch, const int& x, const double& intensity, T* pout) 
    {
      /*
      int i = x;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      i += 1;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      i += 1;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      i += 1;
      pout[i] = (T)(FormatInfo<T>::RANGE - p[first][i]); 
      */
      /**/
      T* pp = p[first];
      for(int i = CHMIN; i <= CHMAX; i++) {
	pout[x+i] = (T)(FormatInfo<T>::RANGE - pp[x+i]); 
      }
      /**/
    }
  };

  

  /*
  template < OP_TEMPLATE_DEF > 
  class Invert: public OperationPTP< OP_TEMPLATE_IMP, 
				     InvertProc<T,CS,PREVIEW,InvertPar>, 
				     InvertPar >
  {
  };
  */

  template < OP_TEMPLATE_DEF > 
  class Invert: public PixelProcessor< OP_TEMPLATE_IMP, InvertPar, InvertProc >
  {
  };


  ProcessorBase* new_invert();
}

#endif 


