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

#include "dynamic_range_compressor_config_v2.hh"


PF::DynamicRangeCompressorConfigV2GUI::DynamicRangeCompressorConfigV2GUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "dynamic range compressor" ),
  modeSelector( this, "method", "method: ", 0 ),
  amount_slider( this, "amount", "amount", 100, 0, 100, 2, 10, 100),
  strength_s_slider( this, "strength_s", "shadows", 0, 0, 100, 0.5, 5, 1),
  strength_h_slider( this, "strength_h", "highlights", 0, 0, 100, 0.5, 5, 1),
  anchor_slider( this, "anchor", "anchor", 0, 0, 100, 0.5, 5, 100),
  local_contrast_slider( this, "local_contrast", "local contrast", 0, -1, 1, 0.05, 0.2, 1),
  show_residual_box( this, "show_residual", "show residual", false),
  enable_equalizer_box( this, "enable_equalizer", "enable equalizer", true),
  blacks_amount_slider( this, "blacks_amount", "blacks", 100, 0, 100, 5, 10, 100),
  shadows_amount_slider( this, "shadows_amount", "shadows", 100, 0, 100, 5, 10, 100),
  midtones_amount_slider( this, "midtones_amount", "midtones", 100, 0, 100, 5, 10, 100),
  highlights_amount_slider( this, "highlights_amount", "highlights", 100, 0, 100, 5, 10, 100),
  whites_amount_slider( this, "whites_amount", "whites", 100, 0, 100, 5, 10, 100),
  usmRadiusSlider( this, "gauss_radius", "radius", 1, 0, 100, 0.05, 0.1, 1),
  bilateralIterationsSlider( this, "bilateral_iterations", "iterations", 1, 1, 10, 1, 1, 1),
  bilateralSigmasSlider( this, "bilateral_sigma_s", "sigma s", 25, 0, 100, 1, 5, 10),
  bilateralSigmarSlider( this, "bilateral_sigma_r", "sigma r", 20, 0, 100, 1, 5, 10),
  guidedRadiusSlider( this, "guided_radius", "coarseness", 25, 0, 100, 0.5, 5, 10),
  guidedThresholdSlider( this, "guided_threshold", "edge threshold", 20, 0.5, 10000.0, 0.5, 5, 1000000),
  lumiBlendSlider( this, "lumi_blend", "luminance blend", 100, 0, 100.0, 5, 10, 100)
{

  usmControlsBox.pack_start( usmRadiusSlider, Gtk::PACK_SHRINK );

  //bilateralControlsBox.pack_start( bilateralIterationsSlider, Gtk::PACK_SHRINK );
  //bilateralControlsBox.pack_start( bilateralSigmasSlider, Gtk::PACK_SHRINK );
  //bilateralControlsBox.pack_start( bilateralSigmarSlider, Gtk::PACK_SHRINK );

  //controlsBox.pack_start( modeSelector, Gtk::PACK_SHRINK, 10 );
  //controlsBox.pack_start( amount_slider, Gtk::PACK_SHRINK, 2 );
  controlsBox.pack_start( strength_s_slider, Gtk::PACK_SHRINK, 2 );
  controlsBox.pack_start( strength_h_slider, Gtk::PACK_SHRINK, 2 );
  //controlsBox.pack_start( anchor_slider, Gtk::PACK_SHRINK, 2 );
  controlsBox.pack_start( local_contrast_slider, Gtk::PACK_SHRINK, 2 );
  //controlsBox.pack_start( bilateralSigmasSlider, Gtk::PACK_SHRINK, 2 );
  //controlsBox.pack_start( bilateralSigmarSlider, Gtk::PACK_SHRINK, 2 );
  controlsBox.pack_start( guidedRadiusSlider, Gtk::PACK_SHRINK, 2 );
  controlsBox.pack_start( guidedThresholdSlider, Gtk::PACK_SHRINK, 2 );
  //controlsBox.pack_start( lumiBlendSlider, Gtk::PACK_SHRINK, 2 );
  controlsBox.pack_start( show_residual_box, Gtk::PACK_SHRINK, 2 );
  //controlsBox.pack_start( separator, Gtk::PACK_SHRINK, 10 );

  //controlsBox.pack_start( amount_slider, Gtk::PACK_SHRINK );
  equalizerCheckboxBox.pack_start( enable_equalizer_box, Gtk::PACK_SHRINK );
  equalizerCheckboxBox.pack_start( equalizerCheckboxPadding, Gtk::PACK_EXPAND_WIDGET );
  //controlsBox.pack_start( equalizerCheckboxBox, Gtk::PACK_SHRINK );

  equalizerBox.pack_start( blacks_amount_slider, Gtk::PACK_SHRINK, 2 );
  equalizerBox.pack_start( shadows_amount_slider, Gtk::PACK_SHRINK, 2 );
  equalizerBox.pack_start( midtones_amount_slider, Gtk::PACK_SHRINK, 2 );
  equalizerBox.pack_start( highlights_amount_slider, Gtk::PACK_SHRINK, 2 );
  equalizerBox.pack_start( whites_amount_slider, Gtk::PACK_SHRINK, 2 );
  equalizerBox.pack_start( equalizerPadding, Gtk::PACK_EXPAND_WIDGET );
  //controlsBox.pack_start( equalizerBox, Gtk::PACK_SHRINK );

  //globalBox.pack_start( controlsBox, Gtk::PACK_SHRINK );
  add_widget( controlsBox );
}




void PF::DynamicRangeCompressorConfigV2GUI::do_update()
{
  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {
  }

  OperationConfigGUI::do_update();
}

