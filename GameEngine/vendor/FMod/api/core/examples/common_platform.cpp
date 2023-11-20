/*==============================================================================
FMOD Example Framework
Copyright (c), Firelight Technologies Pty, Ltd 2013-2023.
==============================================================================*/
#include "common.h"
#include <vector>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::ViewManagement;

TextBlock ^gText;
CoreDispatcher ^gUIDispatcher;
std::wstring gOutputString;
std::vector<char *> gStringList;
unsigned int gKeyboardState;
unsigned int gTouchState;
unsigned int gPressedButtons;
unsigned int gDownButtons;

static const char DATA_PREFIX[] = "ms-appx://";

int FMOD_Main();

void Common_Init(void** /*extraDriverData*/)
{

}

void Common_Close()
{
    for (auto item = gStringList.begin(); item != gStringList.end(); ++item)
    {
        free(*item);
    }

    Application::Current->Exit();
}

void Common_Update()
{
    unsigned int inputState = gKeyboardState | gTouchState;
    gTouchState = 0;

    gPressedButtons = (gDownButtons ^ inputState) & inputState;
    gDownButtons = inputState;

    String ^content = ref new String(gOutputString.c_str());
    gUIDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([content]() { gText->Text = content; }));

    gOutputString.clear();
}

void Common_Exit(int /*returnCode*/)
{
    Application::Current->Exit();
}

void Common_DrawText(const char *text)
{
    wchar_t wideText[256];
    _snwprintf_s(wideText, _countof(wideText), L"%S\n", text);

    gOutputString.append(wideText);
}

void Common_TTY(const char *format, ...)
{
    char string[1024] = { 0 };

    va_list args;
    va_start(args, format);
    Common_vsnprintf(string, 1023, format, args);
    va_end(args);

    OutputDebugStringA(string);
}

bool Common_BtnPress(Common_Button btn)
{
    return ((gPressedButtons & (1 << btn)) != 0);
}

bool Common_BtnDown(Common_Button btn)
{
    return ((gDownButtons & (1 << btn)) != 0);
}

const char *Common_BtnStr(Common_Button btn)
{
    switch (btn)
    {
        case BTN_ACTION1:   return "1";
        case BTN_ACTION2:   return "2";
        case BTN_ACTION3:   return "3";
        case BTN_ACTION4:   return "4";
        case BTN_LEFT:      return "Left";
        case BTN_RIGHT:     return "Right";
        case BTN_UP:        return "Up";
        case BTN_DOWN:      return "Down";
        case BTN_MORE:      return "More"; // Spacebar triggers UI elements, so use something else
        case BTN_QUIT:      return "Esc";
        default:            return "Unknown";
    }
}

const char *Common_MediaPath(const char *fileName)
{
    char *filePath = (char *)calloc(256, sizeof(char));
    sprintf_s(filePath, 256, "%s/media/%s", DATA_PREFIX, fileName);
    gStringList.push_back(filePath);

    return filePath;
}

const char *Common_WritePath(const char *fileName)
{
    char *filePath = (char *)calloc(256, sizeof(char));
    sprintf_s(filePath, 256, "%S\\%s", ApplicationData::Current->TemporaryFolder->Path->Begin(), fileName);
    gStringList.push_back(filePath);

    return filePath;
}

static unsigned int virtualKeyToMask(VirtualKey key)
{
    if (key == VirtualKey::Number1) return (1 << BTN_ACTION1);
    if (key == VirtualKey::Number2) return (1 << BTN_ACTION2);
    if (key == VirtualKey::Number3) return (1 << BTN_ACTION3);
    if (key == VirtualKey::Number4) return (1 << BTN_ACTION4);
    if (key == VirtualKey::Left)    return (1 << BTN_LEFT);
    if (key == VirtualKey::Right)   return (1 << BTN_RIGHT);
    if (key == VirtualKey::Up)      return (1 << BTN_UP);
    if (key == VirtualKey::Down)    return (1 << BTN_DOWN);
    if (key == VirtualKey::Q)       return (1 << BTN_MORE);
    if (key == VirtualKey::Escape)  return (1 << BTN_QUIT);
    return 0;
}

ref struct App sealed : public Application
{
    virtual void OnLaunched(LaunchActivatedEventArgs ^args) override
    {
        if (args->PreviousExecutionState == ApplicationExecutionState::Running)
        {
            Window::Current->Activate();
            return;
        }

        gText = ref new TextBlock();
        gText->FontFamily = ref new FontFamily("Consolas");
        gText->FontSize = 12;
        gText->MaxLines = NUM_ROWS;

        Button ^buttons[9];
        for (int i = 0; i < 9; i++)
        {
            wchar_t wideText[256];
            _snwprintf_s(wideText, _countof(wideText), L"%S", Common_BtnStr((Common_Button)i));

            buttons[i] = ref new Button();
            buttons[i]->Content = ref new String(wideText);
            buttons[i]->Click += ref new RoutedEventHandler([](Object ^sender, RoutedEventArgs^) { gTouchState |= (1 << (int)((Button^)sender)->Tag); });
            buttons[i]->Tag = i;
            buttons[i]->Width = 100;
            buttons[i]->Height = 60;
            buttons[i]->Margin = 2;
        }

        auto topButtonPanel = ref new StackPanel();
        topButtonPanel->Orientation = Orientation::Horizontal;
        topButtonPanel->Children->Append(buttons[0]);
        topButtonPanel->Children->Append(buttons[6]);
        topButtonPanel->Children->Append(buttons[1]);

        auto middleButtonPanel = ref new StackPanel();
        middleButtonPanel->Orientation = Orientation::Horizontal;
        middleButtonPanel->Children->Append(buttons[4]);
        middleButtonPanel->Children->Append(buttons[8]);
        middleButtonPanel->Children->Append(buttons[5]);

        auto bottomButtonPanel = ref new StackPanel();
        bottomButtonPanel->Orientation = Orientation::Horizontal;
        bottomButtonPanel->Children->Append(buttons[2]);
        bottomButtonPanel->Children->Append(buttons[7]);
        bottomButtonPanel->Children->Append(buttons[3]);

        auto buttonPanel = ref new StackPanel();
        buttonPanel->Orientation = Orientation::Vertical;
        buttonPanel->HorizontalAlignment = HorizontalAlignment::Center;
        buttonPanel->VerticalAlignment = VerticalAlignment::Bottom;
        buttonPanel->Children->Append(topButtonPanel);
        buttonPanel->Children->Append(middleButtonPanel);
        buttonPanel->Children->Append(bottomButtonPanel);

        auto grid = ref new Grid();
        grid->Margin = 10;
        grid->Children->Append(gText);
        grid->Children->Append(buttonPanel);

        auto page = ref new Page();
        page->Content = grid;
        page->KeyDown += ref new KeyEventHandler([](Object^, KeyRoutedEventArgs ^args) { gKeyboardState |= virtualKeyToMask(args->Key); });
        page->KeyUp += ref new KeyEventHandler([](Object^, KeyRoutedEventArgs ^args) { gKeyboardState &= ~virtualKeyToMask(args->Key); });
        page->Visibility = Visibility::Visible;

        ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(350, 500));
        ApplicationView::PreferredLaunchWindowingMode = ApplicationViewWindowingMode::PreferredLaunchViewSize;
        ApplicationView::PreferredLaunchViewSize = Size(350, 600);

        Window::Current->Content = page;
        Window::Current->Activate();

        gUIDispatcher = Window::Current->Dispatcher; // Save the dispatcher for this thread (UI thread) so we can use it from other threads
        gUIDispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        CreateThread(nullptr, 0, [](void *) -> unsigned long { return FMOD_Main(); }, nullptr, 0, nullptr);
    }
};

int main(Array<String^>^)
{
    Application::Start(ref new ApplicationInitializationCallback([](ApplicationInitializationCallbackParams^) { ref new App(); }));
    return 0;
}
