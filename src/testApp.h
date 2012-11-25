#pragma once

#include "ofMain.h"
// gui
#include "hEvents.h"
#include "hObject.h"
#include "hWidget.h"

// test
#include "Gaia.h"
#include "Euclid.h"
#include "ConfTrack.h"

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
    
        // constants
        static int const ON_MAIN_COLOR = 0x3AA6D0;//
        static int const ON_SECOND_COLOR = 0xFFA340;
        static int const ON_GREY_COLOR = 0xEEEEEE;
        static int const ON_VAR_COLOR = 0x071871;
    
    protected:
        ofTrueTypeFont * m_font;
        
        // ui to be stored
        map< string, hWidget * > m_ui_elements;
    
        // states
        
        int m_selected_track;
        bool m_synced;
        bool m_playing;
        int  m_mid_delay;
        int  m_bpm;
        float m_swing;
        bool m_var_hold;
        float m_xorvar_ratio;
        float m_jakvar_ratio;
        
        // conf of current track
        ConfTrack m_conf;
		
};
