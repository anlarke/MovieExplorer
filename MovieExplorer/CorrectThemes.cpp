#include "stdafx.h"
#include "MovieExplorer.h"

bool CorrectThemes()
{
#ifdef _DEBUG
	RemoveDirectory(CorrectPath(_T("Themes")), true);
#endif

	// Create themes folder if it does not exist

	if (!DirectoryExists(CorrectPath(_T("Themes"))) && !CreateDirectory(CorrectPath(_T("Themes"))))
		ASSERTRETURN(false);

	RThemeMgr2 themeMgr;
	
	// Correct dark theme

	themeMgr.Open(CorrectPath(_T("Themes\\Dark.xml")));
	themeMgr.SetStr(_T("Name"), _T("Dark"), false);
	themeMgr.SetStr(_T("Author"), _T(""), false);

	themeMgr.SetStr(_T("ReBar"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ReBar"), _T("BackgrTopColor"), _T("#343434"), false);
	themeMgr.SetStr(_T("ReBar"), _T("BackgrBottomColor"), _T("#2a2a2a"), false);
	themeMgr.SetStr(_T("ReBar"), _T("BackgrLineColor"), _T("#323232"), false);

	themeMgr.SetStr(_T("ListView"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("BackgrColor"), _T("#171717"), false);
	themeMgr.SetStr(_T("ListView"), _T("BackgrAltColor"), _T("#1c1c1c"), false);
	themeMgr.SetStr(_T("ListView"), _T("ShadowColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ListView"), _T("RebarShadowAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("ListView"), _T("ItemShadowAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("ListView"), _T("PosterShadowAlpha"), _T("0.05"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFont"), _T(""), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontSize"), _T("14"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontColor"), _T("#f0f0f0"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontBold"), _T("true"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontItalic"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFont"), _T(""), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontSize"), _T("10"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontColor"), _T("#f0f0f0"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontBold"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontItalic"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("LinkColor"), _T("#42a2ec"), false);

	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalRightEdgeColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalRightEdgeAlpha"), _T("0.08"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalTextColor"), _T("#e8e8e8"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalText2Color"), _T("#909090"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTopColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTopAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBottomColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBottomAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfTopAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfBottomAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1TopColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1TopAlpha"), _T("0.05"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1BottomColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1BottomAlpha"), _T("0.05"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder2Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder3Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder3Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTextColor"), _T("#e8e8e8"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightText2Color"), _T("#909090"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTopColor"), _T("#4c5f88"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTopAlpha"), _T("0.87"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBottomColor"), _T("#486ab4"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBottomAlpha"), _T("0.7"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfTopAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfBottomAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1TopColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1TopAlpha"), _T("0.05"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1BottomColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1BottomAlpha"), _T("0.05"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder2Alpha"), _T("0.01"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder3Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder3Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTextColor"), _T("#e8e8e8"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedText2Color"), _T("#909090"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("Font"), _T(""), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("FontSize"), _T("11"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("FontItalic"), _T("false"), false);

	themeMgr.SetStr(_T("StatusBar"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("BackgrTopColor"), _T("#303030"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("BackgrBottomColor"), _T("#2c2c2c"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("BackgrLineColor"), _T("#3b3b3b"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("TextColor"), _T("#f0f0f0"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("Font"), _T(""), false);
	themeMgr.SetStr(_T("StatusBar"), _T("FontSize"), _T(""), false);
	themeMgr.SetStr(_T("StatusBar"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("FontItalic"), _T("false"), false);

	themeMgr.SetStr(_T("LogWnd"), _T("BackgrColor"), _T("#1c1c1c"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("BackgrLineColor"), _T("#383838"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("TextColor"), _T("#f0f0f0"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("Font"), _T("Consolas, Courier New, Courier"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("FontSize"), _T("10"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("FontItalic"), _T("false"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("ShowScrollBar"), _T("false"), false);

	themeMgr.SetStr(_T("ToolBarButton"), _T("IconColor"), _T("#ebebeb"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("IconAlpha"), _T("1.0"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightRadius"), _T("3"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightTopColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightTopAlpha"), _T("0.08"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBottomColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBottomAlpha"), _T("0.08"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder1Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder1Alpha"), _T("0.08"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder2Alpha"), _T("0.06"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedRadius"), _T("3"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedTopColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedTopAlpha"), _T("0.2"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBottomColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBottomAlpha"), _T("0.16"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder1Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder1Alpha"), _T("0.1"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder2Alpha"), _T("0.1"), false);

	themeMgr.SetStr(_T("SearchBox"), _T("BackgrColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("BorderColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("BorderAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("BorderRadius"), _T("3"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("TextColor"), _T("#f0f0f0"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("HintColor"), _T("#909090"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("Font"), _T(""), false);
	themeMgr.SetStr(_T("SearchBox"), _T("FontSize"), _T("10"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("FontItalic"), _T("false"), false);

	themeMgr.SetStr(_T("ScrollBar"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("BackgrColor"), _T("#282828"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("BackgrEdgeColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalColorL"), _T("#383838"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalColorR"), _T("#303030"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge1Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge1Alpha"), _T("0.04"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge2Alpha"), _T("0.02"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightColorL"), _T("#404040"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightColorR"), _T("#383838"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge1Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge1Alpha"), _T("0.04"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge2Alpha"), _T("0.02"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbRadius"), _T("1"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowActiveColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowActiveAlpha"), _T("0.82"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowInactiveColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowInactiveAlpha"), _T("0.26"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperColorL"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperAlphaL"), _T("0.4"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperColorR"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperAlphaR"), _T("0.12"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperEdgeColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperEdgeAlpha"), _T("0.04"), false);

	if (!themeMgr.SaveAs(CorrectPath(_T("Themes\\Dark.xml"))))
		ASSERTRETURN(false);

	// Correct light theme

	themeMgr.Open(CorrectPath(_T("Themes\\Light.xml")));
	themeMgr.SetStr(_T("Name"), _T("Light"), false);
	themeMgr.SetStr(_T("Author"), _T(""), false);

	themeMgr.SetStr(_T("ReBar"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ReBar"), _T("BackgrTopColor"), _T("#efefef"), false);
	themeMgr.SetStr(_T("ReBar"), _T("BackgrBottomColor"), _T("#fbfbfb"), false);
	themeMgr.SetStr(_T("ReBar"), _T("BackgrLineColor"), _T("#e0e0e0"), false);

	themeMgr.SetStr(_T("ListView"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("BackgrColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ListView"), _T("BackgrAltColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ListView"), _T("ShadowColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ListView"), _T("RebarShadowAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("ListView"), _T("ItemShadowAlpha"), _T("0.04"), false);
	themeMgr.SetStr(_T("ListView"), _T("PosterShadowAlpha"), _T("0.2"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFont"), _T(""), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontSize"), _T("14"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontBold"), _T("true"), false);
	themeMgr.SetStr(_T("ListView"), _T("TitleFontItalic"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFont"), _T(""), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontSize"), _T("10"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontBold"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("TextFontItalic"), _T("false"), false);
	themeMgr.SetStr(_T("ListView"), _T("LinkColor"), _T("#136cb2"), false);

	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalRightEdgeColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalRightEdgeAlpha"), _T("0.055"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalTextColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalText2Color"), _T("#909090"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTopAlpha"), _T("0.023"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBottomAlpha"), _T("0.023"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfTopAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfBottomAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1TopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1TopAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1BottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1BottomAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder2Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder3Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder3Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTextColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightText2Color"), _T("#909090"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTopColor"), _T("#ffe074"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTopAlpha"), _T("0.94"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBottomColor"), _T("#fff27e"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBottomAlpha"), _T("0.86"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfTopAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfBottomAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1TopColor"), _T("#c28a30"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1TopAlpha"), _T("0.4"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1BottomColor"), _T("#c2a44d"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1BottomAlpha"), _T("0.4"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder2Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder2Alpha"), _T("0.27"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder3Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder3Alpha"), _T("0.12"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTextColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedText2Color"), _T("#909090"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("Font"), _T(""), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("FontSize"), _T("11"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("CategoryBarButton"), _T("FontItalic"), _T("false"), false);

	themeMgr.SetStr(_T("StatusBar"), _T("BackgrWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("BackgrTopColor"), _T("#f2f2f2"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("BackgrBottomColor"), _T("#f9f9f9"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("BackgrLineColor"), _T("#dbdbdb"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("TextColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("Font"), _T(""), false);
	themeMgr.SetStr(_T("StatusBar"), _T("FontSize"), _T(""), false);
	themeMgr.SetStr(_T("StatusBar"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("StatusBar"), _T("FontItalic"), _T("false"), false);

	themeMgr.SetStr(_T("LogWnd"), _T("BackgrColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("BackgrLineColor"), _T("#dedede"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("TextColor"), _T("#101010"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("Font"), _T("Consolas, Courier New, Courier"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("FontSize"), _T("10"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("FontItalic"), _T("false"), false);
	themeMgr.SetStr(_T("LogWnd"), _T("ShowScrollBar"), _T("true"), false);

	themeMgr.SetStr(_T("ToolBarButton"), _T("IconColor"), _T("#404040"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("IconAlpha"), _T("1.0"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightRadius"), _T("3"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightTopAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBottomAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder1Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder1Alpha"), _T("0.08"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder2Alpha"), _T("0.0"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedWindowsDefault"), _T("false"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedRadius"), _T("3"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedTopColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedTopAlpha"), _T("0.14"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBottomColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBottomAlpha"), _T("0.1"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder1Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder1Alpha"), _T("0.13"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder2Color"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder2Alpha"), _T("0.1"), false);
	
	themeMgr.SetStr(_T("SearchBox"), _T("BackgrColor"), _T("#ffffff"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("BorderColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("BorderAlpha"), _T("0.03"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("BorderRadius"), _T("3"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("TextColor"), _T("#202020"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("HintColor"), _T("#a0a0a0"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("Font"), _T(""), false);
	themeMgr.SetStr(_T("SearchBox"), _T("FontSize"), _T("10"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("FontBold"), _T("false"), false);
	themeMgr.SetStr(_T("SearchBox"), _T("FontItalic"), _T("false"), false);
	
	themeMgr.SetStr(_T("ScrollBar"), _T("BackgrWindowsDefault"), _T("true"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("BackgrColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("BackgrEdgeColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalColorL"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalColorR"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge1Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge1Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge2Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightColorL"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightColorR"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge1Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge1Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge2Color"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge2Alpha"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ThumbRadius"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowActiveColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowActiveAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowInactiveColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("ArrowInactiveAlpha"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperColorL"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperAlphaL"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperColorR"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperAlphaR"), _T("0"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperEdgeColor"), _T("#000000"), false);
	themeMgr.SetStr(_T("ScrollBar"), _T("GripperEdgeAlpha"), _T("0"), false);

	if (!themeMgr.SaveAs(CorrectPath(_T("Themes\\Light.xml"))))
		ASSERTRETURN(false);

	// Correct Win8 theme

	if (IsWin8())
	{
		themeMgr.Open(CorrectPath(_T("Themes\\Win8.xml")));
		themeMgr.SetStr(_T("Name"), _T("Win8"), false);
		themeMgr.SetStr(_T("Author"), _T(""), false);

		themeMgr.SetStr(_T("ReBar"), _T("BackgrWindowsDefault"), _T("true"), false);
		themeMgr.SetStr(_T("ReBar"), _T("BackgrTopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ReBar"), _T("BackgrBottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ReBar"), _T("BackgrLineColor"), _T("#000000"), false);

		themeMgr.SetStr(_T("ListView"), _T("BackgrWindowsDefault"), _T("false"), false);
		themeMgr.SetStr(_T("ListView"), _T("BackgrColor"), _T("#ffffff"), false);
		themeMgr.SetStr(_T("ListView"), _T("BackgrAltColor"), _T("#ffffff"), false);
		themeMgr.SetStr(_T("ListView"), _T("ShadowColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ListView"), _T("RebarShadowAlpha"), _T("0.03"), false);
		themeMgr.SetStr(_T("ListView"), _T("ItemShadowAlpha"), _T("0.04"), false);
		themeMgr.SetStr(_T("ListView"), _T("PosterShadowAlpha"), _T("0.2"), false);
		themeMgr.SetStr(_T("ListView"), _T("TitleFont"), _T(""), false);
		themeMgr.SetStr(_T("ListView"), _T("TitleFontSize"), _T("14"), false);
		themeMgr.SetStr(_T("ListView"), _T("TitleFontColor"), _T("#202020"), false);
		themeMgr.SetStr(_T("ListView"), _T("TitleFontBold"), _T("true"), false);
		themeMgr.SetStr(_T("ListView"), _T("TitleFontItalic"), _T("false"), false);
		themeMgr.SetStr(_T("ListView"), _T("TextFont"), _T(""), false);
		themeMgr.SetStr(_T("ListView"), _T("TextFontSize"), _T("10"), false);
		themeMgr.SetStr(_T("ListView"), _T("TextFontColor"), _T("#202020"), false);
		themeMgr.SetStr(_T("ListView"), _T("TextFontBold"), _T("false"), false);
		themeMgr.SetStr(_T("ListView"), _T("TextFontItalic"), _T("false"), false);
		themeMgr.SetStr(_T("ListView"), _T("LinkColor"), _T("#136cb2"), false);

		themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalRightEdgeColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalRightEdgeAlpha"), _T("0.055"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalTextColor"), _T("#202020"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("NormalText2Color"), _T("#909090"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTopAlpha"), _T("0.023"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBottomAlpha"), _T("0.023"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfTopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfTopAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfBottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightHalfBottomAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1TopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1TopAlpha"), _T("0.03"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1BottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder1BottomAlpha"), _T("0.03"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder2Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder2Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder3Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightBorder3Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightTextColor"), _T("#202020"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("HighlightText2Color"), _T("#909090"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTopColor"), _T("#1979ca"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTopAlpha"), _T("1"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBottomColor"), _T("#1979ca"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBottomAlpha"), _T("1"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfTopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfTopAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfBottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedHalfBottomAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1TopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1TopAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1BottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder1BottomAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder2Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder2Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder3Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedBorder3Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedTextColor"), _T("#ffffff"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("SelectedText2Color"), _T("#c0c0c0"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("Font"), _T(""), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("FontSize"), _T("11"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("FontBold"), _T("false"), false);
		themeMgr.SetStr(_T("CategoryBarButton"), _T("FontItalic"), _T("false"), false);

		themeMgr.SetStr(_T("StatusBar"), _T("BackgrWindowsDefault"), _T("false"), false);
		themeMgr.SetStr(_T("StatusBar"), _T("BackgrTopColor"), _T("#f5f5f5"), false);
		themeMgr.SetStr(_T("StatusBar"), _T("BackgrBottomColor"), _T("#f5f5f5"), false);
		themeMgr.SetStr(_T("StatusBar"), _T("BackgrLineColor"), _T("#dedede"), false);
		themeMgr.SetStr(_T("StatusBar"), _T("TextColor"), _T("#202020"), false);
		themeMgr.SetStr(_T("StatusBar"), _T("Font"), _T(""), false);
		themeMgr.SetStr(_T("StatusBar"), _T("FontSize"), _T(""), false);
		themeMgr.SetStr(_T("StatusBar"), _T("FontBold"), _T("false"), false);
		themeMgr.SetStr(_T("StatusBar"), _T("FontItalic"), _T("false"), false);

		themeMgr.SetStr(_T("LogWnd"), _T("BackgrColor"), _T("#ffffff"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("BackgrLineColor"), _T("#dedede"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("TextColor"), _T("#101010"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("Font"), _T("Consolas, Courier New, Courier"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("FontSize"), _T("10"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("FontBold"), _T("false"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("FontItalic"), _T("false"), false);
		themeMgr.SetStr(_T("LogWnd"), _T("ShowScrollBar"), _T("true"), false);

		themeMgr.SetStr(_T("ToolBarButton"), _T("IconColor"), _T("#404040"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("IconAlpha"), _T("1.0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightWindowsDefault"), _T("true"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightRadius"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightTopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightTopAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBottomAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder1Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder1Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder2Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("HighlightBorder2Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedWindowsDefault"), _T("true"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedRadius"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedTopColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedTopAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBottomColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBottomAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder1Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder1Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder2Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ToolBarButton"), _T("PressedBorder2Alpha"), _T("0"), false);
	
		themeMgr.SetStr(_T("SearchBox"), _T("BackgrColor"), _T("#ffffff"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("BorderColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("BorderAlpha"), _T("0.05"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("BorderRadius"), _T("0"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("TextColor"), _T("#202020"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("HintColor"), _T("#a0a0a0"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("Font"), _T(""), false);
		themeMgr.SetStr(_T("SearchBox"), _T("FontSize"), _T("10"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("FontBold"), _T("false"), false);
		themeMgr.SetStr(_T("SearchBox"), _T("FontItalic"), _T("false"), false);
	
		themeMgr.SetStr(_T("ScrollBar"), _T("BackgrWindowsDefault"), _T("true"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("BackgrColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("BackgrEdgeColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalColorL"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalColorR"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge1Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge1Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge2Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbNormalEdge2Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightColorL"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightColorR"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge1Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge1Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge2Color"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbHighlightEdge2Alpha"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ThumbRadius"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ArrowActiveColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ArrowActiveAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ArrowInactiveColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("ArrowInactiveAlpha"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("GripperColorL"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("GripperAlphaL"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("GripperColorR"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("GripperAlphaR"), _T("0"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("GripperEdgeColor"), _T("#000000"), false);
		themeMgr.SetStr(_T("ScrollBar"), _T("GripperEdgeAlpha"), _T("0"), false);

		if (!themeMgr.SaveAs(CorrectPath(_T("Themes\\Win8.xml"))))
			ASSERTRETURN(false);
	}

	return true;
}
