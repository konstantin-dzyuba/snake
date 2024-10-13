#undef UNICODE
#pragma comment(lib, "d2d1.lib")

#include <Windows.h>
#include <D2D1.h>
#include <vector>
#include <ctime>

using namespace D2D1;

ID2D1Factory* factory;
ID2D1HwndRenderTarget* renderTarget;
ID2D1SolidColorBrush* greenBrush;
ID2D1SolidColorBrush* redBrush;

const int WIDTH = 640;
const int HEIGHT = 480;
const int TILE_SIZE = 32;

int size = 3;
std::vector<POINT> snake = { POINT{2, 0}, POINT{1, 0}, POINT{0, 0} };
char direction = 'D';
POINT food;

void move() {
	POINT tail = snake[snake.size() - 1];

	for (int i = snake.size() - 1; i > 0; i--)
	{
		snake[i] = snake[i - 1];
	}
	
	snake[0] = snake[1];

	switch (direction) {
	case 'W':
		snake[0].y--;
		break;
	case 'A':
		snake[0].x--;
		break;
	case 'S':
		snake[0].y++;
		break;
	case 'D':
		snake[0].x++;
		break;
	}

	for (int i = 0; i < snake.size(); i++) {
		if (snake[i].x == food.x && snake[i].y == food.y) {
			snake.push_back(tail);
			food = { rand() % (WIDTH / TILE_SIZE), rand() % (HEIGHT / TILE_SIZE) };
		}

		for (int j = i + 1; j < snake.size(); j++)
		{
			if (snake[i].x == snake[j].x && snake[i].y == snake[j].y) {
				PostQuitMessage(0);
			}
		}
	}

	if (snake[0].x < 0 || snake[0].y < 0 ||
		(snake[0].x + 1) * TILE_SIZE > WIDTH || (snake[0].y + 1) * TILE_SIZE > HEIGHT) {
		PostQuitMessage(0);
	}
}

void draw() {
	renderTarget->BeginDraw();
	renderTarget->Clear(ColorF(ColorF::SaddleBrown));
	renderTarget->FillRectangle(RectF(food.x * TILE_SIZE, food.y * TILE_SIZE, food.x * TILE_SIZE + TILE_SIZE, food.y * TILE_SIZE + TILE_SIZE), redBrush);

	for (int i = 0; i < snake.size(); i++) {
		POINT point = snake.at(i);
		renderTarget->FillRectangle(RectF(point.x * TILE_SIZE, point.y * TILE_SIZE, point.x * TILE_SIZE + TILE_SIZE, point.y * TILE_SIZE + TILE_SIZE), greenBrush);
	}

	renderTarget->EndDraw();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		draw();
		break;
	case WM_KEYDOWN:
		if (wParam == 'D' && direction != 'A' ||
			wParam == 'A' && direction != 'D' ||
			wParam == 'W' && direction != 'S' ||
			wParam == 'S' && direction != 'W') {
			direction = wParam;
		}
		break;
	case WM_TIMER:
		move();
		InvalidateRect(hwnd, nullptr, false);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	srand(time(nullptr));
	food = { rand() % (WIDTH / TILE_SIZE), rand() % (HEIGHT / TILE_SIZE) };
	RECT client = { 0, 0, WIDTH, HEIGHT };
	RECT window = client;
	AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, false);
	window.right -= window.left;
	window.bottom -= window.top;
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Snake";
	RegisterClass(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window.right, window.bottom, nullptr, nullptr, hInstance, nullptr);
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(client.right, client.bottom)), &renderTarget);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGreen), &greenBrush);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &redBrush);
	SetTimer(hwnd, 1, 500, nullptr);
	ShowWindow(hwnd, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	redBrush->Release();
	greenBrush->Release();
	renderTarget->Release();
	factory->Release();
	return 0;
}
