#include "AddRoomDialog.h"

AddRoomDialog::AddRoomDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Добавить комнату"), wxDefaultPosition, wxSize(400, 220))
{
    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* grid = new wxFlexGridSizer(2, 10, 10);
    grid->AddGrowableCol(1, 1);

    // Room number
    grid->Add(new wxStaticText(this, wxID_ANY, wxT("Номер комнаты:")), 0, wxALIGN_CENTER_VERTICAL);
    m_roomNumber = new wxTextCtrl(this, wxID_ANY);
    m_roomNumber->SetMinSize(wxSize(150, -1));
    grid->Add(m_roomNumber, 1, wxEXPAND);

    // Category
    grid->Add(new wxStaticText(this, wxID_ANY, wxT("Категория:")), 0, wxALIGN_CENTER_VERTICAL);
    m_category = new wxTextCtrl(this, wxID_ANY);
    m_category->SetMinSize(wxSize(200, -1));
    grid->Add(m_category, 1, wxEXPAND);

    // Price
    grid->Add(new wxStaticText(this, wxID_ANY, wxT("Цена за ночь (?):")), 0, wxALIGN_CENTER_VERTICAL);
    m_price = new wxTextCtrl(this, wxID_ANY);
    m_price->SetMinSize(wxSize(150, -1));
    grid->Add(m_price, 1, wxEXPAND);

    topsizer->Add(grid, 1, wxALL | wxEXPAND, 15);
    topsizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
    
    SetSizerAndFit(topsizer);
    Centre();
}

wxString AddRoomDialog::getRoomNumber() const { return m_roomNumber->GetValue(); }
wxString AddRoomDialog::getCategory() const { return m_category->GetValue(); }
wxString AddRoomDialog::getPrice() const { return m_price->GetValue(); }

void AddRoomDialog::setValues(int roomNumber, const wxString& category, Money price)
{
    m_roomNumber->SetValue(wxString::Format(wxT("%d"), roomNumber));
    m_category->SetValue(category);
    m_price->SetValue(wxString::FromUTF8(price.ToString().c_str()));
}
