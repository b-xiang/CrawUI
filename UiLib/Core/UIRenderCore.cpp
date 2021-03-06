#include "StdAfx.h"
#include "UIRenderCore.h"
#include "Renders/UIRenderGDI.h"
#include "Renders/UIRenderSKIA.h"
#include "Renders/UIRenderD2D.h"


namespace UiLib {
    /////////////////////////////////////////////////////////////////////////////////////
    CUIRenderCore *CUIRenderCore::RenderCore = NULL;

    CUIRenderCore::CUIRenderCore(void)
    {
        hRenderAPI = NULL;
        m_pIRenderCore = NULL;
    }

    CUIRenderCore::~CUIRenderCore(void)
    {
        DisConnectRenderCore();
    }

    bool CUIRenderCore::LinkRenderCore()
    {
		return true;
    }

    void CUIRenderCore::DisConnectRenderCore()
    {
		SAFE_DELETE(m_pIRenderCore);
		ReleaseRef_ ReleaseRef = NULL;
		ReleaseRef = (ReleaseRef_)GetProcAddress(hRenderAPI,"ReleaseRef");
		if(ReleaseRef&&ReleaseRef())
		{
			::FreeLibrary(hRenderAPI);
			hRenderAPI = NULL;	
		}
        
    }

    CUIRenderCore* CUIRenderCore::GetInstance(RenderWay _RenderWay)
    {
        switch(_RenderWay)
        {
        case GDI_RENDER:
            RenderCore = new CUIRenderGDI();
            break;
		case D2D_RENDER:
			RenderCore = new CUIRenderD2D();
			break;
        case SKIA_RENDER:
            RenderCore = new CUIRenderSKIA();
            break;
        default:
            break;
        }
        if(RenderCore&&RenderCore->LinkRenderCore())
			return RenderCore;
		else
		{
			SAFE_DELETE(RenderCore);
			RenderCore = new CUIRenderGDI();
			if(RenderCore&&RenderCore->LinkRenderCore())
				return RenderCore;
			else
				return NULL;
		}
    }

    void CUIRenderCore::SetPaintWindow(HWND hWnd)
    {
      
    }

	bool CUIRenderCore::IsWindowGlass()
	{
		if(m_pIRenderCore)
			return m_pIRenderCore->IsWindowGlass();
		return false;
	}

    void CUIRenderCore::SetWindowGlass(bool bGlass)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->SetWindowGlass(bGlass);
    }

    void CUIRenderCore::SetWindowLayered(bool bLayered)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->SetWindowLayered(bLayered);
    }

	bool CUIRenderCore::IsWindowLayered()
	{
		if(m_pIRenderCore)
			return m_pIRenderCore->IsWindowLayered();
		return false;
	}

    HWND CUIRenderCore::GetPaintWindow()
    {
        if(m_pIRenderCore)
           return m_pIRenderCore->GetPaintWindow();
		return NULL;
    }

    int CUIRenderCore::GetOffBmpPitch()
    {
        if(m_pIRenderCore)
            return m_pIRenderCore->GetOffBmpPitch();
		return -1;
    }

    void *CUIRenderCore::GetOffBmpPiexl()
    {
        if(m_pIRenderCore)
            return m_pIRenderCore->GetOffBmpPiexl();
		return NULL;
    }

	void CUIRenderCore::TranslateRO(float fTransX,float fTransY)
	{
		if(m_pIRenderCore)
			m_pIRenderCore->TranslateRO(fTransX,fTransY);
	}

    void CUIRenderCore::BeginDraw(RECT *rc)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->BeginDraw(rc);
    }

    void CUIRenderCore::EndDraw(PAINTSTRUCT &ps)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->EndDraw(ps);
    }

    void CUIRenderCore::SetRenderObj(TRenderObj* pRenderObj,int nCx,int nCy)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->SetRenderObj(pRenderObj,nCx,nCy);
    }

    HDC CUIRenderCore::GetRenderHDC(BOOL bOffScreen)
    {
        if(m_pIRenderCore)
            return m_pIRenderCore->GetRenderHDC(bOffScreen);
		return NULL;
    }

	void CUIRenderCore::FreeRenderHDC()
	{
		if(m_pIRenderCore)
			return m_pIRenderCore->FreeRenderHDC();
	}

    void CUIRenderCore::ReleaseRenderObj()
    {
    }

    bool CUIRenderCore::DrawImageString(CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint,CDuiImage& sImage,bool bNeedAlpha /*= FALSE*/, BYTE bNewFade /*= 255*/ )
    {
        sImage.SetControlRect(rcItem);

        if( bNeedAlpha )
            return DrawImage( pManager, rcItem, rcPaint, sImage, sImage.GetRes(),sImage.GetDest(), sImage.GetSource(), sImage.GetCorner(), sImage.GetMask(), bNewFade, sImage.GetHole(), sImage.GetTiledX(), sImage.GetTiledY(), bNeedAlpha);
        else
            return DrawImage( pManager, rcItem, rcPaint, sImage, sImage.GetRes(),sImage.GetDest(), sImage.GetSource(), sImage.GetCorner(), sImage.GetMask(), sImage.GetFade(), sImage.GetHole(), sImage.GetTiledX(), sImage.GetTiledY());
    }

    bool CUIRenderCore::DrawImage( CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CDuiImage& sImageName,const CDuiString& sImageResType,RECT rcItem,RECT rcBmpPart,RECT rcCorner, DWORD dwMask,BYTE bFade,bool bHole,bool bTiledX,bool bTiledY,bool bNeedAlpha)
    {
        if (sImageName.IsEmpty())
            return false;
        const TImageInfo* data = NULL;
        if( sImageResType.IsEmpty() )
            data = pManager->GetImageEx(sImageName, NULL, dwMask);
        else 
            data = pManager->GetImageEx(sImageName, (LPCTSTR)sImageResType, dwMask);
        if( !data ) 
            return false;
        if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) 
        {
            rcBmpPart.right = data->nX;
            rcBmpPart.bottom = data->nY;
        }
        if (rcBmpPart.right > data->nX) rcBmpPart.right = data->nX;
        if (rcBmpPart.bottom > data->nY) rcBmpPart.bottom = data->nY;

        RECT rcTemp;
        if( !::IntersectRect(&rcTemp, &rcItem, &rc) ) 
            return true;
        if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) ) 
            return true;

        DrawImage(data, rcItem, rcPaint, rcBmpPart, rcCorner, data->alphaChannel, bFade, bHole, bTiledX, bTiledY);
        return true;
    }

    void CUIRenderCore::DrawImage(const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart,const RECT& rcCorners, bool alphaChannel, BYTE uFade, bool hole, bool xtiled, bool ytiled)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->DrawImage(ImgDate,rc,rcPaint,rcBmpPart,rcCorners,alphaChannel,uFade,hole,xtiled,ytiled);
    }

	void CUIRenderCore::GetClipBox(RECT& rcClip)
	{
		if(m_pIRenderCore)
			m_pIRenderCore->GetClipBox(rcClip);
	}

	void CUIRenderCore::Resize()
	{
		if(m_pIRenderCore)
			return m_pIRenderCore->Resize();
	}

    TImageInfo* CUIRenderCore::LoadImage( STRINGorID bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/ )
    {
        return NULL;
    }

	TImageInfo* CUIRenderCore::LoadImage(HBITMAP hBitmap,int nWidth, int nHeight, bool bAlpha)
	{
		return NULL;
	}

    TFontInfo* CUIRenderCore::GetDefaultFontInfo()
    {
        if(m_pIRenderCore)
            return m_pIRenderCore->GetDefaultFontInfo();
		return NULL;
    }

    void CUIRenderCore::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
    {
    }

    TFontInfo* CUIRenderCore::AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
    {
		return NULL;
    }

	void CUIRenderCore::GetTextBounds(int& nWidth,int& nHeight,CPaintManagerUI* pManager, LPCTSTR pstrText, int iFont, UINT uStyle)
	{
		if(m_pIRenderCore)
		{
			TFontInfo *pFontInfo = pManager->GetFontInfo(iFont);
			m_pIRenderCore->GetTextBounds(nWidth,nHeight,pFontInfo,pstrText,uStyle);
		}
	}

    void CUIRenderCore::DrawText(CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle,BOOL bBlur)
    {
		if(m_pIRenderCore)
		{
			TFontInfo *pFontInfo = pManager->GetFontInfo(iFont);
			if(bBlur)
			{
				int nWidht = rc.right-rc.left;
				int nHeight= rc.bottom-rc.top;
				Bitmap bmp(nWidht,nHeight,PixelFormat32bppARGB);
				Graphics canvas(&bmp);
				HDC hDcOffScreen = m_pIRenderCore->GetRenderHDC();

				m_pIRenderCore->DCPushClip(hDcOffScreen);

				Graphics gs(hDcOffScreen);
				Font myFont(hDcOffScreen,pFontInfo->hFont);
				RectF layoutRect(0,0,static_cast<REAL>(nWidht),static_cast<REAL>(nHeight));
				StringFormat format;
				StringAlignment align = StringAlignmentNear;
				if ((uStyle & DT_CENTER) == DT_CENTER)
					align = StringAlignmentCenter; 
				else if ((uStyle & DT_RIGHT) == DT_RIGHT)
					align = StringAlignmentFar;
				format.SetAlignment(align);
				format.SetLineAlignment(StringAlignmentCenter);
				if(uStyle & DT_END_ELLIPSIS)format.SetTrimming( StringTrimmingEllipsisCharacter );
				if(uStyle & DT_SINGLELINE) format.SetFormatFlags( StringFormatFlagsNoWrap );
				Color textcolor(dwTextColor);
				if(textcolor.GetAlpha() == 255 || textcolor.GetAlpha() == 0)
					textcolor = Color(254,textcolor.GetR(),textcolor.GetG(),textcolor.GetB());
				SolidBrush textBrush(textcolor);
				canvas.SetSmoothingMode(SmoothingModeAntiAlias);
				canvas.DrawString(pstrText,-1,&myFont,layoutRect,&format,&textBrush);
				BitmapData data;
				Rect rcLock(0,0,nWidht,nHeight);
				bmp.LockBits(&rcLock,ImageLockModeRead|ImageLockModeWrite,PixelFormat32bppARGB,&data);
				RECT rcBlur = {0,0,nWidht,nHeight};
				CGenerAlgorithm::ShadowBlur(&data.Scan0,data.Stride,0xffffff,rcBlur,5);
				bmp.UnlockBits(&data);
				canvas.DrawString(pstrText,-1,&myFont,layoutRect,&format,&textBrush);
				gs.DrawImage(&bmp,rc.left,rc.top);
				m_pIRenderCore->DCPopClip(hDcOffScreen);
				m_pIRenderCore->FreeRenderHDC();
			}
			else
			{
				m_pIRenderCore->DCPushClip();
				m_pIRenderCore->DrawText(pFontInfo,rc,pstrText,dwTextColor,uStyle);
				m_pIRenderCore->DCPopClip();
			}
		}
    }

	void CUIRenderCore::_DrawHtmlText(CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle,HDC hDC)
	{
		if(!m_pIRenderCore) return;
		bool bLayered = m_pIRenderCore->IsWindowLayered();
		bool bGlass = m_pIRenderCore->IsWindowGlass();

		if(!hDC) return;
		Font *pGDIPFont = NULL;
		Color *pTextColor = NULL;
		SolidBrush *pTextBrush = NULL;
		if( pstrText == NULL || pManager == NULL ) return;
		if( ::IsRectEmpty(&rc) ) return;

		bool bDraw = (uStyle & DT_CALCRECT) == 0;

		CStdPtrArray aFontArray(10);
		CStdPtrArray aColorArray(10);
		CStdPtrArray aPIndentArray(10);


		TEXTMETRIC* pTm = &pManager->GetDefaultFontInfo()->tm;
		HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->GetDefaultFontInfo()->hFont);
		if(bLayered || bGlass)
		{
			SAFE_DELETE(pGDIPFont);
			pGDIPFont = new Font(hDC, pManager->GetDefaultFontInfo()->hFont);
		}
		::SetBkMode(hDC, TRANSPARENT);

		::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
		if(bLayered || bGlass)
		{
			SAFE_DELETE(pTextColor);
			pTextColor = new Color(dwTextColor);
			if(pTextColor->GetAlpha() == 255 || pTextColor->GetAlpha() == 0)
			{
				int R = pTextColor->GetR();
				int G = pTextColor->GetG();
				int B = pTextColor->GetB();
				SAFE_DELETE(pTextColor);
				pTextColor = new Color(254,R,G,B);
			}
			SAFE_DELETE(pTextBrush);
			pTextBrush = new SolidBrush(*pTextColor);
		}
		DWORD dwBkColor = pManager->GetDefaultSelectedBkColor();
		::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));
		Graphics gs(hDC);
		gs.SetSmoothingMode(SmoothingModeAntiAlias);
		if( ((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0 || (uStyle & DT_VCENTER) != 0 || (uStyle & DT_BOTTOM) != 0) && (uStyle & DT_CALCRECT) == 0 ) {
			RECT rcText = { 0, 0, 9999, 100 };
			int nLinks = 0;
			_DrawHtmlText(pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, uStyle | DT_CALCRECT,hDC);
			if( (uStyle & DT_SINGLELINE) != 0 ){
				if( (uStyle & DT_CENTER) != 0 ) {
					rc.left = rc.left + ((rc.right - rc.left) / 2) - ((rcText.right - rcText.left) / 2);
					rc.right = rc.left + (rcText.right - rcText.left);
				}
				if( (uStyle & DT_RIGHT) != 0 ) {
					rc.left = rc.right - (rcText.right - rcText.left);
				}
			}
			if( (uStyle & DT_VCENTER) != 0 ) {
				rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
				rc.bottom = rc.top + (rcText.bottom - rcText.top);
			}
			if( (uStyle & DT_BOTTOM) != 0 ) {
				rc.top = rc.bottom - (rcText.bottom - rcText.top);
			}
		}

		bool bHoverLink = false;
		CDuiString sHoverLink;
		POINT ptMouse = pManager->GetMousePos();
		for( int i = 0; !bHoverLink && i < nLinkRects; i++ ) {
			if( ::PtInRect(prcLinks + i, ptMouse) ) {
				sHoverLink = *(CDuiString*)(sLinks + i);
				bHoverLink = true;
			}
		}

		POINT pt = { rc.left, rc.top };
		int iLinkIndex = 0;
		int cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
		int cyMinHeight = 0;
		int cxMaxWidth = 0;
		POINT ptLinkStart = { 0 };
		bool bLineEnd = false;
		bool bInRaw = false;
		bool bInLink = false;
		bool bInSelected = false;
		int iLineLinkIndex = 0;
		CStdPtrArray aLineFontArray;
		CStdPtrArray aLineColorArray;
		CStdPtrArray aLinePIndentArray;
		LPCTSTR pstrLineBegin = pstrText;
		bool bLineInRaw = false;
		bool bLineInLink = false;
		bool bLineInSelected = false;
		int cyLineHeight = 0;
		bool bLineDraw = false; // 行的第二阶段：绘制
		while( *pstrText != _T('\0') ) {
			if( pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd ) {
				if( *pstrText == _T('\n') ) pstrText++;
				if( bLineEnd ) bLineEnd = false;
				if( !bLineDraw ) {
					if( bInLink && iLinkIndex < nLinkRects ) {
						::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + cyLine);
						CDuiString *pStr1 = (CDuiString*)(sLinks + iLinkIndex - 1);
						CDuiString *pStr2 = (CDuiString*)(sLinks + iLinkIndex);
						*pStr2 = *pStr1;
					}
					for( int i = iLineLinkIndex; i < iLinkIndex; i++ ) {
						prcLinks[i].bottom = pt.y + cyLine;
					}
					if( bDraw ) {
						bInLink = bLineInLink;
						iLinkIndex = iLineLinkIndex;
					}
				}
				else {
					if( bInLink && iLinkIndex < nLinkRects ) iLinkIndex++;
					bLineInLink = bInLink;
					iLineLinkIndex = iLinkIndex;
				}
				if( (uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw) ) break;
				if( bDraw ) bLineDraw = !bLineDraw; // !
				pt.x = rc.left;
				if( !bLineDraw ) pt.y += cyLine;
				if( pt.y > rc.bottom && bDraw ) break;
				ptLinkStart = pt;
				cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
				if( pt.x >= rc.right ) break;
			}
			else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') )
				&& ( pstrText[1] >= _T('a') && pstrText[1] <= _T('z') )
				&& ( pstrText[2] == _T(' ') || pstrText[2] == _T('>') || pstrText[2] == _T('}') ) ) {
					pstrText++;
					LPCTSTR pstrNextStart = NULL;
					switch( *pstrText ) {
					case _T('a'):  // Link
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							if( iLinkIndex < nLinkRects && !bLineDraw ) {
								CDuiString *pStr = (CDuiString*)(sLinks + iLinkIndex);
								pStr->Empty();
								while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
									LPCTSTR pstrTemp = ::CharNext(pstrText);
									while( pstrText < pstrTemp) {
										*pStr += *pstrText++;
									}
								}
							}

							DWORD clrColor = pManager->GetDefaultLinkFontColor();
							if( bHoverLink && iLinkIndex < nLinkRects ) {
								CDuiString *pStr = (CDuiString*)(sLinks + iLinkIndex);
								if( sHoverLink == *pStr ) clrColor = pManager->GetDefaultLinkHoverFontColor();
							}
							aColorArray.Add((LPVOID)clrColor);
							::SetTextColor(hDC,  RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
							if(bLayered || bGlass)
							{
								SAFE_DELETE(pTextColor);
								pTextColor = new Color(clrColor);
								if(pTextColor->GetAlpha() == 255 || pTextColor->GetAlpha() == 0)
								{
									int R = pTextColor->GetR();
									int G = pTextColor->GetG();
									int B = pTextColor->GetB();
									SAFE_DELETE(pTextColor);
									pTextColor = new Color(254,R,G,B);
								}
								SAFE_DELETE(pTextBrush);
								pTextBrush = new SolidBrush(*pTextColor);
							}
							TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
							if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
							if( pFontInfo->bUnderline == false ) {
								TFontInfo* pFontAdded = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								if( pFontAdded == NULL ) pFontAdded = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								aFontArray.Add(pFontAdded);
								pTm = &pFontAdded->tm;
								::SelectObject(hDC, pFontAdded->hFont);
								if(bLayered || bGlass)
								{
									SAFE_DELETE(pGDIPFont);
									pGDIPFont = new Font(hDC, pFontAdded->hFont);
								}
								cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							}
							ptLinkStart = pt;
							bInLink = true;
						}
						break;
					case _T('b'):  // Bold
						{
							pstrText++;
							TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
							if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
							if( pFontInfo->bBold == false ) {
								TFontInfo* pFontAdded = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
								if( pFontAdded == NULL ) pFontAdded = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
								aFontArray.Add(pFontAdded);
								pTm = &pFontAdded->tm;
								::SelectObject(hDC, pFontAdded->hFont);
								if(bLayered || bGlass)
								{
									SAFE_DELETE(pGDIPFont);
									pGDIPFont = new Font(hDC, pFontAdded->hFont);
								}
								cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							}
						}
						break;
					case _T('c'):  // Color
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							if( *pstrText == _T('#')) pstrText++;
							DWORD clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
							aColorArray.Add((LPVOID)clrColor);
							::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
							if(bLayered || bGlass)
							{
								SAFE_DELETE(pTextColor);
								pTextColor = new Color(clrColor);
								if(pTextColor->GetAlpha() == 255 || pTextColor->GetAlpha() == 0)
								{
									int R = pTextColor->GetR();
									int G = pTextColor->GetG();
									int B = pTextColor->GetB();
									SAFE_DELETE(pTextColor);
									pTextColor = new Color(254,R,G,B);
								}
								SAFE_DELETE(pTextBrush);
								pTextBrush = new SolidBrush(*pTextColor);
							}
						}
						break;
					case _T('f'):  // Font
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							LPCTSTR pstrTemp = pstrText;
							int iFont = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
							if( pstrTemp != pstrText ) {
								TFontInfo* pFontInfo = pManager->GetFontInfo(iFont);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
								if(bLayered || bGlass)
								{
									SAFE_DELETE(pGDIPFont);
									pGDIPFont = new Font(hDC, pFontInfo->hFont);
								}
							}
							else {
								CDuiString sFontName;
								int iFontSize = 10;
								CDuiString sFontAttr;
								bool bBold = false;
								bool bUnderline = false;
								bool bItalic = false;
								while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
									pstrTemp = ::CharNext(pstrText);
									while( pstrText < pstrTemp) {
										sFontName += *pstrText++;
									}
								}
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								if( isdigit(*pstrText) ) {
									iFontSize = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
								}
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
									pstrTemp = ::CharNext(pstrText);
									while( pstrText < pstrTemp) {
										sFontAttr += *pstrText++;
									}
								}
								sFontAttr.MakeLower();
								if( sFontAttr.Find(_T("bold")) >= 0 ) bBold = true;
								if( sFontAttr.Find(_T("underline")) >= 0 ) bUnderline = true;
								if( sFontAttr.Find(_T("italic")) >= 0 ) bItalic = true;
								TFontInfo *pFontInfo = pManager->GetFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
								if( pFontInfo == NULL ) pFontInfo = pManager->AddFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
								if(bLayered || bGlass)
								{
									SAFE_DELETE(pGDIPFont);
									pGDIPFont = new Font(hDC, pFontInfo->hFont);
								}
							}
							cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
						}
						break;
					case _T('i'):  // Italic or Image
						{    
							pstrNextStart = pstrText - 1;
							pstrText++;
							CDuiString sImageString = pstrText;
							int iWidth = 0;
							int iHeight = 0;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							const TImageInfo* pImageInfo = NULL;
							CDuiString sName;
							while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
								LPCTSTR pstrTemp = ::CharNext(pstrText);
								while( pstrText < pstrTemp) {
									sName += *pstrText++;
								}
							}
							if( sName.IsEmpty() ) { // Italic
								pstrNextStart = NULL;
								TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
								if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
								if( pFontInfo->bItalic == false ) {
									TFontInfo* pFontAdded = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
									if( pFontAdded == NULL ) pFontAdded = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
									aFontArray.Add(pFontAdded);
									pTm = &pFontAdded->tm;
									::SelectObject(hDC, pFontAdded->hFont);
									if(bLayered || bGlass)
									{
										SAFE_DELETE(pGDIPFont);
										pGDIPFont = new Font(hDC, pFontAdded->hFont);
									}
									cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
								}
							}
							else {
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								int iImageListNum = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
								if( iImageListNum <= 0 ) iImageListNum = 1;
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								int iImageListIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
								if( iImageListIndex < 0 || iImageListIndex >= iImageListNum ) iImageListIndex = 0;

								if( _tcsstr(sImageString.GetData(), _T("file=\'")) != NULL || _tcsstr(sImageString.GetData(), _T("res=\'")) != NULL ) {
									CDuiImage mImageName;
									mImageName.SetImage(sImageString.GetData());
									pImageInfo = pManager->GetImageEx(mImageName,mImageName.GetRes());
								}
								else
								{
									CDuiImage mImageName;
									mImageName.SetImage(sName.GetData());
									pImageInfo = pManager->GetImageEx(mImageName);
								}
								if( pImageInfo ) {
									iWidth = pImageInfo->nX;
									iHeight = pImageInfo->nY;
									if( iImageListNum > 1 ) iWidth /= iImageListNum;

									if( pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0 ) {
										bLineEnd = true;
									}
									else {
										pstrNextStart = NULL;
										if( bDraw && bLineDraw ) {
											CDuiRect rcImage(pt.x, pt.y + cyLineHeight - iHeight, pt.x + iWidth, pt.y + cyLineHeight);
											if( iHeight < cyLineHeight ) { 
												rcImage.bottom -= (cyLineHeight - iHeight) / 2;
												rcImage.top = rcImage.bottom -  iHeight;
											}
											CDuiRect rcBmpPart(0, 0, iWidth, iHeight);
											rcBmpPart.left = iWidth * iImageListIndex;
											rcBmpPart.right = iWidth * (iImageListIndex + 1);
											CDuiRect rcCorner(0, 0, 0, 0);
											DrawImage( pImageInfo, rcImage, rcImage, rcBmpPart, rcCorner,pImageInfo->alphaChannel, 255);
										}

										cyLine = MAX(iHeight, cyLine);
										pt.x += iWidth;
										cyMinHeight = pt.y + iHeight;
										cxMaxWidth = MAX(cxMaxWidth, pt.x);
									}
								}
								else pstrNextStart = NULL;
							}
						}
						break;
					case _T('n'):  // Newline
						{
							pstrText++;
							if( (uStyle & DT_SINGLELINE) != 0 ) break;
							bLineEnd = true;
						}
						break;
					case _T('p'):  // Paragraph
						{
							pstrText++;
							if( pt.x > rc.left ) bLineEnd = true;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							int cyLineExtra = (int)_tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
							aPIndentArray.Add((LPVOID)cyLineExtra);
							cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra);
						}
						break;
					case _T('r'):  // Raw Text
						{
							pstrText++;
							bInRaw = true;
						}
						break;
					case _T('s'):  // Selected text background color
						{
							pstrText++;
							bInSelected = !bInSelected;
							if( bDraw && bLineDraw ) {
								if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
								else ::SetBkMode(hDC, TRANSPARENT);
							}
						}
						break;
					case _T('u'):  // Underline text
						{
							pstrText++;
							TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
							if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
							if( pFontInfo->bUnderline == false ) {
								TFontInfo* pFontAdded = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								if( pFontAdded == NULL ) pFontAdded = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								aFontArray.Add(pFontAdded);
								pTm = &pFontAdded->tm;
								::SelectObject(hDC, pFontAdded->hFont);
								if(bLayered || bGlass)
								{
									SAFE_DELETE(pGDIPFont);
									pGDIPFont = new Font(hDC, pFontAdded->hFont);
								}
								cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							}
						}
						break;
					case _T('x'):  // X Indent
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							int iWidth = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
							pt.x += iWidth;
							cxMaxWidth = MAX(cxMaxWidth, pt.x);
						}
						break;
					case _T('y'):  // Y Indent
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							cyLine = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						}
						break;
					}
					if( pstrNextStart != NULL ) pstrText = pstrNextStart;
					else {
						while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
						pstrText = ::CharNext(pstrText);
					}
			}
			else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') ) && pstrText[1] == _T('/') )
			{
				pstrText++;
				pstrText++;
				switch( *pstrText )
				{
				case _T('c'):
					{
						pstrText++;
						aColorArray.Remove(aColorArray.GetSize() - 1);
						DWORD clrColor = dwTextColor;
						if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
						::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
						if(bLayered || bGlass)
						{
							SAFE_DELETE(pTextColor);
							pTextColor = new Color(clrColor);
							if(pTextColor->GetAlpha() == 255 || pTextColor->GetAlpha() == 0)
							{
								int R = pTextColor->GetR();
								int G = pTextColor->GetG();
								int B = pTextColor->GetB();
								SAFE_DELETE(pTextColor);
								pTextColor = new Color(254,R,G,B);
							}
							SAFE_DELETE(pTextBrush);
							pTextBrush = new SolidBrush(*pTextColor);
						}
					}
					break;
				case _T('p'):
					pstrText++;
					if( pt.x > rc.left ) bLineEnd = true;
					aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
					cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
					break;
				case _T('s'):
					{
						pstrText++;
						bInSelected = !bInSelected;
						if( bDraw && bLineDraw ) {
							if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
							else ::SetBkMode(hDC, TRANSPARENT);
						}
					}
					break;
				case _T('a'):
					{
						if( iLinkIndex < nLinkRects ) {
							if( !bLineDraw ) ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + pTm->tmHeight + pTm->tmExternalLeading);
							iLinkIndex++;
						}
						aColorArray.Remove(aColorArray.GetSize() - 1);
						DWORD clrColor = dwTextColor;
						if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
						::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
						if(bLayered || bGlass)
						{
							SAFE_DELETE(pTextColor);
							pTextColor = new Color(clrColor);
							if(pTextColor->GetAlpha() == 255 || pTextColor->GetAlpha() == 0)
							{
								int R = pTextColor->GetR();
								int G = pTextColor->GetG();
								int B = pTextColor->GetB();
								SAFE_DELETE(pTextColor);
								pTextColor = new Color(254,R,G,B);
							}
							SAFE_DELETE(pTextBrush);
							pTextBrush = new SolidBrush(*pTextColor);
						}
						bInLink = false;
					}
				case _T('b'):
				case _T('f'):
				case _T('i'):
				case _T('u'):
					{
						pstrText++;
						aFontArray.Remove(aFontArray.GetSize() - 1);
						TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
						if( pFontInfo == NULL ) pFontInfo = pManager->GetDefaultFontInfo();
						if( pTm->tmItalic && pFontInfo->bItalic == false ) {
							ABC abc;
							::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
							pt.x += abc.abcC / 2; // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
						}
						pTm = &pFontInfo->tm;
						::SelectObject(hDC, pFontInfo->hFont);
						if(bLayered || bGlass)
						{
							SAFE_DELETE(pGDIPFont);
							pGDIPFont = new Font(hDC, pFontInfo->hFont);
						}
						cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
					}
					break;
				}
				while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
				pstrText = ::CharNext(pstrText);
			}
			else if( !bInRaw &&  *pstrText == _T('<') && pstrText[2] == _T('>') && (pstrText[1] == _T('{')  || pstrText[1] == _T('}')) )
			{
				SIZE szSpace = { 0 };
				::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
				if( bDraw && bLineDraw)
				{
					if(bLayered || bGlass)
					{
						PointF ptOrign(pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
						gs.DrawString(&pstrText[1],1,pGDIPFont,ptOrign,pTextBrush);
					}
					else
						::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
				}
				pt.x += szSpace.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText++;pstrText++;pstrText++;
			}
			else if( !bInRaw &&  *pstrText == _T('{') && pstrText[2] == _T('}') && (pstrText[1] == _T('<')  || pstrText[1] == _T('>')) )
			{
				SIZE szSpace = { 0 };
				::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
				if( bDraw && bLineDraw ) 
				{	
					if(bLayered || bGlass)
					{
						PointF ptOrign(pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
						gs.DrawString(&pstrText[1],1,pGDIPFont,ptOrign,pTextBrush);
					}
					else
						::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
				}
				pt.x += szSpace.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText++;pstrText++;pstrText++;
			}
			else if( !bInRaw &&  *pstrText == _T(' ') )
			{
				SIZE szSpace = { 0 };
				::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
				if( bDraw && bLineDraw ) 
				{
					if(bLayered || bGlass)
					{
						PointF ptOrign(pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
						gs.DrawString(_T(""),1,pGDIPFont,ptOrign,pTextBrush);
					}
					else
						::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, _T(" "), 1);
				}
				pt.x += szSpace.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText++;
			}
			else
			{
				POINT ptPos = pt;
				int cchChars = 0;
				int cchSize = 0;
				int cchLastGoodWord = 0;
				int cchLastGoodSize = 0;
				LPCTSTR p = pstrText;
				LPCTSTR pstrNext;
				SIZE szText = { 0 };
				if( !bInRaw && *p == _T('<') || *p == _T('{') ) p++, cchChars++, cchSize++;
				while( *p != _T('\0') && *p != _T('\n') ) {
					if( bInRaw ) {
						if( ( *p == _T('<') || *p == _T('{') ) && p[1] == _T('/') 
							&& p[2] == _T('r') && ( p[3] == _T('>') || p[3] == _T('}') ) ) {
								p += 4;
								bInRaw = false;
								break;
						}
					}
					else {
						if( *p == _T('<') || *p == _T('{') ) break;
					}
					pstrNext = ::CharNext(p);
					cchChars++;
					cchSize += (int)(pstrNext - p);
					szText.cx = cchChars * pTm->tmMaxCharWidth;
					if( pt.x + szText.cx >= rc.right ) {
						::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
					}
					if( pt.x + szText.cx > rc.right ) {
						if( pt.x + szText.cx > rc.right && pt.x != rc.left) {
							cchChars--;
							cchSize -= (int)(pstrNext - p);
						}
						if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
							cchChars = cchLastGoodWord;
							cchSize = cchLastGoodSize;                 
						}
						if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0 ) {
							cchChars -= 1;
							LPCTSTR pstrPrev = ::CharPrev(pstrText, p);
							if( cchChars > 0 ) {
								cchChars -= 1;
								pstrPrev = ::CharPrev(pstrText, pstrPrev);
								cchSize -= (int)(p - pstrPrev);
							}
							else 
								cchSize -= (int)(p - pstrPrev);
							pt.x = rc.right;
						}
						bLineEnd = true;
						cxMaxWidth = MAX(cxMaxWidth, pt.x);
						break;
					}
					if (!( ( p[0] >= _T('a') && p[0] <= _T('z') ) || ( p[0] >= _T('A') && p[0] <= _T('Z') ) )) {
						cchLastGoodWord = cchChars;
						cchLastGoodSize = cchSize;
					}
					if( *p == _T(' ') ) {
						cchLastGoodWord = cchChars;
						cchLastGoodSize = cchSize;
					}
					p = ::CharNext(p);
				}

				::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
				if( bDraw && bLineDraw ) {
					if( (uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_CENTER) != 0 ) {
						ptPos.x += (rc.right - rc.left - szText.cx)/2;
					}
					else if( (uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_RIGHT) != 0) {
						ptPos.x += (rc.right - rc.left - szText.cx);
					}
					if(bLayered || bGlass)
					{
						PointF ptOrign(ptPos.x, ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
						gs.DrawString(pstrText,cchSize,pGDIPFont,ptOrign,pTextBrush);
					}
					else
						::TextOut(hDC, ptPos.x, ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, pstrText, cchSize);
					if( pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) 
					{
						if(bLayered || bGlass)
						{
							PointF ptOrign(ptPos.x + szText.cx, ptPos.y);
							gs.DrawString(_T("..."),3,pGDIPFont,ptOrign,pTextBrush);
						}
						else
							::TextOut(hDC, ptPos.x + szText.cx, ptPos.y, _T("..."), 3);
					}
				}
				pt.x += szText.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText += cchSize;
			}

			if( pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0') ) bLineEnd = true;
			if( bDraw && bLineEnd ) {
				if( !bLineDraw ) {
					aFontArray.Resize(aLineFontArray.GetSize());
					::CopyMemory(aFontArray.GetData(), aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID));
					aColorArray.Resize(aLineColorArray.GetSize());
					::CopyMemory(aColorArray.GetData(), aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID));
					aPIndentArray.Resize(aLinePIndentArray.GetSize());
					::CopyMemory(aPIndentArray.GetData(), aLinePIndentArray.GetData(), aLinePIndentArray.GetSize() * sizeof(LPVOID));

					cyLineHeight = cyLine;
					pstrText = pstrLineBegin;
					bInRaw = bLineInRaw;
					bInSelected = bLineInSelected;

					DWORD clrColor = dwTextColor;
					if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
					::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
					if(bLayered || bGlass)
					{
						SAFE_DELETE(pTextColor);
						pTextColor = new Color(clrColor);
						if(pTextColor->GetAlpha() == 255 || pTextColor->GetAlpha() == 0)
						{
							int R = pTextColor->GetR();
							int G = pTextColor->GetG();
							int B = pTextColor->GetB();
							SAFE_DELETE(pTextColor);
							pTextColor = new Color(254,R,G,B);
						}
						SAFE_DELETE(pTextBrush);
						pTextBrush = new SolidBrush(*pTextColor);
					}
					TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
					if( pFontInfo == NULL ) pFontInfo = pManager->GetDefaultFontInfo();
					pTm = &pFontInfo->tm;
					::SelectObject(hDC, pFontInfo->hFont);
					if(bLayered || bGlass)
					{
						SAFE_DELETE(pGDIPFont);
						pGDIPFont = new Font(hDC, pFontInfo->hFont);
					}
					if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
				}
				else {
					aLineFontArray.Resize(aFontArray.GetSize());
					::CopyMemory(aLineFontArray.GetData(), aFontArray.GetData(), aFontArray.GetSize() * sizeof(LPVOID));
					aLineColorArray.Resize(aColorArray.GetSize());
					::CopyMemory(aLineColorArray.GetData(), aColorArray.GetData(), aColorArray.GetSize() * sizeof(LPVOID));
					aLinePIndentArray.Resize(aPIndentArray.GetSize());
					::CopyMemory(aLinePIndentArray.GetData(), aPIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID));
					pstrLineBegin = pstrText;
					bLineInSelected = bInSelected;
					bLineInRaw = bInRaw;
				}
			}

			ASSERT(iLinkIndex<=nLinkRects);
		}

		nLinkRects = iLinkIndex;

		// Return size of text when requested
		if( (uStyle & DT_CALCRECT) != 0 ) {
			rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
			rc.right = MIN(rc.right, cxMaxWidth);
		}

		::SelectObject(hDC, hOldFont);

		SAFE_DELETE(pTextColor);
		SAFE_DELETE(pTextBrush);
		SAFE_DELETE(pGDIPFont);
	}

    void CUIRenderCore::DrawHtmlText(CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle ,BOOL bBlur)
    {
        // 考虑到在xml编辑器中使用<>符号不方便，可以使用{}符号代替
        // 支持标签嵌套（如<l><b>text</b></l>），但是交叉嵌套是应该避免的（如<l><b>text</l></b>）
        // The string formatter supports a kind of "mini-html" that consists of various short tags:
        //
        //   Bold:             <b>text</b>
        //   Color:            <c #xxxxxx>text</c>  where x = RGB in hex
        //   Font:             <f x>text</f>        where x = font id
        //   Italic:           <i>text</i>
        //   Image:            <i x y z>            where x = image name and y = imagelist num and z(optional) = imagelist id
        //   Link:             <a x>text</a>        where x(optional) = link content, normal like app:notepad or http:www.xxx.com
        //   NewLine           <n>                  
        //   Paragraph:        <p x>text</p>        where x = extra pixels indent in p
        //   Raw Text:         <r>text</r>
        //   Selected:         <s>text</s>
        //   Underline:        <u>text</u>
        //   X Indent:         <x i>                where i = hor indent in pixels
        //   Y Indent:         <y i>                where i = ver indent in pixels 
		if(!m_pIRenderCore) return;
		HDC hDC = m_pIRenderCore->GetRenderHDC();
		m_pIRenderCore->DCPushClip(hDC);
		if(!hDC)
		{
			m_pIRenderCore->DCPopClip(hDC);
			m_pIRenderCore->FreeRenderHDC();
			return;
		}

		if(bBlur)
		{
			Graphics gs(hDC);

			int nWidht = rc.right-rc.left;
			int nHeight= rc.bottom-rc.top;
			Bitmap bmp(nWidht,nHeight,PixelFormat32bppARGB);
			Graphics canvas(&bmp);

			HDC hDcOffScreen = canvas.GetHDC();
			RECT rcDraw = {0,0,nWidht,nHeight};
			_DrawHtmlText(pManager,rcDraw,pstrText,dwTextColor,prcLinks,sLinks,nLinkRects,uStyle,hDcOffScreen);
			canvas.ReleaseHDC(hDcOffScreen);

			BitmapData data;
			Rect rcLock(0,0,nWidht,nHeight);
			bmp.LockBits(&rcLock,ImageLockModeRead|ImageLockModeWrite,PixelFormat32bppARGB,&data);
			RECT rcBlur = {0,0,nWidht,nHeight};
			CGenerAlgorithm::ShadowBlur(&data.Scan0,data.Stride,0xffffff,rcBlur,5);
			bmp.UnlockBits(&data);

			hDcOffScreen = canvas.GetHDC();
			_DrawHtmlText(pManager,rcDraw,pstrText,dwTextColor,prcLinks,sLinks,nLinkRects,uStyle,hDcOffScreen);
			canvas.ReleaseHDC(hDcOffScreen);

			gs.DrawImage(&bmp,rc.left,rc.top);
		}
		else
			_DrawHtmlText(pManager,rc,pstrText,dwTextColor,prcLinks,sLinks,nLinkRects,uStyle,hDC);
		m_pIRenderCore->DCPopClip(hDC);
		m_pIRenderCore->FreeRenderHDC();
    }

    void CUIRenderCore::DrawColor( const RECT& rc, DWORD color )
    {
		if(m_pIRenderCore)
			m_pIRenderCore->DrawColor(rc,color);
    }

    void CUIRenderCore::DrawGradient( const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical )
    {
		if(m_pIRenderCore)
			m_pIRenderCore->DrawGradient(rc,dwFirst,dwSecond,bVertical);
    }

    void CUIRenderCore::DrawLine(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
    {
		if(m_pIRenderCore)
			m_pIRenderCore->DrawLine(rc,nSize,dwPenColor,nStyle);
    }

    void CUIRenderCore::DrawRect(const RECT& rc, int nSize, DWORD dwPenColor )
    {
		if(m_pIRenderCore)
			m_pIRenderCore->DrawRect(rc,nSize,dwPenColor);
    }

    void CUIRenderCore::DrawRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
    {
		if(m_pIRenderCore)
			m_pIRenderCore->DrawRoundRect(rc,width,height,nSize,dwPenColor);
    }

	void CUIRenderCore::DrawRoundDotRect( const RECT& rc,int width,int height,int nSize, DWORD dwPenColor )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawRoundDotRect(rc,width,height,nSize,dwPenColor);
	}

	void CUIRenderCore::FillRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->FillRoundRect(rc,width,height,nSize,dwPenColor);
	}

	void CUIRenderCore::DrawCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor)
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawCircle(nCenterX,nCenterY,nRadius,dwColor);
	}

	void CUIRenderCore::FillCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->FillCircle(nCenterX,nCenterY,nRadius,dwColor);
	}

	void CUIRenderCore::DrawArc( int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor)
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawArc(nCenterX,nCenterY,nRadius,fStartAngle,fSweepAngle,dwColor);
	}
}