#include "stdafx.h"

#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>
#include <sstream>

std::string encryptParams(const std::string& params, const std::string& publicKeyFile) {
    // Загрузка открытого ключа из файла
    FILE* publicKeyFilePtr = fopen(publicKeyFile.c_str(), "rb");
    if (!publicKeyFilePtr) {
        std::cerr << "Ошибка при открытии файла с открытым ключом" << std::endl;
        return "";
    }
    RSA* publicKey = PEM_read_RSA_PUBKEY(publicKeyFilePtr, NULL, NULL, NULL);
    fclose(publicKeyFilePtr);
    if (!publicKey) {
        std::cerr << "Ошибка при загрузке открытого ключа" << std::endl;
        return "";
    }

    // Преобразование параметров в строку JSON
    std::string paramsString = params;

    // Шифрование параметров с использованием открытого ключа
    int rsaLen = RSA_size(publicKey);
    unsigned char* encrypted = new unsigned char[rsaLen];
    int encryptedLen = RSA_public_encrypt(paramsString.size(), (const unsigned char*)paramsString.c_str(), encrypted, publicKey, RSA_PKCS1_OAEP_PADDING);
    if (encryptedLen == -1) {
        std::cerr << "Ошибка при шифровании параметров" << std::endl;
        RSA_free(publicKey);
        delete[] encrypted;
        return "";
    }

    // Кодирование зашифрованных параметров в строку Base64 для передачи
    std::string base64Encoded(encrypted, encrypted + encryptedLen);
    delete[] encrypted;
    RSA_free(publicKey);

    return base64Encoded;
}

#if 0
int main() {
    // Пример использования функции
    std::string publicKeyFile = "receiver_public.pem";
    std::string params = "{\"name\": \"Astana\", \"count\": 5, \"key\": \"secret_pass\"}";
    std::string encryptedParams = encryptParams(params, publicKeyFile);
    std::cout << "Зашифрованные параметры: " << encryptedParams << std::endl;
    return 0;
}
#endif