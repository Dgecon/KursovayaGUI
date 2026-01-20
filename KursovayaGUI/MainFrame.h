#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/timer.h>
#include <wx/srchctrl.h>
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

    // Sorting handlers
    void OnClientColumnClick(wxListEvent& event);
    void OnRoomColumnClick(wxListEvent& event);
    void OnBookingColumnClick(wxListEvent& event);

    // Search handlers
    void OnSearchClients(wxCommandEvent& event);
    void OnSearchRooms(wxCommandEvent& event);
    void OnSearchBookings(wxCommandEvent& event);

    // UI components
    wxListCtrl* listOfClients = nullptr;
    wxListCtrl* listOfRooms = nullptr;
    wxListCtrl* listOfBookings = nullptr;

    // Search controls
    wxSearchCtrl* m_clientSearch = nullptr;
    wxSearchCtrl* m_roomSearch = nullptr;
    wxSearchCtrl* m_bookingSearch = nullptr;

    // Sort state
    int m_clientSortColumn = -1;
    bool m_clientSortAsc = true;
    int m_roomSortColumn = -1;
    bool m_roomSortAsc = true;
    int m_bookingSortColumn = -1;
    bool m_bookingSortAsc = true;

    // Search filters
    wxString m_clientFilter;
    wxString m_roomFilter;
    wxString m_bookingFilter;

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

    // Sorting helpers
    void sortClients();
    void sortRooms();
    void sortBookings();

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