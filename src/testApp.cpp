#include "testApp.h"
#include "hGui_all.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetFrameRate(60);
    // title
    ofSetWindowTitle("ancient sequencer II");
  	m_font = new ofTrueTypeFont;
	m_font->loadFont("fonts/lmmonolt10-bold.otf", 12, true, true);
    m_big_font = new ofTrueTypeFont;
    m_big_font->loadFont("fonts/lmmonolt10-bold.otf", 28, true, true);
    // init
    m_playing = false;
    m_synced = false;
    m_bpm = 120;
    m_swing = 0.;
    m_xorvar_ratio = 0.;
    m_jakvar_ratio = 0.;
    m_var_hold = false;
    m_selected_track = 0;
    
    // fake conf
    m_conf = ConfTrack();
    m_conf.track_id = 0;
    m_conf.track_onsets = 0;
    m_conf.track_rotation = 0.;
    m_conf.track_size = 16;
    m_conf.track_evenness = 1.;
    m_conf.velocity_max = 13;
    m_conf.velocity_min = 0;
    m_conf.euclid_bias = 0.;
    m_conf.euclid_density = 0.5;
    m_conf.euclid_permutation = 0.;
    m_conf.euclid_evolution_rate = 1.;
    m_conf.euclid_permutation_rate = 1.;
    
    
    // gui
    hGui * gui = hGui::getInstance();
    
    // create and initialize the events engine.
	hEvents * evts = hEvents::getInstance();
	evts->setup();
	evts->addObject("testApp", this);
    
    gui->setup("fonts/lmmonolt10-bold.otf", 7);
    
    hPanel * mainPanel =
    gui->addPanel("mainPanel", NULL, HGUI_ABSOLUTE_POSITION, gui->margin1, gui->margin1*3,
                  ofGetWidth()-gui->margin1*2, ofGetHeight()-((gui->margin1*2)*2), false);
    
    gui->setRootWidget(mainPanel);
    //mainPanel->setVisibleBackground(true);
    gui->addListeners();
    
    int panelW = ofGetWidth() - gui->margin1*2;
    
    hPanel * panel_global =
    gui->addPanel("", mainPanel, HGUI_TOP_LEFT, 0, 0, panelW, 116, true);

    hPanel * panel_transport = gui->addPanel("", panel_global, HGUI_TOP_LEFT, gui->margin1, gui->margin1, panelW/4 - gui->margin1, panel_global->getHeight()-gui->margin1*2, true);
    hLabel * label_transport = gui->addLabel("", panel_transport, HGUI_TOP_LEFT, 2, 0, "TRANSPORT");
    
    hPanel * panel_groove_var = gui->addPanel("", panel_global, HGUI_NEXT_COL, gui->margin1, gui->margin1, panelW/2.71 - gui->margin1, panel_global->getHeight()-gui->margin1*2, true);
    hLabel * label_groove_var = gui->addLabel("", panel_groove_var, HGUI_TOP_LEFT, 2, 0, "GROOVE & FX");
    
    hPanel * panel_library = gui->addPanel("", panel_global, HGUI_NEXT_COL, gui->margin1, gui->margin1, panelW/2.71 - gui->margin1, panel_global->getHeight()-gui->margin1*2, true);
    hLabel * label_library = gui->addLabel("", panel_library, HGUI_TOP_LEFT, 2, 0, "LIBRARY");
    
    // transport buttons
    hCheckBox* ctrl_play = gui->addCheckBox("play_ctrl", panel_transport, HGUI_TOP_LEFT, gui->margin3, gui->margin1*2);
    ctrl_play->setSelected(false);
    ctrl_play->setBoolVar(&m_playing);
    
    // store
    m_ui_elements["play_ctrl"] = ctrl_play;
    // event listener
    evts->addListener("onPlay", this, &testApp::el_onPlay);
    ctrl_play->setMessage("testApp.onPlay");
    hLabel * label_start = gui->addLabel("", panel_transport, HGUI_RIGHT, gui->margin3 ,0, "play");
    
    hCheckBox* ctrl_sync = gui->addCheckBox("sync_ctrl", panel_transport, HGUI_NEXT_ROW, gui->margin3, gui->margin3);
    ctrl_sync->setSelected(false);
    ctrl_sync->setBoolVar(&m_synced);

    hLabel * label_sync = gui->addLabel("", panel_transport, HGUI_RIGHT, gui->margin3 ,0, "sync");
    // store
    m_ui_elements["sync_ctrl"] = ctrl_sync;
    // event listener
    evts->addListener("onSync", this, &testApp::el_onSync);
    ctrl_sync->setMessage("testApp.onSync");
    
    // bpm
    hSlider* slider_bpm = gui->addSlider("bpm_slider", panel_transport, HGUI_NEXT_ROW, gui->margin3, gui->margin3, 100);
    slider_bpm->setRange(30, 250);
    slider_bpm->setIntVar(&m_bpm);
    
    hNumberBox * number_bpm = gui->addNumberBox("bpm_number", panel_transport, HGUI_RIGHT, gui->margin3, 0, 20, "120");
    number_bpm->setRange(30, 250);
    number_bpm->setValue(120);
    slider_bpm->setLinkedNumberBox(number_bpm);
    number_bpm->setLinkedSlider(slider_bpm);
    hLabel * label_bpm = gui->addLabel("", panel_transport, HGUI_RIGHT, gui->margin3 ,0, "BPM");
    
    evts->addListener("onBpm", this, &testApp::el_onBpm);
    slider_bpm->setMessage("testApp.onBpm");
    number_bpm->setMessage("testApp.onBpm");
    
    // midi delay
    hSlider* slider_mid_delay = gui->addSlider("mid_delay_slider", panel_transport, HGUI_NEXT_ROW, gui->margin3, gui->margin3, 100);
    slider_mid_delay->setRange(-24, 24);
    slider_mid_delay->setIntVar(&m_mid_delay);
    slider_mid_delay->setColor(ON_GREY_COLOR);
    
    hNumberBox * number_mid_delay = gui->addNumberBox("mid_delay_number", panel_transport, HGUI_RIGHT, gui->margin3, 0, 20, "0");
    number_mid_delay->setRange(-24, 24);
    number_mid_delay->setValue(0);
    slider_mid_delay->setLinkedNumberBox(number_mid_delay);
    number_mid_delay->setLinkedSlider(slider_mid_delay);
    hLabel * label_mid_delay = gui->addLabel("", panel_transport, HGUI_RIGHT, gui->margin3 ,0, "DELAY");
    
    evts->addListener("onMidDelay", this, &testApp::el_onMidDelay);
    slider_mid_delay->setMessage("testApp.onMidDelay");
    number_mid_delay->setMessage("testApp.onMidDelay");
    
    // swing
    hButton* button_reset_swing = gui->addButton("reset_swing_button", panel_groove_var, HGUI_TOP_LEFT, gui->margin3, gui->margin1*2, 30, "reset");
    hButton* button_random_swing = gui->addButton("random_swing_button", panel_groove_var, HGUI_NEXT_COL, gui->margin3, gui->margin1*2, 30, "randm");
    hSlider* slider_swing = gui->addSlider("swing_slider", panel_groove_var, HGUI_RIGHT, gui->margin3, 0, 100);
    button_reset_swing->setHeight(slider_swing->getHeight());
    button_random_swing->setHeight(slider_swing->getHeight());
    slider_swing->setRange(-0.9, 0.9,2);
    slider_swing->setFloatVar(&m_swing);
    
    hLabel * label_swing = gui->addLabel("", panel_groove_var, HGUI_RIGHT, gui->margin3 ,0, "SWING");
    
    evts->addListener("onSwing", this, &testApp::el_onSwing);
    slider_swing->setMessage("testApp.onSwing");
    
    // variations
    
    // XOR AND JAK // /// /./
    hButton* button_smooth_xor = gui->addButton("smooth_xor_button", panel_groove_var, HGUI_NEXT_ROW, gui->margin3, gui->margin2, 30, "Sxor");
    button_smooth_xor->setHeight(slider_swing->getHeight());
    hButton* button_hard_xor = gui->addButton("hard_xor_button", panel_groove_var, HGUI_RIGHT, gui->margin3, 0, 30, "Hxor");
    button_hard_xor->setHeight(slider_swing->getHeight());
    hButton* button_smooth_jak = gui->addButton("smooth_jak_button", panel_groove_var, HGUI_NEXT_ROW, gui->margin3, gui->margin2, 30, "Sjak");
    button_smooth_jak->setHeight(slider_swing->getHeight());
    hButton* button_hard_jak = gui->addButton("hard_jak_button", panel_groove_var, HGUI_RIGHT, gui->margin3, 0, 30, "Hjak");
    button_hard_jak->setHeight(slider_swing->getHeight());
    m_ui_elements["smooth_xor_ctrl"] = button_smooth_xor;
    m_ui_elements["hard_xor_ctrl"] = button_hard_xor;
    m_ui_elements["smooth_jak_ctrl"] = button_smooth_jak;
    m_ui_elements["hard_jak_ctrl"] = button_hard_jak;
    evts->addListener("onSxor", this, &testApp::el_onSxor);
    evts->addListener("onHxor", this, &testApp::el_onHxor);
    evts->addListener("onSjak", this, &testApp::el_onSjak);
    evts->addListener("onHjak", this, &testApp::el_onHjak);
    button_smooth_xor->setMessage("testApp.onSxor");
    button_hard_xor->setMessage("testApp.onHxor");
    button_smooth_jak->setMessage("testApp.onSjak");
    button_hard_jak->setMessage("testApp.onHjak");
    
    // HOLD // /// /./
    hCheckBox* ctrl_var_hold = gui->addCheckBox("var_hold_ctrl", panel_groove_var, HGUI_RIGHT, gui->margin3, 0);
    ctrl_var_hold->setSelected(false);
    ctrl_var_hold->setBoolVar(&m_var_hold);
    m_ui_elements["var_hold_ctrl"] = ctrl_var_hold;
    evts->addListener("onVarHold", this, &testApp::el_onVarHold);
    ctrl_var_hold->setMessage("testApp.onVarHold");
    hLabel * label_hold = gui->addLabel("", panel_groove_var, HGUI_RIGHT, gui->margin3 ,0, "hold");
    
    // GENERATE
    hPanel * panel_generate =
    gui->addPanel("", mainPanel, HGUI_TOP_LEFT, 0, panel_global->getHeight() + (gui->margin1*2), panelW, 137, true);
    
    hPanel * panel_euclid = gui->addPanel("", panel_generate, HGUI_TOP_LEFT, gui->margin1, gui->margin1, panelW/4 - gui->margin1, panel_generate->getHeight()-gui->margin1*2, true);
    hLabel * label_euclid = gui->addLabel("", panel_euclid, HGUI_TOP_LEFT, 2, 0, "EUCLIDEAN");
    
    hSlider* slider_evenness = gui->addSlider("evenness_slider", panel_euclid, HGUI_TOP_LEFT, gui->margin3, gui->margin1*2, 100);
    slider_evenness->setRange(0., 1., 2. );
    slider_evenness->setFloatVar(&m_conf.track_evenness);
    hLabel * label_evenness = gui->addLabel("", panel_euclid, HGUI_RIGHT, gui->margin3 ,0, "evenness");
    
    hSlider* slider_onsets = gui->addSlider("onsets_slider", panel_euclid, HGUI_NEXT_ROW, gui->margin3, gui->margin3, 100);
    slider_onsets->setRange(0, 64);
    slider_onsets->setIntVar(&m_conf.track_onsets);
    hLabel * label_onsets = gui->addLabel("", panel_euclid, HGUI_RIGHT, gui->margin3 ,0, "onsets");
    m_ui_elements["onsets_ctrl"] = slider_onsets;
    
    hSlider* slider_size = gui->addSlider("size_slider", panel_euclid, HGUI_NEXT_ROW, gui->margin3,  gui->margin3, 100);
    slider_size->setRange(0, 64);
    slider_size->setIntVar(&m_conf.track_size);
    hLabel * label_size = gui->addLabel("", panel_euclid, HGUI_RIGHT, gui->margin3 ,0, "size");
    m_ui_elements["onsets_size"] = slider_size;
    
    hSlider* slider_rotation = gui->addSlider("rotation_slider", panel_euclid, HGUI_NEXT_ROW, gui->margin3,  gui->margin3, 100);
    slider_rotation->setRange(0., 1., 2);
    slider_rotation->setFloatVar(&m_conf.track_rotation);
    hLabel * label_rotation = gui->addLabel("", panel_euclid, HGUI_RIGHT, gui->margin3 ,0, "rotation");
    
    hSlider* slider_bias = gui->addSlider("bias_slider", panel_euclid, HGUI_NEXT_ROW, gui->margin3,  gui->margin3, 100);
    slider_bias->setRange(0., 1., 2);
    slider_bias->setFloatVar(&m_conf.euclid_bias);
    hLabel * label_bias = gui->addLabel("", panel_euclid, HGUI_RIGHT, gui->margin3 ,0, "bias");
    
    evts->addListener("onGenerate", this, &testApp::el_onGenerate);
    
    slider_evenness->setMessage("testApp.onGenerate");
    slider_onsets->setMessage("testApp.onGenerate");
    slider_size->setMessage("testApp.onGenerate");
    slider_rotation->setMessage("testApp.onGenerate");
    slider_bias->setMessage("testApp.onGenerate");
    
    hPanel * panel_velocity = gui->addPanel("", panel_generate, HGUI_NEXT_COL, gui->margin1, gui->margin1, panelW/4 - gui->margin1, panel_generate->getHeight()-gui->margin1*2, true);
    hLabel * label_velocity = gui->addLabel("", panel_velocity, HGUI_TOP_LEFT, 2, 0, "VELOCITY");
    
    
    // VELO
    
    hSlider* slider_minvel = gui->addSlider("minvel_slider", panel_velocity, HGUI_TOP_LEFT, gui->margin3, gui->margin1*2, 100);
    slider_minvel->setRange(0, 15);
    slider_minvel->setIntVar(&m_conf.velocity_min);
    hLabel * label_minvel = gui->addLabel("", panel_velocity, HGUI_RIGHT, gui->margin3 ,0, "minimum");
    
    hSlider* slider_maxvel = gui->addSlider("maxvel_slider", panel_velocity, HGUI_NEXT_ROW, gui->margin3, gui->margin3, 100);
    slider_maxvel->setRange(0, 15);
    slider_maxvel->setIntVar(&m_conf.velocity_max);
    hLabel * label_maxvel = gui->addLabel("", panel_velocity, HGUI_RIGHT, gui->margin3 ,0, "maximum");
    
    
    evts->addListener("onVelocity", this, &testApp::el_onVelocities);
    slider_minvel->setMessage("testApp.onVelocity");
    slider_maxvel->setMessage("testApp.onVelocity");
    
    hButton* button_flat_vel = gui->addButton("button_flat_vel", panel_velocity, HGUI_NEXT_ROW, gui->margin3, gui->margin2, 30, "FLAT");
    button_flat_vel->setHeight(slider_swing->getHeight());
    hButton* button_line_vel = gui->addButton("button_line_vel", panel_velocity, HGUI_RIGHT, gui->margin3, 0, 30, "LINE");
    button_line_vel->setHeight(slider_swing->getHeight());
    hButton* button_sine_vel = gui->addButton("button_sine_vel", panel_velocity, HGUI_RIGHT, gui->margin3, 0, 30, "SINE");
    button_sine_vel->setHeight(slider_swing->getHeight());
    hButton* button_saw_vel = gui->addButton("button_saw_vel", panel_velocity, HGUI_RIGHT, gui->margin3, 0, 30, "SAW");
    button_saw_vel->setHeight(slider_swing->getHeight());
    hButton* button_rnd_vel = gui->addButton("button_rnd_vel", panel_velocity, HGUI_RIGHT, gui->margin3, 0, 30, "RAND");
    button_rnd_vel->setHeight(slider_swing->getHeight());
    
    evts->addListener("onVelFlat", this, &testApp::el_onVelFlat);
    button_flat_vel->setMessage("onVelFlat");
    evts->addListener("onVelLine", this, &testApp::el_onVelLine);
    button_line_vel->setMessage("onVelLine");
    evts->addListener("onVelSine", this, &testApp::el_onVelSine);
    button_sine_vel->setMessage("onVelSine");
    evts->addListener("onVelSaw", this, &testApp::el_onVelSaw);
    button_saw_vel->setMessage("onVelSaw");
    evts->addListener("onVelRnd", this, &testApp::el_onVelRnd);
    button_rnd_vel->setMessage("onVelRnd");
    
    
    evts->addListener("onMute", this, &testApp::el_onMute);
    /****
     *  MIXER GUI
     **/
    for(int i = 0; i < 8; ++i)
    {
        m_mutes[i] = true;
        int ww = panelW - (gui->margin1*7) ;
        int space = gui->margin1 + 1;
        hPanel * pane =
        gui->addPanel("", mainPanel, HGUI_TOP_LEFT, ((ww/8)+space)*i, mainPanel->getHeight() - 115, ww/8 - 1, 115, true);
        hCheckBox* ctrl_mute = gui->addCheckBox("mute_"+ofToString(i)+"_ctrl", pane, HGUI_TOP_LEFT, gui->margin3, gui->margin3);
        ctrl_mute->setWidth(ww/16);
        ctrl_mute->setHeight(ww/16);
        ctrl_mute->setColor(ON_SECOND_COLOR);
        ctrl_mute->setBoolVar(&m_mutes[i]);
        m_ui_elements["mute_"+ofToString(i)+"_panel"] = pane;
        
        if(m_selected_track == i)
        {
            pane->setBackgroundColor(ON_VAR_COLOR);
        }
    }
    
    //hPanel * panel_mixer =
    //gui->addPanel("", mainPanel, HGUI_TOP_LEFT, 0, mainPanel->getHeight() - 116, panelW, 116, true);
    
    // color shit
    gui->checkBoxColor = ON_MAIN_COLOR;
    gui->sliderColor = ON_SECOND_COLOR;
    gui->editBackColor = ON_GREY_COLOR;
    gui->editTextColor = ON_MAIN_COLOR;
}

//--------------------------------------------------------------
void testApp::update()
{
    
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(240, 240, 240);
    ofSetColor(0, 0, 0);
    m_font->drawString("ANCIENT SEQ", 10, 25);
    m_font->drawString("TRACK "+ofToString(m_selected_track+1), 10, 161);
    
    drawTracks();
    
}

// EVENT LISTENERS
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::el_onPlay(hEventArgs &args)
{
    if(args.values.size() > 0)
    {
        
    }
}

void testApp::el_onSync(hEventArgs &args)
{
    if(args.values.size() > 0)
    {
        if(m_synced)
        {
            m_ui_elements["play_ctrl"]->setSelectable(false);
            m_ui_elements["play_ctrl"]->setVisibleBorder(false);
            m_playing = false; // linked
        }
        else
        {
            m_ui_elements["play_ctrl"]->setSelectable(true);
            m_ui_elements["play_ctrl"]->setVisibleBorder(true);
        }
    }
}

void testApp::el_onBpm(hEventArgs &args)
{
    if(args.values.size() > 0)
    {
        //ofLog(OF_LOG_NOTICE, "on BPM " + ofToString(m_bpm));
    }
}

void testApp::el_onMidDelay(hEventArgs &args)
{
    if(args.values.size() > 0)
    {
        
    }
}

void testApp::el_onSwing(hEventArgs &args)
{
    if(args.values.size() > 0)
    {
        
    }
}

void testApp::el_onVarHold(hEventArgs &args)
{
    if(args.values.size() > 0)
    {

    }
}

void testApp::el_onSxor(hEventArgs& args)
{
    if(args.values.size() > 0)
    {
        //cout << "LOL" << endl;
    }
}
void testApp::el_onHxor(hEventArgs& args)
{
    if(args.values.size() > 0)
    {
        //cout << "LOL" << endl;
    }
}
void testApp::el_onSjak(hEventArgs& args)
{
    if(args.values.size() > 0)
    {
        //cout << "LOL" << endl;
    }
}
void testApp::el_onHjak(hEventArgs& args)
{
    if(args.values.size() > 0)
    {
        //cout << "LOL" << endl;
    }
}

void testApp::el_onGenerate(hEventArgs& args)
{
    if(args.values.size() > 0)
    {
        if(m_conf.track_onsets > m_conf.track_size)
        {
            m_conf.track_size = m_conf.track_onsets;
        }
    }
}

void testApp::el_onVelocities(hEventArgs& args)
{
    if(args.values.size() > 0)
    {
        if(m_conf.velocity_min > m_conf.velocity_max)
        {
            m_conf.velocity_max = m_conf.velocity_min;
        }
    }
}

void testApp::el_onVelFlat(hEventArgs& args)
{
    m_conf.velocity_mode = Euclid::VEL_STATIC;
}

void testApp::el_onVelLine(hEventArgs& args)
{
    m_conf.velocity_mode = Euclid::VEL_LINE;
}

void testApp::el_onVelSine(hEventArgs& args)
{
    m_conf.velocity_mode = Euclid::VEL_SINE;
}

void testApp::el_onVelSaw(hEventArgs& args)
{
    m_conf.velocity_mode = Euclid::VEL_RAMP;
}

void testApp::el_onVelRnd(hEventArgs& args)
{
    m_conf.velocity_mode = Euclid::VEL_RAND;
}

void testApp::el_onMute(hEventArgs& args)
{
    map<int,bool>::iterator mute;
    for(mute = m_mutes.begin(); mute != m_mutes.end(); ++mute)
    {
        
    }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    hButton * b;
    switch (key)
    {
        case OF_KEY_RIGHT:
            if(m_selected_track < 7)
            {
                m_selected_track++;
                update_selection();
            }
            break;
        case OF_KEY_LEFT:
            if(m_selected_track)
            {
                m_selected_track--;
                update_selection();
            }
            break;
        case 32: // space bar for play
            if(!m_synced)
            {
                m_playing = !m_playing;
            }
            break;
            
        case 45: // minus
            b = (hButton*)m_ui_elements["smooth_xor_ctrl"];
            b->setPressed(true);
            b->bang();
            break;
            
        case 61: // equal
            b = (hButton*)m_ui_elements["hard_xor_ctrl"];
            b->setPressed(true);
            b->bang();
            break;
            
        case 91: // left square bracket
            b = (hButton*)m_ui_elements["smooth_jak_ctrl"];
            b->setPressed(true);
            b->bang();
            break;
            
        case 93: // right square bracket
            b = (hButton*)m_ui_elements["hard_jak_ctrl"];
            b->setPressed(true);
            b->bang();
            break;
            
        default:
            break;
    }
    
    // mutes
    if(key >= 49 && key <= 58 )
    {
        int idx = key - 49;
        if(m_modifiers.getAppleCommandModifier())
        {
            m_selected_track = idx;
            update_selection();
        }
        else
        {
            m_mutes[idx] = !m_mutes[idx];
        }
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    hButton * b;
    switch (key)
    {
        case 45: // minus
            b = (hButton*)m_ui_elements["smooth_xor_ctrl"];
            b->setPressed(false);
            break;
            
        case 61: // equal
            b = (hButton*)m_ui_elements["hard_xor_ctrl"];
            b->setPressed(false);
            break;
            
        case 91: // left square bracket
            b = (hButton*)m_ui_elements["smooth_jak_ctrl"];
            b->setPressed(false);
            break;
            
        case 93: // right square bracket
            b = (hButton*)m_ui_elements["hard_jak_ctrl"];
            b->setPressed(false);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::drawTracks()
{
    // UI
    for(int i = 0; i < 8; ++i)
    {
        hWidget * pane = m_ui_elements["mute_"+ofToString(i)+"_panel"];
        int yy = pane->getY() - 332;
        int hh = 323;
        if(m_selected_track == i)
        {
            ofSetHexColor(ON_VAR_COLOR);
        }
        else
        {
            ofSetHexColor(ON_GREY_COLOR);
        }
        ofFill();
        ofRect(pane->getX(), yy, pane->getWidth(), hh);
        
        for(int j = 0; j < 64; ++j)
        {
            // back
            if(j % 16 == 0) { ofSetHexColor(0xFFFFFF); }
            else if(j % 4 == 0) { ofSetHexColor(0xEAEAEA); }
            else { ofSetHexColor(0xE1E1E1); }
            ofRect(pane->getX()+2, yy + (5*j) + 2, pane->getWidth()-10, 4);
            ofSetHexColor(0xFFFFFF);
            ofRect(pane->getX()+pane->getWidth()-6, yy + (5*j) + 2, 4, 4);
        }
    }
}

void testApp::update_selection()
{
    // UI
    for(int i = 0; i < 8; ++i)
    {
        hWidget * pane = m_ui_elements["mute_"+ofToString(i)+"_panel"];
        
        if(m_selected_track == i)
        {
            pane->setBackgroundColor(ON_VAR_COLOR);
        }
        else
        {
            pane->setVisibleBackground(false);
        }
        
    }
    
}