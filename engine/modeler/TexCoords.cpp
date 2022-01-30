#include "TexCoords.h"
#include "Texture.h"

void TexCoords::set(TexCoords* pTC, int texN, float x, float y, float w, float h, std::string flipStr) {
	if (texN < 0)
		return;
	Texture* pTex = Texture::textures.at(texN);
	x = x / pTex->size[0];
	y = y / pTex->size[1];
	w = w / pTex->size[0];
	h = h / pTex->size[1];
	set_GL(pTC, x, y, w, h, flipStr);
}
void TexCoords::set_GL(TexCoords* pTC, float x, float y, float w, float h, std::string flipStr) {
	//assuming that x,y,w,h - in GL 0-to-1 format
	pTC->tuvTopLeft[0] = x;
	pTC->tuvTopLeft[1] = y;
	pTC->tuvBottomRight[0] = x + w;
	pTC->tuvBottomRight[1] = y + h;
	flip(pTC, flipStr);
}
void TexCoords::flip(TexCoords* pTC, std::string flipStr) {
	//possible flips: "90" (CCW), "-90" (CW), "180", "h" (horizontal), "v" (vertical) 
	if (flipStr.compare("") == 0)
		return;
	TexCoords outTC;
	if (flipStr.find("90") == 0) { //CCW
		outTC.tuvTopLeft[0] = pTC->tuvBottomRight[0];
		outTC.tuvTopLeft[1] = pTC->tuvTopLeft[1];
		outTC.tuvBottomRight[0] = pTC->tuvTopLeft[0];
		outTC.tuvBottomRight[1] = pTC->tuvBottomRight[1];
	}
	else if (flipStr.find("-90") == 0) { //CW
		outTC.tuvTopLeft[0] = pTC->tuvTopLeft[0];
		outTC.tuvTopLeft[1] = pTC->tuvBottomRight[1];
		outTC.tuvBottomRight[0] = pTC->tuvBottomRight[0];
		outTC.tuvBottomRight[1] = pTC->tuvTopLeft[1];
	}
	else if (flipStr.find("180") == 0) {
		outTC.tuvTopLeft[0] = pTC->tuvBottomRight[0];
		outTC.tuvTopLeft[1] = pTC->tuvBottomRight[1];
		outTC.tuvBottomRight[0] = pTC->tuvTopLeft[0];
		outTC.tuvBottomRight[1] = pTC->tuvTopLeft[1];
	}
	else if (flipStr.find("h") == 0) { //horizontal
		outTC.tuvTopLeft[0] = pTC->tuvBottomRight[0];
		outTC.tuvTopLeft[1] = pTC->tuvTopLeft[1];
		outTC.tuvBottomRight[0] = pTC->tuvTopLeft[0];
		outTC.tuvBottomRight[1] = pTC->tuvBottomRight[1];
	}
	else if (flipStr.find("v") == 0) { //vertical
		outTC.tuvTopLeft[0] = pTC->tuvTopLeft[0];
		outTC.tuvTopLeft[1] = pTC->tuvBottomRight[1];
		outTC.tuvBottomRight[0] = pTC->tuvBottomRight[0];
		outTC.tuvBottomRight[1] = pTC->tuvTopLeft[1];
	}
	memcpy(pTC, &outTC, sizeof(TexCoords));
}