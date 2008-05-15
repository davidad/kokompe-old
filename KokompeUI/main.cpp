#include "Base.h"
#include "SDLPanel.h"

IMPLEMENT_APP(KokompeUI)


bool KokompeUI::OnInit() {
	wxXmlResource::Get()->AddHandler(new SDLPanelXmlHandler);
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->Load(_T("KokompeUI.xrc"));

	KokompeMainFrame *frame = (KokompeMainFrame*)wxXmlResource::Get()->LoadFrame(0, _T("Kokompe"));
	frame->Show(true);
	return true;
}
