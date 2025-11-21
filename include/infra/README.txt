Folder infra/

Chứa phần hạ tầng để lưu trữ dữ liệu và serialization:

1. repositories/: triển khai lưu trữ file CSV
    - AccountRepoFile.hpp, PetRepoFile.hpp, ServiceRepoFile.hpp...
    - Gọi repository interface và thao tác dữ liệu thực tế

2. serialization/: đọc/ghi dữ liệu CSV và map row → entity
    - CsvSerializer.hpp: đọc/ghi CSV
    - Mapper.hpp: ánh xạ dữ liệu từ CSV sang entity
