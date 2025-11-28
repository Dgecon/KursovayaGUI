#pragma once

#include <wx/wx.h>

class AddRoomDialog : public wxDialog
{
public:
 AddRoomDialog(wxWindow* parent);

 wxString getRoomNumber() const;
 wxString getCategory() const;
 wxString getPrice() const;

 // prefill values for editing
 void setValues(int roomNumber, const wxString& category, double price);

private:
 wxTextCtrl* m_roomNumber;
 wxTextCtrl* m_category;
 wxTextCtrl* m_price;
};
