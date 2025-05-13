// gui.cpp - Versão customizada com base no design do Canva
#include "imgui.h"
#include "gui.h"
#include "config.h"
#include <vector>
#include <cstring>
#include <windows.h>
#include <iostream>
using namespace std;

ImVec4 corFundo = ImVec4(0.082f, 0.078f, 0.251f, 1.00f); // #151440
ImVec4 corTexto = ImVec4(0.949f, 0.933f, 0.713f, 1.00f); // #F2EEB6
ImVec4 corInput = ImVec4(0.455f, 0.439f, 0.549f, 1.00f); // #74708C
ImVec4 corBotao = ImVec4(0.478f, 0.494f, 0.749f, 1.00f); // #7A7EBF
ImVec4 corBotaoExcluir = ImVec4(1.00f, 0.298f, 0.298f, 1.00f); // #FF4C4C

// Helper function to get the ImFont pointer by size
ImFont* GetImFont(float size){
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < io.Fonts->Fonts.Size; ++i){
        if (io.Fonts->Fonts[i]->FontSize == size){
            return io.Fonts->Fonts[i];
        }
    }
    return io.FontDefault;
}

static char senhaMestraInput[64] = "";
static std::string senhaMestraAutenticada = "";
static bool senhaMestraDefinida = VerificarArquivoConfig();

void RenderInterface(){
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = corFundo;
    ImGui::GetStyle().Colors[ImGuiCol_Text] = corTexto;

    static char senhaMestra[64] = "";
    static bool autenticado = false;
    static int abaAtual = 0;
    static bool mostrandoFormulario = false;
    static int editandoIndice = -1;
    static char url[128] = "";
    static char usuario[128] = "";
    static char senha[128] = "";
    static std::vector<EntradaCofre> cofre;
    static bool cofreCarregado = false;

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("Pass9Man", nullptr, windowFlags);
    float barraAltura = 30.0f;
    float botaoLargura = 30.0f;
    float espacamentoBotoes = 10.0f;
    float totalBotoes = 2 * botaoLargura + espacamentoBotoes;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, corInput);

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::BeginChild("TitleBar", ImVec2(ImGui::GetWindowWidth(), barraAltura), false, ImGuiWindowFlags_NoScrollWithMouse);

    // ===== ZONA DE ARRASTO =====
    float larguraDrag = ImGui::GetWindowWidth() - totalBotoes - 10;
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::InvisibleButton("##DragBar", ImVec2(larguraDrag, barraAltura));
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)){
        ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
        HWND hwnd = GetActiveWindow();
        RECT rect;
        GetWindowRect(hwnd, &rect);
        SetWindowPos(hwnd, NULL, rect.left + (int)dragDelta.x, rect.top + (int)dragDelta.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    // ===== TÍTULO =====
    ImGui::SetCursorPos(ImVec2(10, 7));
    ImGui::TextColored(corTexto, "PASS9MAN");

    // ===== BOTÕES NO CANTO DIREITO =====
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 70, 5));
    ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
    if (ImGui::Button("_", ImVec2(botaoLargura, 20))){
        ShowWindow(GetActiveWindow(), SW_MINIMIZE);
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
    if (ImGui::Button("X", ImVec2(botaoLargura, 20))){
        PostQuitMessage(0);
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    ImGui::PushFont(ImGui::GetFont());

    if (!senhaMestraDefinida){
        ImVec2 windowSize = ImVec2(400, 250);
        ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - windowSize.y) * 0.5f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::BeginChild("CentralizarSenhaMestra", windowSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float centralColumnWidth = 300.0f;
        float startX = (windowSize.x - centralColumnWidth) * 0.5f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (windowSize.y - 100) * 0.25f - ImGui::GetCursorPosY());

        ImGui::PushFont(GetImFont(50));
        float pass9manWidth = ImGui::CalcTextSize("PASS9MAN").x;
        ImGui::SetCursorPosX(startX + (centralColumnWidth - pass9manWidth) * 0.5f);
        ImGui::Text("PASS9MAN");
        float pass9manHeight = ImGui::CalcTextSize("PASS9MAN").y;
        ImGui::PopFont();

        float paddingVerticalTitulo = -15.0f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + paddingVerticalTitulo);

        ImGui::PushFont(GetImFont(24));
        ImGui::SetCursorPosX(startX + (centralColumnWidth - pass9manWidth) * 0.5f);
        ImGui::Text("PASSWORD\tMANAGER");
        float passwordManagerHeight = ImGui::CalcTextSize("PASSWORD\tMANAGER").y;
        ImGui::PopFont();

        float paddingVerticalAviso = 15.0f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + paddingVerticalAviso);

        ImGui::SetCursorPosX(startX + (centralColumnWidth - ImGui::CalcTextSize("PLEASE, REGISTER YOUR MASTER-KEY").x) * 0.5f);
        ImGui::Text("PLEASE, REGISTER YOUR MASTER-KEY");

        ImGui::SetCursorPosX(startX);
        ImGui::SetNextItemWidth(centralColumnWidth);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, corInput);
        ImGui::InputText("##SenhaRegistro", senhaMestra, IM_ARRAYSIZE(senhaMestra), ImGuiInputTextFlags_Password);
        ImGui::PopStyleColor();

        float buttonWidth = 80.0f;
        ImGui::SetCursorPosX(startX + (centralColumnWidth - buttonWidth) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
        if (ImGui::Button("REGISTER", ImVec2(buttonWidth, 30))){
            if (strlen(senhaMestra) > 0){
                SalvarSenhaMestra(senhaMestra);
                senhaMestraDefinida = true;
                memset(senhaMestra, 0, sizeof(senhaMestra));
            }
        }
        ImGui::PopStyleColor();

        ImGui::EndChild();
    }
    else if (!autenticado){
        ImVec2 windowSize = ImVec2(400, 250);
        ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - windowSize.y) * 0.5f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::BeginChild("CentralizarAutenticacao", windowSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float centralColumnWidth = 300.0f;
        float startX = (windowSize.x - centralColumnWidth) * 0.5f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (windowSize.y - 100) * 0.25f - ImGui::GetCursorPosY());

        ImGui::PushFont(GetImFont(50));
        float pass9manWidth = ImGui::CalcTextSize("PASS9MAN").x;
        ImGui::SetCursorPosX(startX + (centralColumnWidth - pass9manWidth) * 0.5f);
        ImGui::Text("PASS9MAN");
        float pass9manHeight = ImGui::CalcTextSize("PASS9MAN").y;
        ImGui::PopFont();

        float paddingVerticalTitulo = -15.0f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + paddingVerticalTitulo);

        ImGui::PushFont(GetImFont(24));
        ImGui::SetCursorPosX(startX + (centralColumnWidth - pass9manWidth) * 0.5f);
        ImGui::Text("PASSWORD\tMANAGER");
        float passwordManagerHeight = ImGui::CalcTextSize("PASSWORD\tMANAGER").y;
        ImGui::PopFont();

        float paddingVerticalAviso = 15.0f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + paddingVerticalAviso);

        ImGui::SetCursorPosX(startX + (centralColumnWidth - ImGui::CalcTextSize("PLEASE, INSERT YOUR MASTER-KEY").x) * 0.5f);
        ImGui::Text("PLEASE, INSERT YOUR MASTER-KEY");

        ImGui::SetCursorPosX(startX);
        ImGui::SetNextItemWidth(centralColumnWidth);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, corInput);
        ImGui::InputText("##SenhaLogin", senhaMestra, IM_ARRAYSIZE(senhaMestra), ImGuiInputTextFlags_Password);
        ImGui::PopStyleColor();

        float buttonWidth = 80.0f;
        ImGui::SetCursorPosX(startX + (centralColumnWidth - buttonWidth) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
        if (ImGui::Button("LOGIN", ImVec2(buttonWidth, 30))){
            std::cerr << "Botão LOGIN clicado!\n";
            auto senhaInfo = LerSenhaMestra();
            if (senhaInfo.first.empty() || senhaInfo.second.empty()){
                std::cerr << "[ERRO] Configuração inválida. Abortando autenticação.\n";
            }
            if (GerarHashSenhaComSalt(senhaMestra, senhaInfo.first) == senhaInfo.second){
                autenticado = true;
                senhaMestraAutenticada = senhaMestra;
                memset(senhaMestra, 0, sizeof(senhaMestra));
            }
        }
        ImGui::PopStyleColor();

        ImGui::EndChild();
    }else{
        if (!cofreCarregado){
            cofre = CarregarCofre(senhaMestraAutenticada);
            cofreCarregado = true;
        }

        // Menu lateral
        ImGui::BeginChild("Sidebar", ImVec2(250, 0), true);
        ImGui::PushFont(GetImFont(24));
        ImGui::Text("PASS9MAN v0.1");
        ImGui::PopFont();
        ImGui::PushFont(GetImFont(16));
        ImGui::Text("PASSWORD MANAGER");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
        ImGui::PushStyleColor(ImGuiCol_Text, corTexto);
        ImGui::PushFont(GetImFont(16));
        if (ImGui::Button("ACCOUNTS", ImVec2(230, 40))) abaAtual = 0;
        if (ImGui::Button("PASSWORD GENERATOR", ImVec2(230, 40))) abaAtual = 1;
        ImGui::PopFont();
        ImGui::PopStyleColor(2);
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("MainArea", ImVec2(0, 0), true);

        if (abaAtual == 0){
            ImGui::PushFont(GetImFont(24));
            ImGui::Text("ACCOUNTS");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
            ImGui::PushStyleColor(ImGuiCol_Text, corTexto);
            ImGui::PushFont(GetImFont(16));
            if (ImGui::Button("NEW ACCOUNT")){
                mostrandoFormulario = true;
                editandoIndice = -1;
                EntradaCofre entrada;
                memset(url, 0, sizeof(url));
                memset(usuario, 0, sizeof(usuario));
                memset(senha, 0, sizeof(senha));
                SalvarCofre(cofre, senhaMestraAutenticada);
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(2);

            if (mostrandoFormulario){
                ImGui::InputText("URL", url, IM_ARRAYSIZE(url));
                ImGui::InputText("USER", usuario, IM_ARRAYSIZE(usuario));
                ImGui::InputText("PASSWORD", senha, IM_ARRAYSIZE(senha), ImGuiInputTextFlags_Password);

                if (ImGui::Button(editandoIndice == -1 ? "SAVE" : "UPDATE")){
                    std::cerr << "Botão SALVAR/ATUALIZAR clicado!\n";
                    EntradaCofre entrada;
                    entrada.url = url;
                    entrada.usuario = usuario;
                    entrada.senha = senha;

                    if (editandoIndice == -1)
                        cofre.push_back(entrada);
                    else{
                        cofre[editandoIndice] = entrada;
                        editandoIndice = -1;
                    }

                    mostrandoFormulario = false;
                    memset(url, 0, sizeof(url));
                    memset(usuario, 0, sizeof(usuario));
                    memset(senha, 0, sizeof(senha));
                    SalvarCofre(cofre, senhaMestraAutenticada);
                }

                ImGui::SameLine();
                if (ImGui::Button("CANCEL")){
                    mostrandoFormulario = false;
                    editandoIndice = -1;
                }

                ImGui::Separator();
            }

            int colunas = 3;
            float largura = ImGui::GetContentRegionAvail().x / colunas - 10;
            for (int i = 0; i < cofre.size(); ++i){
                const auto& entrada = cofre[i];
                ImGui::PushID(i);
                ImGui::BeginGroup();
                ImGui::PushStyleColor(ImGuiCol_ChildBg, corBotao);
                ImGui::BeginChild("card", ImVec2(largura, 110), true);
                ImGui::PushFont(GetImFont(16));
                ImGui::Text("%s", cofre[i].url.c_str());
                ImGui::Text("User: %s", cofre[i].usuario.c_str());
                ImGui::Text("Pass: ********");
                ImGui::PopFont();
                ImGui::PushStyleColor(ImGuiCol_Button, corTexto);
                ImGui::PushStyleColor(ImGuiCol_Text, corInput);
                ImGui::PushFont(GetImFont(16));
                if (ImGui::Button(("EDIT##" + std::to_string(i)).c_str(), ImVec2(60, 30))){
                    mostrandoFormulario = true;
                    editandoIndice = i;
                    strncpy_s(url, sizeof(url), entrada.url.c_str(), _TRUNCATE);
                    strncpy_s(usuario, sizeof(usuario), entrada.usuario.c_str(), _TRUNCATE);
                    strncpy_s(senha, sizeof(senha), entrada.senha.c_str(), _TRUNCATE);
                }
                ImGui::SameLine();
                if (ImGui::Button(("COPY##" + std::to_string(i)).c_str(), ImVec2(60, 30))){
                    ImGui::SetClipboardText(entrada.senha.c_str());
                }
                ImGui::PopFont();
                ImGui::PopStyleColor(2);
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, corBotaoExcluir);
                ImGui::PushFont(GetImFont(16));
                if (ImGui::Button(("DELETE##" + std::to_string(i)).c_str(), ImVec2(60, 30))){
                    cofre.erase(cofre.begin() + i);
                    SalvarCofre(cofre, senhaMestra);
                    i--;
                }
                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::EndGroup();

                if ((i + 1) % colunas != 0)
                    ImGui::SameLine();
                ImGui::PopID();
            }
        }
        else if (abaAtual == 1){
            static int modo = 2;
            static int tamanho = 12;
            static bool incluirMaiuscula = true;
            static bool incluirMinuscula = true;
            static bool incluirNumeros = true;
            static bool incluirSimbolos = true;
            static char senhaGerada[64] = "";

            float larguraCentro = 500.0f;
            float startX = (ImGui::GetContentRegionAvail().x - larguraCentro) * 0.5f;

            ImGui::PushFont(GetImFont(24));
            ImGui::Text("PASSWORD GENERATOR");
            ImGui::PopFont();

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            // ======= MODO DE GERAÇÃO (Radios centralizados) =======
            ImGui::SetCursorPosX(startX);
            ImGui::Text("Generation Mode:");

            ImGui::SetCursorPosX(startX);
            if (ImGui::RadioButton(" EASY TO PRONOUNCE", modo == 0)) modo = 0;

            ImGui::SameLine();
            if (ImGui::RadioButton(" EASY TO READ", modo == 1)) modo = 1;

            ImGui::SameLine();
            if (ImGui::RadioButton(" ALL CHARACTERS", modo == 2)) modo = 2;

            ImGui::Spacing();
            ImGui::Spacing(); ImGui::Text("");

            // ======= SLIDER =======
            ImGui::SetCursorPosX(startX);
            ImGui::Text("PASSWORD LENGTH");

            ImGui::SetCursorPosX(startX);
            ImGui::SetNextItemWidth(larguraCentro);
            ImGui::SliderInt("##Tamanho", &tamanho, 1, 50, "%d");

            // ======= CHECKBOXES =======
            bool disableNumSym = (modo == 0);
            if (modo == 0){
                incluirNumeros = false;
                incluirSimbolos = false;
            }

            ImGui::Spacing(); ImGui::Text("");
            ImGui::SetCursorPosX(startX);
            ImGui::Text("CHARACTER TYPES:");

            // Centralizar os grupos de checkbox
            float larguraCheckboxColuna = 120.0f;
            float espacamentoColunas = 50.0f;
            float larguraTotalCheckboxes = 2 * larguraCheckboxColuna + espacamentoColunas;
            float startXCheckboxes = (ImGui::GetContentRegionAvail().x - larguraTotalCheckboxes) * 0.5f;

            ImGui::SetCursorPosX(startXCheckboxes);
            ImGui::BeginGroup(); // Coluna 1
            ImGui::Checkbox(" LOWERCASE", &incluirMinuscula);
            ImGui::Checkbox(" UPPERCASE", &incluirMaiuscula);
            ImGui::EndGroup();

            ImGui::SameLine(0.0f, espacamentoColunas);

            ImGui::BeginGroup(); // Coluna 2
            ImGui::BeginDisabled(disableNumSym);
            ImGui::Checkbox(" NUMBERS", &incluirNumeros);
            ImGui::Checkbox(" SYMBOLS", &incluirSimbolos);
            ImGui::EndDisabled();
            ImGui::EndGroup();


            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            // ======= CAMPO DE SENHA + COPY CENTRALIZADO =======
            float larguraCampo = 435;
            float larguraCopy = 60;
            float totalLargura = larguraCampo + larguraCopy + 10;

            float centralX = (ImGui::GetContentRegionAvail().x - totalLargura) * 0.5f;

            ImGui::PushFont(GetImFont(24));
            ImGui::SetCursorPosX(centralX);
            ImGui::SetNextItemWidth(larguraCampo);
            ImGui::InputText("##GeneratedPassword", senhaGerada, IM_ARRAYSIZE(senhaGerada), ImGuiInputTextFlags_ReadOnly);
            ImGui::PopFont();

            ImGui::SameLine();
            if (ImGui::Button("COPY", ImVec2(larguraCopy, 30))){
                ImGui::SetClipboardText(senhaGerada);
            }

            // ======= BOTÃO GERAR SENHA  =======
            ImGui::Spacing(); ImGui::Spacing();
            ImGui::SetCursorPosX(centralX);
            ImGui::PushStyleColor(ImGuiCol_Button, corBotao);
            if (ImGui::Button("GENERATE PASSWORD", ImVec2(totalLargura, 35))){
                bool evitarAmbiguos = (modo == 1);
                std::string senha = GerarSenha(tamanho,incluirMaiuscula,incluirMinuscula,incluirNumeros,incluirSimbolos,evitarAmbiguos);
                if (senha.empty()){
                    strncpy_s(senhaGerada, sizeof(senhaGerada), "<ERRO>", _TRUNCATE);
                }else{
                    strncpy_s(senhaGerada, sizeof(senhaGerada), senha.c_str(), _TRUNCATE);
                }
            }
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
    }
    ImGui::PopFont();
    ImGui::End();
}
