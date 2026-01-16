#include "AddClientDialog.h"
#include "Client.h"

AddClientDialog::AddClientDialog(wxWindow* parent)
 : wxDialog(parent, wxID_ANY, "Добавить клиента", wxDefaultPosition, wxSize(520,420))
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

 // Passport fields
 grid->Add(new wxStaticText(this, wxID_ANY, "Паспорт - серия:"),0, wxALIGN_CENTER_VERTICAL);
 m_series = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_series,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Паспорт - номер:"),0, wxALIGN_CENTER_VERTICAL);
 m_number = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_number,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Кем выдан:"),0, wxALIGN_CENTER_VERTICAL);
 m_givenBy = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_givenBy,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата выдачи:"),0, wxALIGN_CENTER_VERTICAL);
 m_issueDate = new wxDatePickerCtrl(this, wxID_ANY);
 grid->Add(m_issueDate,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Код подразделения:"),0, wxALIGN_CENTER_VERTICAL);
 m_code = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_code,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Дата рождения:"),0, wxALIGN_CENTER_VERTICAL);
 m_birthDate = new wxDatePickerCtrl(this, wxID_ANY);
 grid->Add(m_birthDate,1, wxEXPAND);

 // New controls: child / foreigner
 grid->Add(new wxStaticText(this, wxID_ANY, "Ребёнок? (галочка):"),0, wxALIGN_CENTER_VERTICAL);
 m_isChild = new wxCheckBox(this, wxID_ANY, "Да");
 grid->Add(m_isChild,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Иностранное гражданство? (галочка):"),0, wxALIGN_CENTER_VERTICAL);
 m_isForeigner = new wxCheckBox(this, wxID_ANY, "Да");
 grid->Add(m_isForeigner,1, wxEXPAND);

 // Additional document fields
 grid->Add(new wxStaticText(this, wxID_ANY, "Св-во о рождении (серия и номер) / Номер свидетельства о рождении:"),0, wxALIGN_CENTER_VERTICAL);
 m_birthCert = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_birthCert,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Виза (номер):"),0, wxALIGN_CENTER_VERTICAL);
 m_visa = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_visa,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Международный паспорт (серия и номер):"),0, wxALIGN_CENTER_VERTICAL);
 m_intlPassport = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_intlPassport,1, wxEXPAND);

 topsizer->Add(grid,1, wxALL | wxEXPAND,10);

 wxSizer* btns = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
 topsizer->Add(btns,0, wxEXPAND | wxALL,10);

 SetSizerAndFit(topsizer);

 // Bind OK to validation
 Bind(wxEVT_BUTTON, &AddClientDialog::OnOk, this, wxID_OK);

 // Bind toggles
 m_isChild->Bind(wxEVT_CHECKBOX, &AddClientDialog::OnChildToggle, this);
 m_isForeigner->Bind(wxEVT_CHECKBOX, &AddClientDialog::OnForeignerToggle, this);

 // initialize state: hide fields not applicable
 m_birthCert->Show(false);
 m_visa->Show(false);
 m_intlPassport->Show(false);
}

wxString AddClientDialog::getFirstName() const { return m_first->GetValue(); }
wxString AddClientDialog::getLastName() const { return m_last->GetValue(); }
wxString AddClientDialog::getPhone() const { return m_phone->GetValue(); }

static Date DateFromWxDate(const wxDateTime& dt) {
 if (!dt.IsValid()) return Date();
 return Date(dt.GetDay(), dt.GetMonth()+1, dt.GetYear());
}

Passport AddClientDialog::getPassport() const {
 Passport p;
 long ser=0, num=0;
 
 m_series->GetValue().ToLong(&ser);
 m_number->GetValue().ToLong(&num);
 p.setSeries((int)ser);
 p.setNumber((int)num);
 p.setGivenBy(std::string(m_givenBy->GetValue().ToUTF8().data()));
 wxDateTime idt = m_issueDate->GetValue();
 p.setDateOfIssue(DateFromWxDate(idt));
 p.setCode(std::string(m_code->GetValue().ToUTF8().data()));
 wxDateTime bdt = m_birthDate->GetValue();
 p.setDateOfBirth(DateFromWxDate(bdt));
 p.setFio(std::string((m_first->GetValue() + " " + m_last->GetValue()).ToUTF8().data()));
 return p;
}

// New accessor functions to get child/foreigner/doc fields
bool AddClientDialog::isChild() const { return m_isChild->IsChecked(); }
bool AddClientDialog::isForeigner() const { return m_isForeigner->IsChecked(); }
std::string AddClientDialog::getBirthCertificate() const { return std::string(m_birthCert->GetValue().ToUTF8().data()); }
std::string AddClientDialog::getVisa() const { return std::string(m_visa->GetValue().ToUTF8().data()); }
std::string AddClientDialog::getInternationalPassport() const { return std::string(m_intlPassport->GetValue().ToUTF8().data()); }

void AddClientDialog::OnOk(wxCommandEvent& evt) {
 // Basic validation: names not empty, series/number numeric, issue/birth dates valid
 if (m_first->GetValue().IsEmpty() || m_last->GetValue().IsEmpty()) {
 wxMessageBox("Введите имя и фамилию", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 long ser=0, num=0;
 if (!m_series->GetValue().ToLong(&ser) || !m_number->GetValue().ToLong(&num)) {
 wxMessageBox("Серия и номер паспорта должны быть числами", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 Date issue = DateFromWxDate(m_issueDate->GetValue());
 Date birth = DateFromWxDate(m_birthDate->GetValue());
 if (!issue.isValid() || !birth.isValid()) {
 wxMessageBox("Неверная дата выдачи или дата рождения", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }

 // if child is checked, ensure birth certificate provided
 if (m_isChild->IsChecked()) {
 if (m_birthCert->GetValue().IsEmpty()) {
 wxMessageBox("Для детей требуется ввести свидетельство о рождении", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 }

 // if foreigner is checked, ensure visa and international passport provided
 if (m_isForeigner->IsChecked()) {
 if (m_visa->GetValue().IsEmpty() || m_intlPassport->GetValue().IsEmpty()) {
 wxMessageBox("Для иностранцев требуется ввести визу и международный паспорт", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 }

 // all good
 EndModal(wxID_OK);
}

void AddClientDialog::OnChildToggle(wxCommandEvent& evt)
{
 bool show = m_isChild->IsChecked();
 m_birthCert->Show(show);
 // if showing child document, ensure foreigner fields hidden (optional)
 if (show) {
 // keep foreigner checkbox state unchanged, but we may hide their fields
 m_visa->Show(m_isForeigner->IsChecked());
 m_intlPassport->Show(m_isForeigner->IsChecked());
 }
 GetSizer()->Layout();
 GetSizer()->Fit(this);
}

void AddClientDialog::OnForeignerToggle(wxCommandEvent& evt)
{
 bool show = m_isForeigner->IsChecked();
 m_visa->Show(show);
 m_intlPassport->Show(show);
 // if foreigner, birth cert may still be shown if child is checked
 m_birthCert->Show(m_isChild->IsChecked());
 GetSizer()->Layout();
 GetSizer()->Fit(this);
}

void AddClientDialog::setValues(const Client& client) {
 m_first->SetValue(wxString::FromUTF8(client.getFirstName().c_str()));
 m_last->SetValue(wxString::FromUTF8(client.getLastName().c_str()));
 m_phone->SetValue(wxString::FromUTF8(client.getPhone().c_str()));
 Passport p = client.getPassport();
 m_series->SetValue(wxString::Format("%d", p.getSeries()));
 m_number->SetValue(wxString::Format("%d", p.getNumber()));
 m_givenBy->SetValue(wxString::FromUTF8(p.getGivenBy().c_str()));
 m_issueDate->SetValue(wxDateTime(p.getDateOfIssue().getDay(), (wxDateTime::Month)(p.getDateOfIssue().getMonth()-1), p.getDateOfIssue().getYear()));
 m_code->SetValue(wxString::FromUTF8(p.getCode().c_str()));
 m_birthDate->SetValue(wxDateTime(p.getDateOfBirth().getDay(), (wxDateTime::Month)(p.getDateOfBirth().getMonth()-1), p.getDateOfBirth().getYear()));

 // new fields
 m_isChild->SetValue(client.getIsChild());
 m_isForeigner->SetValue(client.getIsForeigner());
 m_birthCert->SetValue(wxString::FromUTF8(client.getBirthCertificate().c_str()));
 m_visa->SetValue(wxString::FromUTF8(client.getVisa().c_str()));
 m_intlPassport->SetValue(wxString::FromUTF8(client.getInternationalPassport().c_str()));
 // Update visibility directly and relayout
 m_birthCert->Show(client.getIsChild());
 m_visa->Show(client.getIsForeigner());
 m_intlPassport->Show(client.getIsForeigner());
 if (GetSizer()) { GetSizer()->Layout(); GetSizer()->Fit(this); }
}
