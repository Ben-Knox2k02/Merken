#ifndef STUDY_PANEL_H
#define STUDY_PANEL_H

#include "data_structures.h"
#include "dialog.h"
#include <vector>
#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

class StudyPanel : public wxPanel {
	public:
		StudyPanel(wxWindow* parent, int deckID);
		
		int deckID;
		std::vector<Card> cards;
	
		wxBoxSizer* rootSizer;
		wxBoxSizer* topSizer;
		wxBoxSizer* midSizer;
		wxBoxSizer* bottomSizer;
		
		wxStaticText* header;
		wxStaticText* cardText;
		wxStaticText* progressText;
		
		wxTextCtrl* answerCtrl;
		
		wxButton* previousButton;
		wxButton* nextButton;
		wxButton* showAnswerButton;
		wxButton* backButton;
		wxButton* resetButton;
		wxButton* changeButton;
		
		void OnPaint(wxPaintEvent& event);
		void OnShowAnswer(wxCommandEvent& event);
		void OnPrevious(wxCommandEvent& event);
		void OnNext(wxCommandEvent& event);
		void OnBack(wxCommandEvent& event);
		void OnReset(wxCommandEvent& event);
		void OnChange(wxCommandEvent& event);
};

#endif
