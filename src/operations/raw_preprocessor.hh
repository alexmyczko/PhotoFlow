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

#ifndef VIPS_RAW_PREPROCESSOR_H
#define VIPS_RAW_PREPROCESSOR_H

#include <string>

#include "../base/processor.hh"
#include "../base/rawmatrix.hh"

#include "raw_image.hh"


//#define RT_EMU 1


//#define __CLIP(a)  CLIP(a)
#define __CLIP(a) (a)

namespace PF 
{

  extern int raw_preproc_sample_x;
  extern int raw_preproc_sample_y;

  class RawPreprocessorPar: public OpParBase
  {
    dcraw_data_t* image_data;

    PropertyBase wb_mode;
    hlreco_mode_t hlreco_mode;

    Property<float>* wb_red[WB_LAST];
    Property<float>* wb_green[WB_LAST];
    Property<float>* wb_blue[WB_LAST];

    Property<float> camwb_corr_red;
    Property<float> camwb_corr_green;
    Property<float> camwb_corr_blue;

    Property<float> wb_target_L;
    Property<float> wb_target_a;
    Property<float> wb_target_b;

    Property< std::vector< std::vector<int> > > wb_areas;

    Property<float> saturation_level_correction;
    Property<float> black_level_correction;
    Property<float> black_level_correction_r;
    Property<float> black_level_correction_g1;
    Property<float> black_level_correction_g2;
    Property<float> black_level_correction_b;

    float wb_red_current, wb_green_current, wb_blue_current;

  public:
    RawPreprocessorPar();

    /* Set processing hints:
       1. the intensity parameter makes no sense for an image, 
			 creation of an intensity map is not allowed
       2. the operation can work without an input image;
			 the blending will be set in this case to "passthrough" and the image
			 data will be simply linked to the output
		*/
    bool has_intensity() { return false; }
    bool has_opacity() { return false; }
    bool needs_input() { return true; }

    dcraw_data_t* get_image_data() {return image_data; }

    void set_hlreco_mode(hlreco_mode_t m) { hlreco_mode = m; }
    hlreco_mode_t get_hlreco_mode() { return hlreco_mode; }

    void init_wb_coefficients( dcraw_data_t* idata, std::string camera_maker, std::string camera_model );

    wb_mode_t get_wb_mode() { return (wb_mode_t)(wb_mode.get_enum_value().first); }
    float get_wb_red() { return wb_red_current; /*wb_red.get();*/ }
    float get_wb_green() { return wb_green_current; /*wb_green.get();*/ }
    float get_wb_blue() { return wb_blue_current; /*wb_blue.get();*/ }

    float get_preset_wb_red(int id) { return wb_red[id]->get(); }
    float get_preset_wb_green(int id ) { return wb_green[id]->get(); }
    float get_preset_wb_blue(int id) { return wb_blue[id]->get(); }

    float get_camwb_corr_red() { return camwb_corr_red.get(); }
    float get_camwb_corr_green() { return camwb_corr_green.get(); }
    float get_camwb_corr_blue() { return camwb_corr_blue.get(); }

    void set_wb(float r, float g, float b) {
      wb_red_current = r;
      wb_green_current = g;
      wb_blue_current = b;
#ifndef NDEBUG
      std::cout<<"RawPreprocessorPar: setting WB coefficients to "<<r<<","<<g<<","<<b<<std::endl;
#endif
    }
    void add_wb_area(std::vector<int>& area) { wb_areas.get().push_back(area); }
    std::vector< std::vector<int> >& get_wb_areas() { return wb_areas.get(); }

    float get_saturation_level_correction() { return saturation_level_correction.get(); }
    float get_black_level_correction_r() { return black_level_correction_r.get(); }
    float get_black_level_correction_g1() { return black_level_correction_g1.get(); }
    float get_black_level_correction_g2() { return black_level_correction_g2.get(); }
    float get_black_level_correction_b() { return black_level_correction_b.get(); }

    VipsImage* build(std::vector<VipsImage*>& in, int first, 
										 VipsImage* imap, VipsImage* omap, unsigned int& level);
  };

  

  template < OP_TEMPLATE_DEF > 
  class RawPreprocessor
  {
  public: 
    void render(VipsRegion** ireg, int n, int in_first,
								VipsRegion* imap, VipsRegion* omap, 
								VipsRegion* oreg, OpParBase* par)
    {
      RawPreprocessorPar* rdpar = dynamic_cast<RawPreprocessorPar*>(par);
      if( !rdpar ) return;

      float mul[4] = {1,1,1,1};

      hlreco_mode_t hlreco_mode = rdpar->get_hlreco_mode();

      float cam_wb[4] = {
          rdpar->get_image_data()->color.cam_mul[0],
          rdpar->get_image_data()->color.cam_mul[1],
          rdpar->get_image_data()->color.cam_mul[2],
          rdpar->get_image_data()->color.cam_mul[1]
      };
      float cam_wb_max = MAX3(cam_wb[0], cam_wb[1], cam_wb[2]);
      cam_wb[0] /= cam_wb_max;
      cam_wb[1] /= cam_wb_max;
      cam_wb[2] /= cam_wb_max;
      cam_wb[3] /= cam_wb_max;

      //std::cout<<"RawPreprocessor::render(): rdpar->get_wb_mode()="<<rdpar->get_wb_mode()<<std::endl;
      switch( rdpar->get_wb_mode() ) {
      case WB_SPOT:
      case WB_COLOR_SPOT:
				//render_spotwb(ireg, n, in_first, imap, omap, oreg, rdpar);
				//std::cout<<"render_spotwb() called"<<std::endl;
        mul[0] = rdpar->get_wb_red();
        mul[1] = rdpar->get_wb_green();
        mul[2] = rdpar->get_wb_blue();
        mul[3] = rdpar->get_wb_green();
				break;
      default:
        //render_camwb(ireg, n, in_first, imap, omap, oreg, rdpar);
        //std::cout<<"render_camwb() called"<<std::endl;
        mul[0] = rdpar->get_wb_red()*rdpar->get_camwb_corr_red();
        mul[1] = rdpar->get_wb_green()*rdpar->get_camwb_corr_green();
        mul[2] = rdpar->get_wb_blue()*rdpar->get_camwb_corr_blue();
        mul[3] = rdpar->get_wb_green()*rdpar->get_camwb_corr_green();
        break;
      }

      dcraw_data_t* image_data = rdpar->get_image_data();
      VipsRect *r = &oreg->valid;
      int nbands = ireg[in_first]->im->Bands;

      int x, y;
      //float range = image_data->color.maximum - image_data->color.black;
      float range = 1;
      float min_mul = mul[0];
      float max_mul = mul[0];
      /*
      for(int i = 0; i < 4; i++) {
        std::cout<<"cam_mu["<<i<<"]="<<image_data->color.cam_mul[i]<<"  ";
      }
      std::cout<<std::endl;
      */
      for(int i = 1; i < 4; i++) {
        if( mul[i] < min_mul )
          min_mul = mul[i];
        if( mul[i] > max_mul )
          max_mul = mul[i];
      }
#ifdef RT_EMU
      /* RawTherapee emulation */
      range *= max_mul;
#else
      if( hlreco_mode == HLRECO_CLIP )
        range *= min_mul;
      else
        range *= max_mul;
#endif
      //std::cout<<"range="<<range<<"  min_mul="<<min_mul<<"  max_mul="<<max_mul<<std::endl;

      float white_corr = rdpar->get_saturation_level_correction();
      float black_corr[4] = {
          rdpar->get_black_level_correction_r(),
          rdpar->get_black_level_correction_g1(),
          rdpar->get_black_level_correction_b(),
          rdpar->get_black_level_correction_g2()
      };

      //if(r->top==0 && r->left==0) std::cout<<"white_corr="<<white_corr<<std::endl;

      float black[4], white[4];
      for(int i = 0; i < 4; i++) {
        mul[i] = mul[i] / range;
        //black[i] = rdpar->get_black_level_correction() / (image_data->color.maximum - image_data->color.black);
        black[i] = image_data->color.cblack[i] * black_corr[i];
        white[i] = image_data->color.maximum * white_corr;
        //if( nbands != 3 ) black[i] *= 65535;
        //std::cout<<"black="<<rdpar->get_black_level_correction()<<" * 65535 * "
        //    <<exposure<<" / "<<(image_data->color.maximum - image_data->color.black)
        //    <<"="<<black[i]<<std::endl;
        if(false && r->left==0 && r->top==0)
          std::cout<<"black["<<i<<"]="<<black[i]<<"  white["<<i<<"]="<<white[i]<<std::endl;
      }

      //if(r->left==0 && r->top==0) std::cout<<"RawPreprocessor::render_camwb(): nbands="<<nbands<<std::endl;
      if( nbands == 3 ) {
        float* p;
        float* pout;
        float in;
        int line_sz = r->width*3;
        for( y = 0; y < r->height; y++ ) {
          p = (float*)VIPS_REGION_ADDR( ireg[in_first], r->left, r->top + y );
          pout = (float*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );
          for( x=0; x < line_sz; x+=3) {
            //pout[x] = __CLIP( (p[x]-black[0]) * sat_corr * mul[0] - black[0]);
            //pout[x+1] = __CLIP(p[x+1] * sat_corr * mul[1] - black[1]);
            //pout[x+2] = __CLIP(p[x+2] * sat_corr * mul[2] - black[2]);
            //pout[x] = (p[x]-black[0]) * mul[0] / (white[0]-black[0]);
            //pout[x+1] = (p[x+1]-black[1]) * mul[1] / (white[1]-black[1]);
            //pout[x+2] = (p[x+2]-black[2]) * mul[2] / (white[2]-black[2]);
            int i = 0;
            if( black_corr[i] != 1 || white_corr != 1 ) {
            in = p[x+i] * (image_data->color.maximum - image_data->color.cblack[i]) + image_data->color.cblack[i];
            in -= black[i]; in /= (white[i] - black[i]);
            pout[x]   = in * mul[0] / cam_wb[0];
            i = 1;
            in = p[x+i] * (image_data->color.maximum - image_data->color.cblack[i]) + image_data->color.cblack[i];
            in -= black[i]; in /= (white[i] - black[i]);
            pout[x+1] = in * mul[1] / cam_wb[1];
            i = 2;
            in = p[x+i] * (image_data->color.maximum - image_data->color.cblack[i]) + image_data->color.cblack[i];
            in -= black[i]; in /= (white[i] - black[i]);
            pout[x+2] = in * mul[2] / cam_wb[2];
            } else {
              pout[x]   = p[x]   * mul[0] / cam_wb[0];
              pout[x+1] = p[x+1] * mul[1] / cam_wb[1];
              pout[x+2] = p[x+2] * mul[2] / cam_wb[2];
            }
            if( hlreco_mode == HLRECO_CLIP ) {
              pout[x] = CLIP( pout[x] );
              pout[x+1] = CLIP( pout[x+1] );
              pout[x+2] = CLIP( pout[x+2] );
            }
            if(false && r->left==0 && r->top==0) std::cout<<"  p["<<x<<"]="<<p[x]<<"  pout["<<x<<"]="<<pout[x]<<std::endl;
#ifdef RT_EMU
            /* RawTherapee emulation */
            pout[x] *= 65535;
            pout[x+1] *= 65535;
            pout[x+2] *= 65535;
#endif
          }
        }
      } else {
        PF::raw_pixel_t* p;
        PF::raw_pixel_t* pout;
        float rval;
        for( y = 0; y < r->height; y++ ) {
          p = (PF::raw_pixel_t*)VIPS_REGION_ADDR( ireg[in_first], r->left, r->top + y );
          pout = (PF::raw_pixel_t*)VIPS_REGION_ADDR( oreg, r->left, r->top + y );
          PF::RawMatrixRow rp( p );
          PF::RawMatrixRow rpout( pout );
          for( x=0; x < r->width; x++) {
            //std::cout<<"RawPreprocessor: x="<<x<<"  r->width="<<r->width
            //      <<"  size of pel="<<VIPS_IMAGE_SIZEOF_PEL(ireg[in_first]->im)
            //      <<","<<VIPS_IMAGE_SIZEOF_PEL(oreg->im)<<std::endl;
            rpout.color(x) = rp.color(x);
            //rpout[x] = __CLIP(rp[x] * sat_corr * mul[ rp.icolor(x) ] - black[ rp.icolor(x) ]);
            int c = rp.icolor(x);
            if( black_corr[c] != 1 || white_corr != 1 ) {
              rval = rp[x] * (image_data->color.maximum - image_data->color.cblack[c]) + image_data->color.cblack[c];
              rval = (rval - black[c]) / (white[c] - black[c]);
            } else {
              rval = rp[x];
            }
            if( hlreco_mode == HLRECO_CLIP ) {
              rpout[x] = CLIP( rval * mul[c] / cam_wb[c] );
            } else {
              //if( rval > 65500.f ) rpout[x] = 65535.f;
              //else
              rpout[x] = rval * mul[c] / cam_wb[c];
            }
            if(false && r->left==0 && r->top==0 && y<4 && x<4)
              std::cout<<"("<<y<<","<<x<<")  c="<<rp.color(x) //c
              <<"  rp[x]="<<rp[x]
                               <<"  mul[ c ]="<<mul[ c ]
                                                     <<"  black[ c ]="<<black[ c ]
                                                                           <<"  white[ c ]="<<white[ c ]
              <<"  rpout[x]="<<rpout[x]<<std::endl;
#ifdef RT_EMU
            /* RawTherapee emulation */
            rpout[x] *= 65535;
#endif
          }
        }
      }

    }
  };




  ProcessorBase* new_raw_preprocessor();
}

#endif 


