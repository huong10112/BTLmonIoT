#include "Logic.h"

#define NHIET_DO_THAP 18
#define NHIET_DO_CAO 30
#define NGUONG_KHI 300

bool canBatDenSuoi(float nhietDo) {
    return nhietDo < NHIET_DO_THAP;
}

bool canBatQuat(float nhietDo) {
    return nhietDo > NHIET_DO_CAO;
}

bool canBatCoiKhoi(float khiGas) {
    return khiGas > NGUONG_KHI;
}
