#define _CRT_SECURE_NO_WARNINGS

// thu vien c++
#include <iostream>
#include <string>
#include <conio.h>   // _kbhit, _getch
#include <windows.h> // system("cls") on Windows
#include <direct.h>
#include <fstream>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <limits>


// thu vien tu tao
#include "mylib.h"
#include "model.h"
#include "char_utils.h"
#include "flight_time_check.h"

// dinh nghia hang so
#define MAX 100

using namespace std;

int x = 10;
int y = 5;
void ResetColor();
// Khai báo
void box(int x, int y, int w, int h, int t_color, int b_color, const string& nd);
void n_box(int x, int y, int w, int h, int t_color, int b_color, const string nds[], int sl);
void menu();
void thanh_sang(int x, int y, int w, int h, int b_color, const string& nd);
void ClearAt(int x, int y, int w);

//May bay
void FreeDSMB(DSMB& dsmb);
    //them may bay
bool DocDSMB(DSMB& dsmb, string filePath );
bool GhiDSMB(const DSMB& dsmb, string filePath);


bool ThemMayBay(DSMB& dsmb, const char* soHieu, const char* loai, int soCho);
bool Confirm(int x, int y, const char* msg);
int FindIndexBySoHieu(const DSMB& dsmb, const char* sh);
bool XoaMB_TaiViTri(DSMB& dsmb, int idx);
void XemMB(DSMB dsmb, int x0, int y0);

void FormThemMayBay(DSMB& dsmb);
void FormHieuChinhMayBay(DSMB& dsmb);
void FormXoaMayBay(DSMB& dsmb);
// Màn hình con
int ConsoleWidth();
void screen_maybay();
void screen_chuyenbay();
void screen_hanhkhach();

// ==== CHUYEN BAY (khai báo) ====
// Khởi tạo / giải phóng
void InitDSCB(PTRCB& first);
void FreeDSCB(PTRCB& first);

// Tiện ích
int  CountCB(PTRCB first);
nodeChuyenBay* FindCBByMa(PTRCB first, const char* maCB);
bool InsertCB_Append(PTRCB& first, const ChuyenBay& cb);
bool DeleteCB_ByMa(PTRCB& first, const char* maCB);

// File I/O (CSV đơn giản, có dòng đếm ở đầu, giống DSMB)
bool LoadDSCB(PTRCB& first, const char* filePath);
bool SaveDSCB(PTRCB first, const char* filePath);

// In bảng
void XemDSCB(PTRCB first, int x0, int y0);

// Form CRUD
void FormThemChuyenBay(PTRCB& dscb, const DSMB& dsmb);
void FormHieuChinhChuyenBay(PTRCB& dscb, const DSMB& dsmb);
void FormXoaChuyenBay(PTRCB& dscb);

void FormDatVe(PTRCB& dscb);
void FormXemDanhSachVe(PTRCB dscb);
// =============================
int main() {
    DSMB dsmb;

    menu();
    return 0;
}

// =============================
void menu()
{
   
    ShowCur(0);
    // ----- setting ----
    const int w = 20;
    const int h = 2;
    const int step = 2;           // khoảng cách giữa các ô
    const int t_color = 11;
    const int b_color = 1;
    const int b_color_sang = 75;

    // Nội dung từng o
    const int sl = 4;
    string items[sl] = { "MAY BAY", "CHUYEN BAY", "HANH KHACH", "THOAT" };
    ResetColor();

    // Vẽ lần đầu
    system("cls");
    n_box(x, y, w, h, t_color, b_color, items, sl);

    // -------------
    int xp = x;
    int yp = y;          // toạ độ thanh sáng hiện tại
    int xcu = xp;
    int ycu = yp;        // toạ độ thanh sáng trước đó
    bool needRedraw = true;

    while (true)
    {
        if (needRedraw)
        {
            int idxOld = (ycu - y) / step;
            int idxNew = (yp - y) / step;

            thanh_sang(xcu, ycu, w, h, b_color, items[idxOld]); // phục hồi ô cũ
            thanh_sang(xp, yp, w, h, b_color_sang, items[idxNew]); // làm sáng ô mới

            xcu = xp;
            ycu = yp;
            needRedraw = false;
        }

        if (_kbhit())
        {
            char c = _getch();
            if (c == -32 || c == (char)224) // phím mũi tên
            {
                needRedraw = true;
                c = _getch();
                if (c == 72) // Up
                {
                    if (yp != y) yp -= step;
                    else          yp = y + step * (sl - 1);
                }
                else if (c == 80) // Down
                {
                    if (yp != y + step * (sl - 1)) yp += step;
                    else                            yp = y;
                }
            }
            else if (c == 13) // Enter
            {
                int idx = (yp - y) / step;

                if (idx == 0) { // MAY BAY
                    screen_maybay();
                }
                else if (idx == 1) { // CHUYEN BAY
                    screen_chuyenbay();
                }
                else if (idx == 2) { // HANH KHACH
                    screen_hanhkhach();
                }
                else if (idx == 3) { // THOAT
                    break; // thoát chương trình
                }
                ResetColor();

                // Khi màn hình con trả về, vẽ lại menu
                system("cls");
                n_box(x, y, w, h, t_color, b_color, items, sl);
                needRedraw = true;
            }
        }
    }
}

// =============================
// MÀN HÌNH: MÁY BAY
void screen_maybay()
{
    DSMB dsmb;
    while (true)
    {
        ResetColor();
        ShowCur(0);
        system("cls");

        // --- Bảng danh sách ---
        
        DocDSMB(dsmb, "DSMB.txt");
        const int tableX = 5, tableY = 4;
        gotoXY(5, 2);  cout << "=== MAN HINH: MAY BAY ===";
        XemMB(dsmb, tableX, tableY);

        // --- Mini menu bên cạnh bảng ---
        const int w = 22, h = 2, step = 2;
        const int t_color = 11, b_color = 1, b_color_sang = 75;
        const int sl = 4;
        string opts[sl] = { "THEM MAY BAY","HIEU CHINH MAY BAY", "XOA MAY BAY", "QUAY LAI"};

        // VỊ TRÍ MONG MUỐN: x = 20 (theo yêu cầu)
        int mx = tableX + 18+45+8+4;                 // <— anh muốn ở cột 20
        int my = tableY;             // canh hàng đầu của bảng

        // Gợi ý (nếu cần đặt thật sự “bên phải” bảng, tránh chồng lấn):
        // int mx = tableX + (18 + 24 + 8) + 4; // 18+24+8 là tổng độ rộng 3 cột, +4 đệm

        n_box(mx, my, w, h, t_color, b_color, opts, sl);

        // --- Điều khiển ↑/↓ + Enter ---
        int xp = mx, yp = my, xcu = xp, ycu = yp;
        bool needRedraw = true;

        while (true)
        {
            if (needRedraw)
            {
                int idxOld = (ycu - my) / step;
                int idxNew = (yp - my) / step;
                thanh_sang(xcu, ycu, w, h, b_color, opts[idxOld]);
                thanh_sang(xp, yp, w, h, b_color_sang, opts[idxNew]);
                xcu = xp; ycu = yp; needRedraw = false;
            }

            if (_kbhit())
            {
                char c = _getch();
                if (c == 27) { FreeDSMB(dsmb); return; }                         // ESC
                if (c == -32 || c == (char)224) {            // arrows
                    needRedraw = true; c = _getch();
                    if (c == 72) { if (yp != my) yp -= step; else yp = my + step * (sl - 1); }  // Up
                    else if (c == 80) { if (yp != my + step * (sl - 1)) yp += step; else yp = my; } // Down
                }
                else if (c == 13) {                          // Enter
                    int idx = (yp - my) / step;
                    if (idx == 0) {                          // THÊM MÁY BAY
                        ShowCur(1);
                        FormThemMayBay(dsmb);
                        break;                                // vẽ lại toàn màn hình (vòng ngoài)
                    }
                    else if (idx == 1) { ShowCur(1); FormHieuChinhMayBay(dsmb); break; }
                    else if (idx == 2) { ShowCur(1); FormXoaMayBay(dsmb); break; }
                    else if (idx == 3) { FreeDSMB(dsmb); return; }
                   
                }
            }
        }
    }
   
}

int ConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}
// (Có thể để trống 2 màn hình dưới, để sau anh triển khai)
void screen_chuyenbay()
{
    DSMB dsmb; DocDSMB(dsmb, "DSMB.txt");
    PTRCB dscb = nullptr; InitDSCB(dscb); LoadDSCB(dscb, "DSCB.txt");

    while (true)
    {
        ResetColor(); ShowCur(0); system("cls");
        gotoXY(5, 1);  cout << "=== MAN HINH: CHUYEN BAY ===";

        // MENU (trên cùng)
        const int w = 26, h = 2, step = 2;
        const int t_color = 11, b_color = 1, b_color_sang = 75;
        const int sl = 6;
        string opts[sl] = {
            "THEM CHUYEN BAY",
            "HIEU CHINH CHUYEN BAY",
            "XOA CHUYEN BAY",
            "DAT VE",
            "XEM DANH SACH VE",
            "QUAY LAI"
        };

        const int tableX = 5;
        int mx = tableX;
        int my = 3;
        n_box(mx, my, w, h, t_color, b_color, opts, sl);

        int menuBottom = my + (sl - 1) * step + h;
        int tableY = menuBottom + 2;

        // Vẽ bảng
        XemDSCB(dscb, tableX, tableY);

        // Điều khiển phím cho menu
        int xp = mx, yp = my, xcu = xp, ycu = yp;
        bool needRedraw = true;

        while (true) {
            if (needRedraw) {
                int idxOld = (ycu - my) / step;
                int idxNew = (yp - my) / step;
                thanh_sang(xcu, ycu, w, h, b_color, opts[idxOld]);
                thanh_sang(xp, yp, w, h, b_color_sang, opts[idxNew]);
                xcu = xp; ycu = yp; needRedraw = false;
            }
            if (_kbhit()) {
                char c = _getch();
                if (c == 27) { SaveDSCB(dscb, "DSCB.txt"); FreeDSCB(dscb); FreeDSMB(dsmb); return; }
                if (c == -32 || c == (char)224) {
                    needRedraw = true; c = _getch();
                    if (c == 72) { if (yp != my) yp -= step; else yp = my + step * (sl - 1); }
                    else if (c == 80) { if (yp != my + step * (sl - 1)) yp += step; else yp = my; }
                }
                else if (c == 13) {
                    int idx = (yp - my) / step;
                    if (idx == 0) { ShowCur(1); FormThemChuyenBay(dscb, dsmb);         SaveDSCB(dscb, "DSCB.txt"); break; }
                    else if (idx == 1) { ShowCur(1); FormHieuChinhChuyenBay(dscb, dsmb);    SaveDSCB(dscb, "DSCB.txt"); break; }
                    else if (idx == 2) { ShowCur(1); FormXoaChuyenBay(dscb);                SaveDSCB(dscb, "DSCB.txt"); break; }
                    else if (idx == 3) { ShowCur(1); FormDatVe(dscb);                       SaveDSCB(dscb, "DSCB.txt"); break; } // <-- ĐẶT VÉ
                    else if (idx == 4) { ShowCur(1); FormXemDanhSachVe(dscb);                                        break; } // <-- XEM DS VÉ
                    else if (idx == 5) { SaveDSCB(dscb, "DSCB.txt"); FreeDSCB(dscb); FreeDSMB(dsmb); return; }          // QUAY LẠI
                }
            }
        }
    }
}




void screen_hanhkhach()
{
    ResetColor();

    ShowCur(0);
    system("cls");
    SetColor(14);
    gotoXY(5, 2);  cout << "=== MAN HINH: HANH KHACH ===";
    SetColor(7);
    gotoXY(5, 4);  cout << "Nhan ESC de quay lai menu.";
    while (true) { if (_kbhit() && _getch() == 27) break; }
}

// =============================
void thanh_sang(int x, int y, int w, int h, int b_color, const string& nd)
{
    textcolor(b_color);
    for (int iy = y + 1; iy <= y + h - 1; iy++)
        for (int ix = x + 1; ix <= x + w - 1; ix++)
        {
            gotoXY(ix, iy); cout << " ";
        }

    SetColor(7);
    gotoXY(x + 1, y + 1);
    cout << nd;
}

void box(int x, int y, int w, int h, int t_color, int b_color, const string& nd)
{
    textcolor(b_color);
    for (int iy = y + 1; iy <= y + h - 1; iy++)
        for (int ix = x + 1; ix <= x + w - 1; ix++)
        {
            gotoXY(ix, iy); cout << " ";
        }

    SetColor(7);
    gotoXY(x + 1, y + 1);
    cout << nd;

    // viền
    textcolor(1);
    SetColor(t_color);
    if (h <= 1 || w <= 1) return;

    for (int ix = x; ix <= x + w; ix++)
    {
        gotoXY(ix, y);         cout << char(196);
        gotoXY(ix, y + h);     cout << char(196);
    }
    for (int iy = y; iy <= y + h; iy++)
    {
        gotoXY(x, iy);     cout << char(179);
        gotoXY(x + w, iy);     cout << char(179);
    }
    gotoXY(x, y);     cout << char(218);
    gotoXY(x + w, y);     cout << char(191);
    gotoXY(x, y + h); cout << char(192);
    gotoXY(x + w, y + h); cout << char(217);
}

void n_box(int x, int y, int w, int h, int t_color, int b_color, const string nds[], int sl)
{
    for (int i = 0; i < sl; i++)
    {
        box(x, y + (i * 2), w, h, t_color, b_color, nds[i]); // mỗi ô một nội dung
        if (i != 0)
        {
            gotoXY(x, y + (i * 2)); cout << char(195);
            gotoXY(x + w, y + (i * 2)); cout << char(180);
        }
    }
}
void ResetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // fg=7, bg=0
}




bool DocDSMB(DSMB& dsmb, string filePath )
{
    FreeDSMB(dsmb);
    ifstream fin(filePath);
    if (!fin)
        return false;

    // reset
    dsmb.n = 0;
    int soLuong = 0;
    if (!(fin >> soLuong))
        return false;

    fin.ignore(1000, '\n');

    char line[256];

    while (fin.getline(line, sizeof(line)) && dsmb.n < soLuong && dsmb.n < MAX_MB)
    {
        Trim(line);
        if (line[0] == '\0')
            continue;


        char soHieu[MAX_SO_HIEU_MB], loai[MAX_LOAI_MB], soChoStr[16];

        strncpy_s(soHieu, line, MAX_SO_HIEU_MB - 1);
        soHieu[15] = '\0';

        strncpy_s(loai, line + MAX_SO_HIEU_MB -1, MAX_LOAI_MB-1);
        loai[20] = '\0';

        strncpy_s(soChoStr, line + MAX_SO_HIEU_MB + MAX_LOAI_MB -1, sizeof(soChoStr) - 1);
        soChoStr[sizeof(soChoStr) - 1] = '\0';

        Trim(soHieu);
        Trim(loai);
        Trim(soChoStr);

        int soCho = atoi(soChoStr);

      
        MayBay* mb = new MayBay;
        strcpy_s(mb->soHieuMB, soHieu);
        strcpy_s(mb->loaiMB, loai);
        mb->soCho = soCho;

        dsmb.nodes[dsmb.n++] = mb;
    }

    return dsmb.n > 0;
}

void XemMB(DSMB dsmb, int x0, int y0) {
    // Header
    gotoXY(x0, y0);     cout << left << setw(18) << "SO HIEU"
        << setw(24) << "LOAI"
        << setw(8) << "SO CHO";
    gotoXY(x0, y0 + 1);   cout << string(18 + 45 + 8, '-');

    // Dòng dữ liệu
    y0 = y0 + 2;
    for (int i = 0; i < dsmb.n; ++i) {
        gotoXY(x0, y0 + i);
        cout << left << setw(18) << dsmb.nodes[i]->soHieuMB
            << setw(24) << dsmb.nodes[i]->loaiMB
            << setw(8) << dsmb.nodes[i]->soCho;
    }
}

void FreeDSMB(DSMB& dsmb) {
    for (int i = 0; i < dsmb.n; ++i) {
        delete dsmb.nodes[i];
        dsmb.nodes[i] = nullptr;
    }
    dsmb.n = 0;
}





// thêm vào bộ nhớ
bool ThemMayBay(DSMB& dsmb, const char* soHieu, const char* loai, int soCho) {
    if (dsmb.n >= MAX_MB) return false;

    MayBay* mb = new MayBay;
    strncpy_s(mb->soHieuMB, soHieu, sizeof(mb->soHieuMB) - 1);
    mb->soHieuMB[sizeof(mb->soHieuMB) - 1] = '\0';

    strncpy_s(mb->loaiMB, loai, sizeof(mb->loaiMB) - 1);
    mb->loaiMB[sizeof(mb->loaiMB) - 1] = '\0';

    mb->soCho = soCho;
    dsmb.nodes[dsmb.n++] = mb;
    return true;
}

// ghi lại toàn bộ file (để cập nhật dòng đếm + canh cột)
bool GhiDSMB(const DSMB& dsmb, string filePath ) {
    ofstream out(filePath);
    if (!out) return false;
    out << dsmb.n << "\n";
    for (int i = 0; i < dsmb.n; i++) {
        out << left
            << setw(15) << dsmb.nodes[i]->soHieuMB
            << setw(41) << dsmb.nodes[i]->loaiMB
            << dsmb.nodes[i]->soCho << "\n";
    }
    return true;
}



// form nhập máy bay mới
void FormThemMayBay(DSMB& dsmb) {
    ResetColor();
    system("cls");
    ShowCur(1);

    gotoXY(5, 2);  cout << "=== THEM MAY BAY MOI ===";
    gotoXY(5, 4);  cout << "Nhap thong tin (Enter de xac nhan, ESC de thoat):";
    gotoXY(5, 6);  cout << "So hieu  <KT1><KT2>-<1-4so>: ";
    gotoXY(5, 7);  cout << "Loai     (<=40 ky tu): ";
    gotoXY(5, 8);  cout << "So cho   (so nguyen) : ";

    // đọc chuỗi an toàn
    char soHieu[MAX_SO_HIEU_MB], loaiMayBay[MAX_LOAI_MB];
    int sc = 0;
    bool valid = false;
    while(!valid) {
        gotoXY(34, 6); cin.getline(soHieu,MAX_SO_HIEU_MB);
        RemoveAllSpaces(soHieu);
        UpperCase(soHieu);
       
        if (KiemTraDoDai(soHieu, 4, 7) == 0 ||
            !(isalpha(soHieu[0]) && isalpha(soHieu[1]) && soHieu[2] == '-') ||
            KiemTraPhanSauLaChuSo(3, strlen(soHieu), soHieu) == false)
        {
            gotoXY(46, 6); cout << "                                          ";
            gotoXY(46, 6); cout << " => Sai dinh dang!                                                      ";
            gotoXY(34, 6); cout << "           ";
            continue;
        }
        valid = true;
        for (int i = 0; i < dsmb.n; i++)
        {
            if (strcmp(dsmb.nodes[i]->soHieuMB, soHieu) == 0)
            {

                gotoXY(46, 6); cout << "Loi: may bay co so hieu \"" << soHieu
                    << "\" da ton tai. Khong the them.\n";
                gotoXY(34, 6); cout << "           ";
                valid = false;
                break;
            }
        }
    }
    
    while (true)
    {
        gotoXY(28, 7); cin.getline(loaiMayBay, MAX_LOAI_MB);
        if (IsEmpty(loaiMayBay) || strcmp(loaiMayBay, "0") == 0 || !isalpha(loaiMayBay[0]))
        {
            gotoXY(40, 7); cout << "Sai dinh dang" << endl;
            continue;
        }
        break;
    }
    NormalizeSpaces(loaiMayBay);
    UpperCase(loaiMayBay);
    do
    {
        
        gotoXY(28, 8); cin >> sc;
        if (sc < 20)
        {
            gotoXY(40, 8); cout << "So cho phai lon hon hoac bang 20. Vui long nhap lai!\n";
        }
    } while (sc < 20);
     cin.ignore(1000, '\n');


    if (!ThemMayBay(dsmb, soHieu, loaiMayBay, sc)) {
        gotoXY(5, 10); cout << "=> Khong the them (day danh sach?).";
        _getch(); return;
    }

    if (!GhiDSMB(dsmb, "DSMB.txt")) {
        gotoXY(5, 10); cout << "=> Them thanh cong NHUNG khong ghi duoc file!";
        _getch(); return;
    }

    gotoXY(5, 10); cout << "=> Da them & ghi file thanh cong. Nhan phim bat ky de quay lai.";
    _getch();
}

void ClearAt(int x, int y, int w) { gotoXY(x, y); cout << string(w, ' '); gotoXY(x, y); }

bool Confirm(int x, int y, const char* msg) {
    gotoXY(x, y); cout << msg << " (Y/N)"; int ch = _getch();
    ClearAt(x, y, (int)strlen(msg) + 6); return ch == 'y' || ch == 'Y';
}

int FindIndexBySoHieu(const DSMB& dsmb, const char* sh) {
    for (int i = 0; i < dsmb.n; i++) if (_stricmp(dsmb.nodes[i]->soHieuMB, sh) == 0) return i;
    return -1;
}
bool XoaMB_TaiViTri(DSMB& dsmb, int idx) {
    if (idx < 0 || idx >= dsmb.n) return false;
    delete dsmb.nodes[idx];
    for (int i = idx; i < dsmb.n - 1; i++) dsmb.nodes[i] = dsmb.nodes[i + 1];
    dsmb.nodes[dsmb.n - 1] = nullptr; dsmb.n--;
    return true;
}

void FormHieuChinhMayBay(DSMB& dsmb) {
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2);  cout << "=== HIEU CHINH MAY BAY ===";
    gotoXY(5, 4);  cout << "Nhap SO HIEU can sua (0 de huy): ";
    char soHieuCanSua[MAX_SO_HIEU_MB]; gotoXY(38, 4); cin.getline(soHieuCanSua, sizeof(soHieuCanSua));
    if (strcmp(soHieuCanSua, "0") == 0 || IsEmpty(soHieuCanSua)) { ShowCur(0); return; }
    RemoveAllSpaces(soHieuCanSua); UpperCase(soHieuCanSua);

    int idx = FindIndexBySoHieu(dsmb, soHieuCanSua);
    if (idx < 0) { gotoXY(5, 6); cout << "Khong tim thay! Nhan phim bat ky de quay lai"; _getch(); ShowCur(0); return; }

    MayBay* mb = dsmb.nodes[idx];

    // Hien gia tri cu
    gotoXY(5, 6);  cout << "(Enter de giu nguyen)";
    gotoXY(5, 8);  cout << "So hieu moi  [cu: " << mb->soHieuMB << "]: ";
    gotoXY(5, 9);  cout << "Loai moi     [cu: " << mb->loaiMB << "]: ";
    gotoXY(5, 10); cout << "So cho moi   [cu: " << mb->soCho << "]: ";

    // Doc chuoi
    char soHieuMoi[MAX_SO_HIEU_MB] = { 0 }, loaiMoi[MAX_LOAI_MB] = { 0 }, soChoMoi[5] = { 0 };

    // --- So hieu ---
    while (true) {
        gotoXY(35, 8); 
        if (!cin.getline(soHieuMoi, sizeof(soHieuMoi))) {
            // người dùng gõ quá dài -> failbit
            cin.clear();
            cin.ignore(1000, '\n');
            gotoXY(5, 12); cout << "Nhap qua dai (<= 15). Thu lai.";
            ClearAt(35, 8, 50);
            continue;
        }
        
        if (IsEmpty(soHieuMoi)) break;
        RemoveAllSpaces(soHieuMoi); UpperCase(soHieuMoi);
        if (KiemTraDoDai(soHieuMoi, 4, 7) == 0 ||
            !(isalpha(soHieuMoi[0]) && isalpha(soHieuMoi[1]) && soHieuMoi[2] == '-') ||
            KiemTraPhanSauLaChuSo(3, strlen(soHieuMoi), soHieuMoi) == false) {
            gotoXY(5, 12); cout << "So hieu moi sai dinh dang!"; ClearAt(35, 8, 50);  continue;
        }

        if (FindIndexBySoHieu(dsmb, soHieuMoi) >= 0 && _stricmp(mb->soHieuMB, soHieuMoi) != 0) {
            gotoXY(5, 12); cout << "So hieu moi bi trung!          ";  ClearAt(35, 8, 50);  continue;
        }
        strncpy_s(mb->soHieuMB, soHieuMoi, sizeof(mb->soHieuMB));
        break;
        
    }
    // --- Loai ---
    while (true) {
        gotoXY(35, 9);  
        if (!cin.getline(loaiMoi, sizeof(loaiMoi))) {
            // người dùng gõ quá dài -> failbit
            cin.clear();
            cin.ignore(1000, '\n');
            gotoXY(5, 12); cout << "Nhap qua dai (<= 40). Thu lai.";
            ClearAt(35, 9, 100);
            continue;
        }
        if (IsEmpty(loaiMoi)) break; 

        NormalizeSpaces(loaiMoi); UpperCase(loaiMoi);
        if (!isalpha((unsigned char)loaiMoi[0])) { gotoXY(5, 12); cout << "Loai moi sai dinh dang!"; ClearAt(35, 9, 50);  continue; }
        strncpy_s(mb->loaiMB, loaiMoi, sizeof(mb->loaiMB));
        break;
    }

    // --- So cho ---
    long int soChoMoi_int; char* endp;
    while (true) {
        gotoXY(35, 10);
        if (!cin.getline(soChoMoi, sizeof(soChoMoi))) {
            // người dùng gõ quá dài -> failbit
            cin.clear();
            cin.ignore(1000, '\n');
            gotoXY(5, 12); cout << "Nhap qua dai. Thu lai.";
            ClearAt(35, 10, 5);
            continue;
        }
        if (IsEmpty(soChoMoi)) break;
      
        soChoMoi_int = strtol(soChoMoi, &endp, 10); //strtol: chuyen kieu chuoi ve int
        if (endp == soChoMoi || *endp != '\0' || soChoMoi_int < 20 || soChoMoi_int > 2000) {
            gotoXY(5, 12); cout << "So cho moi khong hop le (>=20 & <=2000)!"; ClearAt(35, 10, 5);  ShowCur(0);
            continue;
        }
        mb->soCho = (int)soChoMoi_int;
        break;
    }
   
    

  
    if (!GhiDSMB(dsmb, "DSMB.txt")) {
        gotoXY(5, 14); cout << "Cap nhat thanh cong NHUNG khong ghi duoc file!"; _getch(); ShowCur(0); return;
    }
    gotoXY(5, 14); cout << "Da cap nhat & ghi file. Nhan phim bat ky..."; _getch(); ShowCur(0);
}
void FormXoaMayBay(DSMB& dsmb) {
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2); cout << "=== XOA MAY BAY ===";
    gotoXY(5, 4); cout << "Nhap SO HIEU can xoa (0 de huy): ";
    char soHieuCanXoa[MAX_SO_HIEU_MB]; gotoXY(37, 4); cin.getline(soHieuCanXoa, sizeof(soHieuCanXoa));
    if (strcmp(soHieuCanXoa, "0") == 0 || IsEmpty(soHieuCanXoa)) { ShowCur(0); return; }
    RemoveAllSpaces(soHieuCanXoa); UpperCase(soHieuCanXoa);

    int idx = FindIndexBySoHieu(dsmb, soHieuCanXoa);
    if (idx < 0) { gotoXY(5, 6); cout << "Khong tim thay! Nhan phim bat ky..."; _getch(); ShowCur(0); return; }

    // Show thong tin truoc khi xoa
    gotoXY(5, 6);  cout << "So hieu: " << dsmb.nodes[idx]->soHieuMB;
    gotoXY(5, 7);  cout << "Loai   : " << dsmb.nodes[idx]->loaiMB;
    gotoXY(5, 8);  cout << "So cho : " << dsmb.nodes[idx]->soCho;

    if (!Confirm(5, 10, "Xac nhan XOA?")) { ShowCur(0); return; }

    if (!XoaMB_TaiViTri(dsmb, idx)) { gotoXY(5, 12); cout << "Xoa that bai!"; _getch(); ShowCur(0); return; }
    if (!GhiDSMB(dsmb, "DSMB.txt")) { gotoXY(5, 12); cout << "Xoa xong NHUNG khong ghi duoc file!"; _getch(); ShowCur(0); return; }

    gotoXY(5, 12); cout << "Da xoa & ghi file. Nhan phim bat ky..."; _getch(); ShowCur(0);
}


// ==== DANH SÁCH CHUYẾN BAY: KHỞI TẠO / GIẢI PHÓNG ====
void InitDSCB(PTRCB& first) { first = nullptr; }

void FreeDSCB(PTRCB& first)
{
    while (first)
    {
        PTRCB p = first; first = first->next;
        if (p->cb.dsVe.danhSach) delete[] p->cb.dsVe.danhSach;
        delete p;
    }
}

int CountCB(PTRCB first)
{
    int c = 0; for (PTRCB p = first; p; p = p->next) ++c; return c;
}

PTRCB FindCBByMa(PTRCB first, const char* maCB)
{
    for (PTRCB p = first; p; p = p->next)
        if (_stricmp(p->cb.maCB, maCB) == 0) return p;
    return nullptr;
}

bool InsertCB_Append(PTRCB& first, const ChuyenBay& cb)
{
    nodeChuyenBay* node = new nodeChuyenBay;
    node->cb = cb; node->next = nullptr;
    if (!first) { first = node; return true; }
    PTRCB p = first; while (p->next) p = p->next; p->next = node; return true;
}

bool DeleteCB_ByMa(PTRCB& first, const char* maCB)
{
    PTRCB prev = nullptr, cur = first;
    while (cur && _stricmp(cur->cb.maCB, maCB) != 0) { prev = cur; cur = cur->next; }
    if (!cur) return false;
    if (prev) prev->next = cur->next; else first = cur->next;
    if (cur->cb.dsVe.danhSach) delete[] cur->cb.dsVe.danhSach;
    delete cur; return true;
}

// ==== FILE I/O: CSV có dòng đếm đầu tiên ====
static bool parseInt(const char* s, int& out) {
    char* endp = nullptr; long v = strtol(s, &endp, 10);
    if (endp == s || *endp != '\0') return false; out = (int)v; return true;
}

bool LoadDSCB(PTRCB& first, const char* filePath)
{
    FreeDSCB(first);

    ifstream in(filePath);
    if (!in) return true; // Không có file -> danh sách rỗng

    // Đọc N (dùng getline để tránh trộn >> với getline)
   string s;
    if (!getline(in, s)) return true;
    if (!s.empty() && s.back() == '\r') s.pop_back();
    int n = 0;
    try { n = std::stoi(s); }
    catch (...) { return false; }

    for (int i = 0; i < n; ++i)
    {
        // Đọc 1 dòng header (bỏ dòng trống nếu có)
        std::string line;
        do {
            if (!std::getline(in, line)) return false;
            if (!line.empty() && line.back() == '\r') line.pop_back();
        } while (line.empty());

        // Tách 11 trường bằng dấu phẩy
        char buf[512];
        std::strncpy(buf, line.c_str(), sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char* ctx = nullptr;
        char* tok = std::strtok(buf, ",", &ctx);
        if (!tok) return false;

        ChuyenBay cb{};
        cb.dsVe.soVeDaDat = 0;
        cb.dsVe.danhSach = nullptr;

        // maCB
        std::strcpy(cb.maCB, tok);

        // yyyy,mm,dd,hh,mi
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; cb.ngayGioKhoiHanh.nam = atoi(tok);
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; cb.ngayGioKhoiHanh.thang = atoi(tok);
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; cb.ngayGioKhoiHanh.ngay = atoi(tok);
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; cb.ngayGioKhoiHanh.gio = atoi(tok);
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; cb.ngayGioKhoiHanh.phut = atoi(tok);

        // sanBayDen, soHieuMB
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; strcpy_s(cb.sanBayDen, tok);
        tok = std::strtok(nullptr, ",", &ctx); if (!tok) return false; strcpy_s(cb.soHieuMB, tok);

        // soChoMax, ttcb, soVeDaDat
        tok = strtok_s(nullptr, ",", &ctx); if (!tok) return false; cb.soChoMax = std::atoi(tok);
        if (cb.soChoMax < 0) cb.soChoMax = 0; // <-- thay cho std::max
        tok = strtok_s(nullptr, ",", &ctx); if (!tok) return false; {
            int tt = atoi(tok);
            if (tt < 0 || tt > 3) tt = CON_VE;
            cb.ttcb = (TrangThaiChuyenBay)tt;
        }
        tok = strtok_s(nullptr, ",", &ctx); if (!tok) return false; int k = std::atoi(tok);
        if (k < 0) k = 0;
        if (k > cb.soChoMax) k = cb.soChoMax;

        // Cấp mảng vé
        if (cb.soChoMax > 0) cb.dsVe.danhSach = new Ve[cb.soChoMax];

        // Đọc k dòng CMND
        for (int j = 0; j < k; ++j) {
            if (!std::getline(in, s)) break;
            if (!s.empty() && s.back() == '\r') s.pop_back();
            // trim trắng 2 đầu
            size_t L = 0, R = s.size();
            while (L < R && (s[L] == ' ' || s[L] == '\t')) ++L;
            while (R > L && (s[R - 1] == ' ' || s[R - 1] == '\t')) --R;
            s = s.substr(L, R - L);

            if (cb.dsVe.danhSach)
                strcpy_s(cb.dsVe.danhSach[cb.dsVe.soVeDaDat].soCMND, s.c_str());
            cb.dsVe.soVeDaDat++;
        }

        // cập nhật trạng thái nếu full
        if (cb.dsVe.soVeDaDat >= cb.soChoMax) cb.ttcb = HET_VE;

        InsertCB_Append(first, cb);  // <-- dùng 'first', không phải 'dscb'
    }

    return true;
}


bool SaveDSCB(PTRCB first, const char* filePath)
{
    std::ofstream out(filePath);
    if (!out) return false;

    int n = CountCB(first);
    out << n << "\n";

    for (PTRCB p = first; p; p = p->next)
    {
        const ChuyenBay& c = p->cb;

        // DÒNG HEADER (11 trường)
        out << c.maCB << ","
            << c.ngayGioKhoiHanh.nam << ","
            << c.ngayGioKhoiHanh.thang << ","
            << c.ngayGioKhoiHanh.ngay << ","
            << c.ngayGioKhoiHanh.gio << ","
            << c.ngayGioKhoiHanh.phut << ","
            << c.sanBayDen << ","
            << c.soHieuMB << ","
            << c.soChoMax << ","
            << (int)c.ttcb << ","
            << c.dsVe.soVeDaDat
            << "\n";

        // k DÒNG CMND
        for (int i = 0; i < c.dsVe.soVeDaDat; ++i) {
            out << c.dsVe.danhSach[i].soCMND << "\n";
        }
        // (không bắt buộc) out << "\n"; // nếu thích để 1 dòng trống giữa các block
    }
    return true;
}

// ==== IN BẢNG ====
static const char* TrangThaiStr(TrangThaiChuyenBay t)
{
    switch (t) {
    case HUY_CHUYEN: return "HUY";
    case CON_VE:     return "CON VE";
    case HET_VE:     return "HET VE";
    case HOAN_TAT:   return "HOAN TAT";
    default:         return "?";
    }
}
void XemDSCB(PTRCB first, int x0, int y0)
{
    gotoXY(x0, y0); cout << left
        << setw(12) << "MA CB"
        << setw(20) << "NGAY GIO"
        << setw(24) << "SAN BAY DEN"
        << setw(16) << "SO HIEU MB"
        << setw(10) << "SO CHO"
        << setw(12) << "TRANG THAI"
        << setw(10) << "DA DAT";

    gotoXY(x0, y0 + 1); cout << string(12 + 20 + 24 + 16 + 10 + 12 + 10, '-');

    int r = 0;
    for (PTRCB p = first; p; p = p->next, ++r)
    {
        const ChuyenBay& c = p->cb;
        char bufDT[32];
        snprintf(bufDT, sizeof(bufDT), "%04d-%02d-%02d %02d:%02d",
            c.ngayGioKhoiHanh.nam, c.ngayGioKhoiHanh.thang, c.ngayGioKhoiHanh.ngay,
            c.ngayGioKhoiHanh.gio, c.ngayGioKhoiHanh.phut);

        gotoXY(x0, y0 + 2 + r);
        cout << left
            << setw(12) << c.maCB
            << setw(20) << bufDT
            << setw(24) << c.sanBayDen
            << setw(16) << c.soHieuMB
            << setw(10) << c.soChoMax
            << setw(12) << TrangThaiStr(c.ttcb)
            << setw(10) << c.dsVe.soVeDaDat;
    }
}

// Helpers
static bool ValidDateTime(const ThoiGian& t)
{
    if (t.nam < 1900 || t.nam > 2100) return false;
    if (t.thang < 1 || t.thang > 12)  return false;
    if (t.ngay < 1 || t.ngay  > 31)  return false;
    if (t.gio < 0 || t.gio   > 23)  return false;
    if (t.phut < 0 || t.phut  > 59)  return false;
    return true; // đơn giản; không xét số ngày theo tháng
}
static int FindIndexBySoHieu_MB(const DSMB& dsmb, const char* sh)
{
    for (int i = 0; i < dsmb.n; ++i)
        if (_stricmp(dsmb.nodes[i]->soHieuMB, sh) == 0) return i;
    return -1;
}
void FormThemChuyenBay(PTRCB& dscb, const DSMB& dsmb)
{
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2);  cout << "=== THEM CHUYEN BAY ===";
    gotoXY(5, 4);  cout << "MA CHUYEN BAY (VD: VN 123) : ";

    gotoXY(5, 5);  cout << "NAM (YYYY)           : ";
    gotoXY(5, 6);  cout << "THANG (MM)           : ";
    gotoXY(5, 7);  cout << "NGAY (NN)           : ";

    gotoXY(5, 8);  cout << "GIO  (HH)                : ";
    gotoXY(5, 9);  cout << "PHUT  (MM)                : ";
    gotoXY(5, 10);  cout << "SAN BAY DEN (<=20 ky tu)    : ";
    gotoXY(5, 11);  cout << "SO HIEU MAY BAY (VD: VN-123): ";
    gotoXY(5, 12);  cout << "TRANG THAI (0 HUY / 1 CON VE / 2 HET VE / 3 HOAN TAT): ";
   
    char maCB[MAX_MA_CB_LENGTH] = { 0 };
    char sanBayDen[MAX_SANBAY_DEN] = { 0 };
    char soHieuMB[MAX_SO_HIEU_MB] = { 0 };
    ThoiGian tg{ 0,0,0,0,0 };

    // MA CB (theo format anh đang dùng: 2 chữ + ' ' + số)
    while (true)
    {
        ClearAt( 36, 4, 50 );
        gotoXY(36, 4);
        if (!cin.getline(maCB, sizeof(maCB))) {
            NormalizeSpaces(maCB); UpperCase(maCB);
            // người dùng gõ quá dài -> failbit
            cin.clear();
            cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "Nhap qua dai (<= 15). Thu lai.         ";
            ClearAt(35, 4, 50);
            continue;
        }
        NormalizeSpaces(maCB); UpperCase(maCB);

        if (KiemTraDoDai(maCB, 4, 7) == 0 ||
            !(isalpha((unsigned char)maCB[0]) && isalpha((unsigned char)maCB[1]) && maCB[2] == SPACE_IN_ASCII) ||
            KiemTraPhanSauLaChuSo(3, (int)strlen(maCB), maCB) == false)
        {
            gotoXY(5, 13); cout << "  >> Sai dinh dang! (VD: VN<space>123)           ";
            continue;
        }
        if (FindCBByMa(dscb, maCB)) {
            gotoXY(5, 13); cout << "  >> Ma CB bi trung!                               ";
            continue;
        }
        ClearAt(5, 13, 60);        
        break;
    }

    // NHAP THOI GIAN
    while (true)
    {
        string err;
        int y, m, d, hh, mi;

        // NAM
        gotoXY(36, 5);
        ClearAt(36, 5, 50);
        if (!(cin >> y)) {
            cin.clear(); cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "  >> Nam khong hop le!                            ";
            continue;
        }

        // THANG
        gotoXY(36, 6); ClearAt(36, 6, 50);
        if (!(cin >> m)) {
            cin.clear(); cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "  >> Thang khong hop le!                          ";
            continue;
        }

        // NGAY
        gotoXY(36, 7); ClearAt(36, 7, 50);
        if (!(cin >> d)) {
            cin.clear(); cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "  >> Ngay khong hop le!                           ";
            continue;
        }

        // GIO
        gotoXY(36, 8); ClearAt(36, 8, 50);
        if (!(cin >> hh)) {
            cin.clear(); cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "  >> Gio khong hop le!                            ";
            continue;
        }

        // PHUT
        gotoXY(36, 9); ClearAt(36, 9, 50);
        if (!(cin >> mi)) {
            cin.clear(); cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "  >> Phut khong hop le!                           ";
            continue;
        }

        cin.ignore(1000, '\n');

        tg.nam = y; tg.thang = m; tg.ngay = d; tg.gio = hh; tg.phut = mi;

        // Kiem tra: lich hop le + phai o TUONG LAI (local time)
        if(!flight_time::ValidateFlightDateTime(y,m,d,hh,mi, err,/* allow_equal=*/true)) {
            gotoXY(5, 13); cout << "  >> " << err << string(20, ' ');
            continue;
        }
        if (!flight_time::ValidateFlightDateTimeLeadHours(tg, 12, err ,/* allow_equal=*/true)) {
            gotoXY(5, 13); cout << "  >> " << err << string(20, ' ');
            continue;
        }


        // Xoa thong bao loi va thoat vong
        gotoXY(5, 13); cout << string(70, ' ');
        break;
    }

    // SAN BAY DEN
    while (true)
    {
        gotoXY(36, 10); cin.getline(sanBayDen, sizeof(sanBayDen));
        if (IsEmpty(sanBayDen) || !isalpha((unsigned char)sanBayDen[0])) {
            ClearAt(5, 10, 50);
            gotoXY(5, 13); cout << "  >> San bay den khong hop le!                      ";
            continue;
        }
        NormalizeSpaces(sanBayDen); UpperCase(sanBayDen);
        ClearAt(5, 13, 60);
        break;
    }

    // SO HIEU MB
    int idxMB = -1; int soChoMax = 0;
    while (true)
    {
        ClearAt(36, 11, 60);
        gotoXY(36, 11); cin.getline(soHieuMB, sizeof(soHieuMB));
        RemoveAllSpaces(soHieuMB); UpperCase(soHieuMB);
        if (IsEmpty(soHieuMB)) {
            ClearAt(5, 13, 60);
            gotoXY(5, 13); cout << "  >> So hieu may bay khong duoc de trong!          ";
            continue;
        }
        idxMB = FindIndexBySoHieu_MB(dsmb, soHieuMB);
        if (idxMB < 0) {
            ClearAt(5, 13, 60);
            gotoXY(5, 13); cout << "  >> Khong tim thay may bay trong DSMB!            ";
            continue;
        }
        soChoMax = dsmb.nodes[idxMB]->soCho; // số chỗ theo MB
        ClearAt(5, 13, 60);
        break;
    }

    TrangThaiChuyenBay ttcb = CON_VE; // mac dinh
    int input_ttcb;
   
    while (true) {
        
        // đọc số
        if (!(cin >> input_ttcb)) {
            cin.clear();
            cin.ignore(1000, '\n');
            ClearAt(5, 13, 60);
            gotoXY(5, 13); cout << "  >> Nhap so 0..3";
            continue;
        }
        cin.ignore(1000, '\n'); // bỏ newline để tránh nuốt getline sau

        // phạm vi 0..3
        if (input_ttcb < 0 || input_ttcb > 3) {
            ClearAt(5, 13, 60);
            gotoXY(5, 13); cout << "  >> Nhap gia tri tu 0 toi 3";
            continue;
        }

        ttcb = (TrangThaiChuyenBay)input_ttcb;
        ClearAt(5, 13, 60);
        break;
    }


    // Tạo chuyến bay
    ChuyenBay cb{};
    strncpy_s(cb.maCB, maCB, sizeof(cb.maCB));
    cb.ngayGioKhoiHanh = tg;
    strncpy_s(cb.sanBayDen, sanBayDen, sizeof(cb.sanBayDen));
    strncpy_s(cb.soHieuMB, soHieuMB, sizeof(cb.soHieuMB));
    cb.soChoMax = soChoMax;
    cb.ttcb = ttcb;
    cb.dsVe.soVeDaDat = 0;
    cb.dsVe.danhSach = (soChoMax > 0) ? new Ve[soChoMax] : nullptr;

    InsertCB_Append(dscb, cb);

    gotoXY(5, 14); cout << "=> Da them chuyen bay. Nhan phim bat ky..."; _getch(); ShowCur(0);
}

void FormHieuChinhChuyenBay(PTRCB& dscb, const DSMB& dsmb)
{
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2); cout << "=== HIEU CHINH CHUYEN BAY ===";
    gotoXY(5, 4); cout << "Nhap MA CB can sua (0 de huy): ";
    char maCB[MAX_MA_CB_LENGTH]; gotoXY(36, 4); cin.getline(maCB, sizeof(maCB));
    if (strcmp(maCB, "0") == 0 || IsEmpty(maCB)) { ShowCur(0); return; }
    RemoveAllSpaces(maCB); UpperCase(maCB);

    nodeChuyenBay* node = FindCBByMa(dscb, maCB);
    if (!node) { gotoXY(5, 6); cout << "Khong tim thay!"; _getch(); ShowCur(0); return; }

    ChuyenBay& c = node->cb;

    gotoXY(5, 6);  cout << "(Enter bo qua de giu nguyen)";
    gotoXY(5, 8);  cout << "Ngay (YYYY MM DD)  [cu: " << c.ngayGioKhoiHanh.nam << "-" << c.ngayGioKhoiHanh.thang << "-" << c.ngayGioKhoiHanh.ngay << "]: ";
    gotoXY(5, 9);  cout << "Gio  (HH MM)       [cu: " << c.ngayGioKhoiHanh.gio << ":" << c.ngayGioKhoiHanh.phut << "]: ";
    gotoXY(5, 10); cout << "San bay den        [cu: " << c.sanBayDen << "]: ";
    gotoXY(5, 11); cout << "So hieu may bay    [cu: " << c.soHieuMB << "]: ";
    gotoXY(5, 12); cout << "Trang thai (0 HUY / 1 CON VE / 2 HET VE / 3 HOAN TAT) [cu: " << (int)c.ttcb << "]: ";

    // Date
    char line[64];
    gotoXY(38, 8);
    if (cin.getline(line, sizeof(line)) && !IsEmpty(line)) {
        int y, m, d; if (sscanf_s(line, "%d %d %d", &y, &m, &d) == 3) {
            ThoiGian t = c.ngayGioKhoiHanh; t.nam = y; t.thang = m; t.ngay = d;
            if (ValidDateTime(t)) c.ngayGioKhoiHanh = t;
        }
    }
    // Time
    gotoXY(38, 9);
    if (cin.getline(line, sizeof(line)) && !IsEmpty(line)) {
        int hh, mm; if (sscanf_s(line, "%d %d", &hh, &mm) == 2) {
            ThoiGian t = c.ngayGioKhoiHanh; t.gio = hh; t.phut = mm;
            if (ValidDateTime(t)) c.ngayGioKhoiHanh = t;
        }
    }
    // San bay den
    gotoXY(28, 10);
    if (cin.getline(line, sizeof(line)) && !IsEmpty(line)) {
        NormalizeSpaces(line); UpperCase(line);
        if (isalpha((unsigned char)line[0])) strncpy_s(c.sanBayDen, line, sizeof(c.sanBayDen) - 1);
    }
    // So hieu MB (đổi máy bay => phải đủ chỗ)
    gotoXY(28, 11);
    if (cin.getline(line, sizeof(line)) && !IsEmpty(line)) {
        RemoveAllSpaces(line); UpperCase(line);
        int idx = FindIndexBySoHieu_MB(dsmb, line);
        if (idx >= 0) {
            int newCap = dsmb.nodes[idx]->soCho;
            if (newCap >= c.dsVe.soVeDaDat) {
                strncpy_s(c.soHieuMB, line, sizeof(c.soHieuMB) - 1);
                c.soChoMax = newCap;
                // Re-alloc mảng vé nếu cần
                Ve* newArr = new Ve[newCap];
                int copyN = c.dsVe.soVeDaDat; // chỉ copy số vé đã đặt (chưa lưu chi tiết tại file)
                for (int i = 0; i < copyN; ++i) newArr[i] = c.dsVe.danhSach[i];
                delete[] c.dsVe.danhSach;
                c.dsVe.danhSach = newArr;
                if (c.dsVe.soVeDaDat == c.soChoMax) c.ttcb = HET_VE;
                else if (c.ttcb == HET_VE) c.ttcb = CON_VE;
            }
            else {
                gotoXY(5, 14); cout << "May bay moi khong du suc chua (nho hon so ve da dat)!";
            }
        }
    }
    // Trang thai
    gotoXY(63, 12);
    if (cin.getline(line, sizeof(line)) && !IsEmpty(line)) {
        int tt; if (parseInt(line, tt) && tt >= 0 && tt <= 3) c.ttcb = (TrangThaiChuyenBay)tt;
    }

    gotoXY(5, 16); cout << "Da cap nhat. Nhan phim bat ky..."; _getch(); ShowCur(0);
}

void FormXoaChuyenBay(PTRCB& dscb)
{
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2); cout << "=== XOA CHUYEN BAY ===";
    gotoXY(5, 4); cout << "Nhap MA CB can xoa (0 de huy): ";
    char maCB[MAX_MA_CB_LENGTH]; gotoXY(36, 4); cin.getline(maCB, sizeof(maCB));
    if (strcmp(maCB, "0") == 0 || IsEmpty(maCB)) { ShowCur(0); return; }
    RemoveAllSpaces(maCB); UpperCase(maCB);

    nodeChuyenBay* node = FindCBByMa(dscb, maCB);
    if (!node) { gotoXY(5, 6); cout << "Khong tim thay!"; _getch(); ShowCur(0); return; }

    // Business rule đơn giản: Không cho xóa nếu đã có vé đặt
    if (node->cb.dsVe.soVeDaDat > 0 && node->cb.ttcb != HUY_CHUYEN) {
        gotoXY(5, 8); cout << "Da co ve dat. Chi xoa duoc khi HUY_CHUYEN hoac khong co ve da dat.";
        _getch(); ShowCur(0); return;
    }

    gotoXY(5, 7); cout << "Xac nhan XOA? (Y/N)";
    int ch = _getch();
    if (ch == 'y' || ch == 'Y') {
        if (DeleteCB_ByMa(dscb, maCB))
        {
            gotoXY(5, 9); cout << "Da xoa.";
        }
        else { gotoXY(5, 9); cout << "Xoa that bai!"; }
    }
    _getch(); ShowCur(0);
}

void FormDatVe(PTRCB& dscb)
{
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2); cout << "=== DAT VE ===";
    gotoXY(5, 4); cout << "Nhap MA CHUYEN BAY: ";

    // --- Chon chuyen bay ---
    char maCB[MAX_MA_CB_LENGTH];
    PTRCB node;
    // MA CB (theo format anh đang dùng: 2 chữ + ' ' + số)
    while (true)
    {
        ClearAt(24, 4, 50);
        gotoXY(24, 4);
        if (!cin.getline(maCB, sizeof(maCB))) {
            NormalizeSpaces(maCB); UpperCase(maCB);
            // người dùng gõ quá dài -> failbit
            cin.clear();
            cin.ignore(1000, '\n');
            gotoXY(5, 13); cout << "Nhap qua dai (<= 15). Thu lai.         ";
            ClearAt(24, 4, 50);
            continue;
        }
        NormalizeSpaces(maCB); UpperCase(maCB);

        if (KiemTraDoDai(maCB, 4, 7) == 0 ||
            !(isalpha((unsigned char)maCB[0]) && isalpha((unsigned char)maCB[1]) && maCB[2] == SPACE_IN_ASCII) ||
            KiemTraPhanSauLaChuSo(3, (int)strlen(maCB), maCB) == false)
        {
            gotoXY(5, 13); cout << "  >> Sai dinh dang! (VD: <KT1><KT2><space><1-4 chu so>)           ";
            continue;
        }
        if (!(node = FindCBByMa(dscb,maCB)) ) {
            gotoXY(5, 13); cout << "Khong tim thay chuyen bay";
            continue;
        }

        ClearAt(5, 13, 60);
        break;
    }

    
    ChuyenBay& cb = node->cb;

    // --- Kiem tra trang thai chuyen bay ---
    if (cb.ttcb == HUY_CHUYEN || cb.ttcb == HOAN_TAT || cb.ttcb == HET_VE) {
        gotoXY(5, 6); cout << "Chuyen bay khong the dat ve (HUY/HOAN TAT/HET VE)!";
        _getch(); ShowCur(0); return;
    }

    // --- Dam bao bo nho danh sach ve ---
    if (cb.soChoMax <= 0) {
        gotoXY(5, 6); cout << "Chuyen bay khong co so cho hop le!";
        _getch(); ShowCur(0); return;
    }
    if (cb.dsVe.danhSach == nullptr) {
        cb.dsVe.danhSach = new Ve[cb.soChoMax];
        cb.dsVe.soVeDaDat = 0;
    }

    // --- Full chua? ---
    if (cb.dsVe.soVeDaDat >= cb.soChoMax) {
        cb.ttcb = HET_VE;
        gotoXY(5, 6); cout << "Chuyen bay da het ve!";
        _getch(); ShowCur(0); return;
    }

    // --- Nhap CMND ---
    char cmnd[MAX_CMND_LENGTH] = { 0 };
    while (true) {
        gotoXY(5, 6);  cout << "So CMND (9..12 chu so): ";
        ClearAt(28, 6, 20);
        gotoXY(28, 6); cin.getline(cmnd, sizeof(cmnd));
        RemoveAllSpaces(cmnd);

        // check 9..12 digits
        int n = (int)strlen(cmnd); bool ok = (n >= 9 && n <= 12);
        for (int i = 0; i < n && ok; ++i)
            if (!isdigit((unsigned char)cmnd[i])) ok = false;

        if (!ok) { ClearAt(5, 8, 60); gotoXY(5, 8); cout << "  >> CMND khong hop le!"; continue; }

        // duplicate check
        bool dup = false;
        for (int i = 0; i < cb.dsVe.soVeDaDat; ++i)
            if (strcmp(cb.dsVe.danhSach[i].soCMND, cmnd) == 0) { dup = true; break; }

        if (dup) { ClearAt(5, 8, 60); gotoXY(5, 8); cout << "  >> CMND da ton tai tren chuyen nay!"; continue; }

        ClearAt(5, 8, 60);
        break;
    }

    // --- Ghi ve ---
    int idx = cb.dsVe.soVeDaDat;
    strncpy_s(cb.dsVe.danhSach[idx].soCMND, cmnd, sizeof(cb.dsVe.danhSach[idx].soCMND));
    cb.dsVe.soVeDaDat = idx + 1;

    // --- Cap nhat trang thai chuyen ---
    if (cb.dsVe.soVeDaDat >= cb.soChoMax) cb.ttcb = HET_VE;
    else if (cb.ttcb == HET_VE) cb.ttcb = CON_VE; // neu chua full thi de CON_VE

    gotoXY(5, 10); cout << "=> Dat ve thanh cong. Nhan phim bat ky...";
    _getch(); ShowCur(0);
}

void FormXemDanhSachVe(PTRCB dscb)
{
    ResetColor(); system("cls"); ShowCur(1);
    gotoXY(5, 2); cout << "=== XEM DANH SACH VE ===";
    gotoXY(5, 4); cout << "Nhap MA CHUYEN BAY: ";

    char maCB[MAX_MA_CB_LENGTH]     ;
    gotoXY(26, 4); 
    while (true) {

        if (!cin.getline(maCB, sizeof(maCB))) {
            gotoXY(5, 13); cout << "Sai dinh dang";
            continue;
        }

    }
    NormalizeSpaces(maCB); UpperCase(maCB);

    nodeChuyenBay* node = FindCBByMa(dscb, maCB);
    if (!node) {
        gotoXY(5, 6); cout << "Khong tim thay chuyen bay!";
        _getch(); ShowCur(0); return;
    }
    const ChuyenBay& cb = node->cb;

    int x0 = 5, y0 = 6;
    gotoXY(x0, y0);   cout << left
        << setw(6) << "STT"
        << setw(16) << "CMND";
    gotoXY(x0, y0 + 1); cout << string(6 + 16, '-');

    for (int i = 0; i < cb.dsVe.soVeDaDat; ++i) {
        gotoXY(x0, y0 + 2 + i);
        cout << left
            << setw(6) << (i + 1)
            << setw(16) << cb.dsVe.danhSach[i].soCMND;
    }

    gotoXY(x0, y0 + 3 + cb.dsVe.soVeDaDat);
    cout << "Tong: " << cb.dsVe.soVeDaDat << "/" << cb.soChoMax
        << ".  Nhan phim bat ky de quay lai...";
    _getch(); ShowCur(0);
}
