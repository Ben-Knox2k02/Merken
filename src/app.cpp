#include "app.h"
#include "mainframe.h"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	this->SCREEN_WIDTH = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	this->SCREEN_HEIGHT = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	
	this->windowWidth = this->SCREEN_WIDTH * 4 / 5;
	this->windowHeight = this->SCREEN_HEIGHT * 4 / 5;
	
	MainFrame* mainFrame = new MainFrame("Merken");
	mainFrame->SetSize(this->windowWidth, this->windowHeight);
	mainFrame->Center();
	mainFrame->Show();
	
	this->Print();
	
	return true;
}

int App::GetScreenWidth() const { return this->SCREEN_WIDTH; }
int App::GetScreenHeight() const { return this->SCREEN_HEIGHT; }

int App::GetWindowWidth() { return this->windowWidth; }
int App::GetWindowHeight() { return this->windowHeight; }

void App::SetWindowWidth(int width) { this->windowWidth = width; }
void App::SetWindowHeight(int height) { this->windowHeight = height; }

void App::Print() const {
	std::cout<<"--------------------------------------\n";
	std::cout<<"SCREEN_WIDTH: "<<this->SCREEN_WIDTH<<"\n";
	std::cout<<"SCREEN_HEIGHT: "<<this->SCREEN_HEIGHT<<"\n";
	std::cout<<"Window Width: "<<this->windowWidth<<"\n";
	std::cout<<"Window Height: "<<this->windowHeight<<"\n";
	std::cout<<"---------------------------------------\n\n";
}

