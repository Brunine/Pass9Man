#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "gui.h"
#include "config.h"
#include "Resource.h"

#pragma comment(lib, "d3d11.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain: ponto de entrada
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow){
    // 1. Registrar a janela
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
        GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Pass9ManClass", NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"Pass9Man",
        WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    HICON hIconBig = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    HICON hIconSmall = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);

    // 2. Criar dispositivo DirectX11
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 1280;
    sd.BufferDesc.Height = 800;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    IDXGISwapChain* g_pSwapChain = nullptr;
    ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

    D3D_FEATURE_LEVEL featureLevel;
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK){
        return 1;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    HRESULT hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr) || !pBackBuffer){
        MessageBox(hwnd, L"Erro ao obter o BackBuffer", L"Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr) || !g_mainRenderTargetView){
        MessageBox(hwnd, L"Erro ao criar RenderTargetView", L"Erro", MB_OK | MB_ICONERROR);
        return 1;
    }

    // 3. Inicializar ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    ImGui::StyleColorsDark();

    // Carregar a fonte OpenSans com diferentes tamanhos
    ImFont* openSansFont12 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 12.0f);
    ImFont* openSansFont16 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 16.0f); 
    ImFont* openSansFont20 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 20.0f); 
    ImFont* openSansFont24 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 24.0f); 
    ImFont* openSansFont30 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 30.0f); 
    ImFont* openSansFont40 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 40.0f); 
    ImFont* openSansFont50 = io.Fonts->AddFontFromFileTTF("OpenSans.ttf", 50.0f); 
    if (openSansFont16) {
        io.FontDefault = openSansFont16;
    } else {
        MessageBox(hwnd, L"Erro ao carregar a fonte OpenSans.ttf (tamanho 16)", L"Erro", MB_OK | MB_ICONERROR);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 4. Loop principal
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT){
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        RenderInterface();  // Chamar a GUI principal
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        const float clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }
    // 5. Limpeza
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (g_mainRenderTargetView) g_mainRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
