#define _CRT_SECURE_NO_WARNINGS

#include "MainFrame.h"
#include "Client.h"
#include "Room.h"
#include "Booking.h"
#include "IdGenerator.h"
#include "Persistence.h"
#include "AddClientDialog.h"
#include "AddRoomDialog.h"
#include "AddBookingDialog.h"
#include <wx/listctrl.h>
#include <wx/textdlg.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/scrolwin.h>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <ctime>

// ============================================================================
// IDs
// ============================================================================
enum IDs {
    ID_AddClient = wxID_HIGHEST + 1,
    ID_EditClient,
    ID_DeleteClient,
    ID_AddRoom,
    ID_EditRoom,
    ID_DeleteRoom,
    ID_ChangeRoomStatus,
    ID_AddAmenity,
    ID_AddBooking,
    ID_DeleteBooking,
    ID_CheckIn,
    ID_CheckOut,
    ID_ExportCSV,
    ID_ImportCSV,
    ID_Help,
    ID_ListOfClients,
    ID_ListOfRooms,
    ID_ListOfBookings,
    ID_AutoSaveTimer
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_AddClient, MainFrame::OnAddClient)
    EVT_BUTTON(ID_EditClient, MainFrame::OnEditClient)
    EVT_BUTTON(ID_DeleteClient, MainFrame::OnDeleteClient)
    EVT_BUTTON(ID_AddRoom, MainFrame::OnAddRoom)
  EVT_BUTTON(ID_EditRoom, MainFrame::OnEditRoom)
    EVT_BUTTON(ID_DeleteRoom, MainFrame::OnDeleteRoom)
    EVT_BUTTON(ID_ChangeRoomStatus, MainFrame::OnChangeRoomStatus)
    EVT_BUTTON(ID_AddAmenity, MainFrame::OnAddAmenity)
    EVT_BUTTON(ID_AddBooking, MainFrame::OnAddBooking)
    EVT_BUTTON(ID_DeleteBooking, MainFrame::OnDeleteBooking)
  EVT_BUTTON(ID_CheckIn, MainFrame::OnCheckIn)
    EVT_BUTTON(ID_CheckOut, MainFrame::OnCheckOut)
    EVT_BUTTON(ID_ExportCSV, MainFrame::OnExportCSV)
    EVT_BUTTON(ID_ImportCSV, MainFrame::OnImportCSV)
    EVT_BUTTON(ID_Help, MainFrame::OnHelp)
  EVT_LIST_ITEM_ACTIVATED(ID_ListOfClients, MainFrame::OnClientDblClick)
    EVT_LISTBOX_DCLICK(ID_ListOfRooms, MainFrame::OnRoomDblClick)
    EVT_LISTBOX_DCLICK(ID_ListOfBookings, MainFrame::OnBookingDblClick)
    EVT_TIMER(ID_AutoSaveTimer, MainFrame::OnAutoSaveTimer)
    EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE()

// ============================================================================
// Helper: Create styled button (компактный размер)
// ============================================================================
static wxButton* CreateStyledButton(wxWindow* parent, wxWindowID id, const wxString& label,
     const wxColour& bgColor = wxNullColour,
            const wxColour& fgColor = *wxBLACK)
{
    wxButton* btn = new wxButton(parent, id, label, wxDefaultPosition, wxSize(-1, 28));
if (bgColor.IsOk()) {
        btn->SetBackgroundColour(bgColor);
    }
    btn->SetForegroundColour(fgColor);
    return btn;
}

// ============================================================================
// Helper: Create section with title
// ============================================================================
static wxStaticBoxSizer* CreateSection(wxWindow* parent, const wxString& title, wxOrientation orient = wxVERTICAL)
{
    wxStaticBox* box = new wxStaticBox(parent, wxID_ANY, title);
    wxFont font = box->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    box->SetFont(font);
    return new wxStaticBoxSizer(box, orient);
}

// ============================================================================
// Constructor
// ============================================================================
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800))
{
    SetMinSize(wxSize(900, 600));
    
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY);
    mainPanel->SetBackgroundColour(wxColour(245, 245, 250));
    
    // ========== SCROLLABLE TOOLBAR PANEL (Left side) ==========
    wxScrolledWindow* toolbarScroll = new wxScrolledWindow(mainPanel, wxID_ANY, 
        wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    toolbarScroll->SetBackgroundColour(wxColour(52, 73, 94));
 toolbarScroll->SetMinSize(wxSize(210, -1));
    toolbarScroll->SetScrollRate(0, 10);  // Только вертикальная прокрутка
    
    wxBoxSizer* toolbarSizer = new wxBoxSizer(wxVERTICAL);
    
    // App title
    wxStaticText* appTitle = new wxStaticText(toolbarScroll, wxID_ANY, wxT("Гостиница"));
    wxFont titleFont = appTitle->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    appTitle->SetFont(titleFont);
    appTitle->SetForegroundColour(*wxWHITE);
    toolbarSizer->Add(appTitle, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
    
    wxStaticText* appSubtitle = new wxStaticText(toolbarScroll, wxID_ANY, wxT("Система управления"));
appSubtitle->SetForegroundColour(wxColour(189, 195, 199));
    toolbarSizer->Add(appSubtitle, 0, wxBOTTOM | wxALIGN_CENTER_HORIZONTAL, 10);
 
    toolbarSizer->Add(new wxStaticLine(toolbarScroll, wxID_ANY, wxDefaultPosition, wxSize(-1, 1)), 
     0, wxEXPAND | wxLEFT | wxRIGHT, 8);
    toolbarSizer->AddSpacer(8);
  
    // --- CLIENTS section ---
    wxStaticText* clientsHeader = new wxStaticText(toolbarScroll, wxID_ANY, wxT("КЛИЕНТЫ"));
    clientsHeader->SetForegroundColour(wxColour(149, 165, 166));
    wxFont headerFont = clientsHeader->GetFont();
    headerFont.SetPointSize(8);
    clientsHeader->SetFont(headerFont);
    toolbarSizer->Add(clientsHeader, 0, wxLEFT | wxBOTTOM, 8);
    
    wxButton* btnAddClient = CreateStyledButton(toolbarScroll, ID_AddClient, wxT("+ Добавить клиента"), wxColour(46, 204, 113), *wxWHITE);
    wxButton* btnEditClient = CreateStyledButton(toolbarScroll, ID_EditClient, wxT("Редактировать"));
    wxButton* btnDelClient = CreateStyledButton(toolbarScroll, ID_DeleteClient, wxT("Удалить"), wxColour(231, 76, 60), *wxWHITE);
    
    toolbarSizer->Add(btnAddClient, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnEditClient, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
toolbarSizer->Add(btnDelClient, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
  
    // --- ROOMS section ---
    wxStaticText* roomsHeader = new wxStaticText(toolbarScroll, wxID_ANY, wxT("КОМНАТЫ"));
    roomsHeader->SetForegroundColour(wxColour(149, 165, 166));
  roomsHeader->SetFont(headerFont);
    toolbarSizer->Add(roomsHeader, 0, wxLEFT | wxBOTTOM, 8);
    
    wxButton* btnAddRoom = CreateStyledButton(toolbarScroll, ID_AddRoom, wxT("+ Добавить комнату"), wxColour(46, 204, 113), *wxWHITE);
    wxButton* btnEditRoom = CreateStyledButton(toolbarScroll, ID_EditRoom, wxT("Редактировать"));
    wxButton* btnDelRoom = CreateStyledButton(toolbarScroll, ID_DeleteRoom, wxT("Удалить"), wxColour(231, 76, 60), *wxWHITE);
    wxButton* btnStatus = CreateStyledButton(toolbarScroll, ID_ChangeRoomStatus, wxT("Изменить статус"));
    wxButton* btnAmenity = CreateStyledButton(toolbarScroll, ID_AddAmenity, wxT("Добавить удобства"));
    
    toolbarSizer->Add(btnAddRoom, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
  toolbarSizer->Add(btnEditRoom, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnDelRoom, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnStatus, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnAmenity, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    
    // --- BOOKINGS section ---
    wxStaticText* bookingsHeader = new wxStaticText(toolbarScroll, wxID_ANY, wxT("БРОНИРОВАНИЯ"));
    bookingsHeader->SetForegroundColour(wxColour(149, 165, 166));
    bookingsHeader->SetFont(headerFont);
    toolbarSizer->Add(bookingsHeader, 0, wxLEFT | wxBOTTOM, 8);
    
    wxButton* btnAddBooking = CreateStyledButton(toolbarScroll, ID_AddBooking, wxT("+ Новое бронирование"), wxColour(52, 152, 219), *wxWHITE);
    wxButton* btnDelBooking = CreateStyledButton(toolbarScroll, ID_DeleteBooking, wxT("Удалить бронирование"));
 wxButton* btnCheckIn = CreateStyledButton(toolbarScroll, ID_CheckIn, wxT("Заселение"), wxColour(39, 174, 96), *wxWHITE);
  wxButton* btnCheckOut = CreateStyledButton(toolbarScroll, ID_CheckOut, wxT("Выселение"), wxColour(230, 126, 34), *wxWHITE);
    
  toolbarSizer->Add(btnAddBooking, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnDelBooking, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnCheckIn, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnCheckOut, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    
    // --- IMPORT/EXPORT section ---
    toolbarSizer->Add(new wxStaticLine(toolbarScroll, wxID_ANY, wxDefaultPosition, wxSize(-1, 1)), 
        0, wxEXPAND | wxLEFT | wxRIGHT, 8);
    toolbarSizer->AddSpacer(8);
    
    wxStaticText* dataHeader = new wxStaticText(toolbarScroll, wxID_ANY, wxT("ДАННЫЕ"));
    dataHeader->SetForegroundColour(wxColour(149, 165, 166));
    dataHeader->SetFont(headerFont);
    toolbarSizer->Add(dataHeader, 0, wxLEFT | wxBOTTOM, 8);
    
    wxButton* btnExport = CreateStyledButton(toolbarScroll, ID_ExportCSV, wxT("Экспорт CSV"));
    wxButton* btnImport = CreateStyledButton(toolbarScroll, ID_ImportCSV, wxT("Импорт CSV"));
    wxButton* btnHelp = CreateStyledButton(toolbarScroll, ID_Help, wxT("? Справка"));
    
    toolbarSizer->Add(btnExport, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnImport, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    toolbarSizer->Add(btnHelp, 0, wxEXPAND | wxALL, 5);
    
    toolbarScroll->SetSizer(toolbarSizer);
    toolbarScroll->FitInside();  // Вычислить размер содержимого для прокрутки
    
    // ========== CONTENT PANEL ==========
    wxPanel* contentPanel = new wxPanel(mainPanel, wxID_ANY);
    contentPanel->SetBackgroundColour(wxColour(245, 245, 250));
    
    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topRowSizer = new wxBoxSizer(wxHORIZONTAL);
 
    // --- Clients panel ---
    wxStaticBoxSizer* clientsSection = CreateSection(contentPanel, wxT(" Клиенты (Ctrl+Click для выбора нескольких) "));
    
    listOfClients = new wxListCtrl(clientsSection->GetStaticBox(), ID_ListOfClients,
   wxDefaultPosition, wxDefaultSize,
           wxLC_REPORT | wxBORDER_NONE);
    listOfClients->SetBackgroundColour(*wxWHITE);
    
listOfClients->InsertColumn(0, wxT("ID"), wxLIST_FORMAT_LEFT, 50);
    listOfClients->InsertColumn(1, wxT("Имя"), wxLIST_FORMAT_LEFT, 100);
    listOfClients->InsertColumn(2, wxT("Отчество"), wxLIST_FORMAT_LEFT, 120);
    listOfClients->InsertColumn(3, wxT("Фамилия"), wxLIST_FORMAT_LEFT, 120);
    listOfClients->InsertColumn(4, wxT("Телефон"), wxLIST_FORMAT_LEFT, 120);
    listOfClients->InsertColumn(5, wxT("Тип"), wxLIST_FORMAT_LEFT, 80);
    
    clientsSection->Add(listOfClients, 1, wxEXPAND | wxALL, 5);
    topRowSizer->Add(clientsSection, 1, wxEXPAND | wxALL, 5);
    
    // --- Rooms panel ---
    wxStaticBoxSizer* roomsSection = CreateSection(contentPanel, wxT(" Комнаты "));
    
    listOfRooms = new wxListCtrl(roomsSection->GetStaticBox(), ID_ListOfRooms,
          wxDefaultPosition, wxDefaultSize,
      wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    listOfRooms->SetBackgroundColour(*wxWHITE);
    
    listOfRooms->InsertColumn(0, wxT("№"), wxLIST_FORMAT_LEFT, 50);
    listOfRooms->InsertColumn(1, wxT("Категория"), wxLIST_FORMAT_LEFT, 120);
    listOfRooms->InsertColumn(2, wxT("Цена/ночь"), wxLIST_FORMAT_RIGHT, 100);
    listOfRooms->InsertColumn(3, wxT("Статус"), wxLIST_FORMAT_LEFT, 110);
  listOfRooms->InsertColumn(4, wxT("Удобства"), wxLIST_FORMAT_LEFT, 150);
    
    roomsSection->Add(listOfRooms, 1, wxEXPAND | wxALL, 5);
    topRowSizer->Add(roomsSection, 1, wxEXPAND | wxALL, 5);
    
    contentSizer->Add(topRowSizer, 1, wxEXPAND);
    
 // --- Bookings panel ---
    wxStaticBoxSizer* bookingsSection = CreateSection(contentPanel, wxT(" Бронирования "));
  
    listOfBookings = new wxListCtrl(bookingsSection->GetStaticBox(), ID_ListOfBookings,
            wxDefaultPosition, wxDefaultSize,
  wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    listOfBookings->SetBackgroundColour(*wxWHITE);
    
    listOfBookings->InsertColumn(0, wxT("ID"), wxLIST_FORMAT_LEFT, 50);
    listOfBookings->InsertColumn(1, wxT("Комната"), wxLIST_FORMAT_LEFT, 80);
    listOfBookings->InsertColumn(2, wxT("Гости"), wxLIST_FORMAT_LEFT, 250);
    listOfBookings->InsertColumn(3, wxT("Заезд"), wxLIST_FORMAT_LEFT, 100);
    listOfBookings->InsertColumn(4, wxT("Выезд"), wxLIST_FORMAT_LEFT, 100);
    listOfBookings->InsertColumn(5, wxT("Статус"), wxLIST_FORMAT_LEFT, 120);
    listOfBookings->InsertColumn(6, wxT("Сумма"), wxLIST_FORMAT_RIGHT, 100);
    
    bookingsSection->Add(listOfBookings, 1, wxEXPAND | wxALL, 5);
    contentSizer->Add(bookingsSection, 1, wxEXPAND | wxALL, 5);
    
    contentPanel->SetSizer(contentSizer);
    
    // ========== MAIN LAYOUT ==========
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(toolbarScroll, 0, wxEXPAND);
    mainSizer->Add(contentPanel, 1, wxEXPAND);
    mainPanel->SetSizer(mainSizer);
    
    // ========== STATUS BAR ==========
    CreateStatusBar(4);
    SetStatusText(wxT("Готово"), 0);
    
    // ========== LOAD DATA ==========
    std::vector<Client> tclients;
    std::vector<Room> trooms;
    std::vector<Booking> tbookings;
    int nextC = IdGenerator::getNextClientId();
    int nextR = IdGenerator::getNextRoomId();
    int nextB = IdGenerator::getNextBookingId();
    std::string loadErr;
    
    if (LoadData("data.json", tclients, trooms, tbookings, nextC, nextR, nextB, loadErr)) {
     clients = std::move(tclients);
    rooms = std::move(trooms);
   bookings = std::move(tbookings);
    IdGenerator::setNextIds(nextC, nextR, nextB);
    } else {
        if (!loadErr.empty()) {
      wxLogMessage(wxT("Примечание: %s"), wxString::FromUTF8(loadErr.c_str()));
        }
    }
    
    refreshClientsList();
    refreshRoomsList();
    refreshBookingsList();
    updateStatusBar();
    
// ========== AUTO-SAVE TIMER ==========
 m_autoSaveTimer = new wxTimer(this, ID_AutoSaveTimer);
    m_autoSaveTimer->Start(AUTOSAVE_INTERVAL_MS);
    m_dataChanged = false;
    
    Centre();
}

// ============================================================================
// Destructor
// ============================================================================
MainFrame::~MainFrame()
{
    // Остановка таймера
    if (m_autoSaveTimer) {
 m_autoSaveTimer->Stop();
 }
    SaveDataNow();
}

// ============================================================================
// OnClose
// ============================================================================
void MainFrame::OnClose(wxCloseEvent& event)
{
    if (m_autoSaveTimer) {
 m_autoSaveTimer->Stop();
 }
    SaveDataNow();
    event.Skip();
}

// ============================================================================
// Auto-save timer handler
// ============================================================================
void MainFrame::OnAutoSaveTimer(wxTimerEvent& event)
{
 if (m_dataChanged) {
        SaveDataNow();
        m_dataChanged = false;
    }
}

// ============================================================================
// Save data immediately
// ============================================================================
void MainFrame::SaveDataNow()
{
    if (!SaveData("data.json", clients, rooms, bookings,
         IdGenerator::getNextClientId(), IdGenerator::getNextRoomId(), IdGenerator::getNextBookingId())) {
        wxLogError(wxT("Не удалось сохранить данные"));
    } else {
   // Обновляем время последнего сохранения в статус-баре
        time_t now = time(nullptr);
        tm* local = localtime(&now);
        wxString timeStr = wxString::Format(wxT("Сохранено: %02d:%02d:%02d"), 
     local->tm_hour, local->tm_min, local->tm_sec);
        SetStatusText(timeStr, 3);
    }
}

// ============================================================================
// Mark data as changed (triggers auto-save on next timer tick)
// ============================================================================
void MainFrame::MarkDataChanged()
{
    m_dataChanged = true;
    // Немедленное сохранение для критичных операций
    SaveDataNow();
}

// ============================================================================
// Update status bar
// ============================================================================
void MainFrame::updateStatusBar()
{
 int activeClients = 0, activeRooms = 0, activeBookings = 0;
    for (const auto& c : clients) if (c.isActive()) activeClients++;
    for (const auto& r : rooms) if (r.isActive()) activeRooms++;
    for (const auto& b : bookings) if (b.isActive()) activeBookings++;
 
    SetStatusText(wxString::Format(wxT("Клиентов: %d"), activeClients), 1);
    SetStatusText(wxString::Format(wxT("Комнат: %d | Бронирований: %d"), activeRooms, activeBookings), 2);
}

// ============================================================================
// Double-click handlers
// ============================================================================
void MainFrame::OnClientDblClick(wxListEvent& event)
{
    wxCommandEvent evt;
  OnEditClient(evt);
}

void MainFrame::OnRoomDblClick(wxCommandEvent& event)
{
    wxCommandEvent evt;
  OnEditRoom(evt);
}

void MainFrame::OnBookingDblClick(wxCommandEvent& event)
{
    // Could show booking details dialog
}

// ============================================================================
// Refresh lists
// ============================================================================
void MainFrame::refreshClientsList()
{
    if (!listOfClients) return;
    listOfClients->DeleteAllItems();
    
    long index = 0;
    for (const auto& client : clients) {
        if (!client.isActive()) continue;
        
    long pos = listOfClients->InsertItem(index, wxString::Format("%d", client.getId()));
        listOfClients->SetItem(pos, 1, wxString::FromUTF8(client.getFirstName().c_str()));
        listOfClients->SetItem(pos, 2, wxString::FromUTF8(client.getPatronymic().c_str()));
        listOfClients->SetItem(pos, 3, wxString::FromUTF8(client.getLastName().c_str()));
        listOfClients->SetItem(pos, 4, wxString::FromUTF8(client.getPhone().c_str()));
  
        wxString typeStr;
        if (client.getIsChild()) typeStr = wxT("Ребёнок");
        else if (client.getIsForeigner()) typeStr = wxT("Иностр.");
else typeStr = wxT("Взрослый");
    listOfClients->SetItem(pos, 5, typeStr);
        
        listOfClients->SetItemData(pos, static_cast<long>(client.getId()));
        ++index;
    }
    updateStatusBar();
}

void MainFrame::refreshRoomsList()
{
    if (!listOfRooms) return;
    listOfRooms->DeleteAllItems();
    
    long index = 0;
    for (const auto& r : rooms) {
     if (!r.isActive()) continue;
        
      long pos = listOfRooms->InsertItem(index, wxString::Format("%d", r.getRoomNumber()));
        listOfRooms->SetItem(pos, 1, wxString::FromUTF8(r.getCategory().c_str()));
        listOfRooms->SetItem(pos, 2, wxString::Format(wxT("%.2f"), r.getPricePerNight()));
     
        wxString statusStr = wxString::FromUTF8(RoomStatusToString(r.getStatus()).c_str());
    listOfRooms->SetItem(pos, 3, statusStr);
 
        const auto& amenities = r.getAmenities();
        wxString amenStr;
    for (size_t i = 0; i < amenities.size() && i < 3; ++i) {
       if (i > 0) amenStr += wxT(", ");
          amenStr += wxString::FromUTF8(amenities[i].c_str());
        }
        if (amenities.size() > 3) amenStr += wxT("...");
        listOfRooms->SetItem(pos, 4, amenStr);
        
     listOfRooms->SetItemData(pos, static_cast<long>(r.getId()));
        ++index;
    }
 updateStatusBar();
}

void MainFrame::refreshBookingsList()
{
    if (!listOfBookings) return;
    listOfBookings->DeleteAllItems();
    
    long index = 0;
    for (const auto& b : bookings) {
      if (!b.isActive()) continue;
      
     Room* room = findRoomById(b.getRoomId());
        wxString roomStr = room ? wxString::Format(wxT("%d"), room->getRoomNumber()) : wxT("—");
        
        std::string clientsStr;
        const auto& ids = b.getClientIds();
        for (size_t j = 0; j < ids.size(); ++j) {
  Client* c = findClientById(ids[j]);
    if (c) {
       if (!clientsStr.empty()) clientsStr += ", ";
    clientsStr += c->getFullName();
            }
     }
        if (clientsStr.empty()) clientsStr = "—";
    
 wxString statusStr;
      switch (b.getStatus()) {
         case BookingStatus::CONFIRMED: statusStr = wxT("Подтверждено"); break;
            case BookingStatus::CHECKED_IN: statusStr = wxT("Заселён"); break;
 case BookingStatus::COMPLETED: statusStr = wxT("Завершено"); break;
  case BookingStatus::CANCELLED: statusStr = wxT("Отменено"); break;
        default: statusStr = wxT("—"); break;
        }
        
        long pos = listOfBookings->InsertItem(index, wxString::Format("%d", b.getId()));
        listOfBookings->SetItem(pos, 1, roomStr);
        listOfBookings->SetItem(pos, 2, wxString::FromUTF8(clientsStr.c_str()));
    listOfBookings->SetItem(pos, 3, wxString::FromUTF8(b.getCheckInDate().toString().c_str()));
        listOfBookings->SetItem(pos, 4, wxString::FromUTF8(b.getCheckOutDate().toString().c_str()));
listOfBookings->SetItem(pos, 5, statusStr);
        listOfBookings->SetItem(pos, 6, wxString::Format(wxT("%.2f"), b.getTotalPrice()));
        
        listOfBookings->SetItemData(pos, static_cast<long>(b.getId()));
        ++index;
    }
    updateStatusBar();
}

// ============================================================================
// Find helpers
// ============================================================================
Room* MainFrame::findRoomById(int roomId)
{
    for (auto& room : rooms) {
        if (room.getId() == roomId) return &room;
    }
    return nullptr;
}

Client* MainFrame::findClientById(int clientId)
{
    for (auto& client : clients) {
        if (client.getId() == clientId) return &client;
    }
    return nullptr;
}

// ============================================================================
// Add Client
// ============================================================================
void MainFrame::OnAddClient(wxCommandEvent& event)
{
    AddClientDialog dlg(this);
    if (dlg.ShowModal() != wxID_OK) return;
    
    std::string first = std::string(dlg.getFirstName().ToUTF8().data());
    std::string last = std::string(dlg.getLastName().ToUTF8().data());
    std::string phone = std::string(dlg.getPhone().ToUTF8().data());
    std::string patronymic = std::string(dlg.getPatronymic().ToUTF8().data());
    Passport passport = dlg.getPassport();
    
  bool isChild = dlg.isChild();
    bool isForeigner = dlg.isForeigner();
    std::string birthCert = dlg.getBirthCertificate();
    std::string visa = dlg.getVisa();
    std::string intlPass = dlg.getInternationalPassport();
 
    int clientId = IdGenerator::generateClientId();
    clients.emplace_back(clientId, first, last, phone, passport, true, isChild, isForeigner, birthCert, visa, intlPass, patronymic);
    
    refreshClientsList();
    MarkDataChanged();  // Сохранение!
 SetStatusText(wxT("Клиент добавлен"), 0);
}

// ============================================================================
// Edit Client
// ============================================================================
void MainFrame::OnEditClient(wxCommandEvent& event)
{
    if (!listOfClients) return;
    
    long sel = listOfClients->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
        wxMessageBox(wxT("Пожалуйста, выберите клиента для редактирования."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
}
  
    int clientId = static_cast<int>(listOfClients->GetItemData(sel));
    Client* client = findClientById(clientId);
    if (!client) { wxLogError(wxT("Клиент не найден")); return; }
    
    AddClientDialog dlg(this);
    dlg.setValues(*client);
    if (dlg.ShowModal() != wxID_OK) return;
 
    client->setFirstName(std::string(dlg.getFirstName().ToUTF8().data()));
    client->setLastName(std::string(dlg.getLastName().ToUTF8().data()));
    client->setPhone(std::string(dlg.getPhone().ToUTF8().data()));
    client->setPatronymic(std::string(dlg.getPatronymic().ToUTF8().data()));
    client->setPassport(dlg.getPassport());
    client->setIsChild(dlg.isChild());
    client->setIsForeigner(dlg.isForeigner());
    client->setBirthCertificate(dlg.getBirthCertificate());
    client->setVisa(dlg.getVisa());
    client->setInternationalPassport(dlg.getInternationalPassport());
    
    refreshClientsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Клиент обновлён"), 0);
}

// ============================================================================
// Delete Client
// ============================================================================
void MainFrame::OnDeleteClient(wxCommandEvent& event)
{
    if (!listOfClients) return;
    
    long sel = listOfClients->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
wxMessageBox(wxT("Пожалуйста, выберите клиента для удаления."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }

 int clientId = static_cast<int>(listOfClients->GetItemData(sel));
    
    std::vector<int> blocking;
    for (const auto& b : bookings) {
        if (!b.isActive()) continue;
  const auto& ids = b.getClientIds();
      if (std::find(ids.begin(), ids.end(), clientId) != ids.end()) {
       auto st = b.getStatus();
            if (st == BookingStatus::CONFIRMED || st == BookingStatus::CHECKED_IN) {
     blocking.push_back(b.getId());
            }
        }
    }
    
    if (!blocking.empty()) {
        wxString msg = wxT("Клиента нельзя удалить — есть активные бронирования:\n");
        for (int id : blocking) msg += wxString::Format(wxT("  Бронирование #%d\n"), id);
        wxMessageBox(msg, wxT("Ошибка удаления"), wxOK | wxICON_ERROR, this);
        return;
    }
 
    if (wxMessageBox(wxT("Подтвердить удаление клиента?"), wxT("Подтверждение"), wxYES_NO | wxICON_QUESTION, this) != wxYES) {
        return;
    }
    
    for (auto it = clients.begin(); it != clients.end(); ) {
        if (it->getId() == clientId) {
            it = clients.erase(it);
 } else {
    ++it;
      }
    }
    
    for (auto it = bookings.begin(); it != bookings.end(); ) {
        const auto& ids = it->getClientIds();
    if (std::find(ids.begin(), ids.end(), clientId) != ids.end()) {
            it = bookings.erase(it);
        } else {
            ++it;
        }
    }
    
    refreshClientsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Клиент удалён"), 0);
}

// ============================================================================
// Add Room
// ============================================================================
void MainFrame::OnAddRoom(wxCommandEvent& event)
{
    AddRoomDialog dlg(this);
    if (dlg.ShowModal() != wxID_OK) return;
    
 wxString wxRoomNumber = dlg.getRoomNumber();
 wxString wxCategory = dlg.getCategory();
    wxString wxPrice = dlg.getPrice();
    
    long roomNumber = 0;
    double price = 0.0;
    
    if (!wxRoomNumber.ToLong(&roomNumber)) {
     wxMessageBox(wxT("Некорректный номер комнаты."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
        return;
    }
    if (!wxPrice.ToDouble(&price)) {
        wxMessageBox(wxT("Некорректная цена."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
        return;
    }
    
    std::string category = std::string(wxCategory.ToUTF8().data());
    std::vector<std::string> amenities;
 
    int roomId = IdGenerator::generateRoomId();
rooms.emplace_back(roomId, static_cast<int>(roomNumber), category, price, RoomStatus::AVAILABLE, amenities);
    
    refreshRoomsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Комната добавлена"), 0);
}

// ============================================================================
// Edit Room
// ============================================================================
void MainFrame::OnEditRoom(wxCommandEvent& event)
{
    if (!listOfRooms) return;
    
    long sel = listOfRooms->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
        wxMessageBox(wxT("Пожалуйста, выберите комнату для редактирования."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }

    int roomId = static_cast<int>(listOfRooms->GetItemData(sel));
    Room* room = findRoomById(roomId);
    if (!room) { wxLogError(wxT("Комната не найдена")); return; }
    
    AddRoomDialog dlg(this);
    dlg.setValues(room->getRoomNumber(), wxString::FromUTF8(room->getCategory().c_str()), room->getPricePerNight());
    if (dlg.ShowModal() != wxID_OK) return;
    
    long roomNumber = 0;
    double price = 0.0;
    
    if (!dlg.getRoomNumber().ToLong(&roomNumber)) {
        wxMessageBox(wxT("Некорректный номер комнаты."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
        return;
    }
    if (!dlg.getPrice().ToDouble(&price)) {
        wxMessageBox(wxT("Некорректная цена."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
        return;
    }
    
    room->setRoomNumber(static_cast<int>(roomNumber));
    room->setCategory(std::string(dlg.getCategory().ToUTF8().data()));
    room->setPricePerNight(price);
    
    refreshRoomsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Комната обновлена"), 0);
}

// ============================================================================
// Delete Room
// ============================================================================
void MainFrame::OnDeleteRoom(wxCommandEvent& event)
{
    if (!listOfRooms) return;
    
    long sel = listOfRooms->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
    wxMessageBox(wxT("Пожалуйста, выберите комнату для удаления."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    
    int roomId = static_cast<int>(listOfRooms->GetItemData(sel));
    
    std::vector<int> blocking;
    for (const auto& b : bookings) {
        if (!b.isActive()) continue;
        if (b.getRoomId() == roomId) {
  auto st = b.getStatus();
        if (st == BookingStatus::CONFIRMED || st == BookingStatus::CHECKED_IN) {
        blocking.push_back(b.getId());
    }
        }
    }
    
    if (!blocking.empty()) {
        wxString msg = wxT("Комнату нельзя удалить — есть активные бронирования:\n");
for (int id : blocking) msg += wxString::Format(wxT("  Бронирование #%d\n"), id);
 wxMessageBox(msg, wxT("Ошибка удаления"), wxOK | wxICON_ERROR, this);
      return;
    }
    
    if (wxMessageBox(wxT("Подтвердить удаление комнаты?"), wxT("Подтверждение"), wxYES_NO | wxICON_QUESTION, this) != wxYES) {
        return;
    }
    
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        if (it->getId() == roomId) {
 rooms.erase(it);
        break;
      }
    }
    
    for (auto it = bookings.begin(); it != bookings.end(); ) {
        if (it->getRoomId() == roomId) {
      it = bookings.erase(it);
        } else {
            ++it;
    }
    }
    
    refreshRoomsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Комната удалена"), 0);
}

// ============================================================================
// Change Room Status
// ============================================================================
void MainFrame::OnChangeRoomStatus(wxCommandEvent& event)
{
  if (!listOfRooms) return;
    
    long sel = listOfRooms->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
        wxMessageBox(wxT("Выберите комнату."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
      return;
    }
  
    int roomId = static_cast<int>(listOfRooms->GetItemData(sel));
    Room* room = findRoomById(roomId);
    if (!room) { wxLogError(wxT("Комната не найдена")); return; }
    
    std::vector<RoomStatus> statuses = { RoomStatus::AVAILABLE, RoomStatus::BOOKED, RoomStatus::OCCUPIED, RoomStatus::MAINTENANCE, RoomStatus::CLEANING };
    wxArrayString choices;
    for (auto s : statuses) {
   choices.Add(wxString::FromUTF8(RoomStatusToString(s).c_str()));
    }
    
    wxSingleChoiceDialog dlg(this, wxT("Выберите новый статус комнаты:"), wxT("Изменить статус"), choices);
    
    int curIndex = 0;
    for (size_t i = 0; i < statuses.size(); ++i) {
        if (statuses[i] == room->getStatus()) { curIndex = static_cast<int>(i); break; }
    }
 dlg.SetSelection(curIndex);
    
    if (dlg.ShowModal() == wxID_OK) {
        int selIdx = dlg.GetSelection();
        if (selIdx >= 0 && selIdx < static_cast<int>(statuses.size())) {
            room->setStatus(statuses[selIdx]);
 refreshRoomsList();
            MarkDataChanged();  // Сохранение!
        SetStatusText(wxT("Статус комнаты изменён"), 0);
  }
    }
}

// ============================================================================
// Add Amenity
// ============================================================================
void MainFrame::OnAddAmenity(wxCommandEvent& event)
{
    if (!listOfRooms) return;
    
    long sel = listOfRooms->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
   wxMessageBox(wxT("Пожалуйста, выберите комнату."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    
    int roomId = static_cast<int>(listOfRooms->GetItemData(sel));
    Room* room = findRoomById(roomId);
    if (!room) { wxLogError(wxT("Комната не найдена")); return; }
    
    wxTextEntryDialog dlg(this, wxT("Введите удобство для комнаты:"), wxT("Новое удобство"));
    if (dlg.ShowModal() != wxID_OK) return;
    
    std::string amenity = std::string(dlg.GetValue().ToUTF8().data());
    if (!amenity.empty()) {
      room->addAmenity(amenity);
        refreshRoomsList();
        MarkDataChanged();  // Сохранение!
      SetStatusText(wxT("Удобство добавлено"), 0);
    }
}

// ============================================================================
// Add Booking
// ============================================================================
void MainFrame::OnAddBooking(wxCommandEvent& event)
{
  long selRoom = listOfRooms->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selRoom == -1) {
    wxMessageBox(wxT("Пожалуйста, выберите комнату."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
  }
    
    std::vector<int> clientIds;
    long item = -1;
    while ((item = listOfClients->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1) {
        int clientId = static_cast<int>(listOfClients->GetItemData(item));
        Client* client = findClientById(clientId);
 if (client && client->isActive()) {
            clientIds.push_back(clientId);
        }
    }
    
    if (clientIds.empty()) {
        wxMessageBox(wxT("Пожалуйста, выберите одного или нескольких клиентов.\n\nИспользуйте Ctrl+Click для выбора нескольких гостей."), 
            wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    
    int roomId = static_cast<int>(listOfRooms->GetItemData(selRoom));
    Room* room = findRoomById(roomId);
    if (!room) { wxLogError(wxT("Комната не найдена")); return; }
    
    AddBookingDialog dlg(this);
if (dlg.ShowModal() != wxID_OK) return;
    
    Date ci = dlg.getCheckIn();
    Date co = dlg.getCheckOut();
    
    if (!ci.isValid() || !co.isValid() || !ci.isBefore(co)) {
 wxMessageBox(wxT("Неверные даты бронирования."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
        return;
    }
    
 for (const auto& b : bookings) {
        if (!b.isActive()) continue;
      if (b.getRoomId() != roomId) continue;
      if (Booking::datesOverlap(ci, co, b.getCheckInDate(), b.getCheckOutDate())) {
      wxMessageBox(wxT("Выбранные даты пересекаются с существующим бронированием."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
            return;
        }
    }
    
    if (room->getStatus() == RoomStatus::MAINTENANCE) {
        wxMessageBox(wxT("Комната на техническом обслуживании."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
     return;
    }
    
    int days = std::abs(ci.DifferenceInDays(co));
    double total = days * room->getPricePerNight();
    
    int bookingId = IdGenerator::generateBookingId();
 bookings.emplace_back(bookingId, room->getId(), clientIds, ci, co, true);
 bookings.back().setTotalPrice(total);
    
    if (room->canTransition(RoomStatus::BOOKED)) {
        room->setStatus(RoomStatus::BOOKED);
    }
    updateRoomStatusBasedOnBookings(room->getId());
    
 refreshRoomsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Бронирование создано"), 0);
    
    wxString guestsInfo;
    for (size_t i = 0; i < clientIds.size(); ++i) {
        Client* c = findClientById(clientIds[i]);
        if (c) {
 if (!guestsInfo.IsEmpty()) guestsInfo += wxT(", ");
         guestsInfo += wxString::FromUTF8(c->getFullName().c_str());
        }
  }
    
    wxMessageBox(wxString::Format(wxT("Бронирование создано!\n\nГости (%zu): %s\nСумма: %.2f"), 
     clientIds.size(), guestsInfo, total), 
 wxT("Успешно"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// Delete Booking
// ============================================================================
void MainFrame::OnDeleteBooking(wxCommandEvent& event)
{
    if (!listOfBookings) return;
    
    long sel = listOfBookings->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
  wxMessageBox(wxT("Пожалуйста, выберите бронирование для удаления."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    
    int bookingId = static_cast<int>(listOfBookings->GetItemData(sel));
    
    int deletedRoomId = -1;
    for (auto& b : bookings) {
        if (b.getId() == bookingId) {
            deletedRoomId = b.getRoomId();
   b.setActive(false);
     break;
 }
    }
    
 if (deletedRoomId != -1) {
        updateRoomStatusBasedOnBookings(deletedRoomId);
    }
    
    refreshRoomsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Бронирование удалено"), 0);
}

// ============================================================================
// Check-In
// ============================================================================
void MainFrame::OnCheckIn(wxCommandEvent& event)
{
    if (!listOfBookings) return;
    
    long sel = listOfBookings->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (sel == -1) {
        wxMessageBox(wxT("Пожалуйста, выберите бронирование для заселения."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    
    int bookingId = static_cast<int>(listOfBookings->GetItemData(sel));
    Booking* booking = nullptr;
    for (auto& b : bookings) {
        if (b.getId() == bookingId) { booking = &b; break; }
    }
    
    if (!booking) { wxLogError(wxT("Бронирование не найдено")); return; }
    if (!booking->isActive()) { wxMessageBox(wxT("Бронирование не активно."), wxT("Ошибка"), wxOK | wxICON_ERROR, this); return; }
    if (booking->getStatus() == BookingStatus::CHECKED_IN) { wxMessageBox(wxT("Клиент уже заселён."), wxT("Информация"), wxOK | wxICON_INFORMATION, this); return; }
    if (booking->getStatus() == BookingStatus::CANCELLED || booking->getStatus() == BookingStatus::COMPLETED) {
        wxMessageBox(wxT("Нельзя заселить по отменённой/завершённой броне."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
    return;
    }
    
    time_t t = time(nullptr);
    tm local = *localtime(&t);
    Date today(local.tm_mday, local.tm_mon + 1, local.tm_year + 1900);
    
    if (today.isBefore(booking->getCheckInDate())) {
        wxMessageBox(wxT("Дата заезда ещё не наступила."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
        return;
    }
 
    Room* room = findRoomById(booking->getRoomId());
 if (room && room->getStatus() == RoomStatus::MAINTENANCE) {
        wxMessageBox(wxT("Комната на техническом обслуживании."), wxT("Ошибка"), wxOK | wxICON_ERROR, this);
     return;
    }
    
    if (wxMessageBox(wxT("Подтвердить заселение?"), wxT("Подтверждение"), wxYES_NO | wxICON_QUESTION, this) != wxYES) {
        return;
    }
    
    booking->setStatus(BookingStatus::CHECKED_IN);
    if (room) {
        room->setStatus(RoomStatus::OCCUPIED);
    }
    
    refreshRoomsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Заселение выполнено"), 0);
    wxMessageBox(wxT("Заселение выполнено успешно!"), wxT("Успешно"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// Check-Out
// ============================================================================
void MainFrame::OnCheckOut(wxCommandEvent& event)
{
    if (!listOfBookings) return;
    
    long sel = listOfBookings->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) {
        wxMessageBox(wxT("Пожалуйста, выберите бронирование для выселения."), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
        return;
    }
    
    int bookingId = static_cast<int>(listOfBookings->GetItemData(sel));
    Booking* booking = nullptr;
    for (auto& b : bookings) {
 if (b.getId() == bookingId) { booking = &b; break; }
    }
    
    if (!booking) { wxLogError(wxT("Бронирование не найдено")); return; }
    if (!booking->isActive()) { wxMessageBox(wxT("Бронирование не активно."), wxT("Ошибка"), wxOK | wxICON_ERROR, this); return; }
    if (booking->getStatus() == BookingStatus::COMPLETED) { wxMessageBox(wxT("Бронирование уже завершено."), wxT("Информация"), wxOK | wxICON_INFORMATION, this); return; }
    if (booking->getStatus() == BookingStatus::CANCELLED) { wxMessageBox(wxT("Нельзя выселить по отменённой броне."), wxT("Ошибка"), wxOK | wxICON_ERROR, this); return; }
    
    if (wxMessageBox(wxT("Подтвердить выселение?"), wxT("Подтверждение"), wxYES_NO | wxICON_QUESTION, this) != wxYES) {
        return;
    }
    
    booking->setStatus(BookingStatus::COMPLETED);
    Room* room = findRoomById(booking->getRoomId());
    if (room) {
        room->setStatus(RoomStatus::CLEANING);
    }
    
    refreshRoomsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    SetStatusText(wxT("Выселение выполнено"), 0);
    wxMessageBox(wxT("Выселение выполнено успешно!"), wxT("Успешно"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// Export CSV
// ============================================================================
void MainFrame::OnExportCSV(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Выберите папку для экспорта"),
    wxEmptyString, "clients.csv",
             "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
 
    if (dlg.ShowModal() != wxID_OK) return;
    
    wxFileName fn(dlg.GetPath());
    wxString wxdir = fn.GetPath();
    std::string dir = std::string(wxdir.ToUTF8().data());
    std::string err;
    
    if (!ExportToCSV(dir, clients, rooms, bookings, err)) {
        wxMessageBox(wxString::FromUTF8(err.c_str()), wxT("Ошибка экспорта"), wxOK | wxICON_ERROR, this);
        return;
    }
    
    wxString openCmd = wxString::Format("explorer \"%s\"", wxdir);
    wxExecute(openCmd);
    
    SetStatusText(wxT("Экспорт завершён"), 0);
    wxMessageBox(wxT("Экспорт завершён успешно!"), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// Import CSV
// ============================================================================
void MainFrame::OnImportCSV(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Выберите CSV-файл из папки"),
           wxEmptyString, wxEmptyString,
   "CSV files (*.csv)|*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (dlg.ShowModal() != wxID_OK) return;
    
 wxFileName fn(dlg.GetPath());
    wxString wxdir = fn.GetPath();
 std::string dir = std::string(wxdir.ToUTF8().data());
    
    std::string err;
 int nextC = IdGenerator::getNextClientId();
    int nextR = IdGenerator::getNextRoomId();
    int nextB = IdGenerator::getNextBookingId();
    
    if (!ImportFromCSV(dir, clients, rooms, bookings, nextC, nextR, nextB, err)) {
        wxMessageBox(wxString::FromUTF8(err.c_str()), wxT("Ошибка импорта"), wxOK | wxICON_ERROR, this);
        return;
    }
    
    IdGenerator::setNextIds(nextC, nextR, nextB);
    refreshClientsList();
    refreshRoomsList();
    refreshBookingsList();
    MarkDataChanged();  // Сохранение!
    
    SetStatusText(wxT("Импорт завершён"), 0);
    wxMessageBox(wxT("Импорт завершён успешно!"), wxT("Информация"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// Help
// ============================================================================
void MainFrame::OnHelp(wxCommandEvent& event)
{
    wxString helpText =
        wxT("СПРАВКА ПО ПРИЛОЖЕНИЮ\n")
        wxT("БЫСТРЫЙ СТАРТ:\n")
 wxT("1. Добавьте клиентов (кнопка '+ Добавить клиента')\n")
   wxT("2. Добавьте комнаты (кнопка '+ Добавить комнату')\n")
        wxT("3. Выберите клиентов и комнату, создайте бронирование\n")
     wxT("4. Используйте 'Заселение' и 'Выселение' для управления\n\n")
      wxT("МНОЖЕСТВЕННЫЙ ВЫБОР ГОСТЕЙ:\n")
        wxT("• Ctrl + Click — добавить/убрать клиента\n")
        wxT("• Shift + Click — выбрать диапазон клиентов\n")
        wxT("• Можно заселить нескольких гостей в одну комнату\n\n")
        wxT("АВТОСОХРАНЕНИЕ:\n")
        wxT("• Данные сохраняются автоматически каждые 30 сек\n")
 wxT("• Также сохраняются после каждого изменения\n")
        wxT("• Время последнего сохранения — в статус-баре\n\n")
      wxT("ГОРЯЧИЕ КЛАВИШИ:\n")
        wxT("• Двойной клик — редактирование записи\n\n")
        wxT("═══════════════════════════════════════\n")
        wxT("Версия 1.1 | © 2024");
    
    wxMessageBox(helpText, wxT("Справка"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// Update room status based on bookings
// ============================================================================
void MainFrame::updateRoomStatusBasedOnBookings(int roomId)
{
    Room* room = findRoomById(roomId);
    if (!room) return;
    
    time_t t = time(nullptr);
    tm local = *localtime(&t);
    Date today(local.tm_mday, local.tm_mon + 1, local.tm_year + 1900);
    
  if (room->getStatus() == RoomStatus::MAINTENANCE) return;
    
    bool hasCheckedIn = false;
    bool hasFutureConfirmed = false;
    bool hasCompleted = false;
    
for (const auto& b : bookings) {
        if (!b.isActive()) continue;
  if (b.getRoomId() != roomId) continue;
        
      if (b.getStatus() == BookingStatus::CHECKED_IN) {
   hasCheckedIn = true;
      }
        if (b.getStatus() == BookingStatus::CONFIRMED) {
 if (b.getCheckInDate().equals(today) || b.getCheckInDate().isAfter(today)) {
 hasFutureConfirmed = true;
     }
        }
     if (b.getStatus() == BookingStatus::COMPLETED) {
    hasCompleted = true;
        }
    }
    
    if (hasCheckedIn) {
  room->setStatus(RoomStatus::OCCUPIED);
    } else if (hasFutureConfirmed) {
      room->setStatus(RoomStatus::BOOKED);
    } else if (hasCompleted) {
        room->setStatus(RoomStatus::CLEANING);
    } else {
        room->setStatus(RoomStatus::AVAILABLE);
    }
}