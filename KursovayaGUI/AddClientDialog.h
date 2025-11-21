#pragma once

#include <wx/wx.h>
#include "Passport.h"
class Client;

class AddClientDialog : public wxDialog
{
public:
 AddClientDialog(wxWindow* parent);

 wxString getFirstName() const;
 wxString getLastName() const;
 wxString getPhone() const;
 // returns constructed Passport
 Passport getPassport() const;

 // prefill dialog from existing client
 void setValues(const Client& client);

private:
 wxTextCtrl* m_first;
 wxTextCtrl* m_last;
 wxTextCtrl* m_phone;

 // passport fields
 wxTextCtrl* m_series;
 wxTextCtrl* m_number;
 wxTextCtrl* m_givenBy;
 wxTextCtrl* m_issueDay;
 wxTextCtrl* m_issueMonth;
 wxTextCtrl* m_issueYear;
 wxTextCtrl* m_code;
 wxTextCtrl* m_birthDay;
 wxTextCtrl* m_birthMonth;
 wxTextCtrl* m_birthYear;

 void OnOk(wxCommandEvent& evt);
};
