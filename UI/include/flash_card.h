#ifndef FLASH_CARD_H
#define FLASH_CARD_H

#include <functional>
#include <wx/wx.h>
#include <wx/dcgraph.h>
#include <wx/gbsizer.h>
#include <wx/statline.h>
#include <wx/wrapsizer.h>
#include "flash_card_tag.h"
#include "flash_card_number.h"
#include "icon_button.h"
#include "theme.h"

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
		wxBoxSizer* inner;

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}
			Theme::Get().DrawWell(gc, this, wxRect(0, 0, size.GetWidth() - 1, size.GetHeight() - 1));
		}
};

class FlashCard : public wxPanel {
	public:
		FlashCard(
			wxWindow* parent,
			int number,
			const wxString& front,
			const wxString& back,
			const wxString& tags
		) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			front(front),
			back(back),
			tags(tags),
			selected(false),
			lastFrontWrap(0),
			lastBackWrap(0),
			relayouting(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->numberBox = new FlashCardNumber(this, number);
			this->frontCaption = this->MakeCaption("Front:");
			this->backCaption = this->MakeCaption("Back:");
			this->tagsCaption = this->MakeCaption("Tags:");

			const int padX = this->FromDIP(Theme::Get().size.wellPadX);
			const int padY = this->FromDIP(Theme::Get().size.wellPadY);

			this->frontWell = new FlashCardWell(this);
			this->frontCtrl = this->MakeBody(this->frontWell, TruncateSentences(front));
			this->frontWell->AddPadded(this->frontCtrl, 1, padX, padY);

			this->backWell = new FlashCardWell(this);
			this->backCtrl = this->MakeBody(this->backWell, TruncateSentences(back));
			this->backWell->AddPadded(this->backCtrl, 1, padX, padY);

			this->tagHost = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			this->tagHost->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->tagHost->SetMinSize(wxSize(0, -1));
			this->tagSizer = new wxWrapSizer(wxHORIZONTAL, wxREMOVE_LEADING_SPACES);
			this->tagHost->SetSizer(this->tagSizer);

			this->ApplyTextColours();

			this->fields = new wxFlexGridSizer(2, this->FromDIP(Theme::Get().space.lg), this->FromDIP(Theme::Get().space.xl));
			this->fields->AddGrowableCol(1, 1);

			this->fields->Add(this->frontCaption, 0, wxALIGN_TOP | wxTOP, 4);
			this->fields->Add(this->frontWell, 1, wxEXPAND);
			this->fields->Add(this->backCaption, 0, wxALIGN_TOP | wxTOP, 4);
			this->fields->Add(this->backWell, 1, wxEXPAND);
			this->fields->Add(this->tagsCaption, 0, wxALIGN_TOP | wxTOP, 4);
			this->fields->Add(this->tagHost, 1, wxEXPAND);

			this->RebuildTags();

			this->editButton = new IconButton(this, "UI/assets/edit_icon.png", "Edit");
			this->deleteButton = new IconButton(this, "UI/assets/delete_icon.png", "Delete");
			this->editButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
				if (this->onEdit) {
					this->onEdit();
				}
			});
			this->deleteButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
				if (this->onDelete) {
					this->onDelete();
				}
			});

			wxBoxSizer* actionRow = new wxBoxSizer(wxHORIZONTAL);
			actionRow->AddStretchSpacer(1);
			actionRow->Add(this->editButton, 0);
			actionRow->AddSpacer(Theme::Get().space.xs);
			actionRow->Add(this->deleteButton, 0);

			wxStaticLine* rule = new wxStaticLine(this, wxID_ANY);

			const int vgap = this->FromDIP(Theme::Get().space.md);
			const int hgap = this->FromDIP(Theme::Get().space.lg);
			wxGridBagSizer* grid = new wxGridBagSizer(vgap, hgap);
			grid->Add(this->numberBox, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_TOP);
			grid->Add(this->fields, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);
			grid->Add(rule, wxGBPosition(1, 0), wxGBSpan(1, 2), wxEXPAND);
			grid->Add(actionRow, wxGBPosition(2, 1), wxDefaultSpan, wxEXPAND);
			grid->AddGrowableCol(1, 1);
			grid->AddGrowableRow(0, 1);

			const int pad = this->FromDIP(Theme::Get().size.cardPad);
			wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
			mainSizer->Add(grid, 1, wxEXPAND | wxALL, pad);
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

		void SetOnEdit(std::function<void()> handler) {
			this->onEdit = std::move(handler);
		}

		void SetOnDelete(std::function<void()> handler) {
			this->onDelete = std::move(handler);
		}

	private:
		wxString front;
		wxString back;
		wxString tags;
		FlashCardNumber* numberBox;
		wxFlexGridSizer* fields;
		wxStaticText* frontCaption;
		wxStaticText* backCaption;
		wxStaticText* tagsCaption;
		FlashCardWell* frontWell;
		FlashCardWell* backWell;
		wxPanel* tagHost;
		wxStaticText* frontCtrl;
		wxStaticText* backCtrl;
		wxWrapSizer* tagSizer;
		IconButton* editButton;
		IconButton* deleteButton;
		std::function<void()> onEdit;
		std::function<void()> onDelete;
		bool selected;
		int lastFrontWrap;
		int lastBackWrap;
		bool relayouting;

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
			const int gap = this->FromDIP(Theme::Get().space.sm);
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
			const int wrapWidth = well->GetClientSize().GetWidth() - this->FromDIP(Theme::Get().size.wellPadX * 2);
			if (wrapWidth < this->FromDIP(Theme::Get().size.minWrap)) {
				return false;
			}
			const int deadband = this->FromDIP(Theme::Get().size.wrapDeadband);
			if (lastWrap != 0 && wrapWidth >= lastWrap - deadband && wrapWidth <= lastWrap + deadband) {
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
			if (!this->tagHost->IsShown() || this->tagSizer->IsEmpty()) {
				this->tagHost->SetMinSize(wxSize(0, 0));
				return;
			}

			wxSizerItemList& items = this->tagSizer->GetChildren();
			for (wxSizerItemList::compatibility_iterator node = items.GetFirst(); node; node = node->GetNext()) {
				wxSizerItem* item = node->GetData();
				wxWindow* window = item != nullptr ? item->GetWindow() : nullptr;
				if (auto* tag = dynamic_cast<FlashCardTag*>(window)) {
					tag->FitToLabel();
				}
			}

			const int width = this->tagHost->GetClientSize().GetWidth();
			if (width > 1) {
				const wxSize min = this->tagSizer->CalcMinSizeFromKnownDirection(wxHORIZONTAL, width, -1);
				this->tagHost->SetMinSize(wxSize(0, min.GetHeight()));
				this->tagSizer->SetDimension(wxPoint(0, 0), wxSize(width, min.GetHeight()));
			} else {
				this->tagSizer->Layout();
				this->tagHost->Layout();
			}
			this->tagHost->Refresh();
		}

		void OnSize(wxSizeEvent& event) {
			if (this->relayouting) {
				event.Skip();
				return;
			}
			this->relayouting = true;
			this->WrapFields();
			this->RelayoutTags();
			this->Layout();
			this->Refresh();
			this->relayouting = false;
			event.Skip();
		}

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}

			const Theme& theme = Theme::Get();
			theme.FillCanvas(gc, this, size);
			theme.DrawCard(
				gc,
				this,
				wxRect(0, 0, size.GetWidth() - 1, size.GetHeight() - 1),
				this->selected
			);
		}

		void ApplyTextColours() {
			const wxColour caption = Theme::Get().LabelOn(this->selected);
			this->frontCaption->SetForegroundColour(caption);
			this->backCaption->SetForegroundColour(caption);
			this->tagsCaption->SetForegroundColour(caption);
			this->SetBackgroundColour(wxNullColour);
		}
};

#endif
