#pragma once

#include "ofMain.h"
// gui
#include "hEvents.h"
#include "hObject.h"
#include "hWidget.h"

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
    
        // constants
        static int const ON_MAIN_COLOR = 0x56CAC6;
        static int const ON_SECOND_COLOR = 0x1D7571;
        static int const ON_GREY_COLOR = 0xEEEEEE;
    
    protected:
        ofTrueTypeFont * m_font;
        
        // ui to be stored
        map< string, hWidget * > m_ui_elements;
    
        // states
        bool m_synced;
        bool m_playing;
        int  m_mid_delay;
        int  m_bpm;
        float m_swing;
        bool m_autovar_generate;
        bool m_autovar_set;
        float m_xorvar_ratio;
        float m_jakvar_ratio;
        
		
};
