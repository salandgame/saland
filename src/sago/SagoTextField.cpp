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

#include "SagoTextField.hpp"

namespace sago {
	
struct SagoTextField::SagoTextFieldData {
	sago::SagoDataHolder* tex = nullptr;
	SDL_Surface* textSurface = nullptr;
	SDL_Texture* texture = nullptr;
	std::string fontName = "freeserif";
};
	
SagoTextField::SagoTextField() {
	data = new SagoTextFieldData();
}

SagoTextField::~SagoTextField() {
	delete data;
}

void SagoTextField::SetHolder(SagoDataHolder* holder) {
	data->tex = holder;
}

void SagoTextField::SetText(std::string text) {
	this->text = text;
}

std::string SagoTextField::GetText() const {
	return text;
}

void SagoTextField::Draw(SDL_Renderer* target, int x, int y) const {
	SDL_Color color = { 255, 255, 255, 0 };
	TTF_Font *font = data->tex->getFontPtr(data->fontName, 30);
	data->textSurface = TTF_RenderText_Blended (font, text.c_str(), color);
	data->texture = SDL_CreateTextureFromSurface(target, data->textSurface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(data->texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };
	SDL_RenderCopy(target, data->texture, NULL, &dstrect);
	if (data->texture) {
		SDL_DestroyTexture(data->texture);
		data->texture = nullptr;
	}
	if (data->textSurface) {
		SDL_FreeSurface(data->textSurface);
		data->textSurface = nullptr;
	}
}

}  //namespace sago