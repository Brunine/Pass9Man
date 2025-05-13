# Pass9Man ??

Gerenciador de senhas moderno com GUI em C++ + ImGui + OpenSSL.

## ? Recursos

- Armazenamento local criptografado com AES-256 (via OpenSSL)
- Cofre de senhas protegido por senha-mestra com hash + salt (PBKDF2)
- Interface gr�fica fluida usando ImGui e DirectX 11
- Empacotado com Inno Setup para distribui��o simples

## ?? Como compilar

1. Clone o reposit�rio
2. Abra o projeto no Visual Studio 2022
3. Compile em modo Release x64

Requisitos:
- OpenSSL (incluso ou compilado estaticamente)
- ImGui (j� incluso)

## ?? Instalador

Um script Inno Setup (`installer/Pass9Man.iss`) est� incluso para gerar o instalador `.exe`.

## ?? Licen�a

[MIT](LICENSE)
