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
    // init
    m_playing = false;
    m_synced = false;
    m_bpm = 120;
    m_swing = 0.;
    m_autovar_generate = false;
    m_autovar_set = false;
    m_xorvar_ratio = 0.;
    m_jakvar_ratio = 0.;
    
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
    gui->addPanel("", mainPanel, HGUI_TOP_LEFT, 0, 0, panelW, 160, true);

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
    ctrl_play->setColor(ON_MAIN_COLOR);
    // store
    m_ui_elements["play_ctrl"] = ctrl_play;
    // event listener
    evts->addListener("onPlay", this, &testApp::el_onPlay);
    ctrl_play->setMessage("testApp.onPlay");
    hLabel * label_start = gui->addLabel("", panel_transport, HGUI_RIGHT, gui->margin3 ,0, "play");
    
    hCheckBox* ctrl_sync = gui->addCheckBox("sync_ctrl", panel_transport, HGUI_NEXT_ROW, gui->margin3, gui->margin3);
    ctrl_sync->setSelected(false);
    ctrl_sync->setBoolVar(&m_synced);
    ctrl_sync->setColor(ON_MAIN_COLOR);
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
    slider_bpm->setColor(ON_MAIN_COLOR);
    
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
    slider_swing->setColor(ON_MAIN_COLOR);
    hLabel * moving_label_swing = gui->addLabel("", panel_groove_var, HGUI_RIGHT, 0, 0, "");
    slider_swing->setLinkedLabel(moving_label_swing, true);
    hLabel * label_swing = gui->addLabel("", panel_groove_var, HGUI_RIGHT, gui->margin3 ,0, "SWING");
    
    evts->addListener("onSwing", this, &testApp::el_onSwing);
    slider_swing->setMessage("testApp.onSwing");
    
    Euclid::gen_permuted_intervals(16, 8, 0.8);
    
    /*
    vector<int> test = Euclid::non_even_ivals(32, 8, 0.66);
    string res = "[";
    for(vector<int>::iterator t = test.begin(); t != test.end(); ++t)
    {
        res += ofToString(*t);
        if((t-test.begin()) < test.size()-1)
        {
            res +=  ", ";
        }
    }
    res += "]";
    ofLog(OF_LOG_NOTICE, res);
    */
    //vector<int> test = Gaia::str_to_vel("fffffff000000000");
    //vector<int> test = Gaia::str_to_vel("ff00f00000000000");
    //sofLog(OF_LOG_NOTICE, ofToString(Euclid::evenness(test)));
    
    /*
    
    vector<bool> testb = Euclid::bjorklund(16, 4);
    string res = "[";
    for(vector<bool>::iterator t = testb.begin(); t != testb.end(); ++t)
    {
        res += ofToString(*t);
        if((t-testb.begin()) < testb.size()-1)
        {
            res +=  ", ";
        }
    }
    
    res += "]";
    ofLog(OF_LOG_NOTICE, res);
     */
    //ofLog(OF_LOG_NOTICE, ofToString(Euclid::evenness(testb)));
    /*int st = ofGetElapsedTimeMicros();
    vector<int> a = Euclid::gen_intervals(32, 8, 0.95);
    vector<int> b = Euclid::gen_intervals(32, 8, 0.95);
    vector<int> AB;
    AB.reserve( a.size() + b.size() ); // preallocate memory
    AB.insert( AB.end(), a.begin(), a.end() );
    AB.insert( AB.end(), b.begin(), b.end() );
    
    ofLog(OF_LOG_NOTICE, "time " + ofToString(ofGetElapsedTimeMicros() - st));
    ofLog(OF_LOG_NOTICE, "eveness " + ofToString(Euclid::evenness(AB, 64)));
     */
    /*
    vector<int> prefix;
    vector< vector<int> > res;
    int st = ofGetElapsedTimeMicros();
    Euclid::asc_partitions(1, 48, 16, res, prefix);
    ofLog(OF_LOG_NOTICE, ofToString(ofGetElapsedTimeMicros() - st));
     
    
    for(vector< vector<int> >::iterator it = res.begin(); it != res.end(); ++it)
    {
        string line = "[";
        for(vector<int>::iterator itt = it->begin(); itt != it->end(); ++itt)
        {
            line += ofToString(*itt) + " ";
        }
        line += "] : " + ofToString(accumulate(it->begin(), it->end(), 0));
        ofLog(OF_LOG_NOTICE, line);
    }
    ofLog(OF_LOG_NOTICE, ofToString(res.size()));
    */
    /*
    for(int x = 0; x < 100; ++x)
    {
        
    
    vector<int> testb = Euclid::rnd_parts(60,14);//Euclid::generate_velocities(240,7);
    string res = "[";
    for(vector<int>::iterator t = testb.begin(); t != testb.end(); ++t)
    {
        res += ofToString(*t);
        if((t-testb.begin()) < testb.size()-1)
        {
            res +=  ", ";
        }
    }
    res += "]" + ofToString(accumulate(testb.begin(), testb.end(), 0));
    ofLog(OF_LOG_NOTICE, res);
    ofLog(OF_LOG_NOTICE, "den "+ofToString(Euclid::density(testb)));
        
    }
     */
}

//--------------------------------------------------------------
void testApp::update()
{

}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(220, 220, 220);
    ofSetColor(0, 0, 0);
    m_font->drawString("ANCIENT SEQ", 10, 25);
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
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

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