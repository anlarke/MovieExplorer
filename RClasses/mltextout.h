#pragma once

#pragma warning(disable:4244)

inline bool MLTextOut(HDC hDC, int x, int y, int cx, int cy, RString_ str, int nLineHeight = 0)
{
	if (!hDC || cx <= 0 || cy <= 0)
		return false;

	if (str.IsEmpty() || x + cx <= 0 || y + cy <= 0)
		return true;

	const TCHAR *lpszEnd = (const TCHAR *)str + str.GetLength();
	const TCHAR *p1 = str, *p2, *p3, *p4;
	SIZE sz;
	int yInc = 0, n;
	RECT rc = {x, y, x + cx, y + cy};
	bool bContinue;

	while (true)
	{
		p2 = _tcsstr(p1, _T("\r\n"));
		if (!p2)
			p2 = lpszEnd;

		// check if we're already out of the rect

		if (y + yInc >= rc.bottom)
			break;

		// calculate line length

		GetTextExtentPoint32(hDC, p1, p2 - p1, &sz);

		if (nLineHeight == 0)
			nLineHeight = sz.cy;

		// if line fits

		if (sz.cx <= cx)
		{
			ExtTextOut(hDC, x, y + yInc, ETO_CLIPPED, &rc, p1, p2 - p1, NULL);
			yInc += nLineHeight;
		}

		// when line does not fit

		else
		{
			// estimate the line break point in characters

			n = ((p2 - p1) * cx) / sz.cx;
			if (n < 0)
				n = 0;

			// reverse find nearest space

			for (p3 = p1 + n; p3 > p1; p3--)
				if (*p3 == _T(' '))
					break;

			// if it's one word spanning this line, but it doesn't fit... let's clip it

			if (p3 == p1)
			{
				// find first space on line

				for (p3 = p1; p3 < p2; p3++)
					if (*p3 == _T(' '))
						break;

				ExtTextOut(hDC, x, y + yInc, ETO_CLIPPED, &rc, p1, p3 - p1, NULL);
				yInc += nLineHeight;

				p1 = (p3 == p2 ? p2 + 2 : p3 + 1);
				continue;
			}

			// see if p3 as line end fits

			GetTextExtentPoint32(hDC, p1, p3 - p1, &sz);
			if (sz.cx <= cx)
			{
				// try to add another word until it doesn't fit anymore

				p4 = p3;
				do
				{
					p3 = p4; // save last position that was valid
					for (p4 = p4+1; p4 < p2; p4++)
						if (*p4 == _T(' '))
							break;
					if (p4 == p2)
						break;

					GetTextExtentPoint32(hDC, p1, p4 - p1, &sz);

				} while (sz.cx <= cx);

				ExtTextOut(hDC, x, y + yInc, ETO_CLIPPED, &rc, p1, p3 - p1, NULL);
				yInc += nLineHeight;
				p1 = p3 + 1;
				continue;
			}
			else
			{
				// try to strip another word until it fits

				bContinue = false;

				do
				{
					for (p4 = p3-1; p4 > p1; p4--)
						if (*p4 == _T(' '))
							break;

					// if it's one word spanning this line, but it doesn't fit... let's clip it

					if (p4 == p1)
					{
						// find first space on line

						for (p3 = p1; p3 < p2; p3++)
							if (*p3 == _T(' '))
								break;

						ExtTextOut(hDC, x, y + yInc, ETO_CLIPPED, &rc, p1, p3 - p1, NULL);
						yInc += nLineHeight;

						p1 = (p3 == p2 ? p2 + 2 : p3 + 1);
						bContinue = true;
						break;
					}
					p3 = p4;
					GetTextExtentPoint32(hDC, p1, p3 - p1, &sz);

				} while (sz.cx > cx);

				if (bContinue)
					continue;

				ExtTextOut(hDC, x, y + yInc, ETO_CLIPPED, &rc, p1, p3 - p1, NULL);
				yInc += nLineHeight;
				p1 = p3 + 1;
				continue;
			}
		}

		if (p2 == lpszEnd)
			break;
		p1 = p2 + 2;
	}
	return true;
}

#pragma warning(default:4244)
