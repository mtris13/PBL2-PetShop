/* PetRepoFile.hpp - đọc và ghi các file Pet (UPDATED) */
#pragma once
#include "domain/entities/Cat.hpp"
#include "domain/entities/Dog.hpp"
#include "domain/entities/Pet.hpp"
#include "ds/LinkedList.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct PetStats {
  int totalDogs = 0;
  int soldDogs = 0;
  int availableDogs = 0;
  int totalCats = 0;
  int soldCats = 0;
  int availableCats = 0;
};

class PetRepository {
private:
  std::string dogFilePath;
  std::string catFilePath;
  LinkedList<Dog> m_dogs;
  LinkedList<Cat> m_cats;
  const int petIdLength = 4;
  const std::string invalid = "";

  std::string filePath(const std::string &petCode);
  std::string readingFile(const std::string &petCode);
  void writingFile(const std::string &petCode, const std::string &writeLine);
  bool isDogId(const std::string &petCode);
  // --- THÊM HÀM HELPER ĐỂ XÓA DẤU NGOẶC KÉP ---
  std::string trimQuotes(std::string s);
  std::string toLowercase(std::string &str);

public:
  // GET
  PetRepository(const std::string &dogPath, const std::string &catPath);
  std::string getPetStatus(const std::string &petCode);
  Dog getDogInfo(const std::string &petCode);
  Cat getCatInfo(const std::string &petCode);
  LinkedList<std::string> getAllCatId();
  LinkedList<std::string> getAllDogId();
  // Code MỚI
  void _loadAllCatInfoFromFile();
  void _loadAllDogInfoFromFile();
  LinkedList<Cat> getAllCatInfoAvailable();
  LinkedList<Dog> getAllDogInfoAvailable();
  // SET
  void setDogInfo(const Dog &dog);
  void setCatInfo(const Cat &cat);
  void setStatusUnavailable(const std::string &petCode);
  void
  setStatusAvailable(const std::string &petCode); // use when pet is restored

  // OTHERS
  bool isAvailablePet(const std::string &petCode);
  bool isValidPetId(const std::string &petCode);
  void deletePet(const std::string &petCode);
  std::string generatePetId(std::string &type);
  LinkedList<Cat> searchCat(std::string searchType, std::string &keyword);
  LinkedList<Dog> searchDog(std::string searchType, std::string &keyword);
  PetStats countPet();
};