#ifndef APP_H
#define APP_H

#include <wx/app.h>
#include <wx/wx.h>
#include <optional>
#include "../../DiComposition/composition_root.h"

// Boost.DI's injector is a lambda type (internal linkage). GCC warns when
// that type is a field of App (external linkage). The injector must stay
// visible here so UI files can call create<T>().

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"
#endif

class App : public wxApp {
	public:
		int SCREEN_WIDTH;
		int SCREEN_HEIGHT;
		int windowWidth;
		int windowHeight;
	
		bool OnInit();
		int OnExit();
		
		auto& GetInjector() {
			this->EnsureInjector();
			return *this->injector;
		}
		
		int GetScreenWidth() const;
		int GetScreenHeight() const;
		int GetWindowWidth();
		int GetWindowHeight();
		
		void SetWindowWidth(int width);
		void SetWindowHeight(int height);
		
		void Print() const;

	private:
		void EnsureInjector();
		std::optional<decltype(MakeInjector())> injector;
};

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

wxDECLARE_APP(App);

#endif
