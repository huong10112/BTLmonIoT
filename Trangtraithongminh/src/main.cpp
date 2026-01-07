
// ============ CẤU HÌNH BLYNK ============
#define BLYNK_TEMPLATE_ID "TMPL6ZFNgmN2Y"
#define BLYNK_TEMPLATE_NAME "Trangtraithongminh"
#define BLYNK_AUTH_TOKEN "M9PaMY1X6BD-T6K0gvg3QW_ZukOR7d4A"
#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Hà Trang T5";
char pass[] = "0869218222";


#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <I2CKeyPad.h>
#include <BlynkSimpleEsp32.h>
#include <DFRobot_SHT20.h>



// ============================================
// KHAI BÁO NGUYÊN MẪU HÀM
// ============================================
void xuLyMatKhau();
void docCamBien();
void dieuKhienTuDong();
void hienThiLCD();
void guiDuLieuBlynk();
void scanI2C();

void xuLyNhietDo();
void xuLyDoAm();
void xuLyKhoi();
void xuLyChuyenDong();
void xuLyAnhSang();

// ============ KHỞI TẠO THIẾT BỊ ============
LiquidCrystal_I2C lcd(0x27, 16, 2);
DFRobot_SHT20 sht20_ga;
DFRobot_SHT20 sht20_bo;

// ============================================
// CẤU HÌNH KEYPAD 3x4 QUA PCF8574 I2C
// ============================================
#define I2C_KEYPAD_ADDR 0x20  // Địa chỉ PCF8574 (A0=A1=A2=GND)

const byte ROWS = 4;
const byte COLS = 3;
char keys[] = {
  '1','2','3',
  '4','5','6',
  '7','8','9',
  '*','0','#'
};

// PCF8574 pin mapping: P0-P3 = Rows, P4-P6 = Cols
byte rowPins[ROWS] = {0, 1, 2, 3};  // P0-P3
byte colPins[COLS] = {4, 5, 6};     // P4-P6

I2CKeyPad keypad(I2C_KEYPAD_ADDR);


// ============================================
// ĐỊNH NGHĨA CHÂN ESP32
// ============================================
// Relay 4 kênh (Active LOW)
#define RELAY_QUAT_GA       23  // Quạt gà
#define RELAY_QUAT_BO       19  // Quạt bò
#define RELAY_DEN_TIM_GA    26  // LED Tím gà (đèn sưởi)
#define RELAY_DEN_TIM_BO    27  // LED Tím bò (đèn sưởi)

// LED cảnh báo (qua S8050 + 1kΩ)
#define LED_DO_GA           18  // Độ ẩm cao gà
#define LED_DO_BO           5   // Độ ẩm cao bò
#define LED_VANG            17  // Nhiệt độ cao (CHUNG)
#define LED_XANH_DUONG_GA   16  // Khói gà
#define LED_XANH_DUONG_BO   33  // Khói bò
#define LED_TRANG_NGOAI     4   // Chiếu sáng ngoài

// Cảm biến ADC
#define SENSOR_KHOI_GA      34  // MP-2 gà (có chia áp 1kΩ + 1kΩ)
#define SENSOR_KHOI_BO      35  // MP-2 bò (có chia áp 1kΩ + 1kΩ)
#define SENSOR_ANH_SANG     32  // Cảm biến ánh sáng

// Cảm biến Digital
#define SENSOR_CHUYEN_DONG_GA  36  // HC-SR501 #1 (có pull-down 10kΩ)
#define SENSOR_CHUYEN_DONG_BO  39  // HC-SR501 #2 (có pull-down 10kΩ)

// Buzzer
#define BUZZER              2

// ============================================
// NGƯỠNG CẢM BIẾN
// ============================================
#define NHIET_DO_THAP       20.0  // °C - Bật đèn sưởi
#define NHIET_DO_CAO        35.0  // °C - Bật quạt
#define DO_AM_CAO           80.0  // % - Cảnh báo
#define NGUONG_KHOI         300   // 0-4095 - Cảnh báo khói
#define NGUONG_TOI          2000  // 0-4095 - Bật đèn ngoài

// ============================================
// MẬT KHẨU
// ============================================
String matKhauDung = "1234";
String matKhauNhap = "";
bool daXacThuc = false;

// ============================================
// BIẾN TRẠNG THÁI
// ============================================
unsigned long thoiDiemHienThi = 0;
bool hienThiChuongGa = true;
bool cheDoTuDong = true;

// Dữ liệu cảm biến
struct DuLieuChuong {
  float nhietDo;
  float doAm;
  int khoi;
  bool coChuyenDong;
};

DuLieuChuong chuongGa, chuongBo;
int giaTriAnhSang = 0;

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  TRANG TRẠI THÔNG MINH - ESP32   ║");
  Serial.println("║  2 Chuồng: Gà & Bò                ║");
  Serial.println("║  ✅ Keypad qua PCF8574 I2C        ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  // Khởi tạo I2C
  Wire.begin(21, 22); // SDA=21, SCL=22
  delay(100);
  
  // Quét I2C devices
  Serial.println("🔍 Quét I2C devices...");
  scanI2C();
  
  // Khởi tạo Keypad I2C
  keypad.begin();
  //keypad.setDebounceTime(50);
  Serial.println("✓ Keypad I2C khởi tạo thành công!");
  
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Trang Trai IoT");
  lcd.setCursor(0, 1);
  lcd.print("Khoi dong...");
  
  // Khởi tạo SHT20
  sht20_ga.initSHT20();
  delay(100);
  sht20_bo.initSHT20();
  delay(100);
  
  // Cấu hình Relay (Active LOW - tắt ban đầu)
  pinMode(RELAY_QUAT_GA, OUTPUT);
  pinMode(RELAY_QUAT_BO, OUTPUT);
  pinMode(RELAY_DEN_TIM_GA, OUTPUT);
  pinMode(RELAY_DEN_TIM_BO, OUTPUT);
  digitalWrite(RELAY_QUAT_GA, HIGH);
  digitalWrite(RELAY_QUAT_BO, HIGH);
  digitalWrite(RELAY_DEN_TIM_GA, HIGH);
  digitalWrite(RELAY_DEN_TIM_BO, HIGH);
  
  // Cấu hình LED (qua S8050)
  pinMode(LED_DO_GA, OUTPUT);
  pinMode(LED_DO_BO, OUTPUT);
  pinMode(LED_VANG, OUTPUT);
  pinMode(LED_XANH_DUONG_GA, OUTPUT);
  pinMode(LED_XANH_DUONG_BO, OUTPUT);
  pinMode(LED_TRANG_NGOAI, OUTPUT);
  
  // Cấu hình Buzzer
  pinMode(BUZZER, OUTPUT);
  
  // Cấu hình cảm biến chuyển động
  pinMode(SENSOR_CHUYEN_DONG_GA, INPUT);
  pinMode(SENSOR_CHUYEN_DONG_BO, INPUT);
  
  // Cấu hình ADC
  analogReadResolution(12); // 12-bit (0-4095)
  analogSetAttenuation(ADC_11db); // 0-3.3V
  
  // Kết nối WiFi và Blynk
  Serial.println("\n⏳ Đang kết nối WiFi...");
  lcd.clear();
  lcd.print("Ket noi WiFi...");
  Blynk.begin(auth, ssid, pass);
  
  Serial.println("✓ Kết nối WiFi thành công!");
  Serial.println("✓ Kết nối Blynk thành công!");
  
  delay(1000);
  lcd.clear();
  lcd.print("Nhap mat khau:");
  
  Serial.println("\n▶ Chờ nhập mật khẩu qua Keypad I2C...");
}

// ============================================
// QUÉT I2C DEVICES
// ============================================
void scanI2C() {
  byte count = 0;
  Serial.println("Scanning I2C bus...");
  
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  ✓ Tìm thấy thiết bị: 0x%02X", i);
      
      // Nhận diện thiết bị
      if (i == 0x20) Serial.print(" (PCF8574 - Keypad)");
      if (i == 0x27 || i == 0x3F) Serial.print(" (LCD)");
      if (i == 0x40) Serial.print(" (SHT20 #1)");
      if (i == 0x41) Serial.print(" (SHT20 #2)");
      
      Serial.println();
      count++;
    }
  }
  
  if (count == 0) {
    Serial.println("  ✗ Không tìm thấy thiết bị I2C nào!");
  } else {
    Serial.printf("  → Tổng: %d thiết bị\n", count);
  }
  Serial.println();
}

// ============================================
// LOOP
// ============================================
void loop() {
  Blynk.run();
  
  // Xử lý nhập mật khẩu
  if (!daXacThuc) {
    xuLyMatKhau();
    return;
  }
  
  // Đọc tất cả cảm biến
  docCamBien();
  
  // Điều khiển tự động
  if (cheDoTuDong) {
    dieuKhienTuDong();
  }
  
  // Hiển thị LCD (thay phiên 10s)
  hienThiLCD();
  
  // Gửi dữ liệu lên Blynk (mỗi 2s)
  static unsigned long lanCuoi = 0;
  if (millis() - lanCuoi > 2000) {
    guiDuLieuBlynk();
    lanCuoi = millis();
  }
  
  delay(100);
}

// ============================================
// XỬ LÝ MẬT KHẨU
// ============================================
void xuLyMatKhau() {
  char phim = keypad.getKey();
  
  if (phim) {
    // ✅ Bây giờ có thể dùng Serial.print() thoải mái!
    Serial.print("Phím nhấn: ");
    Serial.println(phim);
    
    if (phim == '#') {
      // Xác nhận mật khẩu
      if (matKhauNhap == matKhauDung) {
        daXacThuc = true;
        lcd.clear();
        lcd.print("Mo khoa!");
        lcd.setCursor(0, 1);
        lcd.print("Chao mung!");
        
        // Còi báo thành công
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        
        Serial.println("✓ XÁC THỰC THÀNH CÔNG!");
        delay(1500);
        lcd.clear();
      } else {
        // Sai mật khẩu
        lcd.clear();
        lcd.print("SAI MAT KHAU!");
        
        // Còi báo lỗi
        for (int i = 0; i < 3; i++) {
          digitalWrite(BUZZER, HIGH);
          delay(100);
          digitalWrite(BUZZER, LOW);
          delay(100);
        }
        
        Serial.println("✗ Sai mật khẩu!");
        delay(1500);
        lcd.clear();
        lcd.print("Nhap mat khau:");
      }
      matKhauNhap = "";
      
    } else if (phim == '*') {
      // Xóa mật khẩu
      matKhauNhap = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      Serial.println("✗ Xóa mật khẩu");
      
    } else {
      // Thêm ký tự
      matKhauNhap += phim;
      lcd.setCursor(0, 1);
      for (int i = 0; i < matKhauNhap.length(); i++) {
        lcd.print("*");
      }
    }
  }
}

// ============================================
// ĐỌC CẢM BIẾN
// ============================================
void docCamBien() {
  // Đọc nhiệt độ & độ ẩm
  chuongGa.nhietDo = sht20_ga.readTemperature();
  chuongGa.doAm = sht20_ga.readHumidity();
  
  chuongBo.nhietDo = sht20_bo.readTemperature();
  chuongBo.doAm = sht20_bo.readHumidity();
  
  // Đọc cảm biến khói MP-2 (có chia áp 1kΩ + 1kΩ)
  chuongGa.khoi = analogRead(SENSOR_KHOI_GA) * 2; // Nhân 2 để về thang 0-4095
  chuongBo.khoi = analogRead(SENSOR_KHOI_BO) * 2;
  
  // Đọc ánh sáng
  giaTriAnhSang = analogRead(SENSOR_ANH_SANG);
  
  // Đọc chuyển động
  chuongGa.coChuyenDong = digitalRead(SENSOR_CHUYEN_DONG_GA);
  chuongBo.coChuyenDong = digitalRead(SENSOR_CHUYEN_DONG_BO);
  
  // Debug Serial (mỗi 5s)
  static unsigned long lanCuoiDebug = 0;
  if (millis() - lanCuoiDebug > 5000) {
    Serial.println("\n╔═════════════════ DỮ LIỆU CẢM BIẾN ═════════════════╗");
    Serial.printf("║ Gà:  %.1f°C  %.0f%%  Khói:%d  PIR:%s\n", 
                  chuongGa.nhietDo, chuongGa.doAm, chuongGa.khoi,
                  chuongGa.coChuyenDong ? "CÓ" : "---");
    Serial.printf("║ Bò:  %.1f°C  %.0f%%  Khói:%d  PIR:%s\n", 
                  chuongBo.nhietDo, chuongBo.doAm, chuongBo.khoi,
                  chuongBo.coChuyenDong ? "CÓ" : "---");
    Serial.printf("║ Ánh sáng: %d (Ngưỡng: %d)\n", giaTriAnhSang, NGUONG_TOI);
    Serial.println("╚════════════════════════════════════════════════════╝\n");
    lanCuoiDebug = millis();
  }
}

// ============================================
// ĐIỀU KHIỂN TỰ ĐỘNG
// ============================================
void dieuKhienTuDong() {
  xuLyNhietDo();
  xuLyDoAm();
  xuLyKhoi();
  xuLyChuyenDong();
  xuLyAnhSang();
}

// Xử lý nhiệt độ
void xuLyNhietDo() {
  // NHIỆT ĐỘ THẤP - Bật đèn sưởi (LED Tím qua Relay)
  digitalWrite(RELAY_DEN_TIM_GA, (chuongGa.nhietDo < NHIET_DO_THAP) ? LOW : HIGH);
  digitalWrite(RELAY_DEN_TIM_BO, (chuongBo.nhietDo < NHIET_DO_THAP) ? LOW : HIGH);
  
  // NHIỆT ĐỘ CAO - Bật LED Vàng (CHUNG) và Quạt (riêng)
  bool coNong = (chuongGa.nhietDo > NHIET_DO_CAO) || (chuongBo.nhietDo > NHIET_DO_CAO);
  digitalWrite(LED_VANG, coNong ? HIGH : LOW);
  
  // Quạt riêng từng chuồng
  digitalWrite(RELAY_QUAT_GA, (chuongGa.nhietDo > NHIET_DO_CAO) ? LOW : HIGH);
  digitalWrite(RELAY_QUAT_BO, (chuongBo.nhietDo > NHIET_DO_CAO) ? LOW : HIGH);
}

// Xử lý độ ẩm
void xuLyDoAm() {
  digitalWrite(LED_DO_GA, (chuongGa.doAm > DO_AM_CAO) ? HIGH : LOW);
  digitalWrite(LED_DO_BO, (chuongBo.doAm > DO_AM_CAO) ? HIGH : LOW);
}

// Xử lý khói
void xuLyKhoi() {
  bool khoiGa = (chuongGa.khoi > NGUONG_KHOI);
  bool khoiBo = (chuongBo.khoi > NGUONG_KHOI);
  
  digitalWrite(LED_XANH_DUONG_GA, khoiGa ? HIGH : LOW);
  digitalWrite(LED_XANH_DUONG_BO, khoiBo ? HIGH : LOW);
  
  // Buzzer ưu tiên cao nhất cho khói
  if (khoiGa || khoiBo) {
    tone(BUZZER, 2000); // 2kHz - nguy hiểm
  } else {
    noTone(BUZZER);
  }
}

// Xử lý chuyển động
void xuLyChuyenDong() {
  // Còi ngắn nếu phát hiện chuyển động (không có khói)
  bool coChuyenDong = chuongGa.coChuyenDong || chuongBo.coChuyenDong;
  bool coKhoi = (chuongGa.khoi > NGUONG_KHOI) || (chuongBo.khoi > NGUONG_KHOI);
  
  if (coChuyenDong && !coKhoi) {
    static unsigned long lanCuoiCoi = 0;
    if (millis() - lanCuoiCoi > 1000) {
      tone(BUZZER, 1000, 200); // 1kHz, 200ms
      lanCuoiCoi = millis();
    }
  }
}

// Xử lý ánh sáng
void xuLyAnhSang() {
  // Trời tối → bật LED trắng ngoài
  digitalWrite(LED_TRANG_NGOAI, (giaTriAnhSang > NGUONG_TOI) ? HIGH : LOW);
}

// ============================================
// HIỂN THỊ LCD
// ============================================
void hienThiLCD() {
  // Thay phiên mỗi 10 giây
  if (millis() - thoiDiemHienThi > 10000) {
    hienThiChuongGa = !hienThiChuongGa;
    thoiDiemHienThi = millis();
    lcd.clear();
  }
  
  if (hienThiChuongGa) {
    lcd.setCursor(0, 0);
    lcd.print("GA:");
    lcd.print(chuongGa.nhietDo, 1);
    lcd.print("C ");
    lcd.print((int)chuongGa.doAm);
    lcd.print("%  ");
    
    lcd.setCursor(0, 1);
    lcd.print("Khoi:");
    lcd.print(chuongGa.khoi);
    lcd.print("   ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("BO:");
    lcd.print(chuongBo.nhietDo, 1);
    lcd.print("C ");
    lcd.print((int)chuongBo.doAm);
    lcd.print("%  ");
    
    lcd.setCursor(0, 1);
    lcd.print("Khoi:");
    lcd.print(chuongBo.khoi);
    lcd.print("   ");
  }
}

// ============================================
// GỬI DỮ LIỆU BLYNK
// ============================================
void guiDuLieuBlynk() {
  Blynk.virtualWrite(V0, chuongGa.nhietDo);
  Blynk.virtualWrite(V1, chuongGa.doAm);
  Blynk.virtualWrite(V2, chuongGa.khoi);
  
  Blynk.virtualWrite(V3, chuongBo.nhietDo);
  Blynk.virtualWrite(V4, chuongBo.doAm);
  Blynk.virtualWrite(V5, chuongBo.khoi);
  
  Blynk.virtualWrite(V6, giaTriAnhSang);
  Blynk.virtualWrite(V7, chuongGa.coChuyenDong || chuongBo.coChuyenDong);
}

// ============================================
// ĐIỀU KHIỂN TỪ BLYNK
// ============================================
BLYNK_WRITE(V20) { 
  cheDoTuDong = param.asInt(); 
  Serial.printf("Chế độ: %s\n", cheDoTuDong ? "TỰ ĐỘNG" : "THỦ CÔNG");
}

BLYNK_WRITE(V10) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_QUAT_GA, param.asInt() ? LOW : HIGH);
    Serial.printf("Quạt Gà: %s\n", param.asInt() ? "BẬT" : "TẮT");
  }
}

BLYNK_WRITE(V11) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_QUAT_BO, param.asInt() ? LOW : HIGH);
    Serial.printf("Quạt Bò: %s\n", param.asInt() ? "BẬT" : "TẮT");
  }
}

BLYNK_WRITE(V12) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_DEN_TIM_GA, param.asInt() ? LOW : HIGH);
    Serial.printf("Đèn sưởi Gà: %s\n", param.asInt() ? "BẬT" : "TẮT");
  }
}

BLYNK_WRITE(V13) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_DEN_TIM_BO, param.asInt() ? LOW : HIGH);
    Serial.printf("Đèn sưởi Bò: %s\n", param.asInt() ? "BẬT" : "TẮT");
  }
}

BLYNK_WRITE(V14) {
  if (!cheDoTuDong) {
    digitalWrite(LED_TRANG_NGOAI, param.asInt() ? HIGH : LOW);
    Serial.printf("LED Trắng: %s\n", param.asInt() ? "BẬT" : "TẮT");
  }
}

BLYNK_WRITE(V15) {
  if (param.asInt()) {
    tone(BUZZER, 1000);
    Serial.println("Buzzer: BẬT");
  } else {
    noTone(BUZZER);
    Serial.println("Buzzer: TẮT");
  }
}