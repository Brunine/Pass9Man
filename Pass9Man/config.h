#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <utility> 

struct EntradaCofre {
    std::string url;
    std::string usuario;
    std::string senha;
};

bool VerificarArquivoConfig();
void SalvarSenhaMestra(const std::string& senha);
std::pair<std::string, std::string> LerSenhaMestra();
std::string GerarSalt(size_t tamanho);
std::string GerarHashSenhaComSalt(const std::string& senha, const std::string& salt);
std::string GerarSenha(int tamanho, bool maiuscula, bool minuscula, bool numeros, bool simbolos, bool evitarAmbiguos);

bool DeriveKey(const std::string& password, unsigned char* key, unsigned char* iv);
std::string EncryptAES(const std::string& plaintext, const std::string& password);
std::string DecryptAES(const std::string& ciphertext, const std::string& password);

void SalvarCofre(const std::vector<EntradaCofre>& cofre, const std::string& masterPassword);
std::vector<EntradaCofre> CarregarCofre(const std::string& masterPassword);

#endif // CONFIG_H
