#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include "json.hpp"

using json = nlohmann::json;

const size_t SALT_SIZE = 16;
const int PBKDF2_ITERATIONS = 10000;
const size_t AES_KEY_SIZE = 32; // AES-256
//const size_t AES_BLOCK_SIZE = 16;

bool VerificarArquivoConfig(){
    std::ifstream config_file("msyfshgoa");
    return config_file.good();
}

std::string GerarSalt(size_t tamanho){
    const char caracteres[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(caracteres) - 2);
    std::string salt;
    for (size_t i = 0; i < tamanho; ++i)
        salt += caracteres[dist(gen)];
    return salt;
}

std::string GerarHashSenhaComSalt(const std::string& senha, const std::string& salt){
    std::string senhaComSalt = senha + salt;
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, senhaComSalt.data(), senhaComSalt.size());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    std::ostringstream oss;
    for (unsigned int i = 0; i < hash_len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    return oss.str();
}

void SalvarSenhaMestra(const std::string& senha){
    std::string salt = GerarSalt(SALT_SIZE);
    std::string hash = GerarHashSenhaComSalt(senha, salt);
    std::ofstream config_file("msyfshgoa", std::ios::trunc);
    if(config_file.is_open()){
        config_file << salt << "\n" << hash << std::endl;
        config_file.close();
    }else{
        std::cerr << "Erro ao salvar config.txt\n";
    }
}

std::pair<std::string, std::string> LerSenhaMestra(){
    std::ifstream config_file("msyfshgoa");
    std::string salt, hash;
    if (!config_file.is_open()) return{ "", "" };
    if (!std::getline(config_file, salt) || !std::getline(config_file, hash)) return{ "", "" };
    return{ salt, hash };
}

bool DeriveKey(const std::string& password, unsigned char* key, unsigned char* iv){
    return EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), nullptr, reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), 10000, key, iv) > 0;
}

std::string EncryptAES(const std::string& plaintext, const std::string& password){
    unsigned char key[AES_KEY_SIZE], iv[AES_BLOCK_SIZE];
    if (!DeriveKey(password, key, iv)) return "";
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string ciphertext;
    ciphertext.resize(plaintext.size() + AES_BLOCK_SIZE);
    int len;
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size());
    int totalLen = len;
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]) + len, &len);
    totalLen += len;
    ciphertext.resize(totalLen);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

std::string DecryptAES(const std::string& ciphertext, const std::string& password){
    unsigned char key[AES_KEY_SIZE], iv[AES_BLOCK_SIZE];
    if (!DeriveKey(password, key, iv)) return "";
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string plaintext;
    plaintext.resize(ciphertext.size());
    int len;
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]), &len, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
    int totalLen = len;
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]) + len, &len) <= 0){
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    totalLen += len;
    plaintext.resize(totalLen);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

void SalvarCofre(const std::vector<EntradaCofre>& cofre, const std::string& masterPassword){
    json j;
    for (const auto& entrada : cofre){
        j.push_back({{"url", entrada.url},{"usuario", entrada.usuario},{"senha", entrada.senha}});
    }
    std::string jsonData = j.dump();
    std::string encrypted = EncryptAES(jsonData, masterPassword);
    std::ofstream out("pfibwbskogvsfs", std::ios::binary);
    out.write(encrypted.data(), encrypted.size());
}

std::vector<EntradaCofre> CarregarCofre(const std::string& masterPassword){
    std::vector<EntradaCofre> cofre;
    std::ifstream in("pfibwbskogvsfs", std::ios::binary);
    if(!in) return cofre;
    std::ostringstream oss;
    oss << in.rdbuf();
    std::string decrypted = DecryptAES(oss.str(), masterPassword);
    if(decrypted.empty()) return cofre;
    try{
        json j = json::parse(decrypted);
        for(const auto& item : j){
            cofre.push_back({ item["url"], item["usuario"], item["senha"] });
        }
    }
    catch(...){
        std::cerr << "Erro ao parsear JSON do cofre.\n";
    }
    return cofre;
}

std::string GerarSenha(int tamanho, bool maiuscula, bool minuscula, bool numeros, bool simbolos, bool evitarAmbiguos){
    std::string caracteres;

    if (maiuscula) caracteres += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (minuscula) caracteres += "abcdefghijklmnopqrstuvwxyz";
    if (numeros)   caracteres += "0123456789";
    if (simbolos)  caracteres += "!@#$%^&*()-_=+[]{};:,.<>?/\\|";

    if (evitarAmbiguos){
        std::string ambiguos = "Il1O0";
        caracteres.erase(std::remove_if(caracteres.begin(), caracteres.end(), [&](char c){
            return ambiguos.find(c) != std::string::npos;
            }), caracteres.end());
    }
    if (caracteres.empty()) return "";
    std::string senha;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, caracteres.size() - 1);
    for (int i = 0; i < tamanho; ++i)
        senha += caracteres[dist(gen)];
    return senha;
}

