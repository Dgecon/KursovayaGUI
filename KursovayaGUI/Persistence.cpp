#include "Persistence.h"
#include "Client.h"
#include "Room.h"
#include "Booking.h"
#include "RoomStatus.h"
#include "Date.h"
#include "Passport.h"
#include <fstream>
#include <cstdio>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string RoomStatusToStr(RoomStatus s) { return RoomStatusToString(s); }
static RoomStatus RoomStatusFromStr(const std::string& s) {
 if (s == "Доступна" || s == u8"Доступна" || s == "AVAILABLE") return RoomStatus::AVAILABLE;
 if (s == "Забронирована" || s == u8"Забронирована" || s == "BOOKED") return RoomStatus::BOOKED;
 if (s == "Занята" || s == u8"Занята" || s == "OCCUPIED") return RoomStatus::OCCUPIED;
 if (s == "На ремонте" || s == u8"На ремонте" || s == "MAINTENANCE") return RoomStatus::MAINTENANCE;
 if (s == "Уборка" || s == u8"Уборка" || s == "CLEANING") return RoomStatus::CLEANING;
 return RoomStatus::AVAILABLE;
}

bool SaveData(const std::string& path,
 const std::vector<Client>& clients,
 const std::vector<Room>& rooms,
 const std::vector<Booking>& bookings,
 int nextClientId, int nextRoomId, int nextBookingId)
{
 try {
 json j;
 j["version"] =1;
 j["nextIds"] = { {"client", nextClientId}, {"room", nextRoomId}, {"booking", nextBookingId} };

 j["clients"] = json::array();
 for (const auto& c : clients) {
 Passport p = c.getPassport();
 json jp = {
 {"series", p.getSeries()},
 {"number", p.getNumber()},
 {"givenBy", p.getGivenBy()},
 {"issue", { {"day", p.getDateOfIssue().getDay()}, {"month", p.getDateOfIssue().getMonth()}, {"year", p.getDateOfIssue().getYear()} }},
 {"code", p.getCode()},
 {"fio", p.getFio()},
 {"birth", { {"day", p.getDateOfBirth().getDay()}, {"month", p.getDateOfBirth().getMonth()}, {"year", p.getDateOfBirth().getYear()} }}
 };
 j["clients"].push_back({
 {"id", c.getId()},
 {"firstName", c.getFirstName()},
 {"lastName", c.getLastName()},
 {"phone", c.getPhone()},
 {"passport", jp},
 {"active", c.isActive()}
 });
 }

 j["rooms"] = json::array();
 for (const auto& r : rooms) {
 j["rooms"].push_back({
 {"id", r.getId()},
 {"roomNumber", r.getRoomNumber()},
 {"category", r.getCategory()},
 {"pricePerNight", r.getPricePerNight()},
 {"status", RoomStatusToStr(r.getStatus())},
 {"amenities", r.getAmenities()},
 {"active", r.isActive()}
 });
 }

 j["bookings"] = json::array();
 for (const auto& b : bookings) {
 auto ci = b.getCheckInDate();
 auto co = b.getCheckOutDate();
 j["bookings"].push_back({
 {"id", b.getId()},
 {"roomId", b.getRoomId()},
 {"clientId", b.getClientId()},
 {"checkIn", { {"day", ci.getDay()}, {"month", ci.getMonth()}, {"year", ci.getYear()} }},
 {"checkOut", { {"day", co.getDay()}, {"month", co.getMonth()}, {"year", co.getYear()} }},
 {"status", (int)b.getStatus()},
 {"totalPrice", b.getTotalPrice()},
 {"active", b.isActive()}
 });
 }

 std::string tmp = path + ".tmp";
 std::ofstream ofs(tmp, std::ios::binary);
 ofs << j.dump(2);
 ofs.close();
 std::remove((path + ".bak").c_str());
 std::rename(path.c_str(), (path + ".bak").c_str());
 std::rename(tmp.c_str(), path.c_str());
 return true;
 } catch (...) {
 return false;
 }
}

bool LoadData(const std::string& path,
 std::vector<Client>& clients,
 std::vector<Room>& rooms,
 std::vector<Booking>& bookings,
 int& nextClientId, int& nextRoomId, int& nextBookingId,
 std::string& error)
{
 try {
 std::ifstream ifs(path);
 if (!ifs.is_open()) {
 error = "Файл не найден";
 return false;
 }
 json j; ifs >> j;

 if (j.contains("nextIds")) {
 nextClientId = j["nextIds"].value("client", nextClientId);
 nextRoomId = j["nextIds"].value("room", nextRoomId);
 nextBookingId = j["nextIds"].value("booking", nextBookingId);
 }

 clients.clear();
 if (j.contains("clients")) {
 for (auto& jc : j["clients"]) {
 Passport p;
 if (jc.contains("passport")) {
 auto jp = jc["passport"];
 p.setSeries(jp.value("series",0));
 p.setNumber(jp.value("number",0));
 p.setGivenBy(jp.value("givenBy", std::string()));
 if (jp.contains("issue")) {
 Date d;
 d.setDay(jp["issue"].value("day",1));
 d.setMonth(jp["issue"].value("month",1));
 d.setYear(jp["issue"].value("year",2024));
 p.setDateOfIssue(d);
 }
 p.setCode(jp.value("code", std::string()));
 p.setFio(jp.value("fio", std::string()));
 if (jp.contains("birth")) {
 Date d;
 d.setDay(jp["birth"].value("day",1));
 d.setMonth(jp["birth"].value("month",1));
 d.setYear(jp["birth"].value("year",2000));
 p.setDateOfBirth(d);
 }
 }
 bool active = jc.value("active", true);
 clients.emplace_back(jc.value("id",0),
 jc.value("firstName", std::string()),
 jc.value("lastName", std::string()),
 jc.value("phone", std::string()),
 p,
 active);
 }
 }

 rooms.clear();
 if (j.contains("rooms")) {
 for (auto& jr : j["rooms"]) {
 int id = jr.value("id",0);
 int roomNumber = jr.value("roomNumber",0);
 std::string category = jr.value("category", std::string());
 double price = jr.value("pricePerNight",0.0);
 RoomStatus status = RoomStatusFromStr(jr.value("status", std::string()));
 std::vector<std::string> amenities;
 if (jr.contains("amenities")) for (auto& a : jr["amenities"]) amenities.push_back(a.get<std::string>());
 bool active = jr.value("active", true);
 rooms.emplace_back(id, roomNumber, category, price, status, amenities, active);
 }
 }

 bookings.clear();
 if (j.contains("bookings")) {
 for (auto& jb : j["bookings"]) {
 int id = jb.value("id",0);
 int roomId = jb.value("roomId",0);
 int clientId = jb.value("clientId",0);
 Date ci, co;
 if (jb.contains("checkIn")) {
 ci.setDay(jb["checkIn"].value("day",1));
 ci.setMonth(jb["checkIn"].value("month",1));
 ci.setYear(jb["checkIn"].value("year",2024));
 }
 if (jb.contains("checkOut")) {
 co.setDay(jb["checkOut"].value("day",1));
 co.setMonth(jb["checkOut"].value("month",1));
 co.setYear(jb["checkOut"].value("year",2024));
 }
 bool active = jb.value("active", true);
 bookings.emplace_back(id, roomId, clientId, ci, co, active);
 if (jb.contains("totalPrice")) bookings.back().setTotalPrice(jb.value("totalPrice",0.0));
 if (jb.contains("status")) bookings.back().setStatus(static_cast<BookingStatus>(jb.value("status", (int)BookingStatus::CONFIRMED)));
 }
 }

 return true;
 } catch (const std::exception& ex) {
 error = ex.what();
 return false;
 } catch (...) {
 error = "Unknown error";
 return false;
 }
}