#pragma once
#include <string>
#include <vector>

class Client;
class Room;
class Booking;

bool SaveData(const std::string& path,
              const std::vector<Client>& clients,
              const std::vector<Room>& rooms,
              const std::vector<Booking>& bookings,
              int nextClientId, int nextRoomId, int nextBookingId);

bool LoadData(const std::string& path,
              std::vector<Client>& clients,
              std::vector<Room>& rooms,
              std::vector<Booking>& bookings,
              int& nextClientId, int& nextRoomId, int& nextBookingId,
              std::string& error);

bool ExportToCSV(const std::string& dir,
              const std::vector<Client>& clients,
              const std::vector<Room>& rooms,
              const std::vector<Booking>& bookings,
              std::string& error);

bool ImportFromCSV(const std::string& dir,
              std::vector<Client>& clients,
              std::vector<Room>& rooms,
              std::vector<Booking>& bookings,
              int& nextClientId, int& nextRoomId, int& nextBookingId,
              std::string& error);