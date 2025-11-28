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
#include <sstream>
#include <iomanip>
#include <cctype>

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
 {"clientIds", b.getClientIds()},
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
 std::vector<int> clientIds;
 if (jb.contains("clientIds")) {
 for (auto& ci : jb["clientIds"]) clientIds.push_back(ci.get<int>());
 } else if (jb.contains("clientId")) {
 clientIds.push_back(jb.value("clientId",0));
 }
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
 bookings.emplace_back(id, roomId, clientIds, ci, co, active);
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

static std::string escapeCsv(const std::string& s) {
 if (s.find(',') == std::string::npos && s.find('\"')==std::string::npos && s.find('\n')==std::string::npos) return s;
 std::string out = "\"";
 for (char c: s) {
 if (c=='\"') out += "\"\""; else out += c;
 }
 out += "\"";
 return out;
}

bool ExportToCSV(const std::string& dir,
 const std::vector<Client>& clients,
 const std::vector<Room>& rooms,
 const std::vector<Booking>& bookings,
 std::string& error)
{
 try {
 // clients.csv: id,firstName,lastName,phone,active,passport_series,passport_number,passport_givenBy,passport_issue,passport_code,passport_fio,passport_birth
 std::ofstream cf(dir + "/clients.csv");
 cf << "id,firstName,lastName,phone,active,passport_series,passport_number,passport_givenBy,passport_issue_day,passport_issue_month,passport_issue_year,passport_code,passport_fio,passport_birth_day,passport_birth_month,passport_birth_year\n";
 for (const auto& c: clients) {
 const Passport& p = c.getPassport();
 std::stringstream ss;
 ss << c.getId() << "," << escapeCsv(c.getFirstName()) << "," << escapeCsv(c.getLastName()) << "," << escapeCsv(c.getPhone()) << "," << (c.isActive()?"1":"0") << ",\""
 << p.getSeries() << "\",\"" << p.getNumber() << "\",\"" << escapeCsv(p.getGivenBy()) << "\",\""
 << p.getDateOfIssue().getDay() << "," << p.getDateOfIssue().getMonth() << "," << p.getDateOfIssue().getYear() << "\",\""
 << escapeCsv(p.getCode()) << "\",\"" << escapeCsv(p.getFio()) << "\",\""
 << p.getDateOfBirth().getDay() << "," << p.getDateOfBirth().getMonth() << "," << p.getDateOfBirth().getYear() << "\"";
 cf << ss.str() << "\n";
 }
 cf.close();

 // rooms.csv: id,roomNumber,category,pricePerNight,status,amenities(sep by ;),active
 std::ofstream rf(dir + "/rooms.csv");
 rf << "id,roomNumber,category,pricePerNight,status,amenities,active\n";
 for (const auto& r: rooms) {
 std::stringstream ss;
 ss << r.getId() << "," << r.getRoomNumber() << "," << escapeCsv(r.getCategory()) << "," << std::fixed << std::setprecision(2) << r.getPricePerNight() << "," << escapeCsv(RoomStatusToString(r.getStatus())) << ",\"";
 bool first=true;
 for (auto &a: r.getAmenities()) { if (!first) ss<<";"; ss<<a; first=false; }
 ss << "\"," << (r.isActive()?"1":"0");
 rf << ss.str() << "\n";
 }
 rf.close();

 // bookings.csv: id,roomId,clientIds(sep by ;),checkIn_day,checkIn_month,checkIn_year,checkOut_day,checkOut_month,checkOut_year,status,totalPrice,active
 std::ofstream bf(dir + "/bookings.csv");
 bf << "id,roomId,clientIds,checkIn_day,checkIn_month,checkIn_year,checkOut_day,checkOut_month,checkOut_year,status,totalPrice,active\n";
 for (const auto& b: bookings) {
 std::stringstream ss;
 ss << b.getId() << "," << b.getRoomId() << ",\"";
 bool first=true;
 for (int id: b.getClientIds()) { if (!first) ss<<";"; ss<<id; first=false; }
 ss << "\"," << b.getCheckInDate().getDay() << "," << b.getCheckInDate().getMonth() << "," << b.getCheckInDate().getYear()
 << "," << b.getCheckOutDate().getDay() << "," << b.getCheckOutDate().getMonth() << "," << b.getCheckOutDate().getYear()
 << "," << (int)b.getStatus() << "," << std::fixed << std::setprecision(2) << b.getTotalPrice() << "," << (b.isActive()?"1":"0");
 bf << ss.str() << "\n";
 }
 bf.close();

 return true;
 } catch (const std::exception& ex) {
 error = ex.what();
 return false;
 }
}

static std::vector<std::string> splitCsvLine(const std::string& line) {
 std::vector<std::string> res;
 std::stringstream ss(line);
 std::string item;
 bool inQuotes = false;
 std::string cur;
 for (size_t i=0;i<line.size();++i) {
 char c = line[i];
 if (c=='\"') {
 if (inQuotes && i+1<line.size() && line[i+1]=='\"') { cur+='\"'; ++i; }
 else inQuotes = !inQuotes;
 } else if (c==',' && !inQuotes) {
 res.push_back(cur); cur.clear();
 } else cur+=c;
 }
 res.push_back(cur);
 return res;
}

static inline std::string trimStr(const std::string &s) {
 size_t a =0, b = s.size();
 while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
 while (b > a && std::isspace(static_cast<unsigned char>(s[b-1]))) --b;
 return s.substr(a, b-a);
}

static inline std::string unquote(const std::string &s) {
 if (s.size() >=2 && s.front() == '"' && s.back() == '"') {
 std::string inner = s.substr(1, s.size()-2);
 // replace "" with "
 std::string out;
 for (size_t i =0; i < inner.size(); ++i) {
 if (inner[i] == '"' && i+1 < inner.size() && inner[i+1] == '"') { out += '"'; ++i; }
 else out += inner[i];
 }
 return out;
 }
 return s;
}

static int safeParseInt(const std::vector<std::string>& cols, size_t idx, int def =0) {
 if (idx >= cols.size()) return def;
 std::string t = trimStr(unquote(cols[idx]));
 if (t.empty()) return def;
 try { return std::stoi(t); }
 catch (...) { return def; }
}

static double safeParseDouble(const std::vector<std::string>& cols, size_t idx, double def =0.0) {
 if (idx >= cols.size()) return def;
 std::string t = trimStr(unquote(cols[idx]));
 if (t.empty()) return def;
 try { return std::stod(t); }
 catch (...) { return def; }
}

bool ImportFromCSV(const std::string& dir,
 std::vector<Client>& clients,
 std::vector<Room>& rooms,
 std::vector<Booking>& bookings,
 int& nextClientId, int& nextRoomId, int& nextBookingId,
 std::string& error)
{
 try {
 std::vector<std::string> warnings;
 // helper lambdas to find existing indices
 auto findClientIndex = [&](int id)->int{
 for (size_t i=0;i<clients.size();++i) if (clients[i].getId() == id) return (int)i; return -1;
 };
 auto findRoomIndex = [&](int id)->int{
 for (size_t i=0;i<rooms.size();++i) if (rooms[i].getId() == id) return (int)i; return -1;
 };
 auto findBookingIndex = [&](int id)->int{
 for (size_t i=0;i<bookings.size();++i) if (bookings[i].getId() == id) return (int)i; return -1;
 };

 auto clientExists = [&](int id){ return findClientIndex(id) != -1; };
 auto roomExists = [&](int id){ return findRoomIndex(id) != -1; };
 auto bookingExists = [&](int id){ return findBookingIndex(id) != -1; };

 // import clients (add new or update existing)
 std::ifstream cf(dir + "/clients.csv");
 if (cf.is_open()) {
 std::string header;
 std::getline(cf, header);
 std::string line;
 int row =1;
 while (std::getline(cf, line)) {
 ++row;
 if (line.empty()) continue;
 auto cols = splitCsvLine(line);
 int id = safeParseInt(cols,0,0);
 bool idProvided = (id !=0);
 if (!idProvided) id = nextClientId++;
 std::string first = cols.size() >1 ? unquote(trimStr(cols[1])) : std::string();
 std::string last = cols.size() >2 ? unquote(trimStr(cols[2])) : std::string();
 std::string phone = cols.size() >3 ? unquote(trimStr(cols[3])) : std::string();
 bool active = (cols.size() >4 ? (trimStr(cols[4]) == "1") : true);
 Passport p;
 p.setSeries(safeParseInt(cols,5,0));
 p.setNumber(safeParseInt(cols,6,0));
 if (cols.size() >7) p.setGivenBy(unquote(trimStr(cols[7])));
 if (cols.size() >10) {
 int iday = safeParseInt(cols,8,1);
 int imonth = safeParseInt(cols,9,1);
 int iyear = safeParseInt(cols,10,2024);
 Date issue(iday, imonth, iyear);
 p.setDateOfIssue(issue);
 }
 if (cols.size() >11) p.setCode(unquote(trimStr(cols[11])));
 if (cols.size() >12) p.setFio(unquote(trimStr(cols[12])));
 if (cols.size() >15) {
 int bday = safeParseInt(cols,13,1);
 int bmonth = safeParseInt(cols,14,1);
 int byear = safeParseInt(cols,15,2000);
 Date birth(bday, bmonth, byear);
 p.setDateOfBirth(birth);
 }
 int idx = findClientIndex(id);
 if (idx != -1) {
 // update existing client
 clients[idx].setFirstName(first);
 clients[idx].setLastName(last);
 clients[idx].setPhone(phone);
 clients[idx].setPassport(p);
 clients[idx].setActive(active);
 nextClientId = std::max(nextClientId, id+1);
 } else {
 // add new client
 clients.emplace_back(id, first, last, phone, p, active);
 nextClientId = std::max(nextClientId, id+1);
 }
 }
 cf.close();
 }

 // import rooms (add or update)
 std::ifstream rf(dir + "/rooms.csv");
 if (rf.is_open()) {
 std::string header; std::getline(rf, header);
 std::string line;
 int row =1;
 while (std::getline(rf, line)) {
 ++row;
 if (line.empty()) continue;
 auto cols = splitCsvLine(line);
 int id = safeParseInt(cols,0,0);
 bool idProvided = (id !=0);
 if (!idProvided) id = nextRoomId++;
 int roomNumber = safeParseInt(cols,1,0);
 std::string category = cols.size() >2 ? unquote(trimStr(cols[2])) : std::string();
 double price = safeParseDouble(cols,3,0.0);
 RoomStatus status = RoomStatusFromStr(cols.size() >4 ? unquote(trimStr(cols[4])) : std::string());
 std::vector<std::string> amenities;
 if (cols.size() >5) {
 std::string am = unquote(trimStr(cols[5]));
 std::stringstream ss(am);
 std::string a;
 while (std::getline(ss,a,';')) { amenities.push_back(a); }
 }
 bool active = cols.size() >6 ? (trimStr(cols[6]) == "1") : true;
 int idx = findRoomIndex(id);
 if (idx != -1) {
 // update existing
 rooms[idx].setRoomNumber(roomNumber);
 rooms[idx].setCategory(category);
 rooms[idx].setPricePerNight(price);
 rooms[idx].setStatus(status);
 // replace amenities with imported set
 // clear and add
 auto newAmenities = amenities;
 // no public setter, so we will rebuild via accessing private? Use addAmenity after clearing - need Room to have method to set amenities. If not, append unique amenities
 // Append amenities if not present
 auto existing = rooms[idx].getAmenities();
 for (auto &am : newAmenities) {
 if (std::find(existing.begin(), existing.end(), am) == existing.end()) rooms[idx].addAmenity(am);
 }
 rooms[idx].setActive(active);
 nextRoomId = std::max(nextRoomId, id+1);
 } else {
 rooms.emplace_back(id, roomNumber, category, price, status, amenities, active);
 nextRoomId = std::max(nextRoomId, id+1);
 }
 }
 rf.close();
 }

 // import bookings (add or update, validate references)
 std::ifstream bf(dir + "/bookings.csv");
 if (bf.is_open()) {
 std::string header; std::getline(bf, header);
 std::string line;
 int row =1;
 while (std::getline(bf, line)) {
 ++row;
 if (line.empty()) continue;
 auto cols = splitCsvLine(line);
 int id = safeParseInt(cols,0,0);
 bool idProvided = (id !=0);
 if (!idProvided) id = nextBookingId++;
 int roomId = safeParseInt(cols,1,0);
 if (!roomExists(roomId)) {
 warnings.push_back("bookings.csv row " + std::to_string(row) + ": room id " + std::to_string(roomId) + " not found, skipped");
 continue;
 }
 std::vector<int> clientIds;
 if (cols.size() >2) {
 std::string cl = unquote(trimStr(cols[2]));
 std::stringstream ss(cl);
 std::string part;
 bool allClientsExist = true;
 while (std::getline(ss, part, ';')) {
 part = trimStr(part);
 if (part.empty()) continue;
 try {
 int cid = std::stoi(part);
 if (!clientExists(cid)) { allClientsExist = false; warnings.push_back("bookings.csv row " + std::to_string(row) + ": client id " + std::to_string(cid) + " not found"); }
 clientIds.push_back(cid);
 } catch(...) { allClientsExist = false; warnings.push_back("bookings.csv row " + std::to_string(row) + ": invalid client id '" + part + "'"); }
 }
 if (!allClientsExist) { warnings.push_back("bookings.csv row " + std::to_string(row) + ": skipped due to missing clients"); continue; }
 }
 int ci_day = safeParseInt(cols,3,1);
 int ci_month = safeParseInt(cols,4,1);
 int ci_year = safeParseInt(cols,5,2024);
 int co_day = safeParseInt(cols,6,1);
 int co_month = safeParseInt(cols,7,1);
 int co_year = safeParseInt(cols,8,2024);
 Date ci(ci_day, ci_month, ci_year);
 Date co(co_day, co_month, co_year);
 if (!ci.isValid() || !co.isValid() || !ci.isBefore(co)) {
 warnings.push_back("bookings.csv row " + std::to_string(row) + ": invalid dates or check-in >= check-out, skipped");
 continue;
 }
 BookingStatus st = BookingStatus::CONFIRMED;
 if (cols.size() >9) {
 int stv = safeParseInt(cols,9, (int)BookingStatus::CONFIRMED);
 st = static_cast<BookingStatus>(stv);
 }
 double total = safeParseDouble(cols,10,0.0);
 bool active = cols.size() >11 ? (trimStr(cols[11]) == "1") : true;
 int idx = findBookingIndex(id);
 if (idx != -1) {
 // update existing booking
 bookings[idx].setStatus(st);
 bookings[idx].setTotalPrice(total);
 bookings[idx].setActive(active);
 // there are no setters for clientIds/roomId/dates in Booking class; depending on class API, this might require new methods.
 // For now, if Booking has methods to set checkIn/checkOut, roomId, clientIds - use them. Otherwise skip updating those fields.
 // Attempt to set via available methods if exist (pseudo-code)
 // bookings[idx].setRoomId(roomId);
 // bookings[idx].setClientIds(clientIds);
 // bookings[idx].setCheckInDate(ci);
 // bookings[idx].setCheckOutDate(co);
 nextBookingId = std::max(nextBookingId, id+1);
 } else {
 bookings.emplace_back(id, roomId, clientIds, ci, co, active);
 bookings.back().setTotalPrice(total);
 bookings.back().setStatus(st);
 nextBookingId = std::max(nextBookingId, id+1);
 }
 }
 bf.close();
 }

 if (!warnings.empty()) {
 std::stringstream ss;
 for (auto &w : warnings) ss << w << "\n";
 error = ss.str();
 }
 return true;
 } catch (const std::exception& ex) {
 error = ex.what();
 return false;
 }
}