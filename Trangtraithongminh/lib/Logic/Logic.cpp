#include "Logic.h"

// ===== NHIỆT ĐỘ =====
bool canBatDenSuoi(float nhietDo) {
    return nhietDo < 20.0;
}

bool canBatQuat(float nhietDo) {
    return nhietDo > 35.0;
}

bool nhietDoCaoBatCanhBao(float ga, float bo) {
    return (ga > 35.0) || (bo > 35.0);
}

// ===== ĐỘ ẨM =====
bool doAmCao(float doAm) {
    return doAm > 80.0;
}

// ===== KHÓI =====
bool canhBaoKhoi(int giaTriKhoi) {
    return giaTriKhoi > 300;
}

// ===== ÁNH SÁNG =====
bool laBanDem(int giaTriAnhSang) {
    return giaTriAnhSang > 2000;
}

// ===== CHUYỂN ĐỘNG =====
bool phatHienXamNhap(bool ga, bool bo) {
    return ga || bo;
}