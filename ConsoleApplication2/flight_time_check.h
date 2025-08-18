#ifndef FLIGHT_TIME_CHECK_H
#define FLIGHT_TIME_CHECK_H

// Strict date-time validation and "must be in the future" checks
// Compatible with MSVC / Visual Studio. No C++20 required.
// Usage:
//   #include "model.h"        // defines struct ThoiGian { int gio, phut, ngay, thang, nam; }
//   #include "flight_time_check.h"
//
//   std::string err;
//   if (!ValidateFlightDateTime(2025, 8, 20, 7, 45, err)) { /* show err */ }
//   if (!ValidateFlightDateTime(tg, err)) { /* show err */ }
//
// Notes:
// - All comparisons use LOCAL time (mktime).
// - We check calendar validity (leap year, days per month), HH:MM ranges,
//   and ensure the moment is strictly in the future (> now).
// - Year range is clamped to [1900, 2100] by default (adjust below if needed).

#include <ctime>
#include <string>
#include <cstdio>
#include "model.h"   // for ThoiGian (nam, thang, ngay, gio, phut)

namespace flight_time {

    // ---- Configurable limits ----
    inline int MIN_YEAR() { return 1900; }
    inline int MAX_YEAR() { return 2100; }

    // ---- Basic calendar helpers ----
    inline bool isLeap(int y) {
        return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
    }

    inline int daysInMonth(int m, int y) {
        if (m < 1 || m > 12) return 0;
        static const int md[13] = { 0, 31,28,31,30,31,30,31,31,30,31,30,31 };
        if (m == 2) return isLeap(y) ? 29 : 28;
        return md[m];
    }

    // Validate only calendar (Y-M-D) and time (H:M) ranges, not "future"
    inline bool ValidateCalendar(int y, int m, int d, int hh, int mi, std::string& err) {
        if (y < MIN_YEAR() || y > MAX_YEAR()) {
            char buf[96]; std::snprintf(buf, sizeof(buf), "Nam phai trong [%d..%d]", MIN_YEAR(), MAX_YEAR());
            err.assign(buf); return false;
        }
        if (m < 1 || m > 12) { err = "Thang phai tu 1 den 12"; return false; }
        if (hh < 0 || hh > 23) { err = "Gio phai tu 0 den 23"; return false; }
        if (mi < 0 || mi > 59) { err = "Phut phai tu 0 den 59"; return false; }
        int md = daysInMonth(m, y);
        if (d < 1 || d > md) {
            char buf[96]; std::snprintf(buf, sizeof(buf), "Ngay phai tu 1 den %d (thang %d nam %d)", md, m, y);
            err.assign(buf); return false;
        }
        return true;
    }

    // Convert given local Y-M-D HH:MM to time_t, verifying the moment really exists (DST-safe)
    inline bool ToLocalTimeT_Strict(int y, int m, int d, int hh, int mi, time_t& out, std::string& err) {
        std::tm tm_in{};
        tm_in.tm_year = y - 1900;
        tm_in.tm_mon = m - 1;
        tm_in.tm_mday = d;
        tm_in.tm_hour = hh;
        tm_in.tm_min = mi;
        tm_in.tm_sec = 0;
        tm_in.tm_isdst = -1; // ask system to determine DST

        // Keep a copy before normalization
        std::tm original = tm_in;

        time_t tt = std::mktime(&tm_in); // may normalize
        if (tt == (time_t)-1) { err = "Thoi diem khong hop le (ngoai khoang thoi gian he thong)"; return false; }

        // Convert back to local broken-down time and compare with original to ensure it exists (e.g., not a skipped DST time)
        std::tm* back = std::localtime(&tt);
        if (!back) { err = "Khong the chuyen doi thoi gian he thong"; return false; }

        if (back->tm_year != original.tm_year ||
            back->tm_mon != original.tm_mon ||
            back->tm_mday != original.tm_mday ||
            back->tm_hour != original.tm_hour ||
            back->tm_min != original.tm_min ||
            back->tm_sec != original.tm_sec) {
            err = "Thoi diem nay khong ton tai trong mui gio hien tai (co the do DST)";
            return false;
        }

        out = tt;
        return true;
    }

    // Compare to "now" (local). Return: -1 = past, 0 = equal to the minute, 1 = future.
    // If input invalid, returns -2 and sets err.
    inline int CompareToNowLocal(int y, int m, int d, int hh, int mi, std::string& err) {
        if (!ValidateCalendar(y, m, d, hh, mi, err)) return -2;
        time_t target;
        if (!ToLocalTimeT_Strict(y, m, d, hh, mi, target, err)) return -2;

        time_t now = std::time(nullptr);

        // Compare at second resolution; equal if within the same minute boundary
        double diff = std::difftime(target, now);
        if (diff > 60.0) return 1;
        if (diff < -60.0) return -1;

        // Within +-60s, decide by flooring to minutes
        std::tm* lt_target = std::localtime(&target);
        std::tm* lt_now = std::localtime(&now);
        if (!lt_target || !lt_now) return (diff > 0) ? 1 : (diff < 0 ? -1 : 0);

        if (lt_target->tm_year == lt_now->tm_year &&
            lt_target->tm_mon == lt_now->tm_mon &&
            lt_target->tm_mday == lt_now->tm_mday &&
            lt_target->tm_hour == lt_now->tm_hour &&
            lt_target->tm_min == lt_now->tm_min) return 0;

        return (diff > 0) ? 1 : -1;
    }

    // Validate full rules for flight input: valid calendar + strictly future (> now).
    // Set allow_equal_now=true if you want to accept "ngay bay = phut hien tai".
    inline bool ValidateFlightDateTime(int y, int m, int d, int hh, int mi,
        std::string& err, TrangThaiChuyenBay& ttcb) {
        if (!ValidateCalendar(y, m, d, hh, mi, err)) return false;

        time_t target;
        if (!ToLocalTimeT_Strict(y, m, d, hh, mi, target, err)) return false;

        time_t now = std::time(nullptr);
        double diff = std::difftime(target, now);

        
        if (diff < 0) 
            ttcb = HOAN_TAT;
        else {
            ttcb = CON_VE;
        }
        return true;
      
        
    }

    // Validate: lịch hợp lệ + thời điểm tồn tại (local) + cách hiện tại >= min_lead_hours
    inline bool ValidateFlightDateTimeLeadHours(const ThoiGian& t, int min_lead_hours, std::string& err, bool allow_equal = false) {
        if (!ValidateCalendar(t.nam, t.thang, t.ngay, t.gio, t.phut, err)) return false;

        time_t target;
        if (!ToLocalTimeT_Strict(t.nam, t.thang, t.ngay, t.gio, t.phut, target, err)) return false;

        time_t now = std::time(nullptr);
        const double need = (double)min_lead_hours * 3600.0;
        double diff = std::difftime(target, now);
        if (allow_equal ? (diff < need) : (diff <= need - 1e-6)) {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "Thoi gian khoi hanh phai cach hien tai it nhat %d gio", min_lead_hours);
            err.assign(buf);
            return false;
        }
        return true;
    }

    
    inline int CompareToNowLocal(const ThoiGian& t, std::string& err) {
        return CompareToNowLocal(t.nam, t.thang, t.ngay, t.gio, t.phut, err);
    }

} // namespace flight_time

#endif // FLIGHT_TIME_CHECK_H
