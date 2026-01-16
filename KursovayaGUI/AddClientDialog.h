#pragma once

#ifndef _CRT_SECURE_WARNINGS
#define _CRT_SECURE_WARNINGS
#endif

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/datetime.h>
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

 // new accessors
 bool isChild() const;
 bool isForeigner() const;
 std::string getBirthCertificate() const;
 std::string getVisa() const;
 std::string getInternationalPassport() const;

private:
 wxTextCtrl* m_first;
 wxTextCtrl* m_last;
 wxTextCtrl* m_phone;

 // passport fields
 wxTextCtrl* m_series;
 wxTextCtrl* m_number;
 wxTextCtrl* m_givenBy;
 wxDatePickerCtrl* m_issueDate; // replaced separate fields
 wxTextCtrl* m_code;
 wxDatePickerCtrl* m_birthDate; // replaced separate fields

 // new: child / foreigner options
 wxCheckBox* m_isChild;
 wxCheckBox* m_isForeigner;

 // additional documents
 wxTextCtrl* m_birthCert; // birth certificate for child
 wxTextCtrl* m_visa; // visa for foreigner
 wxTextCtrl* m_intlPassport; // international passport for foreigner

 void OnOk(wxCommandEvent& evt);
 void OnChildToggle(wxCommandEvent& evt);
 void OnForeignerToggle(wxCommandEvent& evt);
};
