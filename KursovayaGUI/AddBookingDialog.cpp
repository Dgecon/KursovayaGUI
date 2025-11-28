#include "AddBookingDialog.h"

AddBookingDialog::AddBookingDialog(wxWindow* parent)
 : wxDialog(parent, wxID_ANY, "Добавить бронирование", wxDefaultPosition, wxSize(320,240))
{
 wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
 wxFlexGridSizer* grid = new wxFlexGridSizer(2,8,8);
 grid->AddGrowableCol(1,1);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата заезда - день:"),0, wxALIGN_CENTER_VERTICAL);
 m_ci_day = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_ci_day,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата заезда - месяц:"),0, wxALIGN_CENTER_VERTICAL);
 m_ci_month = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_ci_month,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата заезда - год:"),0, wxALIGN_CENTER_VERTICAL);
 m_ci_year = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_ci_year,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата отъезда - день:"),0, wxALIGN_CENTER_VERTICAL);
 m_co_day = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_co_day,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата отъезда - месяц:"),0, wxALIGN_CENTER_VERTICAL);
 m_co_month = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_co_month,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата отъезда - год:"),0, wxALIGN_CENTER_VERTICAL);
 m_co_year = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_co_year,1, wxEXPAND);

 top->Add(grid,1, wxALL | wxEXPAND,10);
 top->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),0, wxEXPAND | wxALL,10);
 SetSizerAndFit(top);

 Bind(wxEVT_BUTTON, &AddBookingDialog::OnOk, this, wxID_OK);
}

Date AddBookingDialog::getCheckIn() const {
 long d=1,m=1,y=2024; m_ci_day->GetValue().ToLong(&d); m_ci_month->GetValue().ToLong(&m); m_ci_year->GetValue().ToLong(&y);
 return Date((int)d,(int)m,(int)y);
}
Date AddBookingDialog::getCheckOut() const {
 long d=1,m=1,y=2024; m_co_day->GetValue().ToLong(&d); m_co_month->GetValue().ToLong(&m); m_co_year->GetValue().ToLong(&y);
 return Date((int)d,(int)m,(int)y);
}

void AddBookingDialog::OnOk(wxCommandEvent& evt) {
 Date ci = getCheckIn();
 Date co = getCheckOut();
 if (!ci.isValid() || !co.isValid() || !ci.isBefore(co)) {
 wxMessageBox("Неверные даты: проверьте коррекность дат и что заезд раньше отъезда.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 EndModal(wxID_OK);
}
