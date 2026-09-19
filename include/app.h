#ifndef APP_H
#define APP_H

#include <wx/app.h>
#include <wx/wx.h>

class App : public wxApp {
	public:
		int SCREEN_WIDTH;
		int SCREEN_HEIGHT;
		int windowWidth;
		int windowHeight;
		
		bool OnInit();
		
		int GetScreenWidth() const;
		int GetScreenHeight() const;
		int GetWindowWidth();
		int GetWindowHeight();
		
		void SetWindowWidth(int width);
		void SetWindowHeight(int height);
		
		void Print() const;
};

wxDECLARE_APP(App);

#endif