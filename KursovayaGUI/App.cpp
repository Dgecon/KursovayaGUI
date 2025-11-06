#define _CRT_SECURE_NO_WARNINGS

#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);
bool App::OnInit()
{
	MainFrame* frame = new MainFrame("HotelManagementSystem");
	frame->Center();
	frame->SetSize(wxSize (920, 650));
	frame->SetBackgroundColour(*wxWHITE);
	frame->Show(true);
	
	return true;
}
