#pragma once

#include "ofMain.h"

// gui
#include "hEvents.h"
#include "hObject.h"
#include "hWidget.h"
#include "hGui_all.h"
#include "VRand.h"

// test
#include "ofxXmlSettings.h"
#include "PennerEasing/Sine.h"
#include "Ancient.h"
#include "Seq.h"
#include "Gaia.h"
#include "Euclid.h"
#include "ConfTrack.h"
#include "KeyModifier.h"

class testApp : public ofBaseApp, public hObject {

	public:
		void setup();
		void update();
		void draw();

        void keyPressed  (int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void exit();
    
        // event listeners
        void el_onPlay(hEventArgs& args);
        void el_onSync(hEventArgs& args);
        void el_onBpm(hEventArgs& args);
        void el_onMidDelay(hEventArgs& args);
        void el_onSwing(hEventArgs& args);
        // vars
        void el_onVarHold(hEventArgs& args);
        void el_onSxor(hEventArgs& args);
        void el_onHxor(hEventArgs& args);
        void el_onSjak(hEventArgs& args);
        void el_onHjak(hEventArgs& args);
        // generation
        void el_onGenerate(hEventArgs& args);
        void el_onVelocities(hEventArgs& args);
        //vels
        void el_onVelFlat(hEventArgs& args);
        void el_onVelLine(hEventArgs& args);
        void el_onVelSine(hEventArgs& args);
        void el_onVelSaw(hEventArgs& args);
        void el_onVelRnd(hEventArgs& args);
    
        void el_onMute(hEventArgs& args);
    
        void el_onEvolve(hEventArgs& args);
        void el_onPresetName(hEventArgs& args);
        void el_onSave(hEventArgs& args);
        void el_onLoad(hEventArgs& args);
        void el_onA4(hEventArgs& args);
    
        // constants
        static int const ON_VAR_COLOR = 0xAAAAAA;//
        static int const ON_SECOND_COLOR = 0xFFA340;
        static int const ON_GREY_COLOR = 0xDDDDDD;
        static int const ON_MAIN_COLOR = 0x000000;
        static int const ON_DARK_COLOR = 0x333333;
        static int const ON_A4_COLOR = 0x69B1FF;
    
    protected:
        // sequencer
        Seq m_seq;
        Ancient m_ancient;
    
        ofTrueTypeFont * m_font;
        ofTrueTypeFont * m_big_font;
        
        // ui to be stored
        map< string, hWidget * > m_ui_elements;
    
        // states
        
        int m_selected_track;
        bool m_synced;
        bool m_playing;
        bool m_a4_mode;
        int  m_mid_delay;
        int  m_bpm;
        float m_swing;
        bool m_var_hold;
        float m_xorvar_ratio;
        float m_jakvar_ratio;
        map<int,bool> m_mutes;
        map<int, bool> m_keys;
        hGui * m_gui;
        
        // conf of current track
        ConfTrack m_conf;
        ofxXmlSettings m_loaded_preset;
    
        //utils OBJC
        KeyModifier m_modifiers;
        
        void update_evolve();
        void update_play();
        void update_mutes();
        void update_selection();
        void update_conf(ConfTrack conf);
        void drawTracks();
        void refresh_presets();
		
};
