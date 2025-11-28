#include <wx/wx.h>
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
    void OnAddBooking(wxCommandEvent& event);
    void OnAddClient(wxCommandEvent& event);
    void OnAddRoom(wxCommandEvent& event);
    void OnAddAmenity(wxCommandEvent& event);
	void OnDeleteBooking(wxCommandEvent& event);
	void OnDeleteRoom(wxCommandEvent& event);
	void OnDeleteClient(wxCommandEvent& event);

    // handle window close to save data
    void OnClose(wxCloseEvent& event);

    wxListBox* listOfClients = nullptr;
    wxListBox* listOfRooms = nullptr;
    wxListBox* listOfBookings = nullptr;

    std::vector<Client> clients;
    std::vector<Room> rooms;
    std::vector<Booking> bookings;

    wxDECLARE_EVENT_TABLE();

    // вспомогательные методы
    Room* findRoomById(int roomId);
    Client* findClientById(int clientId);
    void refreshRoomsList();
    void refreshClientsList();
    void refreshBookingsList();

    // Edit client
    void OnEditClient(wxCommandEvent& event);
    // Edit room
    void OnEditRoom(wxCommandEvent& event);
	// CSV import/export
	void OnExportCSV(wxCommandEvent& event);
	void OnImportCSV(wxCommandEvent& event);
};