#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>

struct HouseNumber {
    int mainNumber;
    std::string suffix;

    HouseNumber(int num, const std::string& suf) : mainNumber(num), suffix(suf) {}
};

// Функция для разбора строки на числовую и буквенную части
HouseNumber parseHouseNumber(const std::string& house) {
    std::regex pattern(R"((\d+)([a-zA-Z\/]*)?)");
    std::smatch match;
    if (std::regex_match(house, match, pattern)) {
        int mainNumber = std::stoi(match[1]);
        std::string suffix = match[2];
        return HouseNumber(mainNumber, suffix);
    }
    return HouseNumber(0, ""); // для неподходящих значений
}

// Компаратор для сортировки
bool houseNumberComparator(const std::string& a, const std::string& b) {
    HouseNumber houseA = parseHouseNumber(a);
    HouseNumber houseB = parseHouseNumber(b);

    if (houseA.mainNumber != houseB.mainNumber) {
        return houseA.mainNumber < houseB.mainNumber;
    }
    return houseA.suffix < houseB.suffix;
}

int main() {
    std::vector<std::string> houseNumbers = {"1", "2", "3", "4/1", "6a", "12", "6b", "10/2"};

    std::sort(houseNumbers.begin(), houseNumbers.end(), houseNumberComparator);

    std::cout << "Отсортированные номера домов:\n";
    for (const auto& house : houseNumbers) {
        std::cout << house << "\n";
    }

    return 0;
}
