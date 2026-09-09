#include "app.h"
#include "mainframe.h"

bool App::OnInit() {
	MainFrame* mainFrame = new MainFrame("Merken");
	mainFrame->SetClientSize(1000, 800);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}

wxIMPLEMENT_APP(App);
