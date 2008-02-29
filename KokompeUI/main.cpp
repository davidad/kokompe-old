#include "Base.h"

#include "SDLPanel.h"
#include "PrimitiveInterface.h"


IMPLEMENT_DYNAMIC_CLASS(KokompeMainFrame, wxFrame)

BEGIN_EVENT_TABLE(KokompeMainFrame, wxFrame)
	EVT_SIZE(KokompeMainFrame::OnSize)
	EVT_MENU(XRCID("KOMENU_PROJECT_QUIT"),  KokompeMainFrame::OnQuit)
	EVT_MENU(XRCID("KOMENU_HELP_ABOUT"), KokompeMainFrame::OnAbout)
	EVT_MENU(XRCID("KOMENU_PROJECT_NEW_2D"), KokompeMainFrame::OnNew)
	EVT_MENU(XRCID("KOMENU_PROJECT_OPEN"), KokompeMainFrame::OnOpen)
	EVT_MENU(XRCID("KOMENU_PROJECT_SAVE"), KokompeMainFrame::OnSave)
END_EVENT_TABLE()

IMPLEMENT_APP(KokompeUI)

bool KokompeUI::OnInit() {
	wxXmlResource::Get()->AddHandler(new SDLPanelXmlHandler);
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->Load(_T("KokompeUI.xrc"));

	KokompeMainFrame *frame = (KokompeMainFrame*)wxXmlResource::Get()->LoadFrame(0, _T("Kokompe"));
	frame->Show(true);
	return true;
}

KokompeMainFrame::KokompeMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size): 
	wxFrame((wxFrame *)NULL, -1, title, pos, size) {
}

void KokompeMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(true);
}

void KokompeMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {

}


void KokompeMainFrame::OnNew(wxCommandEvent& WXUNUSED(event)) {
	
}


void KokompeMainFrame::OnOpen(wxCommandEvent& WXUNUSED(event)) {
	
}

void KokompeMainFrame::OnSave(wxCommandEvent& WXUNUSED(event)) {
	
}

void KokompeMainFrame::OnSize(wxSizeEvent& WXUNUSED(event)) {
	// FIXME: This should resize the SDL viewport appropriately.
}
