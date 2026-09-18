#ifndef FLASH_CARD_H
#define FLASH_CARD_H

#include <wx/wx.h>
#include <wx/dcgraph.h>
#include "flash_card_tag.h"

class FlashCardWell : public wxPanel {
	public:
		explicit FlashCardWell(wxWindow* parent)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE) {
			this->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->inner = new wxBoxSizer(wxVERTICAL);
			this->SetSizer(this->inner);
			this->Bind(wxEVT_PAINT, &FlashCardWell::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		wxBoxSizer* Inner() { return this->inner; }

		void AddPadded(wxWindow* child, int proportion, int padX, int padY) {
			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(child, 1, wxEXPAND | wxTOP | wxBOTTOM, padY);
			this->inner->Add(row, proportion, wxEXPAND | wxLEFT | wxRIGHT, padX);
		}

		void AddPadded(wxSizer* child, int proportion, int padX, int padY) {
			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(child, 1, wxEXPAND | wxTOP | wxBOTTOM, padY);
			this->inner->Add(row, proportion, wxEXPAND | wxLEFT | wxRIGHT, padX);
		}

	private:
		static constexpr int kRadius = 5;

		wxBoxSizer* inner;

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}
			gc.SetBrush(*wxTRANSPARENT_BRUSH);
			gc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1));
			gc.DrawRoundedRectangle(0, 0, size.GetWidth() - 1, size.GetHeight() - 1, kRadius);
		}
};

class FlashCard : public wxPanel {
	public:
		FlashCard(
			wxWindow* parent,
			const wxString& front,
			const wxString& back,
			const wxString& tags
		) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			front(front),
			back(back),
			tags(tags),
			selected(false),
			lastFrontWrap(0),
			lastBackWrap(0) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->frontCaption = this->MakeCaption("Front:");
			this->backCaption = this->MakeCaption("Back:");
			this->tagsCaption = this->MakeCaption("Tags:");

			const int padX = this->FromDIP(15);
			const int padY = this->FromDIP(10);

			this->frontWell = new FlashCardWell(this);
			this->frontCtrl = this->MakeBody(this->frontWell, TruncateSentences(front));
			this->frontWell->AddPadded(this->frontCtrl, 1, padX, padY);

			this->backWell = new FlashCardWell(this);
			this->backCtrl = this->MakeBody(this->backWell, TruncateSentences(back));
			this->backWell->AddPadded(this->backCtrl, 1, padX, padY);

			this->tagHost = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			this->tagHost->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->tagSizer = new wxBoxSizer(wxHORIZONTAL);
			this->tagHost->SetSizer(this->tagSizer);

			this->ApplyTextColours();

			this->fields = new wxFlexGridSizer(2, this->FromDIP(10), this->FromDIP(12));
			this->fields->AddGrowableCol(1, 1);

			this->fields->Add(this->frontCaption, 0, wxALIGN_TOP | wxTOP, 4);
			this->fields->Add(this->frontWell, 1, wxEXPAND);
			this->fields->Add(this->backCaption, 0, wxALIGN_TOP | wxTOP, 4);
			this->fields->Add(this->backWell, 1, wxEXPAND);
			this->fields->Add(this->tagsCaption, 0, wxALIGN_CENTER_VERTICAL);
			this->fields->Add(this->tagHost, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

			this->RebuildTags();

			wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
			mainSizer->Add(this->fields, 1, wxEXPAND | wxALL, this->FromDIP(16));
			this->SetSizer(mainSizer);

			this->Bind(wxEVT_PAINT, &FlashCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &FlashCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
			this->CallAfter([this]() {
				this->RelayoutContents();
			});
		}

		void RelayoutContents() {
			this->WrapFields();
			this->RelayoutTags();
			this->Layout();
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->ApplyTextColours();
			this->Refresh();
			this->frontWell->Refresh();
			this->backWell->Refresh();
			this->RelayoutTags();
		}

		wxString GetFront() const { return this->front; }
		wxString GetBack() const { return this->back; }
		wxString GetTags() const { return this->tags; }

		void SetFront(const wxString& front) {
			this->front = front;
			this->lastFrontWrap = 0;
			this->frontCtrl->SetLabel(TruncateSentences(front));
			this->WrapFields();
			this->Layout();
		}

		void SetBack(const wxString& back) {
			this->back = back;
			this->lastBackWrap = 0;
			this->backCtrl->SetLabel(TruncateSentences(back));
			this->WrapFields();
			this->Layout();
		}

		void SetTags(const wxString& tags) {
			this->tags = tags;
			this->RebuildTags();
			this->Layout();
		}

	private:
		wxString front;
		wxString back;
		wxString tags;
		wxFlexGridSizer* fields;
		wxStaticText* frontCaption;
		wxStaticText* backCaption;
		wxStaticText* tagsCaption;
		FlashCardWell* frontWell;
		FlashCardWell* backWell;
		wxPanel* tagHost;
		wxStaticText* frontCtrl;
		wxStaticText* backCtrl;
		wxBoxSizer* tagSizer;
		bool selected;
		int lastFrontWrap;
		int lastBackWrap;

		static constexpr int kCornerRadius = 5;

		wxStaticText* MakeCaption(const wxString& label) {
			wxStaticText* caption = new wxStaticText(this, wxID_ANY, label);
			caption->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			return caption;
		}

		wxStaticText* MakeBody(wxWindow* parent, const wxString& value) {
			wxStaticText* text = new wxStaticText(parent, wxID_ANY, value);
			text->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			return text;
		}

		static wxString TruncateSentences(const wxString& text) {
			wxString trimmed = text;
			trimmed.Trim(true).Trim(false);
			int sentences = 0;
			for (size_t i = 0; i < trimmed.length(); ++i) {
				const wxUniChar ch = trimmed[i];
				if (ch != '.' && ch != '!' && ch != '?') {
					continue;
				}

				size_t end = i + 1;
				while (end < trimmed.length() && (trimmed[end] == '"' || trimmed[end] == '\'' || trimmed[end] == ')')) {
					++end;
				}

				const bool atEnd = end >= trimmed.length();
				const bool atBreak = !atEnd && (
					trimmed[end] == ' ' ||
					trimmed[end] == '\n' ||
					trimmed[end] == '\r' ||
					trimmed[end] == '\t'
				);
				if (!atEnd && !atBreak) {
					continue;
				}

				++sentences;
				if (sentences < 3) {
					continue;
				}
				if (atEnd) {
					return trimmed;
				}

				wxString result = trimmed.Mid(0, end);
				result.Trim(true);
				return result + "...";
			}
			return trimmed;
		}

		void RebuildTags() {
			this->tagSizer->Clear(true);
			const wxArrayString labels = FlashCardTag::Split(this->tags);
			const int gap = this->FromDIP(6);
			for (const wxString& label : labels) {
				this->tagSizer->Add(
					new FlashCardTag(this->tagHost, label),
					0,
					wxALIGN_CENTER_VERTICAL | wxFIXED_MINSIZE | wxRIGHT | wxBOTTOM,
					gap
				);
			}
			const bool hasTags = !labels.empty();
			this->fields->Show(this->tagsCaption, hasTags);
			this->fields->Show(this->tagHost, hasTags);
			this->fields->Layout();
			this->tagHost->Layout();
		}

		bool WrapField(wxStaticText* ctrl, const wxString& display, FlashCardWell* well, int& lastWrap) {
			const int wrapWidth = well->GetClientSize().GetWidth() - this->FromDIP(30);
			if (wrapWidth < this->FromDIP(40)) {
				return false;
			}
			if (lastWrap != 0 && wrapWidth >= lastWrap - this->FromDIP(8) && wrapWidth <= lastWrap + this->FromDIP(8)) {
				return false;
			}
			lastWrap = wrapWidth;
			ctrl->SetLabel(display);
			ctrl->Wrap(wrapWidth);
			return true;
		}

		void WrapFields() {
			this->WrapField(this->frontCtrl, TruncateSentences(this->front), this->frontWell, this->lastFrontWrap);
			this->WrapField(this->backCtrl, TruncateSentences(this->back), this->backWell, this->lastBackWrap);
		}

		void RelayoutTags() {
			wxSizerItemList& items = this->tagSizer->GetChildren();
			for (wxSizerItemList::compatibility_iterator node = items.GetFirst(); node; node = node->GetNext()) {
				wxSizerItem* item = node->GetData();
				wxWindow* window = item != nullptr ? item->GetWindow() : nullptr;
				if (auto* tag = dynamic_cast<FlashCardTag*>(window)) {
					tag->FitToLabel();
				}
			}
			this->tagSizer->Layout();
			this->tagHost->Layout();
			this->tagHost->Refresh();
		}

		void OnSize(wxSizeEvent& event) {
			this->WrapFields();
			this->RelayoutTags();
			this->Refresh();
			event.Skip();
		}

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}

			wxColour background = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
			if (this->GetParent() != nullptr) {
				background = this->GetParent()->GetBackgroundColour();
			}

			gc.SetPen(*wxTRANSPARENT_PEN);
			gc.SetBrush(wxBrush(background));
			gc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

			if (!this->selected) {
				return;
			}

			gc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
			gc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), this->FromDIP(kCornerRadius));
		}

		void ApplyTextColours() {
			const wxColour caption = wxSystemSettings::GetColour(
				this->selected ? wxSYS_COLOUR_HIGHLIGHTTEXT : wxSYS_COLOUR_WINDOWTEXT
			);
			this->frontCaption->SetForegroundColour(caption);
			this->backCaption->SetForegroundColour(caption);
			this->tagsCaption->SetForegroundColour(caption);
			this->SetBackgroundColour(wxNullColour);
		}
};

#endif
