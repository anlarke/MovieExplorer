#pragma once

class CToolBarButton : public RCustomToolBarButton
{
	friend class RWindow;

public:
	CToolBarButton()
	{
	}

	~CToolBarButton()
	{
	}

protected:
	virtual void PrepareDraw()
	{
		int cx, cy;
		m_mdc.GetDimensions(&cx, &cy);

		if (cx < SCX(10) || cy < SCX(10))
			return;

		// Let base class create default windows sprites

		RCustomToolBarButton::PrepareDraw();

		// Normal sprite (not used)

		m_sprNormal.Destroy();

		// Highlight sprite

		if (!GETTHEMEBOOL(_T("ToolBarButton"), _T("HighlightWindowsDefault")))
		{
			int r = GETTHEMEINT(_T("ToolBarButton"), _T("HighlightRadius"));
			m_sprHighlight.Create(cy, cy, SCX(r+1), 0, SCX(r+1), 0);
			
			DrawRoundedRectGradientV(m_sprHighlight, 0, 0, cy, cy, SCX(r), 
					GETTHEMECOLOR(_T("ToolBarButton"), _T("HighlightTopColor")),
					GETTHEMECOLOR(_T("ToolBarButton"), _T("HighlightBottomColor")),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("HighlightTopAlpha")) * 255.0f),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("HighlightBottomAlpha")) * 255.0f));

			DrawRoundedRect(m_sprHighlight, 0, 0, cy, cy, SCX(r),
					GETTHEMECOLOR(_T("ToolBarButton"), _T("HighlightBorder1Color")),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("HighlightBorder1Alpha")) * 255.0f), false);

			DrawRoundedRect(m_sprHighlight, SCX(1), SCY(1), cy - 2 * SCX(1), cy - 2 * SCY(1), SCX(r),
					GETTHEMECOLOR(_T("ToolBarButton"), _T("HighlightBorder2Color")),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("HighlightBorder2Alpha")) * 255.0f), false);
		}

		// Pressed sprite

		if (!GETTHEMEBOOL(_T("ToolBarButton"), _T("PressedWindowsDefault")))
		{
			int r = GETTHEMEINT(_T("ToolBarButton"), _T("PressedRadius"));
			m_sprPressed.Create(cy, cy, SCX(r+1), 0, SCX(r+1), 0);
			
			DrawRoundedRectGradientV(m_sprPressed, 0, 0, cy, cy, SCX(r), 
					GETTHEMECOLOR(_T("ToolBarButton"), _T("PressedTopColor")),
					GETTHEMECOLOR(_T("ToolBarButton"), _T("PressedBottomColor")),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("PressedTopAlpha")) * 255.0f),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("PressedBottomAlpha")) * 255.0f));

			DrawRoundedRect(m_sprPressed, 0, 0, cy, cy, SCX(r),
					GETTHEMECOLOR(_T("ToolBarButton"), _T("PressedBorder1Color")),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("PressedBorder1Alpha")) * 255.0f), false);

			DrawRoundedRect(m_sprPressed, SCX(1), SCY(1), cy - 2 * SCX(1), cy - 2 * SCY(1), SCX(r),
					GETTHEMECOLOR(_T("ToolBarButton"), _T("PressedBorder2Color")),
					roundf(GETTHEMEFLOAT(_T("ToolBarButton"), _T("PressedBorder2Alpha")) * 255.0f), false);
		}
	}
};
