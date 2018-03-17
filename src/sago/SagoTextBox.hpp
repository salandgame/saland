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

#ifndef SAGOTEXTBOX_HPP
#define SAGOTEXTBOX_HPP

#include "SagoDataHolder.hpp"
#include "SagoTextField.hpp"

namespace sago {

class SagoTextBox {
public:
	SagoTextBox();
	virtual ~SagoTextBox();
	void SetHolder(SagoDataHolder* holder);
    void SetText(const char* text);
	void SetColor(const SDL_Color& color);
	void SetFont(const char* fontName);
	void SetFontSize(int fontSize);
	void SetOutline(int outlineSize, const SDL_Color& color);
	void SetMaxWidth(int width);
    const std::string& GetText() const;
	void Draw(SDL_Renderer* target, int x, int y);
	void UpdateCache();
private:
	void AppendLineToCache(const std::string& text);
	void SplitAndAppendLineToCache(TTF_Font* font, const std::string& text);
	SagoTextBox(const SagoTextBox& orig) = delete;
	SagoTextBox& operator=(const SagoTextBox& base) = delete;
	struct SagoTextBoxData;
	SagoTextBoxData *data;
};

}  //namespace sago

#endif /* SAGOTEXTBOX_HPP */

