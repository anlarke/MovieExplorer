#ifndef __RRECT_H__
#define __RRECT_H__

class RRect : public RECT
{
public:
	RRect()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	RRect(const RECT& rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
	}

	RRect(LONG l, LONG t, LONG r, LONG b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	~RRect()
	{
	}

	const LONG& GetX() const
	{
		return left;
	}

	LONG& SetX(const LONG& value)
	{
		right += (value - left);
		return left = value;
	}

	const LONG& GetY() const
	{
		return top;
	}

	LONG& SetY(const LONG& value)
	{
		bottom += (value - top);
		return top = value;
	}

	const LONG Width() const
	{
		return right - left;
	}

	const LONG Height() const
	{
		return bottom - top;
	}

	LONG SetWidth(const LONG& value)
	{
		right = left + value;
		return value;
	}

	LONG SetHeight(const LONG& value)
	{
		bottom = top + value;
		return value;
	}

	__declspec(property(put = SetX, get = GetX)) LONG x;
	__declspec(property(put = SetY, get = GetY)) LONG y;
	__declspec(property(put = SetWidth, get = Width)) LONG cx;
	__declspec(property(put = SetHeight, get = Height)) LONG cy;
};

#endif // __RRECT_H__
