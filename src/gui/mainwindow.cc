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

#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#if defined(__APPLE__)
#include <gtkosxapplication.h>
#endif

#include "../base/file_util.hh"
#include "../base/imageprocessor.hh"
#include "../base/pf_file_loader.hh"
#include "tablabelwidget.hh"
#include "layertree.hh"
#include "mainwindow.hh"


typedef struct {
  Gtk::Widget* widget;
} WidgetDestroyData;

static gboolean widget_destroy_cb (WidgetDestroyData * data)
{
  if( data ) {
    std::cout<<"widget_destroy_cb() called."<<std::endl;
    std::cout<<"widget_destroy_cb(): destroying widget..."<<std::endl;
    delete( data->widget );
    std::cout<<"widget_destroy_cb(): ...widget destroyed."<<std::endl;
    g_free( data );
  }
  return false;
}


PF::MainWindow::MainWindow():
#ifdef GTKMM_2
mainBox(),
editorBox(),
controlBox(),
#endif
#ifdef GTKMM_3
mainBox(Gtk::ORIENTATION_VERTICAL),
editorBox(Gtk::ORIENTATION_VERTICAL),
controlBox(Gtk::ORIENTATION_VERTICAL),
topButtonBox1(Gtk::ORIENTATION_HORIZONTAL),
topButtonBox2(Gtk::ORIENTATION_HORIZONTAL),
#endif
files_frame( _("files") ),
editing_frame( _("editing") ),
img_open( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-open.png" ),
img_save( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-save.png" ),
img_save_as( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-save-as.png" ),
img_export( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-export.png" ),
img_load_preset( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-open.png" ),
img_save_preset( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-save.png" ),
img_trash( PF::PhotoFlow::Instance().get_data_dir()+"/icons/actions/16x16/document-save.png" ),
buttonOpen( /*_("Open")*/ ),
buttonSave( /*_("Save")*/ ),
buttonSaveAs( /*_("Save as")*/ ),
buttonExport( /*_("Export")*/ ),
buttonExit( _("Exit") ),
buttonTest("Test"),
buttonNewLayer(_("New Adjustment")),
buttonNewGroup(_("New Group")),
buttonDelLayer(),
buttonLoadPreset(),
buttonSavePreset()
{
  set_title("Photo Flow");
  // Sets the border width of the window.
  set_border_width(0);
  //set_default_size(120,80);
  set_default_size(1280,700);

  add(mainBox);
  
  //make_menus();

  //mainBox.pack_start(topButtonBox, Gtk::PACK_SHRINK);
  mainBox.pack_start(top_box, Gtk::PACK_SHRINK);
  mainBox.pack_start(editorBox);

  //top_box.pack_start( editing_frame, Gtk::PACK_SHRINK );
  //top_box.pack_start( files_frame, Gtk::PACK_SHRINK );

  files_frame.add( topButtonBox1 );
  editing_frame.add( topButtonBox2 );

  editorBox.pack_start(viewerNotebook);

  //VImage* image = new VImage("../testimages/lena512color.jpg");
  //imageArea.set_image("../testimages/lena512color-bis.jpg");
  //imageArea.set_image( filename );
  //set_image( filename );

  //viewerNotebook.append_page(buttonTest,"test");
  //imageArea_scrolledWindow.add(imageArea);

  buttonOpen.set_image( img_open ); buttonOpen.set_size_request(40,-1);
  buttonOpen.set_tooltip_text( _("Open existing file") );
  buttonSave.set_image( img_save ); buttonSave.set_size_request(40,-1);
  buttonSave.set_tooltip_text( _("Save current image") );
  buttonSaveAs.set_image( img_save_as ); buttonSaveAs.set_size_request(40,-1);
  buttonSaveAs.set_tooltip_text( _("Save current image with a different name") );
  buttonExport.set_image( img_export ); buttonExport.set_size_request(40,-1);
  buttonExport.set_tooltip_text( _("Export current image to raster format") );

  top_box.set_spacing(4);
  top_box.set_border_width(4);
  top_box.pack_start(buttonOpen, Gtk::PACK_SHRINK, 0);
  top_box.pack_start(buttonSave, Gtk::PACK_SHRINK, 0);
  top_box.pack_start(buttonSaveAs, Gtk::PACK_SHRINK, 0);
  top_box.pack_start(buttonExport, Gtk::PACK_SHRINK, 0);

  topButtonBox2.pack_start(buttonNewLayer, Gtk::PACK_SHRINK);
  topButtonBox2.pack_start(buttonNewGroup, Gtk::PACK_SHRINK);
  topButtonBox2.pack_start(buttonDelLayer, Gtk::PACK_SHRINK);
  buttonLoadPreset.set_image( img_load_preset ); buttonLoadPreset.set_size_request(40,-1);
  buttonLoadPreset.set_tooltip_text( _("Open existing file") );
  topButtonBox2.pack_start(buttonLoadPreset, Gtk::PACK_SHRINK);
  buttonSavePreset.set_image( img_save_preset ); buttonSavePreset.set_size_request(40,-1);
  buttonSavePreset.set_tooltip_text( _("Open existing file") );
  topButtonBox2.pack_start(buttonSavePreset, Gtk::PACK_SHRINK);

  top_box.pack_start(buttonExit, Gtk::PACK_SHRINK); buttonExit.set_size_request(70,-1);
  //topButtonBox.set_border_width(5);
  //topButtonBox.set_layout(Gtk::BUTTONBOX_START);



  buttonOpen.signal_clicked().connect( sigc::mem_fun(*this,
      &MainWindow::on_button_open_clicked) );

  buttonSave.signal_clicked().connect( sigc::mem_fun(*this,
      &MainWindow::on_button_save_clicked) );

  buttonSaveAs.signal_clicked().
      connect( sigc::mem_fun(*this,
          &MainWindow::on_button_saveas_clicked) );

  buttonExport.signal_clicked().connect( sigc::mem_fun(*this,
      &MainWindow::on_button_export_clicked) );

  buttonExit.signal_clicked().connect( sigc::mem_fun(*this,
      &MainWindow::on_button_exit) );

  viewerNotebook.signal_switch_page().
      connect( sigc::mem_fun(*this,
          &MainWindow::on_my_switch_page) );


  //imageArea.signal_configure_event().connect(sigc::mem_fun(imageArea,
  //									  &ImageArea::on_resize));

  //treeFrame.set_border_width(10);
  //treeFrame.set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);


  //controlBox.pack_start(layersWidget);

  //layersWidget.set_layer_manager( imageArea.get_layer_manager() );
  //layersWidget.set_image( imageArea.get_image() );

  /*
    controlBox.pack_start(treeNotebook);

    treeNotebook.set_tab_pos(Gtk::POS_LEFT); 

    treeFrame.add(layerTree);


    layerTree.set_reorderable();
    LayerTree* layertree = new LayerTree( imageArea.get_layer_manager() );
    layerTree.set_model(layertree->get_model());

    layerTree.append_column_editable("V", layertree->get_columns().col_visible);
    layerTree.append_column_editable("Name", layertree->get_columns().col_name);

    treeNotebook.append_page(treeFrame,"Layers");
    Widget* page = treeNotebook.get_nth_page(-1);
    Gtk::Label* label = (Gtk::Label*)treeNotebook.get_tab_label(*page);
    label->set_angle(90);
   */


  show_all_children();


}

PF::MainWindow::~MainWindow()
{
  PF::PhotoFlow::Instance().set_active_image( NULL );
  std::cout<<"~MainWindow(): deleting images"<<std::endl;
  for( unsigned int i = 0; i < image_editors.size(); i++ ) {
    if( image_editors[i] )
      delete( image_editors[i] );
  }
  /*
  ProcessRequestInfo request;
  request.request = PF::PROCESSOR_END;
  PF::ImageProcessor::Instance().submit_request( request );	
  */
  //delete pf_image;
}


void PF::MainWindow::make_menus()
{
  //Create actions for menus and toolbars:
  m_refActionGroup = Gtk::ActionGroup::create();

  //File menu:
  m_refActionGroup->add( Gtk::Action::create("FileMenu", "File") );
  m_refActionGroup->add( Gtk::Action::create("FileOpen", Gtk::Stock::OPEN),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_open_clicked) );
  m_refActionGroup->add( Gtk::Action::create("FileSave", Gtk::Stock::SAVE),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_save_clicked) );
  m_refActionGroup->add( Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_saveas_clicked) );
  m_refActionGroup->add( Gtk::Action::create("FileExport", _("Export")),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_export_clicked) );
  m_refActionGroup->add( Gtk::Action::create("FilePrefs", Gtk::Stock::PREFERENCES),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_open_clicked) );
  m_refActionGroup->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_exit) );

  //Layers menu:
  m_refActionGroup->add( Gtk::Action::create("LayerMenu", "Layer") );

  //Help menu:
  m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
  m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
      sigc::mem_fun(*this, &PF::MainWindow::on_button_exit) );

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info =
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='FileMenu'>"
      "      <menuitem action='FileOpen'/>"
      "      <menuitem action='FileSave'/>"
      "      <menuitem action='FileSaveAs'/>"
      "      <menuitem action='FileExport'/>"
      "      <separator/>"
      "      <menuitem action='FilePrefs'/>"
      "      <separator/>"
      "      <menuitem action='FileQuit'/>"
      "    </menu>"
      "    <menu action='LayerMenu'>"
      "    </menu>"
      "    <menu action='HelpMenu'>"
      "      <menuitem action='HelpAbout'/>"
      "    </menu>"
      "  </menubar>"
      "</ui>";

  try
  {
    m_refUIManager->add_ui_from_string(ui_info);
  }
  catch(const Glib::Error& ex)
  {
    std::cerr << "building menus failed: " <<  ex.what();
  }

  //Get the menubar and toolbar widgets, and add them to a container widget:
  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
  Gtk::Widget* pMenuFileQuit = m_refUIManager->get_widget("/MenuBar/FileMenu/FileQuit");
  Gtk::Widget* pMenuFilePrefs = m_refUIManager->get_widget("/MenuBar/FileMenu/FilePrefs");
  Gtk::Widget* pMenuHelpAbout = m_refUIManager->get_widget("/MenuBar/HelpMenu/HelpAbout");
//#ifdef MAC_INTEGRATION
#if defined(__APPLE__)
    GtkosxApplication* osxApp  = (GtkosxApplication *)g_object_new (GTKOSX_TYPE_APPLICATION, NULL);
    gboolean falseval = FALSE;
    gboolean trueval = TRUE;
    //RTWindow *rtWin = this;
    //g_signal_connect (osxApp, "NSApplicationBlockTermination", G_CALLBACK (osx_should_quit_cb), rtWin);
    //g_signal_connect (osxApp, "NSApplicationWillTerminate",  G_CALLBACK (osx_will_quit_cb), rtWin);
    //g_signal_connect (osxApp, "NSApplicationOpenFile", G_CALLBACK (osx_open_file_cb), rtWin);

    //menubar = gtk_menu_bar_new ();
    if(pMenubar) {
      gtkosx_application_set_menu_bar (osxApp, GTK_MENU_SHELL (pMenubar->gobj()));
      //mainBox.pack_start(*pMenubar, Gtk::PACK_SHRINK, 5);
    }
    if( pMenuFileQuit ) {
      //gtk_widget_hide(GTK_WIDGET(pMenuFileQuit->gobj()));
    }
    if( pMenuHelpAbout ) {
      gtkosx_application_insert_app_menu_item  (osxApp, GTK_WIDGET (pMenuHelpAbout->gobj()), 0);
      GtkWidget* sep = gtk_separator_menu_item_new ();
      gtkosx_application_insert_app_menu_item  (osxApp, sep, 1);
    }
    if( pMenuFilePrefs ) {
      gtkosx_application_insert_app_menu_item  (osxApp, GTK_WIDGET (pMenuFilePrefs->gobj()), 2);
    }
    gtkosx_application_set_use_quartz_accelerators (osxApp, TRUE);
    gtkosx_application_ready (osxApp);
#else //MAC_INTEGRATION
    if(pMenubar) {
      mainBox.pack_start(*pMenubar, Gtk::PACK_SHRINK, 5);
    }
#endif //MAC_INTEGRATION

}


void PF::MainWindow::on_button_clicked()
{
  std::cout << "Hello World" << std::endl;
}


void PF::MainWindow::remove_all_tabs()
{
  while( true ) {
    int npages = viewerNotebook.get_n_pages();
    std::cout<<"MainWindow::on_delete_event(): npages="<<npages<<std::endl;
    if( npages == 0 ) break;
    Gtk::Widget* w = viewerNotebook.get_nth_page(npages-1);
    std::cout<<"MainWindow::on_delete_event(): tab="<<npages-1<<"  w="<<w<<std::endl;
    if( !w ) continue;
    std::cout<<"MainWindow::on_delete_event(): removing tab #"<<npages-1<<std::endl;
    remove_tab( w );
  }
}


void PF::MainWindow::on_button_exit()
{
  remove_all_tabs();
  hide();
}


bool PF::MainWindow::on_delete_event( GdkEventAny* event )
{
  remove_all_tabs();
  return false;
}


#define LOAD_PFI

void
PF::MainWindow::open_image( std::string filename )
{
  char* fullpath = strdup( filename.c_str() );
  PF::ImageEditor* editor = new PF::ImageEditor( fullpath );
  image_editors.push_back( editor );

  char* fname = basename( fullpath );

  HTabLabelWidget* tabwidget = 
      new HTabLabelWidget( std::string(fname),
          editor );
  tabwidget->signal_close.connect( sigc::mem_fun(*this, &PF::MainWindow::remove_tab) ); 
  viewerNotebook.append_page( *editor, *tabwidget );
  //std::cout<<"MainWindow::open_image(): notebook page appended"<<std::endl;
  free(fullpath);
  editor->set_tab_label_widget( tabwidget );
  editor->show();
  //std::cout<<"MainWindow::open_image(): editor shown"<<std::endl;
  //editor->open();
  viewerNotebook.set_current_page( -1 );
  //std::cout<<"MainWindow::open_image(): current notebook page selected"<<std::endl;
  if( editor->get_image() && editor->get_image()->is_modified() )
    // To properly update the tab label
    editor->get_image()->modified();
  //getchar();

  /*
  std::vector<VipsImage*> in;

  pf_image = new PF::Image();

  std::string ext;
  if( getFileExtension( "/", filename, ext ) &&
      ext == "pfi" ) {

    PF::load_pf_image( filename, pf_image );
    PF::PhotoFlow::Instance().set_image( pf_image );
    layersWidget.set_image( pf_image );
    pf_image->add_view( VIPS_FORMAT_UCHAR, 0 );

  } else {

    PF::PhotoFlow::Instance().set_image( pf_image );
    layersWidget.set_image( pf_image );

    pf_image->add_view( VIPS_FORMAT_UCHAR, 0 );

    PF::LayerManager& layer_manager = pf_image->get_layer_manager();

    PF::Processor<PF::ImageReaderPar,PF::ImageReader>* imgread = 
      new PF::Processor<PF::ImageReaderPar,PF::ImageReader>();
    imgread->get_par()->set_file_name( filename );

    PF::Layer* limg = layer_manager.new_layer();
    limg->set_processor( imgread );
    limg->set_name( "input image" );

    PF::ImageReadConfigDialog* img_config = 
      new PF::ImageReadConfigDialog( limg );
    imgread->get_par()->set_config_ui( img_config );
    //img_config->set_layer( limg );
    //img_config->set_image( pf_image );

    //layer_manager.get_layers().push_back( limg );
    layersWidget.add_layer( limg );

  }

  imageArea.set_view( pf_image->get_view(0) );
  //pf_image->signal_modified.connect( sigc::mem_fun(&imageArea, &ImageArea::update_image) );
  pf_image->update();
   */


  /*
  //PF::ProcessorBase* invert = 
  //  new PF::Processor<PF::Invert,PF::InvertPar>();

  PF::Processor<PF::BrightnessContrast,PF::BrightnessContrastPar>* bc = 
  new PF::Processor<PF::BrightnessContrast,PF::BrightnessContrastPar>();
  bc->get_par()->set_brightness(0.2);
  //bc->get_par()->set_contrast(0.5);
  bc->get_par()->set_opacity(0.5);
  bc->get_par()->set_blend_mode(PF::PF_BLEND_NORMAL);

  PF::Processor<PF::Gradient,PF::GradientPar>* gradient = 
  new PF::Processor<PF::Gradient,PF::GradientPar>();

  PF::Processor<PF::VipsOperationProc,PF::VipsOperationPar>* vips_op = 
  new PF::Processor<PF::VipsOperationProc,PF::VipsOperationPar>();
  vips_op->get_par()->set_op( "gamma" );

  PF::Layer* lbc = layer_manager.new_layer();
  lbc->set_processor( bc );
  lbc->set_name( "brightness/contrast" );

  PF::BrightnessContrastConfigDialog* bc_config = 
  new PF::BrightnessContrastConfigDialog();
  bc->get_par()->set_config_ui( bc_config );
  bc_config->set_layer( lbc );
  //bc_config->set_image( pf_image );

  PF::Layer* lgrad = layer_manager.new_layer();
  lgrad->set_processor( gradient );
  lgrad->set_name( "vertical gradient" );

  PF::Layer* linv1 = layer_manager.new_layer();
  linv1->set_processor( new PF::Processor<PF::Invert,PF::InvertPar>() );
  linv1->set_name( "invert 1" );

  PF::Layer* lvips = layer_manager.new_layer();
  lvips->set_processor( vips_op );
  lvips->set_name( "VIPS gamma adjustment" );

  PF::VipsOperationConfigDialog* vips_config = 
  new PF::VipsOperationConfigDialog();
  vips_op->get_par()->set_config_ui( vips_config );
  vips_config->set_layer( lvips );
  //vips_config->set_image( pf_image );
  vips_config->set_op( "gamma" );
   */


  //lbc->imap_insert( lgrad );
  //layer_manager.get_layers().push_back( lbc );
  //layer_manager.get_layers().push_back( linv1 );
  //layer_manager.get_layers().push_back( lvips );


  //layer_manager->build_chain( PF::PF_COLORSPACE_RGB, VIPS_FORMAT_UCHAR, 100,100 );

  //layer_manager->rebuild_all( PF::PF_COLORSPACE_RGB, VIPS_FORMAT_UCHAR, 100,100 );
  //update_image();
  /*
    display_image = im_open( "display_image", "p" );

    VipsImage* out = layer_manager->get_output();
    if(out) {
    if (vips_sink_screen (out, display_image, NULL,
    64, 64, (2000/64)*(2000/64), 0, sink_notify, this))
    verror ();


    region = vips_region_new (display_image);
    std::cout<<"Image size: "<<display_image->Xsize<<","<<display_image->Ysize<<std::endl;
    set_size_request (display_image->Xsize, display_image->Ysize);
    }
   */
}


void PF::MainWindow::on_button_open_clicked()
{
  //return;

  Gtk::FileChooserDialog dialog( _("Open image"),
      Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  //Glib::RefPtr<Gtk::FileFilter> filter_pfi = Gtk::FileFilter::create();
  //filter_pfi->set_name("Photoflow files");
  //filter_pfi->add_pattern("*.pfi");
  //dialog.add_filter(filter_pfi);

#ifdef GTKMM_2
  Gtk::FileFilter filter_tiff;
  filter_tiff.set_name( _("Image files") );
  filter_tiff.add_mime_type("image/tiff");
  filter_tiff.add_mime_type("image/jpeg");
  filter_tiff.add_mime_type("image/png");
  filter_tiff.add_mime_type("image/x-3fr");
  filter_tiff.add_mime_type("image/x-adobe-dng");
  filter_tiff.add_mime_type("image/x-arw;image/x-bay");
  filter_tiff.add_mime_type("image/x-canon-cr2");
  filter_tiff.add_mime_type("image/x-canon-crw");
  filter_tiff.add_mime_type("image/x-cap");
  filter_tiff.add_mime_type("image/x-cr2");
  filter_tiff.add_mime_type("image/x-crw");
  filter_tiff.add_mime_type("image/x-dcr");
  filter_tiff.add_mime_type("image/x-dcraw");
  filter_tiff.add_mime_type("image/x-dcs");
  filter_tiff.add_mime_type("image/x-dng");
  filter_tiff.add_mime_type("image/x-drf");
  filter_tiff.add_mime_type("image/x-eip");
  filter_tiff.add_mime_type("image/x-erf");
  filter_tiff.add_mime_type("image/x-fff");
  filter_tiff.add_mime_type("image/x-fuji-raf");
  filter_tiff.add_mime_type("image/x-iiq");
  filter_tiff.add_mime_type("image/x-k25");
  filter_tiff.add_mime_type("image/x-kdc");
  filter_tiff.add_mime_type("image/x-mef");
  filter_tiff.add_mime_type("image/x-minolta-mrw");
  filter_tiff.add_mime_type("image/x-mos");
  filter_tiff.add_mime_type("image/x-mrw");
  filter_tiff.add_mime_type("image/x-nef");
  filter_tiff.add_mime_type("image/x-nikon-nef");
  filter_tiff.add_mime_type("image/x-nrw");
  filter_tiff.add_mime_type("image/x-olympus-orf");
  filter_tiff.add_mime_type("image/x-orf");
  filter_tiff.add_mime_type("image/x-panasonic-raw");
  filter_tiff.add_mime_type("image/x-pef");
  filter_tiff.add_mime_type("image/x-pentax-pef");
  filter_tiff.add_mime_type("image/x-ptx");
  filter_tiff.add_mime_type("image/x-pxn");
  filter_tiff.add_mime_type("image/x-r3d");
  filter_tiff.add_mime_type("image/x-raf");
  filter_tiff.add_mime_type("image/x-raw");
  filter_tiff.add_mime_type("image/x-rw2");
  filter_tiff.add_mime_type("image/x-rwl");
  filter_tiff.add_mime_type("image/x-rwz");
  filter_tiff.add_mime_type("image/x-sigma-x3f");
  filter_tiff.add_mime_type("image/x-sony-arw");
  filter_tiff.add_mime_type("image/x-sony-sr2");
  filter_tiff.add_mime_type("image/x-sony-srf");
  filter_tiff.add_mime_type("image/x-sr2");
  filter_tiff.add_mime_type("image/x-srf");
  filter_tiff.add_mime_type("image/x-x3f");
  filter_tiff.add_pattern("*.pfi");
  Gtk::FileFilter filter_all;
  filter_all.set_name( _("All files") );
  filter_all.add_pattern("*.*");
#endif
#ifdef GTKMM_3
  Glib::RefPtr<Gtk::FileFilter> filter_tiff = Gtk::FileFilter::create();
  filter_tiff->set_name( _("Image files") );
  filter_tiff->add_mime_type("image/tiff");
  filter_tiff->add_mime_type("image/jpeg");
  filter_tiff->add_mime_type("image/png");
  filter_tiff->add_mime_type("image/x-3fr");
  filter_tiff->add_mime_type("image/x-adobe-dng");
  filter_tiff->add_mime_type("image/x-arw;image/x-bay");
  filter_tiff->add_mime_type("image/x-canon-cr2");
  filter_tiff->add_mime_type("image/x-canon-crw");
  filter_tiff->add_mime_type("image/x-cap");
  filter_tiff->add_mime_type("image/x-cr2");
  filter_tiff->add_mime_type("image/x-crw");
  filter_tiff->add_mime_type("image/x-dcr");
  filter_tiff->add_mime_type("image/x-dcraw");
  filter_tiff->add_mime_type("image/x-dcs");
  filter_tiff->add_mime_type("image/x-dng");
  filter_tiff->add_mime_type("image/x-drf");
  filter_tiff->add_mime_type("image/x-eip");
  filter_tiff->add_mime_type("image/x-erf");
  filter_tiff->add_mime_type("image/x-fff");
  filter_tiff->add_mime_type("image/x-fuji-raf");
  filter_tiff->add_mime_type("image/x-iiq");
  filter_tiff->add_mime_type("image/x-k25");
  filter_tiff->add_mime_type("image/x-kdc");
  filter_tiff->add_mime_type("image/x-mef");
  filter_tiff->add_mime_type("image/x-minolta-mrw");
  filter_tiff->add_mime_type("image/x-mos");
  filter_tiff->add_mime_type("image/x-mrw");
  filter_tiff->add_mime_type("image/x-nef");
  filter_tiff->add_mime_type("image/x-nikon-nef");
  filter_tiff->add_mime_type("image/x-nrw");
  filter_tiff->add_mime_type("image/x-olympus-orf");
  filter_tiff->add_mime_type("image/x-orf");
  filter_tiff->add_mime_type("image/x-panasonic-raw");
  filter_tiff->add_mime_type("image/x-pef");
  filter_tiff->add_mime_type("image/x-pentax-pef");
  filter_tiff->add_mime_type("image/x-ptx");
  filter_tiff->add_mime_type("image/x-pxn");
  filter_tiff->add_mime_type("image/x-r3d");
  filter_tiff->add_mime_type("image/x-raf");
  filter_tiff->add_mime_type("image/x-raw");
  filter_tiff->add_mime_type("image/x-rw2");
  filter_tiff->add_mime_type("image/x-rwl");
  filter_tiff->add_mime_type("image/x-rwz");
  filter_tiff->add_mime_type("image/x-sigma-x3f");
  filter_tiff->add_mime_type("image/x-sony-arw");
  filter_tiff->add_mime_type("image/x-sony-sr2");
  filter_tiff->add_mime_type("image/x-sony-srf");
  filter_tiff->add_mime_type("image/x-sr2");
  filter_tiff->add_mime_type("image/x-srf");
  filter_tiff->add_mime_type("image/x-x3f");
  filter_tiff->add_pattern("*.pfi");
  Glib::RefPtr<Gtk::FileFilter> filter_all = Gtk::FileFilter::create();
  filter_all->set_name( _("All files") );
  filter_all->add_pattern("*.*");
#endif
  dialog.add_filter(filter_tiff);
  dialog.add_filter(filter_all);

  if( !last_dir.empty() ) dialog.set_current_folder( last_dir );

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result) {
  case(Gtk::RESPONSE_OK): 
      {
    std::cout << "Open clicked." << std::endl;

    //Notice that this is a std::string, not a Glib::ustring.
    std::string filename = dialog.get_filename();
    last_dir = dialog.get_current_folder();
    std::cout << "File selected: " <<  filename << std::endl;
    char* fullpath = realpath( filename.c_str(), NULL );
    if(!fullpath)
      return;
    open_image( fullpath );
    free(fullpath);
    break;
      }
  case(Gtk::RESPONSE_CANCEL): 
      {
    std::cout << "Cancel clicked." << std::endl;
    break;
      }
  default: 
  {
    std::cout << "Unexpected button clicked." << std::endl;
    break;
  }
  }
}


void PF::MainWindow::on_button_save_clicked()
{
  int page = viewerNotebook.get_current_page();
  Gtk::Widget* widget = viewerNotebook.get_nth_page( page );
  if( widget ) {
    PF::ImageEditor* editor = dynamic_cast<PF::ImageEditor*>( widget );
    if( editor && editor->get_image() ) {
      bool saved = false;
      if( !(editor->get_image()->get_filename().empty()) ) {
        std::string ext;
        PF::get_file_extension( editor->get_image()->get_filename(), ext );
        std::cout<<"ext: "<<ext<<std::endl;
        if( ext == "pfi" ) {
          editor->get_image()->save( editor->get_image()->get_filename() );
          saved = true;
          char* fullpath = strdup( editor->get_image()->get_filename().c_str() );
          char* fname = basename( fullpath );
          HTabLabelWidget* tabwidget = (HTabLabelWidget*)viewerNotebook.get_tab_label( *widget );
          if( tabwidget ) tabwidget->set_label( fname );
        }
      }
      if( !saved )
        on_button_saveas_clicked();
    }
  }
}


void PF::MainWindow::on_button_saveas_clicked()
{
  Gtk::FileChooserDialog dialog( _("Save image as..."),
      Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  int page = viewerNotebook.get_current_page();
  Gtk::Widget* widget = viewerNotebook.get_nth_page( page );
  if( !widget ) return;
  PF::ImageEditor* editor = dynamic_cast<PF::ImageEditor*>( widget );
  if( !editor || !(editor->get_image()) ) return;

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

#ifdef GTKMM_2
  Gtk::FileFilter filter_pfi;
  filter_pfi.set_name( _("Photoflow files") );
  filter_pfi.add_pattern("*.pfi");
#endif
#ifdef GTKMM_3
  Glib::RefPtr<Gtk::FileFilter> filter_pfi = Gtk::FileFilter::create();
  filter_pfi->set_name( _("Photoflow files") );
  filter_pfi->add_pattern("*.pfi");
#endif
  dialog.add_filter(filter_pfi);

  if( !(editor->get_image()->get_filename().empty()) ) {
    std::string ext;
    PF::get_file_extension( editor->get_image()->get_filename(), ext );
    std::cout<<"ext: "<<ext<<std::endl;
    if( ext != "pfi" ) {

      char* fullpath = strdup( editor->get_image()->get_filename().c_str() );
      std::string fname = basename( fullpath );
      char* dname = dirname( fullpath );
      char* fulldir = realpath( dname, NULL );
      if(fulldir) {
        dialog.set_current_folder( fulldir );
        free( fulldir );
      }
      std::string fname_new = PF::replace_file_extension( fname, "pfi" );
      dialog.set_current_name( fname_new.c_str() );
    } else {
      if( !last_dir.empty() ) dialog.set_current_folder( last_dir );
    }
  } else {
    if( !last_dir.empty() ) dialog.set_current_folder( last_dir );
  }
  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result) {
  case(Gtk::RESPONSE_OK): 
      {
    std::cout << "Save clicked." << std::endl;

    //Notice that this is a std::string, not a Glib::ustring.
    last_dir = dialog.get_current_folder();
    std::string filename = dialog.get_filename();
    std::string extension;
    if( get_file_extension(filename, extension) ) {
      if( extension != "pfi" )
        filename += ".pfi";
    }
    std::cout << "File selected: " <<  filename << std::endl;
    editor->get_image()->save( filename );
    char* cfilename = strdup( editor->get_image()->get_filename().c_str() );
    char* fname = basename( cfilename );
    free( cfilename );

    HTabLabelWidget* tabwidget = (HTabLabelWidget*)viewerNotebook.get_tab_label( *widget );
    if( tabwidget ) tabwidget->set_label( fname );

    std::string infoname = editor->get_image()->get_backup_filename();
    infoname += ".info";
    std::ofstream of;
    of.open( infoname.c_str() );
    if( of ) {
      char* fullpath = realpath( editor->get_image()->get_filename().c_str(), NULL );
      if(fullpath) {
        of<<fullpath;
        free( fullpath );
      }
    }
    break;
      }
  case(Gtk::RESPONSE_CANCEL): 
        {
    std::cout << "Cancel clicked." << std::endl;
    break;
        }
  default: 
  {
    std::cout << "Unexpected button clicked." << std::endl;
    break;
  }
  }
}


void PF::MainWindow::on_button_export_clicked()
{
  Gtk::FileChooserDialog dialog( _("Export image as..."),
      Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

#ifdef GTKMM_2
  Gtk::FileFilter filter_tiff;
  filter_tiff.set_name( _("TIFF files") );
  filter_tiff.add_mime_type("image/tiff");
#endif
#ifdef GTKMM_3
  Glib::RefPtr<Gtk::FileFilter> filter_tiff = Gtk::FileFilter::create();
  filter_tiff->set_name( _("TIFF files") );
  filter_tiff->add_mime_type("image/tiff");
#endif
  dialog.add_filter(filter_tiff);

#ifdef GTKMM_2
  Gtk::FileFilter filter_jpeg;
  filter_jpeg.set_name( _("JPEG files") );
  filter_jpeg.add_mime_type("image/jpeg");
#endif
#ifdef GTKMM_3
  Glib::RefPtr<Gtk::FileFilter> filter_jpeg = Gtk::FileFilter::create();
  filter_jpeg->set_name( _("JPEG files") );
  filter_jpeg->add_mime_type("image/jpeg");
#endif
  dialog.add_filter(filter_jpeg);

  if( !last_dir.empty() ) dialog.set_current_folder( last_dir );

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result) {
  case(Gtk::RESPONSE_OK): 
      {
    std::cout << "Export clicked." << std::endl;

    //Notice that this is a std::string, not a Glib::ustring.
    last_dir = dialog.get_current_folder();
    std::string filename = dialog.get_filename();
    std::cout << "File selected: " <<  filename << std::endl;
    int page = viewerNotebook.get_current_page();
    Gtk::Widget* widget = viewerNotebook.get_nth_page( page );
    if( widget ) {
      PF::ImageEditor* editor = dynamic_cast<PF::ImageEditor*>( widget );
      if( editor && editor->get_image() )
        editor->get_image()->export_merged( filename );
    }
    break;
      }
  case(Gtk::RESPONSE_CANCEL): 
      {
    std::cout << "Cancel clicked." << std::endl;
    break;
      }
  default: 
  {
    std::cout << "Unexpected button clicked." << std::endl;
    break;
  }
  }
}



void PF::MainWindow::remove_tab( Gtk::Widget* widget )
{
  //#ifndef NDEBUG
  std::cout<<"PF::MainWindow::remove_tab() called."<<std::endl;
  //#endif
  int page = viewerNotebook.page_num( *widget );
  if( page < 0 ) return;
  if( page >= viewerNotebook.get_n_pages() ) return;

  Gtk::Widget* tabwidget = viewerNotebook.get_tab_label( *widget );

  Gtk::Widget* widget2 = viewerNotebook.get_nth_page( page );
  if( widget != widget2 ) return;

  PF::ImageEditor* editor = dynamic_cast<PF::ImageEditor*>( widget );
  if( !editor ) return;

  g_assert( editor->get_image() != NULL );
  std::cout<<"  editor->get_image()->is_modified(): "<<editor->get_image()->is_modified()<<std::endl;
  if( editor->get_image()->is_modified() ) {
    char* fullpath = strdup( editor->get_image()->get_filename().c_str() );
    char* fname = basename( fullpath );
    //Glib::ustring msg = _("Image \"");
    //msg += fname;
    //msg += "\" contains unsaved data.\nDo you want to save it before closing?";
    char tstr[501];
    snprintf( tstr, 500, _("Image \"%s\" contains unsaved data. Do you want to save it before closing?"),
        fname);
    Gtk::MessageDialog dialog(tstr,
        false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
    dialog.set_transient_for(*this);
    dialog.set_default_response( Gtk::RESPONSE_YES );

    //Show the dialog and wait for a user response:
    int result = dialog.run();

    //Handle the response:
    switch(result) {
    case Gtk::RESPONSE_YES:
      std::cout<<"PF::MainWindow::remove_tab(): response=YES"<<std::endl;
      on_button_save_clicked();
      break;
    case Gtk::RESPONSE_NO:
      std::cout<<"PF::MainWindow::remove_tab(): response=NO"<<std::endl;
      break;
    default:
      break;
    }

  }

  std::string bckname = editor->get_image()->get_backup_filename();
  unlink( bckname.c_str() );
  bckname += ".info";
  unlink( bckname.c_str() );

  if( PF::PhotoFlow::Instance().get_active_image() == editor->get_image() ) {
    PF::PhotoFlow::Instance().set_active_image( NULL );
    if( editor->get_image() ) {
      // Make sure that aching of current image is stopped
      PF::Pipeline* pipeline = editor->get_image()->get_pipeline( 0 );
      if( pipeline ) {
        editor->get_image()->update( pipeline, true );
      }
    }
  }

  for( unsigned int i = 0; i < image_editors.size(); i++ ) {
    if( image_editors[i] != widget ) continue;
    image_editors.erase( image_editors.begin()+i );
    break;
  }

  viewerNotebook.remove_page( page );
  widget->hide();
  //delete( widget );
  //if( tabwidget )
  //  delete( tabwidget );

  WidgetDestroyData * update = g_new (WidgetDestroyData, 1);
  update->widget = widget;
  g_idle_add ((GSourceFunc) widget_destroy_cb, update);

  update = g_new (WidgetDestroyData, 1);
  update->widget = tabwidget;
  g_idle_add ((GSourceFunc) widget_destroy_cb, update);

//#ifndef NDEBUG
  std::cout<<"PF::MainWindow::remove_tab() page #"<<page<<" removed."<<std::endl;
//#endif
}


void PF::MainWindow::on_my_switch_page(
#ifdef GTKMM_2
    GtkNotebookPage* 	page,
#endif
#ifdef GTKMM_3
    Widget* page,
#endif
    guint page_num)
{
  Gtk::Widget* widget = viewerNotebook.get_nth_page( page_num );
  if( widget ) {
    PF::ImageEditor* editor = dynamic_cast<PF::ImageEditor*>( widget );
    if( editor && editor->get_image() ) {
      PF::PhotoFlow::Instance().set_active_image( editor->get_image() );
      //std::cout<<"MainWindow: image #"<<page_num<<" activated"<<std::endl;
    }
  }
}
