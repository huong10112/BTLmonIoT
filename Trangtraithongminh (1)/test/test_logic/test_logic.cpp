#include <unity.h>
#include "Logic.h"

void setUp(void) {}
void tearDown(void) {}

// ===== TEST NHIỆT ĐỘ =====
void test_bat_den_suoi_khi_nhiet_do_thap() {
    TEST_ASSERT_TRUE(canBatDenSuoi(15.0));
}

void test_khong_bat_den_suoi_khi_nhiet_do_binh_thuong() {
    TEST_ASSERT_FALSE(canBatDenSuoi(25.0));
}

void test_bat_quat_khi_nhiet_do_cao() {
    TEST_ASSERT_TRUE(canBatQuat(36.0));
}

void test_khong_bat_quat_khi_nhiet_do_binh_thuong() {
    TEST_ASSERT_FALSE(canBatQuat(25.0));
}

void test_bat_canh_bao_nhiet_do_cao() {
    TEST_ASSERT_TRUE(nhietDoCaoBatCanhBao(36.0, 30.0));
}

// ===== TEST ĐỘ ẨM =====
void test_canh_bao_do_am_cao() {
    TEST_ASSERT_TRUE(doAmCao(85.0));
}

void test_khong_canh_bao_do_am_thap() {
    TEST_ASSERT_FALSE(doAmCao(60.0));
}

// ===== TEST KHÓI =====
void test_bat_canh_bao_khoi() {
    TEST_ASSERT_TRUE(canhBaoKhoi(400));
}

void test_khong_bat_canh_bao_khoi() {
    TEST_ASSERT_FALSE(canhBaoKhoi(150));
}

// ===== TEST ÁNH SÁNG =====
void test_xac_dinh_ban_dem() {
    TEST_ASSERT_TRUE(laBanDem(2500));
}

void test_xac_dinh_ban_ngay() {
    TEST_ASSERT_FALSE(laBanDem(1500));
}

// ===== TEST CHUYỂN ĐỘNG =====
void test_phat_hien_xam_nhap_ga() {
    TEST_ASSERT_TRUE(phatHienXamNhap(true, false));
}

void test_phat_hien_xam_nhap_bo() {
    TEST_ASSERT_TRUE(phatHienXamNhap(false, true));
}

void test_khong_co_xam_nhap() {
    TEST_ASSERT_FALSE(phatHienXamNhap(false, false));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_bat_den_suoi_khi_nhiet_do_thap);
    RUN_TEST(test_khong_bat_den_suoi_khi_nhiet_do_binh_thuong);
    RUN_TEST(test_bat_quat_khi_nhiet_do_cao);
    RUN_TEST(test_khong_bat_quat_khi_nhiet_do_binh_thuong);
    RUN_TEST(test_bat_canh_bao_nhiet_do_cao);

    RUN_TEST(test_canh_bao_do_am_cao);
    RUN_TEST(test_khong_canh_bao_do_am_thap);

    RUN_TEST(test_bat_canh_bao_khoi);
    RUN_TEST(test_khong_bat_canh_bao_khoi);

    RUN_TEST(test_xac_dinh_ban_dem);
    RUN_TEST(test_xac_dinh_ban_ngay);

    RUN_TEST(test_phat_hien_xam_nhap_ga);
    RUN_TEST(test_phat_hien_xam_nhap_bo);
    RUN_TEST(test_khong_co_xam_nhap);

    return UNITY_END();
}
