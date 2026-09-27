#include <windows.h>

const int HOTKEY_JUST_OPEN_ID = 1; // Alt + A
const int HOTKEY_SCREENSHOT_ID = 2;  // Alt + X

// Исправленная функция симуляции нажатия Win + Shift + S
void OpenSnippingTool() {
    // Нам нужно сначала принудительно "отпустить" ALT, который зажал пользователь,
    // а затем последовательно нажать Win, Shift, S, и затем их отпустить.
    INPUT inputs[8] = {};

    // 0. Отпускаем ALT (чтобы он не мешал)
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_MENU; inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

    // 1. Зажимаем клавиши (Win, Shift, S)
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_LWIN;  inputs[1].ki.dwFlags = 0;
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_SHIFT; inputs[2].ki.dwFlags = 0;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = 'S';      inputs[3].ki.dwFlags = 0;

    // 2. Отпускаем клавиши
    inputs[4].type = INPUT_KEYBOARD; inputs[4].ki.wVk = 'S';      inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[5].type = INPUT_KEYBOARD; inputs[5].ki.wVk = VK_SHIFT;  inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[6].type = INPUT_KEYBOARD; inputs[6].ki.wVk = VK_LWIN;   inputs[6].ki.dwFlags = KEYEVENTF_KEYUP;
    
    // 3. Возвращаем ALT в нажатое состояние (опционально, для чистоты симуляции)
    inputs[7].type = INPUT_KEYBOARD; inputs[7].ki.wVk = VK_MENU;   inputs[7].ki.dwFlags = 0;

    SendInput(8, inputs, sizeof(INPUT));
}

// Исправленная функция симуляции Ctrl + V
void PasteFromClipboard() {
    INPUT inputs[4] = {};

    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_CONTROL; inputs[0].ki.dwFlags = 0;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = 'V';        inputs[1].ki.dwFlags = 0;

    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = 'V';        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_CONTROL;  inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(4, inputs, sizeof(INPUT));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!RegisterHotKey(NULL, HOTKEY_JUST_OPEN_ID, MOD_ALT, 'A')) return 1;
    if (!RegisterHotKey(NULL, HOTKEY_SCREENSHOT_ID, MOD_ALT, 'X')) {
        UnregisterHotKey(NULL, HOTKEY_JUST_OPEN_ID);
        return 1;
    }

    MSG msg = { 0 };
    const wchar_t* gemini_url = L"https://google.com/ai";

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == HOTKEY_JUST_OPEN_ID) {
                ShellExecuteW(NULL, L"open", gemini_url, NULL, NULL, SW_SHOWNORMAL);
            }
            else if (msg.wParam == HOTKEY_SCREENSHOT_ID) {
                // Очищаем буфер обмена перед скриншотом, чтобы старая картинка не сработала мгновенно
                if (OpenClipboard(NULL)) {
                    EmptyClipboard();
                    CloseClipboard();
                }

                OpenSnippingTool();

                bool hasImage = false;
                // Ждем скриншот максимум 20 секунд (100 итераций по 200 мс)
                for (int i = 0; i < 100; i++) {
                    Sleep(200); 
                    if (IsClipboardFormatAvailable(CF_BITMAP)) {
                        hasImage = true;
                        break;
                    }
                }

                if (hasImage) {
                    ShellExecuteW(NULL, L"open", gemini_url, NULL, NULL, SW_SHOWNORMAL);
                    // Даем вкладке браузера гарантированно прогрузиться
                    Sleep(2500); 
                    PasteFromClipboard();
                }
            }
        }
    }

    UnregisterHotKey(NULL, HOTKEY_JUST_OPEN_ID);
    UnregisterHotKey(NULL, HOTKEY_SCREENSHOT_ID);
    return 0;
}
