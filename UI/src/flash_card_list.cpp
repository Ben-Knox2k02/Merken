#include "flash_card_list.h"
#include "flash_card_tag.h"
#include "card_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/image.h>
#include <wx/bmpbndl.h>
#include <wx/vector.h>

namespace {
wxString CardIconPath() {
	const wxString relative = "UI/assets/flash_card_icon.png";
	wxFileName exe(wxStandardPaths::Get().GetExecutablePath());
	const wxString candidates[] = {
		wxGetCwd() + wxFileName::GetPathSeparator() + relative,
		exe.GetPath() + wxFileName::GetPathSeparator() + relative,
		exe.GetPath() + "/../../../" + relative
	};
	for (const wxString& candidate : candidates) {
		wxFileName file(candidate);
		file.MakeAbsolute();
		if (file.FileExists()) {
			return file.GetFullPath();
		}
	}
	return relative;
}

bool IsDarkTheme() {
	const wxColour bg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	return (bg.Red() + bg.Green() + bg.Blue()) < 3 * 128;
}

class CenteredTextRenderer : public wxDataViewCustomRenderer {
	public:
		CenteredTextRenderer()
			: wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_INERT, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL) {}

		bool SetValue(const wxVariant& value) override {
			this->text = value.GetString();
			return true;
		}

		bool GetValue(wxVariant& value) const override {
			value = this->text;
			return true;
		}

		bool Render(wxRect cell, wxDC* dc, int state) override {
			const int padX = this->GetView() != nullptr ? this->GetView()->FromDIP(10) : 10;
			const wxSize extent = dc->GetTextExtent(this->text);
			wxRect textRect = cell;
			textRect.x += padX;
			textRect.width -= padX * 2;
			if (textRect.width < 0) {
				textRect.width = 0;
			}
			textRect.y += (cell.GetHeight() - extent.GetHeight()) / 2;
			textRect.height = extent.GetHeight();
			this->RenderText(this->text, 0, textRect, dc, state);
			return true;
		}

		wxSize GetSize() const override {
			return wxSize(-1, -1);
		}

	private:
		wxString text;
};

class TagBadgeRenderer : public wxDataViewCustomRenderer {
	public:
		TagBadgeRenderer()
			: wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_INERT, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL) {}

		bool SetValue(const wxVariant& value) override {
			this->tags = FlashCardTag::Split(value.GetString());
			return true;
		}

		bool GetValue(wxVariant& value) const override {
			value = wxJoin(this->tags, ',');
			return true;
		}

		bool Render(wxRect cell, wxDC* dc, int) override {
			wxWindow* view = this->GetView();
			const int padX = view != nullptr ? view->FromDIP(10) : 10;
			const int gap = view != nullptr ? view->FromDIP(6) : 6;
			int x = cell.GetX() + padX;
			for (const wxString& tag : this->tags) {
				const wxSize size = FlashCardTag::Measure(*dc, tag, view);
				if (x + size.GetWidth() > cell.GetRight()) {
					break;
				}
				const int y = cell.GetY() + (cell.GetHeight() - size.GetHeight()) / 2;
				FlashCardTag::Draw(*dc, wxRect(x, y, size.GetWidth(), size.GetHeight()), tag, FlashCardTag::Style::Default);
				x += size.GetWidth() + gap;
			}
			return true;
		}

		wxSize GetSize() const override {
			return wxSize(-1, -1);
		}

	private:
		wxArrayString tags;
};
}

wxDECLARE_APP(App);

FlashCardList::FlashCardList(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId) {
	this->cardIcon = this->LoadCardIcon();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Cards in Deck");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->cardList = new wxDataViewCtrl(this, wxID_ANY);
	this->cardList->AppendBitmapColumn(wxEmptyString, 0, wxDATAVIEW_CELL_INERT, this->FromDIP(56), wxALIGN_CENTER, 0);
	this->cardList->AppendColumn(new wxDataViewColumn("Front", new CenteredTextRenderer(), 1, this->FromDIP(260), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE));
	this->cardList->AppendColumn(new wxDataViewColumn("Back", new CenteredTextRenderer(), 2, this->FromDIP(260), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE));
	this->cardList->AppendColumn(new wxDataViewColumn("Tags", new TagBadgeRenderer(), 3, this->FromDIP(220), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE));
	this->cardList->SetRowHeight(this->FromDIP(44));

	this->cardViewModel = new wxDataViewListStore();
	this->cardList->AssociateModel(this->cardViewModel);
	this->cardViewModel->DecRef();
	this->rootSizer->Add(this->cardList, 1, wxEXPAND | wxALL, 10);

	this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->addButton = new wxButton(this, wxID_ANY, "Add");
	this->editButton = new wxButton(this, wxID_ANY, "Edit");
	this->deleteButton = new wxButton(this, wxID_ANY, "Delete");
	this->studyButton = new wxButton(this, wxID_ANY, "Study Deck");
	this->buttonSizer->Add(this->addButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->editButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->deleteButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->studyButton, 0);
	this->rootSizer->Add(this->buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

	this->SetSizer(this->rootSizer);
	this->LoadCards();

	this->addButton->Bind(wxEVT_BUTTON, &FlashCardList::OnAdd, this);
	this->editButton->Bind(wxEVT_BUTTON, &FlashCardList::OnEdit, this);
	this->deleteButton->Bind(wxEVT_BUTTON, &FlashCardList::OnDelete, this);
	this->studyButton->Bind(wxEVT_BUTTON, &FlashCardList::OnStudy, this);
}

wxBitmapBundle FlashCardList::LoadCardIcon() {
	wxImage image;
	if (!image.LoadFile(CardIconPath(), wxBITMAP_TYPE_PNG)) {
		return wxBitmapBundle::FromBitmap(wxBitmap(24, 24));
	}
	if (IsDarkTheme() && image.HasAlpha()) {
		const int width = image.GetWidth();
		const int height = image.GetHeight();
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				image.SetRGB(x, y, 255 - image.GetRed(x, y), 255 - image.GetGreen(x, y), 255 - image.GetBlue(x, y));
			}
		}
	}

	const int dip = 24;
	wxVector<wxBitmap> bitmaps;
	for (int scale = 1; scale <= 3; ++scale) {
		wxImage scaled = image.Copy();
		scaled.Rescale(dip * scale, dip * scale, wxIMAGE_QUALITY_HIGH);
		bitmaps.push_back(wxBitmap(scaled));
	}
	return wxBitmapBundle::FromBitmaps(bitmaps);
}

void FlashCardList::AppendCard(int cardId, const wxString& front, const wxString& back, const wxString& tags) {
	wxVector<wxVariant> row;
	row.push_back(wxVariant(this->cardIcon));
	row.push_back(wxVariant(front));
	row.push_back(wxVariant(back));
	row.push_back(wxVariant(tags));
	this->cardViewModel->AppendItem(row, static_cast<wxUIntPtr>(cardId));
}

void FlashCardList::SetDeck(int deckId) {
	this->deckId = deckId;
	this->LoadCards();
}

void FlashCardList::LoadCards() {
	this->cardViewModel->DeleteAllItems();
	if (this->deckId == 0) { return; }

	auto useCase = wxGetApp().GetInjector().create<GetCardsUseCase>();
	GetCardsResponse response = useCase.Execute(GetCardsRequest{this->deckId});
	for (const CardResponse& card : response.cards) {
		this->AppendCard(card.cardId, wxString(card.front), wxString(card.back), wxString(card.tags));
	}
}

int FlashCardList::GetSelectedRow() const {
	wxDataViewItem item = this->cardList->GetSelection();
	if (!item.IsOk()) { return -1; }
	return static_cast<int>(this->cardViewModel->GetRow(item));
}

int FlashCardList::GetSelectedCardId() const {
	wxDataViewItem item = this->cardList->GetSelection();
	if (!item.IsOk()) { return 0; }
	return static_cast<int>(this->cardViewModel->GetItemData(item));
}

void FlashCardList::OnAdd(wxCommandEvent&) {
	if (this->deckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Add Card", wxOK | wxICON_WARNING);
		return;
	}

	CardDialog dialog(this, "Add Card");
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string front = dialog.frontCtrl->GetValue().ToStdString();
	const std::string back = dialog.backCtrl->GetValue().ToStdString();
	if (front.empty() || back.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Add Card", wxOK | wxICON_WARNING);
		return;
	}

	CreateCardRequest request;
	request.deckId = this->deckId;
	request.front = front;
	request.back = back;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<CreateCardUseCase>();
	CreateCardResponse response = useCase.Execute(request);
	if (response.cardId == 0) {
		ShowCenteredMessage(this, "Could not save card.", "Add Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void FlashCardList::OnEdit(wxCommandEvent&) {
	const int cardId = this->GetSelectedCardId();
	if (cardId == 0) {
		ShowCenteredMessage(this, "No card selected.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}

	const int row = this->GetSelectedRow();
	wxDataViewItem item = this->cardViewModel->GetItem(static_cast<unsigned int>(row));
	wxVariant front, back, tags;
	this->cardViewModel->GetValue(front, item, 1);
	this->cardViewModel->GetValue(back, item, 2);
	this->cardViewModel->GetValue(tags, item, 3);

	CardDialog dialog(this, "Edit Card");
	dialog.frontCtrl->SetValue(front.GetString());
	dialog.backCtrl->SetValue(back.GetString());
	dialog.tagCtrl->SetValue(tags.GetString());
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string newFront = dialog.frontCtrl->GetValue().ToStdString();
	const std::string newBack = dialog.backCtrl->GetValue().ToStdString();
	if (newFront.empty() || newBack.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}

	UpdateCardRequest request;
	request.deckId = this->deckId;
	request.cardId = cardId;
	request.front = newFront;
	request.back = newBack;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<UpdateCardUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not update card.", "Edit Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void FlashCardList::OnDelete(wxCommandEvent&) {
	ShowCenteredMessage(this, "Delete is not implemented yet.", "Delete Card", wxOK | wxICON_INFORMATION);
}

void FlashCardList::OnStudy(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowStudyDeck();
	}
}
