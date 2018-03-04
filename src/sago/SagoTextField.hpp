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

#ifndef SAGOTEXTFIELD_HPP
#define SAGOTEXTFIELD_HPP

#include "SagoDataHolder.hpp"

namespace sago {

class SagoTextField {
public:
	SagoTextField();
	virtual ~SagoTextField();
    void SetHolder(SagoDataHolder* holder);
    void SetText(std::string text);
	void SetColor(const SDL_Color& color);
	void SetFont(const std::string& fontName);
	void SetFontSize(int fontSize);
    std::string GetText() const;
	void Draw(SDL_Renderer* target, int x, int y);
	void UpdateCache(SDL_Renderer* target);
	void ClearCache();
private:
	std::string text = "";
	std::string renderedText = "";
	SagoTextField(const SagoTextField& orig) = delete;
	struct SagoTextFieldData;
	SagoTextFieldData *data;
};

}  //namespace sago

#endif /* SAGOTEXTFIELD_HPP */

