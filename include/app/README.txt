Folder app/

Chứa các file liên quan đến giao diện ứng dụng (CLI) và controllers:

- App.hpp: khởi tạo và chạy ứng dụng
- Menu.hpp: các menu CLI cho user
- controllers/: xử lý logic cho từng loại user
    - AdminController.hpp: logic quản lý admin
    - StaffController.hpp: logic quản lý nhân viên
    - ClientController.hpp: logic cho client
    - AuthController.hpp: đăng nhập, đăng ký, phân quyền
