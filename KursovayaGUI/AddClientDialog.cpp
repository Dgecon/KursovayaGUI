#include "AddClientDialog.h"
#include "Client.h"

AddClientDialog::AddClientDialog(wxWindow* parent)
 : wxDialog(parent, wxID_ANY, "Добавить клиента", wxDefaultPosition, wxSize(400,500), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
 wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

 wxFlexGridSizer* grid = new wxFlexGridSizer(2,8,8);
 grid->AddGrowableCol(1,1);

 // Helper to add a label + control + optional error label. minWidth allows larger inputs.
 // Reduced default minWidth so dialog can be narrower.
 auto AddRow = [&](const wxString& label, wxWindow* control, wxStaticText** errPtr = nullptr, int minWidth =200, wxStaticText** labelPtr = nullptr){
 wxStaticText* lbl = new wxStaticText(this, wxID_ANY, label);
 if (labelPtr) *labelPtr = lbl;
 grid->Add(lbl,0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
 if (minWidth >0) control->SetMinSize(wxSize(minWidth, -1));
 wxBoxSizer* v = new wxBoxSizer(wxVERTICAL);
 v->Add(control,0, wxEXPAND);
 if (errPtr){
 *errPtr = new wxStaticText(this, wxID_ANY, "");
 (*errPtr)->SetForegroundColour(*wxRED);
 (*errPtr)->Hide();
 v->Add(*errPtr,0, wxTOP,2);
 } else {
 v->AddSpacer(0);
 }
 grid->Add(v,1, wxEXPAND);
 };

 // Name / Patronymic / Last / Phone
 m_first = new wxTextCtrl(this, wxID_ANY);
 AddRow("Имя:", m_first, &m_firstErr,300);

 m_patronymic = new wxTextCtrl(this, wxID_ANY);
 AddRow("Отчество (необязательно):", m_patronymic, nullptr,300);

 m_last = new wxTextCtrl(this, wxID_ANY);
 AddRow("Фамилия:", m_last, nullptr,300);

 m_phone = new wxTextCtrl(this, wxID_ANY);
 AddRow("Телефон:", m_phone, nullptr,220, &m_phoneLabel);

 // Passport fields (series/number narrower)
 m_series = new wxTextCtrl(this, wxID_ANY);
 AddRow("Паспорт - серия:", m_series, nullptr,100, &m_seriesLabel);

 m_number = new wxTextCtrl(this, wxID_ANY);
 AddRow("Паспорт - номер:", m_number, &m_passErr,140, &m_numberLabel);

 m_givenBy = new wxTextCtrl(this, wxID_ANY);
 AddRow("Кем выдан:", m_givenBy, nullptr,280, &m_givenByLabel);

 m_issueDate = new wxDatePickerCtrl(this, wxID_ANY);
 AddRow("Дата выдачи:", m_issueDate, nullptr,180, &m_issueDateLabel);

 m_code = new wxTextCtrl(this, wxID_ANY);
 AddRow("Код подразделения:", m_code, nullptr,140, &m_codeLabel);

 m_birthDate = new wxDatePickerCtrl(this, wxID_ANY);
 AddRow("Дата рождения:", m_birthDate, nullptr,180);

 // Child / Foreigner
 m_isChild = new wxCheckBox(this, wxID_ANY, "Да");
 AddRow("Ребёнок? (галочка):", m_isChild, nullptr,100);

 m_isForeigner = new wxCheckBox(this, wxID_ANY, "Да");
 AddRow("Иностранное гражданство? (галочка):", m_isForeigner, nullptr,100);

 // Additional document fields
 m_birthCert = new wxTextCtrl(this, wxID_ANY);
 AddRow("Св-во о рождении (серия и номер) / Номер свидетельства о рождении:", m_birthCert, &m_birthCertErr,300);

 m_visa = new wxTextCtrl(this, wxID_ANY);
 AddRow("Виза (номер):", m_visa, nullptr,220);

 m_intlPassport = new wxTextCtrl(this, wxID_ANY);
 AddRow("Международный паспорт (серия и номер):", m_intlPassport, &m_foreignerErr,220);

 topsizer->Add(grid,1, wxALL | wxEXPAND,10);

 wxSizer* btns = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
 topsizer->Add(btns,0, wxEXPAND | wxALL,10);

 SetSizerAndFit(topsizer);
 // Allow sizer to provide min size and enable resizing
 topsizer->SetSizeHints(this);
 // Ensure initial size (narrower and taller)
 SetSize(wxSize(480,500));

 // Bind OK to validation
 Bind(wxEVT_BUTTON, &AddClientDialog::OnOk, this, wxID_OK);

 // Bind toggles
 m_isChild->Bind(wxEVT_CHECKBOX, &AddClientDialog::OnChildToggle, this);
 m_isForeigner->Bind(wxEVT_CHECKBOX, &AddClientDialog::OnForeignerToggle, this);

 // inline validation binds
 m_first->Bind(wxEVT_TEXT, &AddClientDialog::OnNameChanged, this);
 m_series->Bind(wxEVT_TEXT, &AddClientDialog::OnPassportChanged, this);
 m_number->Bind(wxEVT_TEXT, &AddClientDialog::OnPassportChanged, this);
 m_birthCert->Bind(wxEVT_TEXT, &AddClientDialog::OnBirthCertChanged, this);
 m_visa->Bind(wxEVT_TEXT, &AddClientDialog::OnForeignerDocsChanged, this);
 m_intlPassport->Bind(wxEVT_TEXT, &AddClientDialog::OnForeignerDocsChanged, this);

 // initialize state: hide fields not applicable
 m_birthCert->Show(false);
 m_visa->Show(false);
 m_intlPassport->Show(false);
 if (m_firstErr) m_firstErr->Hide();
 if (m_passErr) m_passErr->Hide();
 if (m_birthCertErr) m_birthCertErr->Hide();
 if (m_foreignerErr) m_foreignerErr->Hide();
}

wxString AddClientDialog::getFirstName() const { return m_first->GetValue(); }
wxString AddClientDialog::getLastName() const { return m_last->GetValue(); }
wxString AddClientDialog::getPhone() const { return m_phone->GetValue(); }
wxString AddClientDialog::getPatronymic() const { return m_patronymic->GetValue(); }

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
 // build fio: first [patronymic] last
 wxString fio = m_first->GetValue();
 if (!m_patronymic->GetValue().IsEmpty()) fio += " " + m_patronymic->GetValue();
 fio += " " + m_last->GetValue();
 p.setFio(std::string(fio.ToUTF8().data()));
 return p;
}

// New accessor functions to get child/foreigner/doc fields
bool AddClientDialog::isChild() const { return m_isChild->IsChecked(); }
bool AddClientDialog::isForeigner() const { return m_isForeigner->IsChecked(); }
std::string AddClientDialog::getBirthCertificate() const { return std::string(m_birthCert->GetValue().ToUTF8().data()); }
std::string AddClientDialog::getVisa() const { return std::string(m_visa->GetValue().ToUTF8().data()); }
std::string AddClientDialog::getInternationalPassport() const { return std::string(m_intlPassport->GetValue().ToUTF8().data()); }

void AddClientDialog::OnOk(wxCommandEvent& evt) {
 if (!ValidateAll()) return;
 EndModal(wxID_OK);
}

void AddClientDialog::OnChildToggle(wxCommandEvent& evt)
{
 UpdateVisibility();
}

void AddClientDialog::OnForeignerToggle(wxCommandEvent& evt)
{
 UpdateVisibility();
}

void AddClientDialog::UpdateVisibility()
{
 bool isChild = m_isChild->IsChecked();
 bool isForeigner = m_isForeigner->IsChecked();

 // birth certificate visibility
 m_birthCert->Show(isChild);
 if (m_birthCertErr) m_birthCertErr->Show(false);

 // foreigner docs visibility
 m_visa->Show(isForeigner);
 m_intlPassport->Show(isForeigner);
 if (m_foreignerErr) m_foreignerErr->Show(false);

 // passport & phone hidden for children
 bool showPassportPhone = !isChild;
 if (m_phoneLabel) m_phoneLabel->Show(showPassportPhone);
 if (m_phone) m_phone->Show(showPassportPhone);
 if (m_seriesLabel) m_seriesLabel->Show(showPassportPhone);
 if (m_series) m_series->Show(showPassportPhone);
 if (m_numberLabel) m_numberLabel->Show(showPassportPhone);
 if (m_number) m_number->Show(showPassportPhone);
 if (m_passErr) {
 if (!showPassportPhone) m_passErr->Hide();
 }
 if (m_givenByLabel) m_givenByLabel->Show(showPassportPhone);
 if (m_givenBy) m_givenBy->Show(showPassportPhone);
 if (m_issueDateLabel) m_issueDateLabel->Show(showPassportPhone);
 if (m_issueDate) m_issueDate->Show(showPassportPhone);
 if (m_codeLabel) m_codeLabel->Show(showPassportPhone);
 if (m_code) m_code->Show(showPassportPhone);

 if (!showPassportPhone) {
 if (m_phone) m_phone->SetValue("");
 if (m_series) m_series->SetValue("");
 if (m_number) m_number->SetValue("");
 }

 GetSizer()->Layout();
 GetSizer()->Fit(this);
}


// validation handlers
void AddClientDialog::OnNameChanged(wxCommandEvent& evt) {
 if (!m_first->GetValue().IsEmpty() && !m_last->GetValue().IsEmpty()) { m_firstErr->Show(false); }
 else { m_firstErr->SetLabel("Имя и фамилия обязательны"); m_firstErr->Show(true); }
 GetSizer()->Layout();
}

void AddClientDialog::OnPassportChanged(wxCommandEvent& evt) {
 // skip passport validation for child clients
 if (m_isChild->IsChecked()) { if (m_passErr) m_passErr->Hide(); return; }
 long ser=0, num=0;
 if (m_series->GetValue().ToLong(&ser) && m_number->GetValue().ToLong(&num)) { if (m_passErr) m_passErr->Hide(); }
 else { if (m_passErr) { m_passErr->SetLabel("Серия/номер паспорта должны быть числами"); m_passErr->Show(true); } }
 GetSizer()->Layout();
}

void AddClientDialog::OnBirthCertChanged(wxCommandEvent& evt) {
 if (!m_birthCert->GetValue().IsEmpty()) { m_birthCertErr->Show(false); }
 else if (m_isChild->IsChecked()) { m_birthCertErr->SetLabel("Требуется свидетельство для ребёнка"); m_birthCertErr->Show(true); }
 GetSizer()->Layout();
}

void AddClientDialog::OnForeignerDocsChanged(wxCommandEvent& evt) {
 if (!m_visa->GetValue().IsEmpty() && !m_intlPassport->GetValue().IsEmpty()) { m_foreignerErr->Show(false); }
 else if (m_isForeigner->IsChecked()) { m_foreignerErr->SetLabel("Виза и паспорт обязательны для иностранца"); m_foreignerErr->Show(true); }
 GetSizer()->Layout();
}

bool AddClientDialog::ValidateAll() {
 bool ok = true;
 if (m_first->GetValue().IsEmpty() || m_last->GetValue().IsEmpty()) { m_firstErr->SetLabel("Имя и фамилия обязательны"); m_firstErr->Show(true); ok = false; }
 long ser=0, num=0;
 // passport and phone not required for children
 if (!m_isChild->IsChecked()) {
 if (!m_series->GetValue().ToLong(&ser) || !m_number->GetValue().ToLong(&num)) { m_passErr->SetLabel("Серия/номер паспорта должны быть числами"); m_passErr->Show(true); ok = false; }
 if (m_phone->GetValue().IsEmpty()) { if (m_phoneLabel){/* we don't have error label for phone, just mark invalid via message */ } }
 }
 if (m_isChild->IsChecked() && m_birthCert->GetValue().IsEmpty()) { m_birthCertErr->SetLabel("Требуется свидетельство о рождении"); m_birthCertErr->Show(true); ok = false; }
 if (m_isForeigner->IsChecked() && (m_visa->GetValue().IsEmpty() || m_intlPassport->GetValue().IsEmpty())) { m_foreignerErr->SetLabel("Виза и паспорт обязательны для иностранца"); m_foreignerErr->Show(true); ok = false; }
 GetSizer()->Layout();
 return ok;
}

void AddClientDialog::setValues(const Client& client) {
 m_first->SetValue(wxString::FromUTF8(client.getFirstName().c_str()));
 m_patronymic->SetValue(wxString::FromUTF8(client.getPatronymic().c_str()));
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
 // Ensure UI respects child/foreigner state (hide phone/passport for child)
 // Trigger the same logic as toggles bound to the checkboxes
 UpdateVisibility();
 if (GetSizer()) { GetSizer()->Layout(); GetSizer()->Fit(this); }
}
