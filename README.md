chạy test dùng lệnh: pio test -vv hoặc pio test -e native
Các test được thiết kế là unit test cho logic xử lý, không phụ thuộc phần cứng. Vì vậy chúng được chạy trong môi trường native. 
Việc build test trên esp32dev yêu cầu hàm setup và loop nên không phù hợp với unit test mức logic.
Dự án không thể kiểm thử trực tiếp phần cứng bằng unit test, do đó nhóm tách các khối xử lý logic ra khỏi phần giao tiếp phần cứng. 
  Các hàm logic được kiểm thử độc lập bằng framework Unity trên môi trường native của PlatformIO.
