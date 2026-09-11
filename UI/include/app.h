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

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
