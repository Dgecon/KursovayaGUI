#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/datetime.h>
#include "Date.h"

class AddBookingDialog : public wxDialog {
public:
 AddBookingDialog(wxWindow* parent);
 Date getCheckIn() const;
 Date getCheckOut() const;
private:
 wxDatePickerCtrl* m_ci_date;
 wxDatePickerCtrl* m_co_date;
 void OnOk(wxCommandEvent& evt);
};
