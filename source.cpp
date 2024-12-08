#define _CRT_SECURE_NO_WARNINGS
#include "check-box.h"
#include "combo-box.h"
#include "configuration.h"
#include "context-menu.h"
#include "generator.h"
#include "input-box.h"
#include "label.h"
#include "logger.h"
#include "progress-bar.h"
#include "resource.h"
#include "storage.h"
#include "streamline.h"
#include "tooltip.h"
#include "Windows.h"
#include "workstation.h"
#include <iostream>
#include "list-box.h"

#define _S(stream) [&]() {std::stringstream ss; \
ss << stream; \
return ss.str(); \
} ()

#define DEFAULT_GENERATOR_RATE 60 
#define DEFAULT_STREAMLINE_RATE 20

int queue_to_setup = 0;
int new_queue_size = 0;

HINSTANCE hInst;

Generator generator;

Streamline  g_ws1(DEFAULT_STREAMLINE_RATE),
			ws1_ws2(DEFAULT_STREAMLINE_RATE);

Workstation workstation_1, 
            workstation_2(2);

Storage storage;

HANDLE h_generator_thread;
HANDLE h_ws1_thread, h_ws2_thread;
HANDLE h_thread_g_ws1, h_thread_ws1_ws2, h_thread_ws2_s;

Label *label_generator_state,   *label_generator_rate;
Label *label_sl_g_ws1_length,   *label_sl_ws1_ws2_length,   *label_sl_total_length;
Label *label_ws1_state,         *label_ws1_rt;
Label *label_ws2_state,         *label_ws2_rt;
Label *label_storage_stored;

Label *label_ws_stat_title;

ProgressBar* pbar_g;
Tooltip* ttip_generator_progress;
ProgressBar *pbar_ws1, *pbar_ws2;
Tooltip* ttip_ws1_progress_tip, *ttip_ws2_progress_tip;

ComboBox *cbox_ws_stat_picker;
InputBox* input_ws_stat;

ContextMenu* contextmenu;

HWND h_toolbar;
HWND h_status_bar;

CheckBox *checkbox_ws1_state, *checkbox_ws2_state;
ListBox *lb_g_ws1, *lb_ws1_ws2;

Tooltip* ttip_reset_stat;
HWND h_button_reset_stat;

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI generator_thread(LPVOID lpParam) {
    generator.run();
    return 0;
}

void streamline_tick(TickInfo info) {
    double avg = (double)info.processed / (double)info.in_wait;
    auto s = _S("avg time: " << avg << "\r\n"
                << "wait: " << info.in_wait << "\r\n"
                << "total: " << info.processed);

    input_ws_stat->set_text(s);
}

void update_ws_stat(int ws_index) {
	label_ws_stat_title->set_text(_S("Workstation selected: " << ws_index));

    Runnable* r = nullptr;
    if (ws_index == 0) {
        ws1_ws2.on_tick.clear();
        g_ws1.on_tick.attach(streamline_tick);
    }
    else if (ws_index == 1) {
        g_ws1.on_tick.clear();
        ws1_ws2.on_tick.attach(streamline_tick);
    }
}

HWND create_toolbar(HWND hWndParent) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES; 
    InitCommonControlsEx(&icex);

    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE  ,
        0, 0, 0, 0,
        hWndParent, (HMENU)IDR_TOOLBAR1, GetModuleHandle(NULL), NULL);

    if (!hWndToolbar) {
        MessageBox(hWndParent, "Failed to create toolbar!", "Error", MB_OK | MB_ICONERROR);
        return NULL;
    }

    TBADDBITMAP tbBitmap = { GetModuleHandle(NULL), IDR_TOOLBAR1 };
    SendMessage(hWndToolbar, TB_ADDBITMAP, 2, (LPARAM)&tbBitmap);

    TBBUTTON tbButtons[] = {
        { MAKELONG(0, 0), ID_GENERATOR_START, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Start" },
        { MAKELONG(1, 0), ID_GENERATOR_STOP, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Stop" },
    };

    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, ARRAYSIZE(tbButtons), (LPARAM)tbButtons);

    return hWndToolbar;
}
void create_status_bar(HWND hwnd) {
	h_status_bar = CreateWindowEx(
		0,                             
		STATUSCLASSNAME,              
		nullptr,                     
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 
		0, 0, 0, 0,                     
		hwnd,                          
		(HMENU)1,                     
		GetModuleHandle(NULL),       
		nullptr                     
	);

    SendMessage(h_status_bar, SB_SETTEXT, (WPARAM)0, (LPARAM)"ПИ-21а. Евтеев Дмитрий. Поточная линия");
}

void init_models() {
    generator.set_output_to(&g_ws1);
    
    g_ws1.set_output_to(&workstation_1);
    workstation_1.set_output_to(&ws1_ws2);

    ws1_ws2.set_output_to(&workstation_2);
    workstation_2.set_output_to(&storage);
}
void init_threads(HWND hwnd) {
    h_generator_thread = CreateThread(NULL, 0, [](LPVOID) -> DWORD {
        generator.run();
        return 0;
    }, nullptr, 0, nullptr);

    h_thread_g_ws1 = CreateThread(NULL, 0, [](LPVOID) -> DWORD {
        g_ws1.run();
        return 0;
    }, nullptr, 0, nullptr);


    h_thread_ws1_ws2 = CreateThread(NULL, 0, [](LPVOID) -> DWORD {
        ws1_ws2.run();
        return 0;
    }, nullptr, 0, nullptr);


    h_ws1_thread = CreateThread(NULL, 0, [](LPVOID) -> DWORD {
        workstation_1.run();
        return 0;
    }, nullptr, 0, nullptr);

    h_ws2_thread = CreateThread(NULL, 0, [](LPVOID) -> DWORD {
        workstation_2.run();
        return 0;
    }, nullptr, 0, nullptr);
}
void read_config() {
    sl_config.load();

    logger  << "===== CONFIG LOADED ======\n"
            << sl_config.list()
			<< "==========================" << std::endl;

    generator.set_rate(sl_config.generator_rate());
}
void init_handles(HWND hwnd) {
    int w = 200;
    int h = 20;

    int gapx = 5;

    int x = 10;
    int y = 40;

    contextmenu = new ContextMenu(700, hwnd);
    contextmenu->add_item(701, "Setup queue size");

    label_generator_state = new Label(hwnd, x, y + h * 0, w, h, "generator state", 1);
    label_generator_rate =  new Label(hwnd, x, y + h * 1, w, h, "generator rate", 2);
    pbar_g =                new ProgressBar(401, hwnd, x, y + h * 2, w, h);
    ttip_generator_progress = new Tooltip(hwnd, pbar_g->handle(), 301, "Progress of generator");
    generator.on_state_change.attach([&](GeneratorState state) {
        std::string str;
        switch (state)
        {
        case Work:   str = "work";    break;
        case Idle:   str = "idle";    break;
        case Stop:   str = "stop";    break;
        }
        label_generator_state->set_text(_S("generator state: " << str));
	});
    generator.on_rate_change.attach([&](int rate) {
        label_generator_rate->set_text(_S("generator rate: " << rate));
	});
    generator.on_progress_change.attach([&](int p) {
        int percent = ((double)p / (double)generator.rate()) * 100;
        pbar_g->set_position(percent);
	});

    new Label(hwnd, x, y + h * 4, w, h, "g->ws1 content: ", 3);
    lb_g_ws1 = new ListBox(hwnd, x, y + h * 5, w, h * 8, 501);
    g_ws1.on_recv.attach([&](Item* item) {
        lb_g_ws1->add_item(_S(item));
	});
    g_ws1.on_push.attach([&](Item* item) {
        lb_g_ws1->pop_item();
	});

    new Label(hwnd, x + (gapx+w), y + h * 4, w, h, "ws1->ws2 content: ", 4);
    lb_ws1_ws2 = new ListBox(hwnd, x + (gapx + w), y + h * 5, w, h * 8, 502);
    ws1_ws2.on_recv.attach([&](Item* item) {
        lb_ws1_ws2->add_item(_S(item));
	});
    ws1_ws2.on_push.attach([&](Item* item) {
        lb_ws1_ws2->pop_item();
	});
    
    label_sl_g_ws1_length =     new Label(hwnd, x + gapx + w, y + h * 0, w, h, "g_ws1 len", 4);
    label_sl_ws1_ws2_length =   new Label(hwnd, x + gapx + w, y + h * 1, w, h, "ws1_ws2 len", 5);
    label_sl_total_length =     new Label(hwnd, x + gapx + w, y + h * 2, w, h, "ws1_ws2 len", 6);
    g_ws1.on_change.attach([&](Queue<Item*>& q) {
        label_sl_g_ws1_length->set_text(_S("g->ws1: " << q.len() << "/" << q.cap()));
        label_sl_total_length->set_text(_S("total length: " << g_ws1.queue().len() + ws1_ws2.queue().len()));
    });
    ws1_ws2.on_change.attach([&](Queue<Item*>& q) {
        label_sl_ws1_ws2_length->set_text(_S("ws1->ws2: " << q.len() << "/" << q.cap()));
        label_sl_total_length->set_text(_S("total length: " << g_ws1.queue().len() + ws1_ws2.queue().len()));
    });

    
    label_ws1_state =   new Label( hwnd, x + (gapx + w) * 2, y + h * 0, w, h, "ws1 state", 7);
    label_ws1_rt =      new Label( hwnd, x + (gapx + w) * 2, y + h * 1, w, h, "ws1 rt", 8);
    pbar_ws1 =          new ProgressBar(301, hwnd, x + (gapx + w) * 2, y + h * 2, w, h);
    checkbox_ws1_state = new CheckBox(601, hwnd, x + (gapx + w) * 2, y + h * 3, 10, h);
    checkbox_ws1_state->set_checked(true);
    checkbox_ws1_state->on_check.attach([&](bool val) {
        if (val == true) {
            workstation_1.start();
        }
        else {
            workstation_1.stop();
        }
	});
    workstation_1.on_state_change.attach([&](WorkstationState state) {
        std::string str;
        if (state == WS_Process) {
            str = "process";
        }
        else if (state == WS_Idle) {
            str = "idle";
        }
        else if (state == WS_Stop) {
            str = "stop";
        }

        label_ws1_state->set_text(_S("workstation#" << workstation_1.id() << " state: " << str));
	});
    workstation_1.on_progress.attach([&](WorkstationProgress p) {
        std::string msg;
        switch (p.state) {
        case PS_Empty: msg = _S("ws1 is empty"); break;
        case PS_Processing: msg = _S("item#" << p.item_id << "(" << p.tick_elapsed << "/" << p.tick_estimate << ")"); break;
        case PS_Ready: msg = _S("item#" << p.item_id << " ready to send"); break;
        case PS_Gone: msg = _S("item#" << p.item_id << " sent"); break;
        }

        int completed_percent = 0;
        if (p.state == PS_Processing) {
            completed_percent = ((double)p.tick_elapsed / (double)p.tick_estimate) * 100;
        }
        else if (p.state == PS_Ready) {
            completed_percent = 100;
        }

        pbar_ws1->set_position(completed_percent);
        label_ws1_rt->set_text(msg);
	});

    label_ws2_state =   new Label( hwnd, x + (gapx + w) * 3, y + h * 0, w, h, "ws2 state", 9);
    label_ws2_rt =      new Label( hwnd, x + (gapx + w) * 3, y + h * 1, w, h, "ws2 rt", 10);
    pbar_ws2 =          new ProgressBar(302, hwnd, x + (gapx + w) * 3, y + h * 2, w, h);
    checkbox_ws2_state = new CheckBox(602, hwnd, x + (gapx + w) * 3, y + h * 3, 10, h);
    checkbox_ws2_state->set_checked(true);
    checkbox_ws2_state->on_check.attach([&](bool val) {
        if (val == true) {
            workstation_2.start();
        }
        else {
            workstation_2.stop();
        }
	});
    workstation_2.on_state_change.attach([&](WorkstationState state) {
        std::string str;
		if (state == WS_Process) {
            str = "process";
        }
        else if (state == WS_Idle) {
            str = "idle";
        }
        else if (state == WS_Stop) {
            str = "stop";
        }
            

        label_ws2_state->set_text(_S("workstation#" << workstation_2.id() << " state: " << str));
	});

    workstation_2.on_progress.attach([&](WorkstationProgress p) {
        std::string msg;
        switch (p.state) {
        case PS_Empty: msg = _S("ws1 is empty"); break;
        case PS_Processing: msg = _S("item#" << p.item_id << "(" << p.tick_elapsed << "/" << p.tick_estimate << ")"); break;
        case PS_Ready: msg = _S("item#" << p.item_id << " ready to send"); break;
        case PS_Gone: msg = _S("item#" << p.item_id << " sent"); break;
        }

        int completed_percent = 0;
        if (p.state == PS_Processing) {
            completed_percent = ((double)p.tick_elapsed / (double)p.tick_estimate) * 100;
        }
        else if (p.state == PS_Ready) {
            completed_percent = 100;
        }

        pbar_ws2->set_position(completed_percent);
        label_ws2_rt->set_text(msg);
	});

    label_storage_stored =      new Label( hwnd, x + (gapx + w) * 4, y + h * 0, w, h, "stored", 11);
    storage.on_stored_change.attach([&](int stored) {
        label_storage_stored->set_text(_S("stored: " << stored));
	});


    cbox_ws_stat_picker = new ComboBox(hwnd, x + (gapx + w) * 3, y + h * 5, w, h * 3, IDR_WS_STAT_COMBOBOX);
	cbox_ws_stat_picker->add_item(std::to_string(workstation_1.id()));
	cbox_ws_stat_picker->add_item(std::to_string(workstation_2.id()));
    cbox_ws_stat_picker->on_change.attach(update_ws_stat);

    h_button_reset_stat = CreateWindow(
        "BUTTON",
        "reset stat",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x + (gapx + w) * 4, y + h * 5, w, h,
        hwnd,
        (HMENU)IDB_RESET_STAT,
        hInst,
        NULL
    ); 

    label_ws_stat_title = new Label( hwnd, x + (gapx + w) * 3, y + (h * 6), w * 2, h, "Select workstaion for stat", 12);
    input_ws_stat = new InputBox(hwnd, x + (gapx + w) * 3, y + (h * 7), w * 2, h * 3, IDR_INPUT_WS_STAT);

    ttip_reset_stat = new Tooltip(hwnd, h_button_reset_stat, 401, "Reset statistics");
    ttip_ws1_progress_tip = new Tooltip(hwnd, pbar_ws1->handle(), 402, "Progress of workstation#0");
    ttip_ws2_progress_tip = new Tooltip(hwnd, pbar_ws2->handle(), 403, "Progress of workstation#1");

    generator.set_rate(sl_config.generator_rate());
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    const char CLASS_NAME[] = "ConveyorApp";

    INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_BAR_CLASSES; 
	InitCommonControlsEx(&icex);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ConveyorApp";

    RegisterClassW(&wc);

    HMENU h_menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU));
    

    HWND hWnd = CreateWindowEx(0, CLASS_NAME, "Производственная поточная линия",
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1200, 400,
                                nullptr, h_menu, hInstance, nullptr);

    if (!hWnd) return -1;

    ShowWindow(hWnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

INT_PTR CALLBACK DialogProc(HWND h, UINT u, WPARAM wp, LPARAM lp) {
	static Streamline* target = nullptr;

    switch (u)
    {
    case WM_INITDIALOG:
        if (queue_to_setup == 0) {
            CheckRadioButton(h, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
        }
        else if (queue_to_setup == 1) {
            CheckRadioButton(h, IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
        }
        return TRUE;
    case WM_COMMAND: 
        switch (LOWORD(wp)) {
        case IDOK:
            EndDialog(h, 0);

            if (IsDlgButtonChecked(h, IDC_RADIO1)) {
                target = &g_ws1;
            }
            else if (IsDlgButtonChecked(h, IDC_RADIO2)) {
                target = &ws1_ws2;
            }

            if (target) {
				char buffer[256];
				GetDlgItemText(h, IDC_EDIT1, buffer, 256);
				new_queue_size = std::atoi(buffer);

				target->set_cap(new_queue_size);
            }

			EndDialog(h, 0);
			return TRUE;

        case IDCANCEL:
			EndDialog(h, 0);
			return TRUE;
        }
		break;

    case WM_CLOSE: 
        EndDialog(h, 0);
		return TRUE;
    }

    return FALSE;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        create_toolbar(hWnd);
        create_status_bar(hWnd);
        init_models();
        init_handles(hWnd);
        init_threads(hWnd);
        read_config();
        break;

    case WM_CONTEXTMENU:
        contextmenu->enable(hWnd);
        break;

    case WM_COMMAND: {
        auto p = LOWORD(wParam);
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            int cb_id = p;
            if (cb_id == cbox_ws_stat_picker->id()) {
                cbox_ws_stat_picker->set_selected_index(cbox_ws_stat_picker->get_selected_index());
            }
        }
        else {
			if (p == ID_OPEN_LOG) {
				STARTUPINFOA si = { sizeof(si) };
				PROCESS_INFORMATION pi;
				char cmd[256];

				std::sprintf(cmd, "notepad.exe %s", "log.txt");

				CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			}
			else if (p == ID_EXIT) {
				DestroyWindow(hWnd);
			}
			else if (p == ID_GENERATOR_START) {
				generator.start();
			}
			else if (p == ID_GENERATOR_STOP) {
				generator.stop();
			}
			else if (p == ID_OPEN_CONFIG_FILE) {
				STARTUPINFOA si = { sizeof(si) };
				PROCESS_INFORMATION pi;
				char cmd[256];

				std::sprintf(cmd, "notepad.exe %s", "config.txt");
				CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			}
			else if (p == ID_READ_CONFIG) {
				read_config();
            }
            else if (p == IDB_RESET_STAT) {
                auto s = cbox_ws_stat_picker->get_selected_item();

                try {
					int i = std::stoi(s);
					if(-1 != i) {
						if (i == 0) {
							g_ws1.reset_stat();
						}
						else if (i == 1) {
							ws1_ws2.reset_stat();
						}
					}
                } catch(...){}
            }
            else if (p == 701) {
                DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc);
            }
            else if (p == 601) {
                checkbox_ws1_state->set_checked(checkbox_ws1_state->is_checked());
            }
            else if (p == 602) {
                checkbox_ws2_state->set_checked(checkbox_ws2_state->is_checked());
            }
        }
        break;
    }

    case WM_SIZE: {
        SendMessage(h_status_bar, WM_SIZE, 0, 0); 
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

