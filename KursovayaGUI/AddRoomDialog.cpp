#include "AddRoomDialog.h"

AddRoomDialog::AddRoomDialog(wxWindow* parent)
 : wxDialog(parent, wxID_ANY, "Добавить комнату", wxDefaultPosition, wxSize(360,200))
{
 wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
 wxFlexGridSizer* grid = new wxFlexGridSizer(2,8,8);
 grid->AddGrowableCol(1,1);

 grid->Add(new wxStaticText(this, wxID_ANY, "Номер комнаты:"),0, wxALIGN_CENTER_VERTICAL);
 m_roomNumber = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_roomNumber,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Категория:"),0, wxALIGN_CENTER_VERTICAL);
 m_category = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_category,1, wxEXPAND);

 grid->Add(new wxStaticText(this, wxID_ANY, "Цена за ночь:"),0, wxALIGN_CENTER_VERTICAL);
 m_price = new wxTextCtrl(this, wxID_ANY);
 grid->Add(m_price,1, wxEXPAND);

 topsizer->Add(grid,1, wxALL | wxEXPAND,10);
 topsizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),0, wxEXPAND | wxALL,10);
 SetSizerAndFit(topsizer);
}

wxString AddRoomDialog::getRoomNumber() const { return m_roomNumber->GetValue(); }
wxString AddRoomDialog::getCategory() const { return m_category->GetValue(); }
wxString AddRoomDialog::getPrice() const { return m_price->GetValue(); }

void AddRoomDialog::setValues(int roomNumber, const wxString& category, double price) {
 m_roomNumber->SetValue(wxString::Format("%d", roomNumber));
 m_category->SetValue(category);
 m_price->SetValue(wxString::Format("%.2f", price));
}
