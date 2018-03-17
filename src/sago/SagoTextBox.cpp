/*
Copyright (c) 2018 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SagoTextBox.hpp"
#include "SagoTextField.hpp"
#include <vector>
#include <iostream>

namespace sago {

struct SagoTextBox::SagoTextBoxData {
	sago::SagoDataHolder* tex = nullptr;
	std::string fontName = "freeserif";
	SDL_Color color = { 255, 255, 255, 0 };
	SDL_Color outlineColor = { 255, 255, 0, 0 };
	int fontSize = 16;
	int outline = 0;
	std::string text = "";
	std::string renderedText = "";
	std::vector<SagoTextField> lines;
	int maxWidth = 0;
};
	
SagoTextBox::SagoTextBox() {
	data = new SagoTextBoxData();
}

SagoTextBox::~SagoTextBox() {
	delete data;
}

void SagoTextBox::SetHolder(SagoDataHolder* holder) {
	data->tex = holder;
}

void SagoTextBox::SetText(const char* text) {
	data->text = text;
}

void SagoTextBox::SetColor(const SDL_Color& color) {
	data->color = color;
}

void SagoTextBox::SetFont(const char* fontName) {
	data->fontName = fontName;
}

void SagoTextBox::SetFontSize(int fontSize) {
	data->fontSize = fontSize;
}

void SagoTextBox::SetOutline(int outlineSize, const SDL_Color& color) {
	data->outline = outlineSize;
	data->outlineColor = color;
}

void SagoTextBox::SetMaxWidth(int width) {
	data->maxWidth = width;
}

const std::string& SagoTextBox::GetText() const {
	return data->text;
}

void SagoTextBox::AppendLineToCache(const std::string& text) {
	data->lines.resize(data->lines.size()+1);
	SagoTextField& tf = data->lines.back();
	tf.SetHolder(data->tex);
	tf.SetFont(data->fontName.c_str());
	tf.SetFontSize(data->fontSize);
	tf.SetColor(data->color);
	tf.SetOutline(data->outline, data->outlineColor);
	tf.SetText(text.c_str());
}

void SagoTextBox::SplitAndAppendLineToCache(TTF_Font* font, const std::string& text) {
	int width = data->maxWidth;
	TTF_SizeUTF8(font, text.c_str(),&width, nullptr);
	std::cerr << "Width: " << width << "\n";
	if (data->maxWidth <= 0 || width <= data->maxWidth) {
		AppendLineToCache(text);
		return;
	}
	int splitLocation = 1;
	bool splitDone = false;
	while (!splitDone) {
		size_t nextSpace = text.find(' ', splitLocation+1);
		std::string attemptSubString = text.substr(0, nextSpace);
		TTF_SizeUTF8(font, attemptSubString.c_str(),&width, nullptr);
		if (width <= data->maxWidth && nextSpace != std::string::npos) {
			splitLocation = nextSpace;
		}
		else {
			splitDone = true;
		}
	}
	if (splitLocation == 1) {
		splitDone = false;
		while (!splitDone) {
			size_t nextSplit = splitLocation+1;
			std::string attemptSubString = text.substr(0, nextSplit);
			TTF_SizeUTF8(font, attemptSubString.c_str(),&width, nullptr);
			if (width <= data->maxWidth && nextSplit != text.length()) {
				splitLocation = nextSplit;
			}
			else {
				splitDone = true;
			}
		}
	}
	std::string firstPart = text.substr(0, splitLocation);
	AppendLineToCache(firstPart);
	std::string secondPart = text.substr(splitLocation+1);
	SplitAndAppendLineToCache(font, secondPart);
}

void SagoTextBox::UpdateCache() {
	TTF_Font *font = data->tex->getFontPtr(data->fontName, data->fontSize);
	const char delim = '\n';
	const std::string& s = data->text;
	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos)
	{
		const std::string& theSubString = s.substr(start, end - start);
		SplitAndAppendLineToCache(font, theSubString);
		start = end + 1;
		end = s.find(delim, start);
	}
	SplitAndAppendLineToCache(font, s.substr(start, end));
	data->renderedText = data->text;
}

void SagoTextBox::Draw(SDL_Renderer* target, int x, int y) {
	if (data->text != data->renderedText) {
		UpdateCache();
	}
	TTF_Font *font = data->tex->getFontPtr(data->fontName, data->fontSize);
	int lineSkip = TTF_FontLineSkip(font);
	for (size_t i = 0; i < data->lines.size(); ++i) {
		data->lines[i].Draw(target, x, y+i*lineSkip);
	}
}

}  //namespace sago