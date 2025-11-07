# File Inspector (GTK Version)

Một chương trình **C/GTK3** đơn giản cho **Linux** để xem thông tin chi tiết của tệp hoặc thư mục.

## Yêu cầu hệ thống

- Hệ điều hành: Debian/Ubuntu (hoặc dẫn xuất)
- Trình biên dịch: `gcc` (thuộc `build-essential`)
- Công cụ & thư viện: `pkg-config`, `libgtk-3-dev`

Cài đặt nhanh các gói cần thiết:
```
    sudo apt update
    sudo apt install build-essential pkg-config libgtk-3-dev
```

## Biên dịch và Chạy

1) Clone mã nguồn
```
    git clone https://github.com/kiet293/fileinfo.git
```

2) Di chuyển vào thư mục dự án
```
    cd fileinfo
```
3) Biên dịch
```
    make
```
4) Chạy ứng dụng
```
    ./fileinspector
```
> Lưu ý: Dự án sử dụng `Makefile`, vì vậy chỉ cần `make` để biên dịch trên hệ thống đã cài đủ phụ thuộc.


## Dọn dẹp

Xoá các tệp sinh ra khi biên dịch:

    make clean


## Góp ý & Báo lỗi

Nếu gặp lỗi hoặc có đề xuất tính năng, vui lòng mở **Issue** trên GitHub của dự án.
