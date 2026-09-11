#include "app.h"
#include "mainframe.h"

void App::EnsureInjector() {
	if (!this->injector.has_value()) {
		this->injector.emplace(MakeInjector());
	}
}

bool App::OnInit() {
	this->EnsureInjector();

	MainFrame* mainFrame = new MainFrame("Merken");
	mainFrame->SetClientSize(1000, 800);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}

int App::OnExit() {
	this->injector.reset();
	return wxApp::OnExit();
}

wxIMPLEMENT_APP(App);
