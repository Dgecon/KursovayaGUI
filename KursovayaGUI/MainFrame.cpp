#define _CRT_SECURE_NO_WARNINGS

#include "Client.h"
#include "MainFrame.h"
#include "Room.h"
#include "Booking.h"
#include "IdGenerator.h"
#include "Persistence.h"
#include "AddClientDialog.h"
#include "AddRoomDialog.h"
#include "AddBookingDialog.h"
#include <vector>
#include <wx/listbox.h>
#include <wx/textdlg.h>
#include <wx/wx.h>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <wx/filename.h>

enum IDs {
    ID_AddClient = 1,
    ID_ListOfClients = 2,
    ID_ListOfRooms = 3,
    ID_AddRoom = 4,
    ID_AddAmenity = 5,
    ID_ListOfBookings = 6,
    ID_AddBooking = 7,
    ID_EditClient = 11,
	ID_DeleteBooking = 8,
	ID_DeleteRoom = 9,
	ID_DeleteClient = 10,
	ID_EditRoom = 12,
	ID_ExportCSV = 13,
	ID_ImportCSV = 14

};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(ID_AddClient, MainFrame::OnAddClient)
EVT_BUTTON(ID_AddRoom, MainFrame::OnAddRoom)
EVT_BUTTON(ID_AddAmenity, MainFrame::OnAddAmenity)
EVT_BUTTON(ID_AddBooking, MainFrame::OnAddBooking)
EVT_BUTTON(ID_DeleteBooking, MainFrame::OnDeleteBooking)
EVT_BUTTON(ID_DeleteRoom, MainFrame::OnDeleteRoom)
EVT_BUTTON(ID_DeleteClient, MainFrame::OnDeleteClient)
EVT_BUTTON(ID_EditClient, MainFrame::OnEditClient)
EVT_BUTTON(ID_EditRoom, MainFrame::OnEditRoom)
EVT_BUTTON(ID_ExportCSV, MainFrame::OnExportCSV)
EVT_BUTTON(ID_ImportCSV, MainFrame::OnImportCSV)
EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
 : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800,600))
{
 wxPanel* panel = new wxPanel(this, wxID_ANY);
 wxStaticText* clientsLabel = new wxStaticText(panel, wxID_ANY, "Клиенты", wxPoint(320,10), wxSize(100,20));
 wxStaticText* roomsLabel = new wxStaticText(panel, wxID_ANY, "Комнаты", wxPoint(680,10), wxSize(50,20));
 wxButton* AddClientButton = new wxButton(panel, ID_AddClient, "Добавить клиента", wxPoint(20,30), wxSize(140,30));
 wxButton* EditClientButton = new wxButton(panel, ID_EditClient, "Редактировать клиента", wxPoint(20,70), wxSize(140,30));

 EditClientButton->Bind(wxEVT_BUTTON, &MainFrame::OnEditClient, this, ID_EditClient);
 wxButton* DeleteClientButton = new wxButton(panel, ID_DeleteClient, "Удалить клиента", wxPoint(20,110), wxSize(140,30));
 wxButton* AddRoomButton = new wxButton(panel, ID_AddRoom, "Добавить комнату", wxPoint(20,150), wxSize(140,30));
 wxButton* EditRoomButton = new wxButton(panel, ID_EditRoom, "Редактировать комнату", wxPoint(20,190), wxSize(140,30));
 wxButton* DeleteRoomButton = new wxButton(panel, ID_DeleteRoom, "Удалить комнату", wxPoint(20,230), wxSize(130,30));
 wxButton* AddAmenityButton = new wxButton(panel, ID_AddAmenity, "Добавить удобства", wxPoint(750,0), wxSize(130,30));
 wxButton* AddBookingButton = new wxButton(panel, ID_AddBooking, "Добавить бронирование", wxPoint(10,470), wxSize(150,30));
 wxButton* DeleteBookingButton = new wxButton(panel, ID_DeleteBooking, "Удалить бронирование", wxPoint(10,510), wxSize(140,30));
 wxButton* ExportCSVButton = new wxButton(panel, ID_ExportCSV, "Экспорт CSV", wxPoint(10,430), wxSize(150,30));
 wxButton* ImportCSVButton = new wxButton(panel, ID_ImportCSV, "Импорт CSV", wxPoint(10,390), wxSize(150,30));

 listOfClients = new wxListBox(panel, ID_ListOfClients, wxPoint(170,30), wxSize(350,400),0, nullptr, wxLB_EXTENDED);
 listOfRooms = new wxListBox(panel, ID_ListOfRooms, wxPoint(530,30), wxSize(350,400));
 listOfBookings = new wxListBox(panel, ID_ListOfBookings, wxPoint(170,470), wxSize(660,120));
 listOfClients->SetBackgroundColour(wxColour(240,240,240));
 listOfRooms->SetBackgroundColour(wxColour(240,240,240));
 listOfBookings->SetBackgroundColour(wxColour(240,240,240));
 DeleteBookingButton->SetBackgroundColour(wxColour(255,153,153));
 DeleteRoomButton->SetBackgroundColour(wxColour(255,153,153));
 DeleteClientButton->SetBackgroundColour(wxColour(255,153,153));
 ExportCSVButton->SetBackgroundColour(wxColour(153,204,255));
 ImportCSVButton->SetBackgroundColour(wxColour(153,255,204));

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
 if (!loadErr.empty()) wxLogError(loadErr);
 }

 refreshClientsList();
 refreshRoomsList();
 refreshBookingsList();
}

MainFrame::~MainFrame() {
 if (!SaveData("data.json", clients, rooms, bookings,
 IdGenerator::getNextClientId(), IdGenerator::getNextRoomId(), IdGenerator::getNextBookingId()))
 {
 wxLogError("Не удалось сохранить данные при закрытии");
 }
}

void MainFrame::OnClose(wxCloseEvent& event) {
 // Save data
 if (!SaveData("data.json", clients, rooms, bookings,
 IdGenerator::getNextClientId(), IdGenerator::getNextRoomId(), IdGenerator::getNextBookingId()))
 {
 wxLogError("Не удалось сохранить данные");
 }

 event.Skip();
}

void MainFrame::OnAddBooking(wxCommandEvent& event) {
 if (!listOfBookings) {
 wxLogError("listOfBookings == nullptr");
 return;
 }

 long selRoom = listOfRooms->GetSelection();
 if (selRoom == wxNOT_FOUND) {
 wxMessageBox("Пожалуйстa, выберите комнату.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }

 wxArrayInt sels;
 listOfClients->GetSelections(sels);
 if (sels.IsEmpty()) {
 wxMessageBox("Пожалуйстa, выберите одного или нескольких клиентов.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }

 void* roomData = listOfRooms->GetClientData(selRoom);
 if (!roomData) { wxLogError("Нет client data для выбранной комнаты"); return; }
 int roomId = static_cast<int>(reinterpret_cast<std::intptr_t>(roomData));
 Room* room = findRoomById(roomId);
 if (!room) { wxLogError("Не найденa комната по ID"); return; }

 // collect client ids
 std::vector<int> clientIds;
 for (auto idx : sels) {
 void* cd = listOfClients->GetClientData(idx);
 if (!cd) continue;
 int cid = static_cast<int>(reinterpret_cast<std::intptr_t>(cd));
 Client* c = findClientById(cid);
 if (c && c->isActive()) clientIds.push_back(cid);
 }
 if (clientIds.empty()) {
 wxMessageBox("Не выбраны действительные клиенты.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }

 // Show dialog to pick dates
 AddBookingDialog dlg(this);
 if (dlg.ShowModal() != wxID_OK) return;
 Date ci = dlg.getCheckIn();
 Date co = dlg.getCheckOut();

 // Check for overlapping active bookings for this room
 for (const auto& b : bookings) {
 if (!b.isActive()) continue;
 if (b.getRoomId() != roomId) continue;
 Date oci = b.getCheckInDate();
 Date oco = b.getCheckOutDate();
 // overlap if ci < oco && oci < co
 if (ci.isBefore(oco) && oci.isBefore(co)) {
 wxMessageBox("Выбранные даты пересекаются с существующими бронированиями для этой комнаты.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 }

 int bookingId = IdGenerator::generateBookingId();
 bookings.emplace_back(bookingId, room->getId(), clientIds, ci, co, true);
 room->setStatus(RoomStatus::BOOKED);

 int days = std::abs(ci.DifferenceInDays(co));
 double totalPrice = days * room->getPricePerNight();
 bookings.back().setTotalPrice(totalPrice);

 refreshRoomsList();
 refreshBookingsList();
}
void MainFrame::OnDeleteBooking(wxCommandEvent& event) {
    if (!listOfBookings) {
        wxLogError("listOfBookings == nullptr");
        return;
    }
    long sel = listOfBookings->GetSelection();
    if (sel == wxNOT_FOUND) {
        wxMessageBox("Пожалуйста, выберите бронирование для удаления.", "Информация", wxOK | wxICON_INFORMATION, this);
        return;
    }
    void* data = listOfBookings->GetClientData(sel);
    if (!data) {
        wxLogError("Нет client data для выбранной строки");
        return;
    }
    int bookingId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));

    // Soft-delete booking by id
    for (auto& b : bookings) {
        if (b.getId() == bookingId) { b.setActive(false);
        // set room status to CLEANING if room still active
        Room* room = findRoomById(b.getRoomId());
        if (room) room->setStatus(RoomStatus::CLEANING);
        break; }
    }

    refreshRoomsList();
    refreshBookingsList();
}
void MainFrame::OnAddRoom(wxCommandEvent& event) {
 AddRoomDialog dlg(this);
 if (dlg.ShowModal() != wxID_OK) return;

 wxString wxRoomNumber = dlg.getRoomNumber();
 wxString wxCategory = dlg.getCategory();
 wxString wxPrice = dlg.getPrice();

 long roomNumber =0;
 double price =0.0;

 bool okNumber = wxRoomNumber.ToLong(&roomNumber);
 bool okPrice = wxPrice.ToDouble(&price);

 if (!okNumber) {
 wxMessageBox("Некорректный номер комнаты.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }
 if (!okPrice) {
 wxMessageBox("Некорректная цена.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }

 std::string category = std::string(wxCategory.ToUTF8().data());
 std::vector<std::string> amenities;

 int roomId = IdGenerator::generateRoomId();
 rooms.emplace_back(roomId, static_cast<int>(roomNumber), category, price, RoomStatus::AVAILABLE, amenities);

 refreshRoomsList();
}

void MainFrame::OnDeleteRoom(wxCommandEvent& event) {
 if (!listOfRooms) {
 wxLogError("listOfRooms == nullptr");
 return;
 }

 long sel = listOfRooms->GetSelection();
 if (sel == wxNOT_FOUND) {
 wxMessageBox("Пожалуйста, выберите комнату для удаления.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }

 void* data = listOfRooms->GetClientData(sel);
 if (!data) {
 wxLogError("Нет client data для выбранной строки");
 return;
 }

 int roomId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));

 // Check for blocking active bookings
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
 wxString msg = "Комнату нельзя удалить — есть активные бронирования:\n";
 for (int id : blocking) msg += wxString::Format(" Бронирование #%d\n", id);
 wxMessageBox(msg, "Ошибка удаления", wxOK | wxICON_ERROR, this);
 return;
 }

 if (wxMessageBox("Подтвердить удаление комнаты?", "Подтверждение", wxYES_NO | wxICON_QUESTION, this) != wxYES) {
 return;
 }

 // Find room index in vector and erase it
 bool removed = false;
 for (auto it = rooms.begin(); it != rooms.end(); ++it) {
 if (it->getId() == roomId) {
 rooms.erase(it);
 removed = true;
 break;
 }
 }

 if (!removed) {
 wxLogError("Не найден объект комнаты с id=%d при попытке удаления", roomId);
 wxMessageBox("Не найден объект комнаты для удаления. Проверьте логи.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }

 // Remove related bookings (erase those that reference this room)
 for (auto it = bookings.begin(); it != bookings.end(); ) {
 if (it->getRoomId() == roomId) it = bookings.erase(it);
 else ++it;
 }

 // Remove matching items from the list control
 for (int i = (int)listOfRooms->GetCount() -1; i >=0; --i) {
 void* cd = listOfRooms->GetClientData(i);
 if (!cd) continue;
 int id = static_cast<int>(reinterpret_cast<std::intptr_t>(cd));
 if (id == roomId) listOfRooms->Delete(i);
 }

 refreshRoomsList();
 refreshBookingsList();
}

void MainFrame::OnAddAmenity(wxCommandEvent& event) {
    if (!listOfRooms) {
        wxLogError("listOfRooms == nullptr");
        return;
    }

    long sel = listOfRooms->GetSelection();
    if (sel == wxNOT_FOUND) {
        wxMessageBox("Пожалуйста, выберите комнату.", "Информация", wxOK | wxICON_INFORMATION, this);
        return;
    }

    void* data = listOfRooms->GetClientData(sel);
    if (!data) {
        wxLogError("Нет client data для выбранной строки");
        return;
    }
    int roomId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));
    Room* room = findRoomById(roomId);
    if (!room) {
        wxLogError("Не могу найти комнату по ID");
        return;
    }

    wxTextEntryDialog dlgAmenity(this, "Введите удобство для комнаты:", "Новое удобство");
    if (dlgAmenity.ShowModal() != wxID_OK) return;

    wxString wxAmenity = dlgAmenity.GetValue();
    std::string amenity = std::string(wxAmenity.ToUTF8().data());

    room->addAmenity(amenity);
    refreshRoomsList();
}

void MainFrame::OnAddClient(wxCommandEvent& event) {
 AddClientDialog dlg(this);
 if (dlg.ShowModal() != wxID_OK) return;

 wxString wxFirst = dlg.getFirstName();
 wxString wxLast = dlg.getLastName();
 wxString wxPhone = dlg.getPhone();
 Passport passport = dlg.getPassport();

 std::string first = std::string(wxFirst.ToUTF8().data());
 std::string last = std::string(wxLast.ToUTF8().data());
 std::string phone = std::string(wxPhone.ToUTF8().data());

 int clientId = IdGenerator::generateClientId();
 clients.emplace_back(clientId, first, last, phone, passport);

 refreshClientsList();
}
void MainFrame::OnDeleteClient(wxCommandEvent& event) {
 if (!listOfClients) {
 wxLogError("listOfClients == nullptr");
 return;
 }

 wxArrayInt sels;
 listOfClients->GetSelections(sels);
 if (sels.IsEmpty()) {
 wxMessageBox("Пожалуйста, выберите клиента для удаления.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }
 if (sels.GetCount() >1) {
 wxMessageBox("Пожалуйста, выберите только одного клиента для удаления.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }
 int sel = sels[0];
 void* data = listOfClients->GetClientData(sel);
 if (!data) {
 wxLogError("Нет client data для выбранной строки");
 return;
 }
 int clientId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));
 // Check for active bookings
 std::vector<int> blocking;
 for (const auto& b : bookings) {
 if (!b.isActive()) continue;
 auto ids = b.getClientIds();
 if (std::find(ids.begin(), ids.end(), clientId) != ids.end()) {
 auto st = b.getStatus();
 if (st == BookingStatus::CONFIRMED || st == BookingStatus::CHECKED_IN) {
 blocking.push_back(b.getId());
 }
 }
 }
 if (!blocking.empty()) {
 wxString msg = "Клиента нельзя удалить — есть активные бронирования:\n";
 for (int id : blocking) msg += wxString::Format(" Бронирование #%d\n", id);
 wxMessageBox(msg, "Ошибка удаления", wxOK | wxICON_ERROR, this);
 return;
 }

 if (wxMessageBox("Подтвердить удаление клиента?", "Подтверждение", wxYES_NO | wxICON_QUESTION, this) != wxYES) {
 return;
 }

 // Remove all clients with this id (handle duplicates from import)
 bool anyRemoved = false;
 for (auto it = clients.begin(); it != clients.end(); ) {
 if (it->getId() == clientId) {
 it = clients.erase(it);
 anyRemoved = true;
 } else ++it;
 }
 if (!anyRemoved) {
 wxLogError("Не найден клиент с id=%d для удаления", clientId);
 wxMessageBox("Не удалось найти клиента для удаления. Проверьте логи.", "Ошибка", wxOK | wxICON_ERROR, this);
 return;
 }

 // Remove related bookings (erase those that reference this client)
 for (auto it = bookings.begin(); it != bookings.end(); ) {
 auto ids = it->getClientIds();
 if (std::find(ids.begin(), ids.end(), clientId) != ids.end()) {
 it = bookings.erase(it);
 } else ++it;
 }

 // Remove matching items from the list control
 for (int i = (int)listOfClients->GetCount() -1; i >=0; --i) {
 void* cd = listOfClients->GetClientData(i);
 if (!cd) continue;
 int id = static_cast<int>(reinterpret_cast<std::intptr_t>(cd));
 if (id == clientId) listOfClients->Delete(i);
 }

 refreshClientsList();
 refreshBookingsList();
}
void MainFrame::OnEditClient(wxCommandEvent& event) {
 if (!listOfClients) {
 wxLogError("listOfClients == nullptr");
 return;
 }

 wxArrayInt sels;
 listOfClients->GetSelections(sels);
 if (sels.IsEmpty()) {
 wxMessageBox("Пожалуйста, выберите клиента для редактирования.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }
 if (sels.GetCount() >1) {
 wxMessageBox("Пожалуйста, выберите только одного клиента для редактирования.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }

 int sel = sels[0];
 void* data = listOfClients->GetClientData(sel);
 if (!data) { wxLogError("Нет client data для выбранного элемента"); return; }
 int clientId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));
 Client* client = findClientById(clientId);
 if (!client) { wxLogError("Клиент не найден"); return; }

 AddClientDialog dlg(this);
 dlg.setValues(*client);
 if (dlg.ShowModal() != wxID_OK) return;

 // update client fields: first, last, phone, passport
 client->setFirstName(std::string(dlg.getFirstName().ToUTF8().data()));
 client->setLastName(std::string(dlg.getLastName().ToUTF8().data()));
 client->setPhone(std::string(dlg.getPhone().ToUTF8().data()));
 Passport p = dlg.getPassport();
 client->setPassport(p);

 refreshClientsList();
}
void MainFrame::OnEditRoom(wxCommandEvent& event) {
 long sel = listOfRooms->GetSelection();
 if (sel == wxNOT_FOUND) { wxMessageBox("Пожалуйста, выберите комнату для редактирования.", "Информация", wxOK | wxICON_INFORMATION, this); return; }
 void* data = listOfRooms->GetClientData(sel);
 if (!data) { wxLogError("Нет client data для выбранной строки"); return; }
 int roomId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));
 Room* room = findRoomById(roomId);
 if (!room) { wxLogError("Не могу найти комнату по ID"); return; }

 AddRoomDialog dlg(this);
 dlg.setValues(room->getRoomNumber(), wxString::FromUTF8(room->getCategory().c_str()), room->getPricePerNight());
 if (dlg.ShowModal() != wxID_OK) return;

 wxString wxRoomNumber = dlg.getRoomNumber();
 wxString wxCategory = dlg.getCategory();
 wxString wxPrice = dlg.getPrice();

 long roomNumber =0; double price =0.0;
 bool okNumber = wxRoomNumber.ToLong(&roomNumber);
 bool okPrice = wxPrice.ToDouble(&price);
 if (!okNumber) { wxMessageBox("Некорректный номер комнаты.", "Ошибка", wxOK | wxICON_ERROR, this); return; }
 if (!okPrice) { wxMessageBox("Некорректная цена.", "Ошибка", wxOK | wxICON_ERROR, this); return; }

 // update mutable fields
 room->setPricePerNight(price);
 room->setRoomNumber(static_cast<int>(roomNumber));
 room->setCategory(std::string(wxCategory.ToUTF8().data()));

 refreshRoomsList();
}

void MainFrame::refreshRoomsList() {
    if (!listOfRooms) return;
    listOfRooms->Clear();
    for (const auto& r : rooms) {
        if (!r.isActive()) continue;
        wxString display = wxString::Format("ID:%d, Номер:%d, Категория:%s, Цена:%.2f, Статус:%s",
            r.getId(),
            r.getRoomNumber(),
            wxString::FromUTF8(r.getCategory().c_str()),
            r.getPricePerNight(),
            wxString::FromUTF8(RoomStatusToString(r.getStatus()).c_str()));
        listOfRooms->Append(display, reinterpret_cast<void*>(static_cast<std::intptr_t>(r.getId())));
    }
    listOfRooms->Refresh();
    listOfRooms->Update();
}

void MainFrame::refreshClientsList() {
 if (!listOfClients) return;
 listOfClients->Clear();
 for (const auto& client : clients) {
 if (!client.isActive()) continue;
 wxString display = wxString::Format("%d. %s %s, Тел: %s",
 client.getId(),
 wxString::FromUTF8(client.getFirstName().c_str()),
 wxString::FromUTF8(client.getLastName().c_str()),
 wxString::FromUTF8(client.getPhone().c_str()));
 listOfClients->Append(display, reinterpret_cast<void*>(static_cast<std::intptr_t>(client.getId())));
 }
 listOfClients->Refresh();
 listOfClients->Update();
}

void MainFrame::refreshBookingsList() {
 if (!listOfBookings) return;
 listOfBookings->Clear();
 for (const auto& booking : bookings) {
 if (!booking.isActive()) continue;
 Room* room = findRoomById(booking.getRoomId());

 wxString roomInfo = room ? wxString::Format("Комната %d", room->getRoomNumber()) : "Комната не найдена";

 // build clients string
 std::string clientsStr;
 for (int cid : booking.getClientIds()) {
 Client* c = findClientById(cid);
 if (c) {
 if (!clientsStr.empty()) clientsStr += ", ";
 clientsStr += c->getFullName();
 }
 }
 if (clientsStr.empty()) clientsStr = "Клиенты не найдены";

 wxString display = wxString::Format("Бронирование #%d: %s, %s, %s - %s, %.2f руб.",
 booking.getId(),
 roomInfo,
 wxString::FromUTF8(clientsStr.c_str()),
 wxString::FromUTF8(booking.getCheckInDate().toString().c_str()),
 wxString::FromUTF8(booking.getCheckOutDate().toString().c_str()),
 booking.getTotalPrice());

 listOfBookings->Append(display, reinterpret_cast<void*>(static_cast<std::intptr_t>(booking.getId())));
 }
 listOfBookings->Refresh();
 listOfBookings->Update();
}

Room* MainFrame::findRoomById(int roomId) {
 for (auto& room : rooms) {
 if (room.getId() == roomId) return &room;
 }
 return nullptr;
}

Client* MainFrame::findClientById(int clientId) {
 for (auto& client : clients) {
 if (client.getId() == clientId) return &client;
 }
 return nullptr;
}

void MainFrame::OnExportCSV(wxCommandEvent& event) {
 // Let user pick a target file (any file in desired folder) so they can use file search
 wxFileDialog dlg(this, "Выберите файл в папке для экспорта (будут созданы clients.csv, rooms.csv, bookings.csv в той же папке)",
 wxEmptyString, "clients.csv",
 "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
 if (dlg.ShowModal() != wxID_OK) return;

 wxFileName fn(dlg.GetPath());
 wxString wxdir = fn.GetPath();
 std::string dir = std::string(wxdir.ToUTF8().data());
 std::string err;
 if (!ExportToCSV(dir, clients, rooms, bookings, err)) {
 wxMessageBox(wxString::FromUTF8(err.c_str()), "Ошибка экспорта", wxOK | wxICON_ERROR, this);
 return;
 }

 // Open folder in Explorer so user can see files (use native path)
 wxString openCmd = wxString::Format("explorer \"%s\"", wxdir);
 wxExecute(openCmd);

 wxMessageBox("Экспорт завершён", "Информация", wxOK | wxICON_INFORMATION, this);
}


void MainFrame::OnImportCSV(wxCommandEvent& event) {
 // Let user pick any CSV file in folder to import all CSVs from that folder (searchable)
 wxFileDialog dlg(this, "Выберите CSV-файл из папки с файлами (clients.csv/rooms.csv/bookings.csv)",
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
 wxMessageBox(wxString::FromUTF8(err.c_str()), "Ошибка импорта", wxOK | wxICON_ERROR, this);
 return;
 }
 IdGenerator::setNextIds(nextC, nextR, nextB);
 refreshClientsList();
 refreshRoomsList();
 refreshBookingsList();
 wxMessageBox("Импорт завершён", "Информация", wxOK | wxICON_INFORMATION, this);
}