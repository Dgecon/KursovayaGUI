#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/timer.h>
#include <vector>
#include "Client.h"
#include "Room.h"
#include "Booking.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);
    ~MainFrame();

private:
    // Event handlers
    void OnAddBooking(wxCommandEvent& event);
    void OnAddClient(wxCommandEvent& event);
    void OnAddRoom(wxCommandEvent& event);
    void OnAddAmenity(wxCommandEvent& event);
    void OnDeleteBooking(wxCommandEvent& event);
    void OnDeleteRoom(wxCommandEvent& event);
    void OnDeleteClient(wxCommandEvent& event);
    void OnEditClient(wxCommandEvent& event);
    void OnEditRoom(wxCommandEvent& event);
    void OnChangeRoomStatus(wxCommandEvent& event);
    void OnCheckIn(wxCommandEvent& event);
    void OnCheckOut(wxCommandEvent& event);
    void OnExportCSV(wxCommandEvent& event);
    void OnImportCSV(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    // Double-click handlers
    void OnClientDblClick(wxListEvent& event);
    void OnRoomDblClick(wxCommandEvent& event);
    void OnBookingDblClick(wxCommandEvent& event);

    // Auto-save
    void OnAutoSaveTimer(wxTimerEvent& event);
    void SaveDataNow();  // Немедленное сохранение
    void MarkDataChanged();  // Пометить что данные изменились

    // UI components
    wxListCtrl* listOfClients = nullptr;
    wxListCtrl* listOfRooms = nullptr;
    wxListCtrl* listOfBookings = nullptr;

    // Auto-save timer
    wxTimer* m_autoSaveTimer = nullptr;
    bool m_dataChanged = false;  // Флаг изменения данных
    static const int AUTOSAVE_INTERVAL_MS = 30000;  // 30 секунд

    // Data
    std::vector<Client> clients;
    std::vector<Room> rooms;
    std::vector<Booking> bookings;

    // Helper methods
    Room* findRoomById(int roomId);
    Client* findClientById(int clientId);
    void refreshRoomsList();
    void refreshClientsList();
    void refreshBookingsList();
    void updateStatusBar();
    void updateRoomStatusBasedOnBookings(int roomId);

    // UI setup helpers
    void CreateMenuBar();
    void CreateToolBar();
    void SetupStatusBar();
    wxPanel* CreateClientsPanel(wxWindow* parent);
    wxPanel* CreateRoomsPanel(wxWindow* parent);
    wxPanel* CreateBookingsPanel(wxWindow* parent);

    wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H