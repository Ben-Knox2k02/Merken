#include <wx/wx.h>
#include <optional>
#include "../../Composition/composition_root.h"

class App : public wxApp {
	public:
		bool OnInit() override;
		int OnExit() override;
		auto& GetInjector() {
			this->EnsureInjector();
			return *this->injector;
		}

	private:
		void EnsureInjector();
		std::optional<decltype(MakeInjector())> injector;
};
