#pragma once

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include "Primitives.h"

class PrimitiveDialog : public wxDialog {
	public:
		PrimitiveDialog();
		~PrimitiveDialog();

		void OnOkay(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);

	private:
		DECLARE_DYNAMIC_CLASS(PrimitiveDialog)
		DECLARE_EVENT_TABLE()
};



class PrimitiveInterface {
	/*
	 *  This class implements the GUI-end interface for primitives in general.
	 *
	 */
	
	public:
		PrimitiveInterface();
		PrimitiveInterface(Primitive* p): primitive(p) { PrimitiveInterface(); };
		~PrimitiveInterface();
	
		void SetPrimitive(Primitive* p) { primitive = p; };
		Primitive* GetPrimitive() { return primitive; };
		
		void OpenPropertiesDialog();
		void ClosePropertiesDialog();
	
	private:
		Primitive* primitive;
		PrimitiveDialog* settingsdialog;
};
