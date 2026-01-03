


// ============ CẤU HÌNH BLYNK ============
#define BLYNK_TEMPLATE_ID "TMPL6ZFNgmN2Y"
#define BLYNK_TEMPLATE_NAME "Trangtraithongminh"
#define BLYNK_AUTH_TOKEN "M9PaMY1X6BD-T6K0gvg3QW_ZukOR7d4A"
#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Nheeeee___";
char pass[] = "matkhauuu";

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <BlynkSimpleEsp32.h>
#include <DFRobot_SHT20.h>

// ============ KHỞI TẠO THIẾT BỊ ============
LiquidCrystal_I2C lcd(0x27, 16, 2);
DFRobot_SHT20 sht20_ga;
DFRobot_SHT20 sht20_bo;

// ============ CẤU HÌNH KEYPAD 4x4 ============
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 14, 25, 27};   // OUTPUT
byte colPins[COLS] = {34, 35, 39};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ============ ĐỊNH NGHĨA CHÂN ESP32 ============
// Relay Module 4 kênh (Active LOW)
#define RELAY_DEN_SUOI_GA     23
#define RELAY_DEN_SUOI_BO     22
#define RELAY_QUAT_GA         21
#define RELAY_QUAT_BO         19

// LED Cảnh Báo
#define LED_DO                18  // Nhiệt độ thấp
#define LED_VANG              5   // Nhiệt độ cao
#define LED_TIM               17  // Độ ẩm cao
#define LED_XANH_DUONG        16  // Khói
#define LED_XANH_LA           4   // Phát hiện người
#define LED_TRANG_TRONG       15  // Đèn trong chuồng
#define LED_TRANG_NGOAI       2   // Đèn ngoài chuồng

// Cảm Biến Analog (ADC)
#define SENSOR_KHOI_GA        34  // MP-2 chuồng gà
#define SENSOR_KHOI_BO        35  // MP-2 chuồng bò
#define SENSOR_ANH_SANG       25  // Photoresistor

// Cảm Biến Digital
#define SENSOR_CHUYEN_DONG_GA 36  // HC-SR501 chuồng gà
#define SENSOR_CHUYEN_DONG_BO 39  // HC-SR501 chuồng bò

// Thiết Bị Điều Khiển
#define BUZZER                26
#define NUT_COI               27
#define NUT_DEN               14

// ============ NGƯỠNG CẢM BIẾN ============
#define NHIET_DO_THAP         20.0  // °C
#define NHIET_DO_CAO          35.0  // °C
#define DO_AM_CAO             80.0  // %
#define NGUONG_KHOI           300   // 0-4095
#define NGUONG_TOI            2000  // 0-4095

// ============ BIẾN TOÀN CỤC ============
String matKhauDung = "4769";
String matKhauNhap = ""; 
bool daXacThuc = false;
bool cheDoTuDong = true;

// Biến điều khiển thủ công
bool coiThucong = false;
bool denThuCong = false;

// Biến hiển thị LCD
unsigned long thoiDiemCapNhat = 0;
bool hienThiChuongGa = true;

// Dữ liệu cảm biến
struct DuLieuChuong {
  float nhietDo;
  float doAm;
  int khoi;
  bool coNguoi;
};

DuLieuChuong chuongGa, chuongBo;
int giaTriAnhSang = 0;

// ============ KHAI BÁO HÀM ============
void khoiTaoThietBi();
void xuLyXacThuc();
void docCamBien();
void dieuKhienTuDong();
void xuLyNutNhan();
void hienThiLCD();
void guiDuLieuBlynk();

// Hàm xử lý logic
void xuLyNhietDo();
void xuLyDoAm();
void xuLyKhoi();
void xuLyPhatHienNguoi();
void xuLyAnhSang();

// ============ SETUP ============
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== TRANG TRAI THONG MINH ===");
  
  khoiTaoThietBi();
  
  // Kết nối WiFi và Blynk
  lcd.clear();
  lcd.print("Ket noi WiFi...");
  Blynk.begin(auth, ssid, pass);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Trang Trai IoT");
  lcd.setCursor(0, 1);
  lcd.print("Nhap mat khau:");
  
  Serial.println("=== San sang hoat dong ===");
}

// ============ LOOP ============
void loop() {
  Blynk.run();
  
  // Kiểm tra xác thực
  if (!daXacThuc) {
    xuLyXacThuc();
    return;
  }
  
  // Đọc tất cả cảm biến
  docCamBien();
  
  // Xử lý nút nhấn trực tiếp
  xuLyNutNhan();
  
  // Điều khiển tự động
  if (cheDoTuDong) {
    dieuKhienTuDong();
  }
  
  // Hiển thị LCD thay phiên
  hienThiLCD();
  
  // Gửi dữ liệu lên Blynk (mỗi 2s)
  static unsigned long lanCuoi = 0;
  if (millis() - lanCuoi > 2000) {
    guiDuLieuBlynk();
    lanCuoi = millis();
  }
  
  delay(100);
}

// ============ KHỞI TẠO THIẾT BỊ ============
void khoiTaoThietBi() {
  // LCD I2C
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Khoi dong...");
  
  // I2C cho SHT20
  Wire.begin(21, 22); // SDA=21, SCL=22
  sht20_ga.initSHT20();
  delay(100);
  sht20_bo.initSHT20();
  delay(100);
  
  // Relay (Active LOW - tắt ban đầu)
  pinMode(RELAY_DEN_SUOI_GA, OUTPUT);
  pinMode(RELAY_DEN_SUOI_BO, OUTPUT);
  pinMode(RELAY_QUAT_GA, OUTPUT);
  pinMode(RELAY_QUAT_BO, OUTPUT);
  digitalWrite(RELAY_DEN_SUOI_GA, HIGH);
  digitalWrite(RELAY_DEN_SUOI_BO, HIGH);
  digitalWrite(RELAY_QUAT_GA, HIGH);
  digitalWrite(RELAY_QUAT_BO, HIGH);
  
  // LED
  pinMode(LED_DO, OUTPUT);
  pinMode(LED_VANG, OUTPUT);
  pinMode(LED_TIM, OUTPUT);
  pinMode(LED_XANH_DUONG, OUTPUT);
  pinMode(LED_XANH_LA, OUTPUT);
  pinMode(LED_TRANG_TRONG, OUTPUT);
  pinMode(LED_TRANG_NGOAI, OUTPUT);
  
  // Buzzer và nút nhấn
  pinMode(BUZZER, OUTPUT);
  pinMode(NUT_COI, INPUT_PULLUP);
  pinMode(NUT_DEN, INPUT_PULLUP);
  
  // Cảm biến chuyển động
  pinMode(SENSOR_CHUYEN_DONG_GA, INPUT);
  pinMode(SENSOR_CHUYEN_DONG_BO, INPUT);
  
  // Cấu hình ADC
  analogReadResolution(12); // 12-bit (0-4095)
  analogSetAttenuation(ADC_11db); // 0-3.3V
}

// ============ XÁC THỰC MẬT KHẨU ============
void xuLyXacThuc() {
  char phim = keypad.getKey();
  
  if (phim) {
    Serial.print("Phim nhan: ");
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
        
        Serial.println(">>> Xac thuc thanh cong!");
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
        
        Serial.println(">>> Sai mat khau!");
        delay(1500);
        lcd.clear();
        lcd.print("Nhap mat khau:");
      }
      matKhauNhap = "";
      
    } else if (phim == '*') {
      // Xóa mật khẩu đã nhập
      matKhauNhap = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      
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

// ============ ĐỌC CẢM BIẾN ============
void docCamBien() {
  // Chuồng gà
  chuongGa.nhietDo = sht20_ga.readTemperature();
  chuongGa.doAm = sht20_ga.readHumidity();
  chuongGa.khoi = analogRead(SENSOR_KHOI_GA);
  chuongGa.coNguoi = digitalRead(SENSOR_CHUYEN_DONG_GA);
  
  // Chuồng bò
  chuongBo.nhietDo = sht20_bo.readTemperature();
  chuongBo.doAm = sht20_bo.readHumidity();
  chuongBo.khoi = analogRead(SENSOR_KHOI_BO);
  chuongBo.coNguoi = digitalRead(SENSOR_CHUYEN_DONG_BO);
  
  // Ánh sáng
  giaTriAnhSang = analogRead(SENSOR_ANH_SANG);
}

// ============ ĐIỀU KHIỂN TỰ ĐỘNG ============
void dieuKhienTuDong() {
  xuLyNhietDo();
  xuLyDoAm();
  xuLyKhoi();
  xuLyPhatHienNguoi();
  xuLyAnhSang();
}

void xuLyNhietDo() {
  bool thapGa = (chuongGa.nhietDo < NHIET_DO_THAP);
  bool thapBo = (chuongBo.nhietDo < NHIET_DO_THAP);
  bool caoGa = (chuongGa.nhietDo > NHIET_DO_CAO);
  bool caoBo = (chuongBo.nhietDo > NHIET_DO_CAO);
  
  // LED đỏ - Nhiệt độ thấp
  digitalWrite(LED_DO, (thapGa || thapBo) ? HIGH : LOW);
  
  // Đèn sưởi
  digitalWrite(RELAY_DEN_SUOI_GA, thapGa ? LOW : HIGH);
  digitalWrite(RELAY_DEN_SUOI_BO, thapBo ? LOW : HIGH);
  
  // LED vàng - Nhiệt độ cao
  digitalWrite(LED_VANG, (caoGa || caoBo) ? HIGH : LOW);
  
  // Quạt làm mát
  digitalWrite(RELAY_QUAT_GA, caoGa ? LOW : HIGH);
  digitalWrite(RELAY_QUAT_BO, caoBo ? LOW : HIGH);
}

void xuLyDoAm() {
  bool doAmCao = (chuongGa.doAm > DO_AM_CAO) || (chuongBo.doAm > DO_AM_CAO);
  digitalWrite(LED_TIM, doAmCao ? HIGH : LOW);
}

void xuLyKhoi() {
  bool coKhoi = (chuongGa.khoi > NGUONG_KHOI) || (chuongBo.khoi > NGUONG_KHOI);
  
  digitalWrite(LED_XANH_DUONG, coKhoi ? HIGH : LOW);
  
  // Còi cảnh báo khói (nếu không điều khiển thủ công)
  if (coKhoi && !coiThucong) {
    tone(BUZZER, 2000); // 2kHz
  } else if (!coiThucong) {
    noTone(BUZZER);
  }
}

void xuLyPhatHienNguoi() {
  bool coNguoi = chuongGa.coNguoi || chuongBo.coNguoi;
  
  digitalWrite(LED_XANH_LA, coNguoi ? HIGH : LOW);
  
  // Còi cảnh báo xâm nhập (ngắn)
  if (coNguoi && !coiThucong) {
    static unsigned long thoiDiemCoi = 0;
    if (millis() - thoiDiemCoi > 1000) {
      tone(BUZZER, 1000, 200); // 1kHz, 200ms
      thoiDiemCoi = millis();
    }
  }
}

void xuLyAnhSang() {
  // Trời tối -> bật đèn trắng
  if (giaTriAnhSang > NGUONG_TOI && !denThuCong) {
    digitalWrite(LED_TRANG_TRONG, HIGH);
    digitalWrite(LED_TRANG_NGOAI, HIGH);
  } else if (!denThuCong) {
    digitalWrite(LED_TRANG_TRONG, LOW);
    digitalWrite(LED_TRANG_NGOAI, LOW);
  }
}

// ============ XỬ LÝ NÚT NHẤN ============
void xuLyNutNhan() {
  static bool trangThaiCuoiCoi = HIGH;
  static bool trangThaiCuoiDen = HIGH;
  
  bool nutCoi = digitalRead(NUT_COI);
  bool nutDen = digitalRead(NUT_DEN);
  
  // Nút còi (nhấn = LOW)
  if (nutCoi == LOW && trangThaiCuoiCoi == HIGH) {
    coiThucong = !coiThucong;
    digitalWrite(BUZZER, coiThucong ? HIGH : LOW);
    Serial.println(coiThucong ? "Coi BAT" : "Coi TAT");
    delay(200);
  }
  trangThaiCuoiCoi = nutCoi;
  
  // Nút đèn (nhấn = LOW)
  if (nutDen == LOW && trangThaiCuoiDen == HIGH) {
    denThuCong = !denThuCong;
    digitalWrite(LED_TRANG_TRONG, denThuCong ? HIGH : LOW);
    digitalWrite(LED_TRANG_NGOAI, denThuCong ? HIGH : LOW);
    Serial.println(denThuCong ? "Den BAT" : "Den TAT");
    delay(200);
  }
  trangThaiCuoiDen = nutDen;
}

// ============ HIỂN THỊ LCD ============
void hienThiLCD() {
  // Thay phiên mỗi 10 giây
  if (millis() - thoiDiemCapNhat > 10000) {
    hienThiChuongGa = !hienThiChuongGa;
    thoiDiemCapNhat = millis();
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
    lcd.print("Khoi: ");
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
    lcd.print("Khoi: ");
    lcd.print(chuongBo.khoi);
    lcd.print("   ");
  }
}

// ============ GỬI DỮ LIỆU BLYNK ============
void guiDuLieuBlynk() {
  Blynk.virtualWrite(V0, chuongGa.nhietDo);
  Blynk.virtualWrite(V1, chuongGa.doAm);
  Blynk.virtualWrite(V2, chuongGa.khoi);
  
  Blynk.virtualWrite(V3, chuongBo.nhietDo);
  Blynk.virtualWrite(V4, chuongBo.doAm);
  Blynk.virtualWrite(V5, chuongBo.khoi);
  
  Blynk.virtualWrite(V6, giaTriAnhSang);
}

// ============ BLYNK VIRTUAL PINS ============
// Điều khiển đèn sưởi
BLYNK_WRITE(V10) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_DEN_SUOI_GA, param.asInt() ? LOW : HIGH);
  }
}

BLYNK_WRITE(V11) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_DEN_SUOI_BO, param.asInt() ? LOW : HIGH);
  }
}

// Điều khiển quạt
BLYNK_WRITE(V12) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_QUAT_GA, param.asInt() ? LOW : HIGH);
  }
}

BLYNK_WRITE(V13) {
  if (!cheDoTuDong) {
    digitalWrite(RELAY_QUAT_BO, param.asInt() ? LOW : HIGH);
  }
}

// Điều khiển còi
BLYNK_WRITE(V14) {
  coiThucong = param.asInt();
  digitalWrite(BUZZER, coiThucong ? HIGH : LOW);
}

// Điều khiển đèn
BLYNK_WRITE(V15) {
  denThuCong = param.asInt();
  digitalWrite(LED_TRANG_TRONG, denThuCong ? HIGH : LOW);
  digitalWrite(LED_TRANG_NGOAI, denThuCong ? HIGH : LOW);
}

// Chế độ tự động/thủ công
BLYNK_WRITE(V20) {
  cheDoTuDong = param.asInt();
  Serial.println(cheDoTuDong ? "CHE DO TU DONG" : "CHE DO THU CONG");
}