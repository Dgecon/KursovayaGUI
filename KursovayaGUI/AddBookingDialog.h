#pragma once

#include <wx/wx.h>
#include "Date.h"

class AddBookingDialog : public wxDialog {
public:
 AddBookingDialog(wxWindow* parent);
 Date getCheckIn() const;
 Date getCheckOut() const;
private:
 wxTextCtrl* m_ci_day;
 wxTextCtrl* m_ci_month;
 wxTextCtrl* m_ci_year;
 wxTextCtrl* m_co_day;
 wxTextCtrl* m_co_month;
 wxTextCtrl* m_co_year;
 void OnOk(wxCommandEvent& evt);
};
