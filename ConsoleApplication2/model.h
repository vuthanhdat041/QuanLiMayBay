#pragma once
#ifndef MODEL_HPP
#define MODEL_HPP

       
#define MAX_MB 300          
#define MAX_SO_HIEU_MB 16  
#define MAX_LOAI_MB 41      
#define MAX_CMND_LENGTH 13  
#define MAX_MA_CB_LENGTH 13 
#define MAX_SANBAY_DEN 20   
#define MAX_HO_LENGTH 20    
#define MAX_TEN_LENGTH 30  
#define MAX_GIOI_TINH 4     

const int SPACE_IN_ASCII = 32;
// ===== May bay =====
struct MayBay
{
    char soHieuMB[MAX_SO_HIEU_MB] = "";
    char loaiMB[MAX_LOAI_MB] = "";
    int soCho = 0;
};

struct DSMB
{
    int n = 0;
    MayBay* nodes[MAX_MB];
};

// ===== Chuyen bay =====
struct ThoiGian
{
    int phut;
    int gio;
    int ngay;
    int thang;
    int nam;
};

enum TrangThaiChuyenBay
{
    HUY_CHUYEN = 0,
    CON_VE = 1,
    HET_VE = 2,
    HOAN_TAT = 3
};

struct Ve
{
    char soCMND[MAX_CMND_LENGTH] = "";
};

struct DSVe
{
    int soVeDaDat;
    Ve* danhSach = nullptr;
};

struct ChuyenBay
{
    char maCB[MAX_MA_CB_LENGTH];
    ThoiGian ngayGioKhoiHanh;
    char sanBayDen[MAX_SANBAY_DEN];
    char soHieuMB[MAX_SO_HIEU_MB];
    int soChoMax = 0;
    TrangThaiChuyenBay ttcb;
    DSVe dsVe;
};

struct nodeChuyenBay
{
    ChuyenBay cb;
    nodeChuyenBay* next;
};

typedef nodeChuyenBay* PTRCB;

// ===== Hanh khach =====
struct HanhKhach
{
    char soCM[MAX_CMND_LENGTH];
    char ho[MAX_HO_LENGTH];
    char ten[MAX_TEN_LENGTH];
    char phai[MAX_GIOI_TINH];
};

struct nodeHK
{
    HanhKhach hk;
    nodeHK* left, * right;
};

typedef nodeHK* treeHK;

#endif // MODEL_HPP
