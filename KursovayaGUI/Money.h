#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Simple fixed-point money type for RUB with2 decimals (kopeks).
class Money
{
public:
 using storage_type = std::int64_t;

 Money() = default;
 explicit Money(storage_type kopeks) : m_kopeks(kopeks) {}

 static Money FromKopeks(storage_type kopeks) { return Money(kopeks); }

 // Parses "123", "123.4", "123.45" (also accepts comma as decimal separator).
 // Returns false if invalid.
 static bool TryParse(const std::string& s, Money& out)
 {
 std::string t;
 t.reserve(s.size());
 for (char c : s) {
 if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
 if (c == ',') c = '.';
 t.push_back(c);
 }
 if (t.empty()) return false;

 bool neg = false;
 size_t i =0;
 if (t[0] == '-') { neg = true; i =1; }
 if (i >= t.size()) return false;

 // Split into whole + frac
 auto dotPos = t.find('.', i);
 std::string whole = (dotPos == std::string::npos) ? t.substr(i) : t.substr(i, dotPos - i);
 std::string frac = (dotPos == std::string::npos) ? std::string() : t.substr(dotPos +1);

 if (whole.empty()) whole = "0";
 if (!std::all_of(whole.begin(), whole.end(), ::isdigit)) return false;
 if (!frac.empty() && !std::all_of(frac.begin(), frac.end(), ::isdigit)) return false;
 if (frac.size() >2) return false;

 // normalize frac to2 digits
 while (frac.size() <2) frac.push_back('0');

 storage_type w =0;
 try {
 w = static_cast<storage_type>(std::stoll(whole));
 } catch (...) {
 return false;
 }
 storage_type f =0;
 try {
 f = frac.empty() ?0 : static_cast<storage_type>(std::stoll(frac));
 } catch (...) {
 return false;
 }

 storage_type kopeks = w *100 + f;
 if (neg) kopeks = -kopeks;
 out = Money(kopeks);
 return true;
 }

 storage_type ToKopeks() const { return m_kopeks; }

 double ToDouble() const { return static_cast<double>(m_kopeks) /100.0; }

 std::string ToString() const
 {
 storage_type absK = (m_kopeks <0) ? -m_kopeks : m_kopeks;
 storage_type rub = absK /100;
 storage_type kop = absK %100;
 std::ostringstream ss;
 if (m_kopeks <0) ss << '-';
 ss << rub << '.' << std::setw(2) << std::setfill('0') << kop;
 return ss.str();
 }

 // Arithmetic
 Money operator+(const Money& other) const { return Money(m_kopeks + other.m_kopeks); }
 Money operator-(const Money& other) const { return Money(m_kopeks - other.m_kopeks); }
 Money& operator+=(const Money& other) { m_kopeks += other.m_kopeks; return *this; }
 Money& operator-=(const Money& other) { m_kopeks -= other.m_kopeks; return *this; }

 Money operator*(std::int64_t n) const { return Money(m_kopeks * n); }

 // Comparisons
 bool operator<(const Money& other) const { return m_kopeks < other.m_kopeks; }
 bool operator>(const Money& other) const { return m_kopeks > other.m_kopeks; }
 bool operator<=(const Money& other) const { return m_kopeks <= other.m_kopeks; }
 bool operator>=(const Money& other) const { return m_kopeks >= other.m_kopeks; }
 bool operator==(const Money& other) const { return m_kopeks == other.m_kopeks; }
 bool operator!=(const Money& other) const { return m_kopeks != other.m_kopeks; }

private:
 storage_type m_kopeks =0;
};
