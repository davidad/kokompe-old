#pragma once

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/xrc/xmlres.h>


class KokompeUI : public wxApp {
	public:
		virtual bool OnInit();
};


class KokompeMainFrame : public wxFrame {
	public:
		KokompeMainFrame() {};
		KokompeMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

		void OnSize(wxSizeEvent& event);
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnNew(wxCommandEvent& event);
		void OnOpen(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);

	private:
		DECLARE_DYNAMIC_CLASS(KokompeMainFrame)
		DECLARE_EVENT_TABLE()
};

