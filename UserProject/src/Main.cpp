#include "include/Main.h"

HelloPolycodeApp::HelloPolycodeApp(PolycodeView *view) {

	core = new POLYCODE_CORE(view, 640,480,false,false,0,0,90);

	CoreServices::getInstance()->getResourceManager()->addArchive("Resources/default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);	
	
	Screen *screen = new Screen();
		
	ScreenShape *shape = new ScreenShape(ScreenShape::SHAPE_RECT, 100, 100);
	shape->setColor(1.0, 0.0, 0.0, 1.0);
	shape->setPosition(150,240);
	screen->addChild(shape);
}

HelloPolycodeApp::~HelloPolycodeApp() {
}

bool HelloPolycodeApp::Update() {
    return core->updateAndRender();
}

#ifdef _WINDOWS
#include "windows.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Polycode Example");
	HelloPolycodeApp *app = new HelloPolycodeApp(view);

	MSG Msg;
	do {
		if(PeekMessage(&Msg, NULL, 0,0,PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	} while(app->Update());
	return Msg.wParam;
}
#else
int main(int argc, char *argv[]) {
	PolycodeView *view = new PolycodeView("Hello Polycode!");
	HelloPolycodeApp *app = new HelloPolycodeApp(view);
	while(app->Update()) {}
	return 0;
}
#endif