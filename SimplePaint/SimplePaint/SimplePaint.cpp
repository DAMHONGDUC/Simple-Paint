#include "framework.h"
#include "SimplePaint.h"
#include <WindowsX.h>
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <commdlg.h>
#include <vector>
#include "Shape.h"
#include <fstream>
#include <string>
#include <sstream>
#include "ShapeFactory.h"
#include "Tokenizer.h"

#define IMAGE_WIDTH     18
#define IMAGE_HEIGHT    18
#define BUTTON_WIDTH    0
#define BUTTON_HEIGHT   0
#define MAX_LOADSTRING 100    

// khai báo hàm 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL                OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void                OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void                OnClose(HWND hwnd);
void                OnDestroy(HWND hwnd);
void                OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void                OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
void                OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
void                OnPaint(HWND hwnd);
void                OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void                OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

void                saveToFile(string filename);
void                openFromfile(string filename);
void                openFileDialog();
void                saveFileDialog();
void                SetTitle();
int                 askFor_Save();
void                colorDialog(HWND hwnd);

// khai báo biến toàn cục
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

CHOOSECOLOR  cc; // Thông tin màu chọn
COLORREF  acrCustClr[16]; // Mảng custom color
HBRUSH  hbrush; // Tạo ra brush từ màu đã chọn
HDC hdc;

bool isLDown = false;
bool isRDown = false;
bool isSave = false;
bool isModify = false;
bool isSelect = false;
bool isCut = false;
bool isCopy = false;
bool isPaste = true;

PPoint p1, p2, g1;
PPoint center, A1, B1;

vector<shared_ptr<IShape>> redo ;
vector<shared_ptr<IShape>> shapes;

int currPenSize = 1;
int currPenStyle = PS_SOLID;
ShapeMode currShapeMode = LINE;
COLORREF currColor = RGB(255, 0, 0);
shared_ptr<IShape> currShape = NULL;

int selectIndex;
COLORREF selectColor;
shared_ptr<IShape> selectShape = NULL;

int window_x = 220;
int window_y = 40;
int window_Width = 1100;
int window_Height = 800;

string defaultFilePath = "Untitled";
string currFilePath = "Untitled";

HWND hStatusBar;
HPEN hPen = CreatePen(currPenStyle, currPenSize, currColor);
PAINTSTRUCT ps;
HWND g_hWnd;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLEPAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEPAINT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEPAINT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEPAINT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        window_x, window_y, window_Width, window_Height, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
        HANDLE_MSG(hWnd, WM_RBUTTONDOWN, OnRButtonDown);
        HANDLE_MSG(hWnd, WM_RBUTTONUP, OnRButtonUp);
        HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
        HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    g_hWnd = hwnd;

    // Lấy font hệ thống
    LOGFONT lf;
    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
        lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
        lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);

    hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, L"", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, window_Height, window_Width, 20, hwnd, (HMENU)IDC_STATUS_BAR, hInst, NULL);
    SendMessage(hStatusBar, WM_SETFONT, WPARAM(hFont), TRUE);

    InitCommonControls();
    TBBUTTON tbButtons[] =
    {
        { STD_FILENEW,	ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_FILEOPEN,	ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_FILESAVE,	ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
        { STD_UNDO,	IDC_UNDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_REDOW, IDC_REDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
 
    HWND hToolBarWnd = CreateToolbarEx(hwnd,
        WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS,
        ID_TOOLBAR, sizeof(tbButtons) / sizeof(TBBUTTON), HINST_COMMCTRL,
        0, tbButtons, sizeof(tbButtons) / sizeof(TBBUTTON),
        BUTTON_WIDTH, BUTTON_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT,
        sizeof(TBBUTTON));

    TBBUTTON buttonsToAdd[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 }, // Nút phân cách
        { STD_CUT,	ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_COPY,	ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_PASTE, ID_EDIT_PASTE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_DELETE, ID_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(buttonsToAdd) / sizeof(TBBUTTON),
        (LPARAM)(LPTBBUTTON)&buttonsToAdd);

    // add nút clear all
    TBBUTTON userButtons1[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
        { 0, IDC_BTNCLEAR,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
      
    };

    TBADDBITMAP	tbBitmap1 = { hInst, IDB_BITMAP3 };

    int idx1 = SendMessage(hToolBarWnd, TB_ADDBITMAP, (WPARAM)sizeof(tbBitmap1) / sizeof(TBADDBITMAP),
        (LPARAM)(LPTBADDBITMAP)&tbBitmap1);

    userButtons1[1].iBitmap += idx1;

    SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(userButtons1) / sizeof(TBBUTTON),
        (LPARAM)(LPTBBUTTON)&userButtons1);


    // add nút ELLIPSE, RECTANGLE, LINE
    TBBUTTON userButtons[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
        { 0, ID_DRAW_ELLIPSE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 1, ID_DRAW_RECTANGLE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 2, ID_DRAW_LINE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
    };
    
    TBADDBITMAP	tbBitmap = { hInst, IDB_BITMAP1 };

    int idx = SendMessage(hToolBarWnd, TB_ADDBITMAP, (WPARAM)sizeof(tbBitmap) / sizeof(TBADDBITMAP),
        (LPARAM)(LPTBADDBITMAP)&tbBitmap);

    userButtons[1].iBitmap += idx;
    userButtons[2].iBitmap += idx;
    userButtons[3].iBitmap += idx;
    
    SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(userButtons) / sizeof(TBBUTTON),
        (LPARAM)(LPTBBUTTON)&userButtons);

    SetTitle();

    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDM_ABOUT:
    {
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
        break;
    }
    case IDC_BTNCLEAR:
    {
        currShape = NULL;
        selectShape = NULL;
        redo.clear();
        shapes.clear();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case IDC_UNDO:
    {
        if (shapes.size() >= 1)
        {         
            redo.push_back(shapes[shapes.size() - 1]);
            shapes.pop_back();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    }
    case IDC_REDO:
    {
        if (redo.size() >= 1)
        {       
            shapes.push_back(redo[redo.size() - 1]);
            redo.pop_back();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    }
    case ID_FILE_NEW:
    {                 
        //TH1: đang ở trong màn hình new 
        //* khi ấn new.nếu màn hình không có hình = > thì tạo màn hình new mới
        //* khi ấn new.nếu màn hình có hình và chưa lưu = > hỏi users có lưu k ? 
        //      sau đó tạo màn hình new mới
        //*khi ấn new.nếu màn hình có hình và đã lưu = > thì tạo màn hình new mới
        if (currFilePath == defaultFilePath)
        {
            if (shapes.size() > 0 && !isSave)
            {             
                if (askFor_Save() == IDOK)
                    saveFileDialog();
            }
        }
        //TH2: nếu ở trong 1 file đã open
        //* khi ấn new.nếu màn hình chưa chỉnh sửa gì = > thì tạo màn hình new mới
        //* khi ấn new.nếu màn hình đã chỉnh sửa và chưa lưu = > hỏi users có lưu k ? sau đó
        //      tạo màn hình new mới
        //*khi ấn new.nếu màn hình đã chỉnh sửa và đã lưu = > thì tạo màn hình new mới
        else
        {
            if (isModify && !isSave)
            {
                if (askFor_Save() == IDOK)
                    saveFileDialog();
            }
        }
        currFilePath = defaultFilePath;
        SetTitle();
        shapes.clear();
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    
    case ID_FILE_OPEN:
    {
        //TH1 : ở màn hình new.
        //*ta muốn open file, màn hình không có gì = > thì open
        //*ta ấn open file, màn hình có hình nhưng chưa lưu = > hỏi users có lưu k sau đó open file
        //ta ấn open file, màn hình có hình nhưng đã lưu = > thì open
        if (currFilePath == defaultFilePath)
        {
            if (shapes.size() > 0 && !isSave)
            {
                if (askFor_Save() == IDOK)
                    saveFileDialog();
            }
        }
        // TH2 : ở màn hình vừa open file A
        //*ta chưa chỉnh sửa gì với file A mà muốn open file B => open
        //*ta đã chỉnh sửa với file A nhưng chưa lưu => hỏi users có lưu k sau đó open
        //*đã chỉnh sửa và cũng đã lưu mà muốn open file B = > open
        else
        {
            if (isModify && !isSave)
            {
                if (askFor_Save() == IDOK)
                    saveFileDialog();
            }
        }
        openFileDialog();
        SetTitle();
        InvalidateRect(g_hWnd, NULL, TRUE);      
        break;
    }
    case ID_FILE_SAVE:
    {    
        saveFileDialog();
        SetTitle();
        InvalidateRect(g_hWnd, NULL, TRUE);
        break;
    }
    case ID_EDIT_CUT:
    {
        if (isSelect)
        {
            shapes.erase(shapes.begin() + selectIndex);
            InvalidateRect(g_hWnd, NULL, TRUE);
            isCut = true;
        }
        break;
    }
    case ID_EDIT_COPY:
    {       
        if (isSelect)
        {
            isCopy = true;
        }
        break;
    }
    case ID_EDIT_PASTE:
    {
        isPaste = false;
        break;
    }
    case ID_EDIT_DELETE:
    {
        if (isSelect)
        {
            shapes.erase(shapes.begin() + selectIndex);
            isSelect = false;         
            InvalidateRect(g_hWnd, NULL, TRUE);
        }
        break;
    }
    case ID_EXIT:
    {
        //MessageBox(0, L"Hello", 0, 0);
        //TH1: đang ở màn hình new 
        //*nếu có hình và chưa lưu => hỏi users có lưu không sau đó thoát
        //*nếu có hình và đã lưu => thoát 
        //*nếu không có hình => thoát
        if (currFilePath == defaultFilePath)
        {
            if (shapes.size() > 0 && !isSave)
            {
                if (askFor_Save() == IDOK)
                    saveFileDialog();
            }
        }
        //TH2: nếu ở trong 1 file đã open
        //*nếu có chỉnh sửa và chưa lưu => hỏi users có lưu không sau đó thoát
        //*nếu có chỉnh sửa và đã lưu => thoát
        //*nếu không chỉnh sửa => thoát
        else
        {
            if (isModify && !isSave)
            {
                if (askFor_Save()== IDOK)
                    saveFileDialog();
            }
        }
        DestroyWindow(g_hWnd);
        break;
    }
    case ID_COLOR:
    {
        colorDialog(hwnd);
        break;
    }
    case ID_DRAW_ELLIPSE:
    {
        currShapeMode = ELLIPSE;
        break;
    }
    case ID_DRAW_RECTANGLE:
    {
        currShapeMode = RECTANGLE;
        break;
    }
    case ID_DRAW_LINE:
    {
        currShapeMode = LINE;
        break;
    }
    case ID_PENSTYLE_SOLID:
    {
        currPenSize = 1;
        currPenStyle = PS_SOLID;
        break;
    }
    case ID_PENSTYLE_DASH:
    {
        currPenSize = 1;
        currPenStyle = PS_DASH;
        break;
    }
    case ID_PENSTYLE_DOT:
    {
        currPenSize = 1;
        currPenStyle = PS_DOT;
        break;
    }
    case ID_PENSTYLE_PS_DASHDOT:
    {
        currPenSize = 1;
        currPenStyle = PS_DASHDOT;
        break;
    }
    case ID_PENSIZE_add:
    {
        currPenSize += 2;
        break;
    }
    case ID_PENSIZE_sub:
    {
        currPenSize -= 2;
        if (currPenSize < 1) currPenSize = 1;
        break;
    }
    case ID_PENSIZE_1 :
    {
        currPenSize = 1;
        break;
    }
    case ID_PENSIZE_2:
    {
        currPenSize = 2;
        break;
    }
    case ID_PENSIZE_3:
    {
        currPenSize = 3;
        break;
    }
    case ID_PENSIZE_4:
    {
        currPenSize = 4;
        break;
    }
    case ID_PENSIZE_5:
    {
        currPenSize = 5;
        break;
    }
    }
}

void OnClose(HWND hwnd)
{
    if (currFilePath == defaultFilePath)
    {
        if (shapes.size() > 0 && !isSave)
        {
            if (askFor_Save() == IDOK)
                saveFileDialog();
        }
    }
    else
    {
        if (isModify && !isSave)
        {
            if (askFor_Save() == IDOK)
                saveFileDialog();
        }
    }
    PostQuitMessage(0);
}

void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (!isLDown)
    {
        p1.set_x(x);
        p1.set_y(y);   
        isLDown = true;
    }
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    isLDown = false;
    p2.set_x(x);
    p2.set_y(y);
  
    if (!isPaste && selectShape != NULL)
    {
        int k1 = p1.get_x() - center.get_x();
        int k2 = p1.get_y() - center.get_y();

        shapes.push_back(ShapeFactory::Instance()->Create(selectShape->getType(), PPoint(A1.get_x() + k1, A1.get_y() + k2), PPoint(B1.get_x() + k1, B1.get_y() + k2), selectShape->getColor(), selectShape->getPenStyle(), selectShape->getPenSize()));
       
        isPaste = true;
        isCut = false;
        isSelect = false;

        isModify = true;
        isSave = false;

        // tắt trạng thái select
        shapes[selectIndex]->setPenSize(selectShape->getPenSize());
        shapes[selectIndex]->setColor(selectShape->getColor());
        shapes[selectIndex]->setPenStyle(selectShape->getPenStyle());    
    }
    else shapes.push_back(ShapeFactory::Instance()->Create(currShapeMode, p1, p2, currColor, currPenStyle, currPenSize));
    
    InvalidateRect(hwnd, NULL, TRUE);
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    //Status bar
    WCHAR buffer[128];
    wsprintf(buffer, L"%d, %dpx", x, y);
    SetWindowText(hStatusBar, buffer);

    if (isLDown)
    {
        p2.set_x(x);
        p2.set_y(y);
        InvalidateRect(hwnd, NULL, TRUE); 
    }
}

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    hdc = BeginPaint(hwnd, &ps);
   
    for (int i = 0; i < shapes.size(); i++)
    {     
        hPen = CreatePen(shapes[i]->getPenStyle(), shapes[i]->getPenSize(), shapes[i]->getColor());
        SelectObject(hdc, hPen);       
        shapes[i]->Draw(hdc);  
    }

    if (isLDown)
    {
        isModify = true;
        isSave = false;
        hPen = CreatePen(currPenStyle, currPenSize, currColor);
        SelectObject(hdc, hPen);
              
        switch (currShapeMode)
        {
        case LINE:
        {
            shared_ptr<LLine> a(new LLine(p2, p1, currColor, currPenStyle, currPenSize));
            currShape = a;                   
            break;
        }
        case RECTANGLE:
        {
            shared_ptr<RRectangle> a(new RRectangle(p2, p1, currColor, currPenStyle, currPenSize));
            currShape = a;
            break;
        }
        case ELLIPSE:
        {
            shared_ptr<EEllipse> a(new EEllipse(p2, p1, currColor, currPenStyle, currPenSize));
            currShape = a;
            break;
        }
        }     
        currShape->Draw(hdc);
    }
    
    EndPaint(hwnd, &ps);
}

void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (!isRDown)
    {
        g1.set_x(x);
        g1.set_y(y);
        isRDown = true;
    }
}

void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    isRDown = false;
    if (!isSelect)
    {
        for (int i = 0; i < shapes.size(); i++)
            if (shapes[i]->Is_in(g1))
            {
                selectIndex = i;
                isSelect = true;

                switch (shapes[i]->getType())
                {
                case LINE:
                {
                    shared_ptr<LLine> a(new LLine(shapes[i]->get_A(), shapes[i]->get_B(), shapes[i]->getColor(), shapes[i]->getPenStyle(), shapes[i]->getPenSize()));
                    selectShape = a;
                    break;
                }
                case RECTANGLE:
                {
                    shared_ptr<RRectangle> a(new RRectangle(shapes[i]->get_A(), shapes[i]->get_B(), shapes[i]->getColor(), shapes[i]->getPenStyle(), shapes[i]->getPenSize()));
                    selectShape = a;
                    break;
                }
                case ELLIPSE:
                {
                    shared_ptr<EEllipse> a(new EEllipse(shapes[i]->get_A(), shapes[i]->get_B(), shapes[i]->getColor(), shapes[i]->getPenStyle(), shapes[i]->getPenSize()));
                    selectShape = a;
                    break;
                }
                }

                A1.copy(shapes[i]->get_A());
                B1.copy(shapes[i]->get_B());
                PPoint C((A1.get_x() + B1.get_x()) / 2, (A1.get_y() + B1.get_y()) / 2);
                center.copy(C);

                // bật trạng thái select
                shapes[i]->setPenSize(1);
                shapes[i]->setColor(RGB(128, 64, 0));
                shapes[i]->setPenStyle(PS_DASHDOTDOT);
                InvalidateRect(hwnd, NULL, TRUE);
            }
    }
    else if (isSelect)
    {
        // tắt trạng thái select
        shapes[selectIndex]->setPenSize(selectShape->getPenSize());
        shapes[selectIndex]->setColor(selectShape->getColor());
        shapes[selectIndex]->setPenStyle(selectShape->getPenStyle());
        InvalidateRect(hwnd, NULL, TRUE);

        isSelect = false;
        selectIndex = 0;
        selectShape = NULL;
    }
}

void saveToFile(string filePath)
{
    int m = shapes.size();

    ofstream os(filePath, ios::binary | ios::out);
    if (os.is_open())
    {
        os << m << endl;

        for (int i = 0; i < m; i++)
        {
            os << shapes[i]->getType() << endl;
            os << shapes[i]->getPenStyle() << endl;
            os << shapes[i]->getPenSize() << endl;
            os << shapes[i]->getColor() << endl;
            os << shapes[i]->toString() << endl;
        }
        os.close();
    }
}

void openFromfile(string filePath)
{
    int m = 0;
    int type;
    int penStyle;
    int penSize;
    COLORREF color;
    string s;
    shapes.clear();

    ifstream is(filePath, ios::binary | ios::in);
    if (is.is_open())
    {
        is >> m;

        for (int i = 0; i < m; i++)
        {
            is >> type;
            is >> penStyle;
            is >> penSize;
            is >> color;
            is.ignore();
            getline(is, s);
            
            vector<string> tokens = Tokenizer::split(s, " ");
            shapes.push_back(ShapeFactory::Instance()->getShape(type, tokens[0], tokens[1], color, penStyle, penSize));
        }
        is.close();      
    }
   
}

void openFileDialog()
{
    OPENFILENAME ofn;
    WCHAR szFilePath[MAX_PATH] = L"";
    WCHAR szFileTitle[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); 
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFilter = L"Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFilePath;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"bin";
   
    if (GetOpenFileName(&ofn))
    {
        wstring ws(szFilePath);
        string filePath(ws.begin(), ws.end());
        currFilePath = filePath;
        openFromfile(filePath);
        isModify = false;
        isSave = false;
        InvalidateRect(g_hWnd, NULL, TRUE);
    }
}

void saveFileDialog()
{
    //nếu đã lưu trước đó thì không cần hiệp hộp thoại lưu
    if (currFilePath != defaultFilePath)
    {
        saveToFile(currFilePath);
    }
    //nếu chưa lưu trước đó thì cần hiện hộp thoại lưu cho users đặt tên
    else
    {
        OPENFILENAME ofn;
        WCHAR szFilePath[MAX_PATH] = L" .bin";

        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = g_hWnd;
        ofn.lpstrFilter = L"Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = szFilePath;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"bin";

        if (GetSaveFileName(&ofn))
        {
            wstring ws(szFilePath);
            string filePath(ws.begin(), ws.end());
            currFilePath = filePath;
            saveToFile(filePath);
        }
    }
    isSave = true;
}

void SetTitle()
{
    string s = "19120481 - SimplePaint - [" + currFilePath + "]";
    wstring stemp = wstring(s.begin(), s.end());
    LPCWSTR sw = stemp.c_str();
    SetWindowText(g_hWnd, sw);
}

int askFor_Save()
{
    wstringstream ws;
    wstring wsTmp(currFilePath.begin(), currFilePath.end());
    ws << "Do you want to save changes to " << wsTmp << "?";
    int result = MessageBox(g_hWnd, ws.str().c_str(), L"Paint", MB_OKCANCEL | MB_ICONQUESTION);
    return result;
}

void colorDialog(HWND hwnd)
{
    ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwnd;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = currColor;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColor(&cc))
    {
        hbrush = CreateSolidBrush(cc.rgbResult);
        currColor = cc.rgbResult;
    }
}

