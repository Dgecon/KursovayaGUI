#include "AddBookingDialog.h"
#include <wx/datectrl.h>
#include <wx/datetime.h>
#include <wx/sizer.h>

AddBookingDialog::AddBookingDialog(wxWindow* parent)
 : wxDialog(parent, wxID_ANY, "Добавить бронирование", wxDefaultPosition, wxSize(360,220))
{
 wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
 wxFlexGridSizer* grid = new wxFlexGridSizer(2,8,8);
 grid->AddGrowableCol(1,1);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата заезда:"),0, wxALIGN_CENTER_VERTICAL);
 m_ci_date = new wxDatePickerCtrl(this, wxID_ANY);
 grid->Add(static_cast<wxWindow*>(m_ci_date),1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата отъезда:"),0, wxALIGN_CENTER_VERTICAL);
 m_co_date = new wxDatePickerCtrl(this, wxID_ANY);
 grid->Add(static_cast<wxWindow*>(m_co_date),1, wxEXPAND);

 top->Add(grid,1, wxALL | wxEXPAND,10);
 top->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),0, wxEXPAND | wxALL,10);
 SetSizerAndFit(top);

 Bind(wxEVT_BUTTON, &AddBookingDialog::OnOk, this, wxID_OK);
}

static Date DateFromWx(const wxDateTime& dt) {
 if (!dt.IsValid()) return Date();
 int d = dt.GetDay();
 int m = dt.GetMonth() +1; // wx uses0-based months
 int y = dt.GetYear();
 return Date(d,m,y);
}

Date AddBookingDialog::getCheckIn() const {
 wxDatePickerCtrl* picker = static_cast<wxDatePickerCtrl*>(m_ci_date);
 wxDateTime dt = picker->GetValue();
 return DateFromWx(dt);
}

Date AddBookingDialog::getCheckOut() const {
 wxDatePickerCtrl* picker = static_cast<wxDatePickerCtrl*>(m_co_date);
 wxDateTime dt = picker->GetValue();
 return DateFromWx(dt);
}

void AddBookingDialog::OnOk(wxCommandEvent& evt) {
 Date ci = getCheckIn();
 Date co = getCheckOut();
 if (!ci.isValid() || !co.isValid() || !ci.isBefore(co)) {
 wxMessageBox("Неверные даты: проверьте корректность дат и что заезд < отъезда.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 EndModal(wxID_OK);
}
