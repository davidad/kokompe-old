#pragma once

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <SDL/SDL.h>

class SDLPanel : public wxPanel {
	/*
	 *  This class makes SDL into a wxWidget.
	 */
	DECLARE_CLASS(SDLPanel)
	DECLARE_EVENT_TABLE()
    
	private:
		SDL_Surface *screen;

		void onPaint(wxPaintEvent &event);
		void onEraseBackground(wxEraseEvent &event);
		void onIdle(wxIdleEvent &event);
		void createScreen();

	public:
		SDLPanel(wxWindow *parent);
		SDLPanel();
		~SDLPanel();
};


class SDLPanelXmlHandler : public wxXmlResourceHandler {
	/*
	 *  This class exposes the SDLPanel widget to XRC.
	 */
	DECLARE_DYNAMIC_CLASS(SDLPanelXmlHandler)
	
	public:
		SDLPanelXmlHandler();
		virtual wxObject *DoCreateResource();
		virtual bool CanHandle(wxXmlNode *node);
};

