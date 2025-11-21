#include "AddClientDialog.h"

AddClientDialog::AddClientDialog(wxWindow* parent)
 : wxDialog(parent, wxID_ANY, "Добавить клиента", wxDefaultPosition, wxSize(520,360))
{
 wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

 wxFlexGridSizer* grid = new wxFlexGridSizer(2,8,8);
 grid->AddGrowableCol(1,1);

 grid->Add(new wxStaticText(this, wxID_ANY, "Имя:"),0, wxALIGN_CENTER_VERTICAL);
 m_first = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_first,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Фамилия:"),0, wxALIGN_CENTER_VERTICAL);
 m_last = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_last,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Телефон:"),0, wxALIGN_CENTER_VERTICAL);
 m_phone = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_phone,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Паспорт - серия:"),0, wxALIGN_CENTER_VERTICAL);
 m_series = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_series,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Паспорт - номер:"),0, wxALIGN_CENTER_VERTICAL);
 m_number = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_number,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Кем выдан:"),0, wxALIGN_CENTER_VERTICAL);
 m_givenBy = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_givenBy,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата выдачи (дд мм гггг) - день:"),0, wxALIGN_CENTER_VERTICAL);
 m_issueDay = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_issueDay,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата выдачи - месяц:"),0, wxALIGN_CENTER_VERTICAL);
 m_issueMonth = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_issueMonth,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата выдачи - год:"),0, wxALIGN_CENTER_VERTICAL);
 m_issueYear = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_issueYear,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Код подразделения:"),0, wxALIGN_CENTER_VERTICAL);
 m_code = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_code,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата рождения - день:"),0, wxALIGN_CENTER_VERTICAL);
 m_birthDay = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_birthDay,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата рождения - месяц:"),0, wxALIGN_CENTER_VERTICAL);
 m_birthMonth = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_birthMonth,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата рождения - год:"),0, wxALIGN_CENTER_VERTICAL);
 m_birthYear = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_birthYear,1, wxEXPAND);

 topsizer->Add(grid,1, wxALL | wxEXPAND,10);

 wxSizer* btns = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
 topsizer->Add(btns,0, wxEXPAND | wxALL,10);

 SetSizerAndFit(topsizer);

 Bind(wxEVT_BUTTON, &AddClientDialog::OnOk, this, wxID_OK);
}

wxString AddClientDialog::getFirstName() const { return m_first->GetValue(); }
wxString AddClientDialog::getLastName() const { return m_last->GetValue(); }
wxString AddClientDialog::getPhone() const { return m_phone->GetValue(); }

Passport AddClientDialog::getPassport() const {
 Passport p;
 long ser=0, num=0, iday=1, imonth=1, iyear=2024, bday=1, bmonth=1, byear=2000;
 
 m_series->GetValue().ToLong(&ser);
 m_number->GetValue().ToLong(&num);
 p.setSeries((int)ser);
 p.setNumber((int)num);
 p.setGivenBy(std::string(m_givenBy->GetValue().ToUTF8().data()));
 m_issueDay->GetValue().ToLong(&iday);
 m_issueMonth->GetValue().ToLong(&imonth);
 m_issueYear->GetValue().ToLong(&iyear);
 Date issue((int)iday,(int)imonth,(int)iyear);
 p.setDateOfIssue(issue);
 p.setCode(std::string(m_code->GetValue().ToUTF8().data()));
 m_birthDay->GetValue().ToLong(&bday);
 m_birthMonth->GetValue().ToLong(&bmonth);
 m_birthYear->GetValue().ToLong(&byear);
 Date birth((int)bday,(int)bmonth,(int)byear);
 p.setDateOfBirth(birth);
 p.setFio(std::string((m_first->GetValue() + " " + m_last->GetValue()).ToUTF8().data()));
 return p;
}

void AddClientDialog::OnOk(wxCommandEvent& evt) {
 if (m_first->GetValue().IsEmpty() || m_last->GetValue().IsEmpty()) {
 wxMessageBox("Введите имя и фамилию", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 long ser=0, num=0;
 if (!m_series->GetValue().ToLong(&ser) || !m_number->GetValue().ToLong(&num)) {
 wxMessageBox("Серия и номер паспорта должны быть числами", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 long iday=1, imonth=1, iyear=2024, bday=1, bmonth=1, byear=2000;
 m_issueDay->GetValue().ToLong(&iday);
 m_issueMonth->GetValue().ToLong(&imonth);
 m_issueYear->GetValue().ToLong(&iyear);
 m_birthDay->GetValue().ToLong(&bday);
 m_birthMonth->GetValue().ToLong(&bmonth);
 m_birthYear->GetValue().ToLong(&byear);
 Date issue((int)iday,(int)imonth,(int)iyear);
 Date birth((int)bday,(int)bmonth,(int)byear);
 if (!issue.isValid() || !birth.isValid()) {
 wxMessageBox("Неверная дата выдачи или дата рождения", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 EndModal(wxID_OK);
}
