#include <unity.h>
#include "Logic.h"

// BẮT BUỘC PHẢI CÓ
void setUp(void) {
    // chạy trước mỗi test
}

void tearDown(void) {
    // chạy sau mỗi test
}

// ===== TEST NHIỆT ĐỘ =====
void test_bat_den_suoi_khi_nhiet_do_thap() {
    TEST_ASSERT_TRUE(canBatDenSuoi(15.0));
}

void test_khong_bat_den_suoi_khi_nhiet_do_binh_thuong() {
    TEST_ASSERT_FALSE(canBatDenSuoi(25.0));
}

void test_bat_quat_khi_nhiet_do_cao() {
    TEST_ASSERT_TRUE(canBatQuat(35.0));
}

// ➕ TEST MỚI
void test_khong_bat_quat_khi_nhiet_do_binh_thuong() {
    TEST_ASSERT_FALSE(canBatQuat(25.0));
}

// ===== TEST KHÓI MQ-2 =====
void test_bat_coi_khi_khoi_cao() {
    TEST_ASSERT_TRUE(canBatCoiKhoi(400));
}

// ➕ TEST MỚI
void test_khong_bat_coi_khi_khoi_thap() {
    TEST_ASSERT_FALSE(canBatCoiKhoi(150));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_bat_den_suoi_khi_nhiet_do_thap);
    RUN_TEST(test_khong_bat_den_suoi_khi_nhiet_do_binh_thuong);
    RUN_TEST(test_bat_quat_khi_nhiet_do_cao);
    RUN_TEST(test_khong_bat_quat_khi_nhiet_do_binh_thuong);
    RUN_TEST(test_bat_coi_khi_khoi_cao);
    RUN_TEST(test_khong_bat_coi_khi_khoi_thap);

    return UNITY_END();
}
