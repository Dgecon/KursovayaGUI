#define _CRT_SECURE_NO_WARNINGS

#include "Client.h"
#include "MainFrame.h"
#include "Room.h"
#include "Booking.h"
#include "IdGenerator.h"
#include "Persistence.h"
#include "AddClientDialog.h"
#include "AddRoomDialog.h"
#include <vector>
#include <wx/listbox.h>
#include <wx/textdlg.h>
#include <wx/wx.h>
#include <sstream>
#include <cstdint>

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
	ID_DeleteClient = 10

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
 // Bind explicitly to ensure the button triggers the handler
 EditClientButton->Bind(wxEVT_BUTTON, &MainFrame::OnEditClient, this, ID_EditClient);
 wxButton* DeleteClientButton = new wxButton(panel, ID_DeleteClient, "Удалить клиента", wxPoint(20,110), wxSize(140,30));
 wxButton* AddRoomButton = new wxButton(panel, ID_AddRoom, "Добавить комнату", wxPoint(20,150), wxSize(140,30));
 wxButton* DeleteRoomButton = new wxButton(panel, ID_DeleteRoom, "Удалить комнату", wxPoint(20,190), wxSize(130,30));
 wxButton* AddAmenityButton = new wxButton(panel, ID_AddAmenity, "Добавить удобства", wxPoint(750,0), wxSize(130,30));
 wxButton* AddBookingButton = new wxButton(panel, ID_AddBooking, "Добавить бронирование", wxPoint(10,470), wxSize(150,30));
 wxButton* DeleteBookingButton = new wxButton(panel, ID_DeleteBooking, "Удалить бронирование", wxPoint(10,510), wxSize(140,30));

 listOfClients = new wxListBox(panel, ID_ListOfClients, wxPoint(170,30), wxSize(350,400));
 listOfRooms = new wxListBox(panel, ID_ListOfRooms, wxPoint(530,30), wxSize(350,400));
 listOfBookings = new wxListBox(panel, ID_ListOfBookings, wxPoint(170,470), wxSize(660,120));
 listOfClients->SetBackgroundColour(wxColour(240,240,240));
 listOfRooms->SetBackgroundColour(wxColour(240,240,240));
 listOfBookings->SetBackgroundColour(wxColour(240,240,240));
 DeleteBookingButton->SetBackgroundColour(wxColour(255,153,153));
 DeleteRoomButton->SetBackgroundColour(wxColour(255,153,153));
 DeleteClientButton->SetBackgroundColour(wxColour(255,153,153));

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
    long selClient = listOfClients->GetSelection();

    if (selRoom == wxNOT_FOUND || selClient == wxNOT_FOUND) {
        wxMessageBox("Пожалуйстa, выберите комнату и клиента.", "Информация", wxOK | wxICON_INFORMATION, this);
        return;
    }

    void* roomData = listOfRooms->GetClientData(selRoom);
    void* clientData = listOfClients->GetClientData(selClient);
    if (!roomData || !clientData) {
        wxLogError("Нет client data для выбранного элемента");
        return;
    }

    int roomId = static_cast<int>(reinterpret_cast<std::intptr_t>(roomData));
    int clientId = static_cast<int>(reinterpret_cast<std::intptr_t>(clientData));

    Room* room = findRoomById(roomId);
    Client* client = findClientById(clientId);
    if (!room || !client) {
        wxLogError("Не найден клиент или комната по ID");
        return;
    }

    if (room->getStatus() == RoomStatus::AVAILABLE) {
        int bookingId = IdGenerator::generateBookingId();
        Date checkIn; // по умолчанию
        Date checkOut; // по умолчанию

        bookings.emplace_back(bookingId, room->getId(), client->getId(), checkIn, checkOut);
        room->setStatus(RoomStatus::BOOKED);

        int days = checkIn.DifferenceInDays(checkOut);
        double totalPrice = days * room->getPricePerNight();
        bookings.back().setTotalPrice(totalPrice);

        refreshRoomsList();
        refreshBookingsList();
    } else {
        wxMessageBox("Комната не доступна для бронирования.", "Информация", wxOK | wxICON_INFORMATION, this);
    }
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
    Booking& booking = bookings[sel];
    Room* room = findRoomById(booking.getRoomId());
    if (room) {
        room->setStatus(RoomStatus::CLEANING);
    }
    bookings.erase(bookings.begin() + sel);
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
    std::vector<int> blocking;
    for (const auto& b : bookings) {
        if (b.getRoomId() == roomId) {
            auto st = b.getStatus();
            if (st == BookingStatus::CONFIRMED || st == BookingStatus::CHECKED_IN) {
                blocking.push_back(b.getId());
            }
        }
    }
    if (!blocking.empty()) {
        wxString msg = "Комнату нельзя удалить — есть активные бронирования:\n";
        for (int id : blocking) msg += wxString::Format("  Бронирование #%d\n", id);
        wxMessageBox(msg, "Ошибка удаления", wxOK | wxICON_ERROR, this);
        return;
    }

    if (wxMessageBox("Подтвердить удаление комнаты?", "Подтверждение", wxYES_NO | wxICON_QUESTION, this) != wxYES) {
        return;
    }

    auto it = std::remove_if(rooms.begin(), rooms.end(),
        [roomId](const Room& r) { return r.getId() == roomId; });
    if (it != rooms.end()) {
        rooms.erase(it, rooms.end());

        bookings.erase(std::remove_if(bookings.begin(), bookings.end(),
            [roomId](const Booking& b) { return b.getRoomId() == roomId; }), bookings.end());

        refreshRoomsList();
        refreshBookingsList();
    } else {
        wxLogError("Не могу найти комнату по ID для удаления");
    }
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
    long sel = listOfClients->GetSelection();
    if (sel == wxNOT_FOUND) {
        wxMessageBox("Пожалуйста, выберите клиента для удаления.", "Информация", wxOK | wxICON_INFORMATION, this);
        return;
    }
    void* data = listOfClients->GetClientData(sel);
    if (!data) {
        wxLogError("Нет client data для выбранной строки");
        return;
    }
    int clientId = static_cast<int>(reinterpret_cast<std::intptr_t>(data));
    std::vector<int> blocking;
    for (const auto& b : bookings) {
        if (b.getClientId() == clientId) {
            auto st = b.getStatus();
            if (st == BookingStatus::CONFIRMED || st == BookingStatus::CHECKED_IN) {
                blocking.push_back(b.getId());
            }
        }
    }
    if (!blocking.empty()) {
        wxString msg = "Клиента нельзя удалить — есть активные бронирования:\n";
        for (int id : blocking) msg += wxString::Format("  Бронирование #%d\n", id);
        wxMessageBox(msg, "Ошибка удаления", wxOK | wxICON_ERROR, this);
        return;
    }

    if (wxMessageBox("Подтвердить удаление клиента?", "Подтверждение", wxYES_NO | wxICON_QUESTION, this) != wxYES) {
        return;
    }

    auto it = std::remove_if(clients.begin(), clients.end(),
        [clientId](const Client& c) { return c.getId() == clientId; });
    if (it != clients.end()) {
        clients.erase(it, clients.end());

        bookings.erase(std::remove_if(bookings.begin(), bookings.end(),
            [clientId](const Booking& b) { return b.getClientId() == clientId; }), bookings.end());

        refreshClientsList();
        refreshBookingsList();
    } else {
        wxLogError("Не могу найти клиента по ID для удаления");
    }
}
void MainFrame::OnEditClient(wxCommandEvent& event) {
 long sel = listOfClients->GetSelection();
 if (sel == wxNOT_FOUND) {
 wxMessageBox("Пожалуйста, выберите клиента для редактирования.", "Информация", wxOK | wxICON_INFORMATION, this);
 return;
 }
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

Room* MainFrame::findRoomById(int roomId) {
    for (auto& room : rooms) {
        if (room.getId() == roomId) {
            return &room;
        }
    }
    return nullptr;
}

Client* MainFrame::findClientById(int clientId) {
    for (auto& client : clients) {
        if (client.getId() == clientId) {
            return &client;
        }
    }
    return nullptr;
}

void MainFrame::refreshRoomsList() {
    if (!listOfRooms) return;
    listOfRooms->Clear();
    for (const auto& r : rooms) {
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
 wxString display = wxString::Format("%d. %s %s, Тел: %s, Паспорт: %s",
 client.getId(),
 wxString::FromUTF8(client.getFirstName().c_str()),
 wxString::FromUTF8(client.getLastName().c_str()),
 wxString::FromUTF8(client.getPhone().c_str()),
 wxString::FromUTF8(client.getPassport().toString().c_str()));
 listOfClients->Append(display, reinterpret_cast<void*>(static_cast<std::intptr_t>(client.getId())));
 }
 listOfClients->Refresh();
 listOfClients->Update();
}

void MainFrame::refreshBookingsList() {
 if (!listOfBookings) return;
 listOfBookings->Clear();
 for (const auto& booking : bookings) {
 Room* room = findRoomById(booking.getRoomId());
 Client* client = findClientById(booking.getClientId());

 wxString roomInfo = room ? wxString::Format("Комната %d", room->getRoomNumber()) : "Комната не найдена";
 wxString clientInfo = client ? wxString::FromUTF8(client->getFullName().c_str()) : "Клиент не найден";

 wxString display = wxString::Format("Бронирование #%d: %s, %s, %s - %s, %.2f руб.",
 booking.getId(),
 roomInfo,
 clientInfo,
 wxString::FromUTF8(booking.getCheckInDate().toString().c_str()),
 wxString::FromUTF8(booking.getCheckOutDate().toString().c_str()),
 booking.getTotalPrice());

 listOfBookings->Append(display);
 }
 listOfBookings->Refresh();
 listOfBookings->Update();
}