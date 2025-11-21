Folder domain/

Chứa business logic cốt lõi:

1. entities/: đại diện các đối tượng trong hệ thống
    - Account.hpp: class cơ sở cho user
    - Admin.hpp, Staff.hpp, Client.hpp: kế thừa Account, phân quyền khác nhau
    - Pet.hpp, Voucher.hpp, Bill.hpp, Booking.hpp, WishlistItem.hpp

2. repositories/: định nghĩa interface repository
    - IAccountRepository.hpp, IPetRepository.hpp, IServiceRepository.hpp...
    - Chỉ khai báo phương thức CRUD, không chứa logic lưu trữ

3. services/: triển khai logic nghiệp vụ
    - AuthService.hpp: đăng nhập/đăng ký
    - PetService.hpp, BookingService.hpp, BillingService.hpp, VoucherService.hpp, ReportService.hpp
