/*
KTech, Kaup's C++ 2D terminal game engine library.
Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

This file is part of KTech.

KTech is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

KTech is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with KTech. If not, see <https://www.gnu.org/licenses/>.
*/

// Unit tests for header-only basic types (Point, UPoint, RGB, RGBA, Cell, CellA).
// These types have no engine dependencies and can be tested without linking KTechLibrary.

#include "ktech/basic/point.hpp"
#include "ktech/basic/upoint.hpp"
#include "ktech/basic/rgb.hpp"
#include "ktech/basic/rgba.hpp"
#include "ktech/basic/cell.hpp"
#include "ktech/basic/cella.hpp"
#include "ktech/utility/rgbcolors.hpp"
#include "ktech/utility/rgbacolors.hpp"

#include <cstdlib>
#include <iostream>

namespace
{
int g_failures = 0;
int g_passed = 0;

void Expect(bool condition, const char* expression, const char* file, int line)
{
	if (condition)
	{
		++g_passed;
		return;
	}
	++g_failures;
	std::cerr << "FAIL " << file << ':' << line << " -- " << expression << '\n';
}
} // namespace

#define EXPECT(expr) Expect(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

static void TestPoint()
{
	using KTech::Point;
	using KTech::UPoint;

	// Default construction
	EXPECT(Point{} == Point(0, 0));
	EXPECT(Point().x == 0);
	EXPECT(Point().y == 0);

	// Value construction
	const Point a(3, -4);
	EXPECT(a.x == 3);
	EXPECT(a.y == -4);

	// Construction from UPoint
	const Point fromU(UPoint(7, 11));
	EXPECT(fromU == Point(7, 11));

	// Equality
	EXPECT(Point(1, 2) == Point(1, 2));
	EXPECT(!(Point(1, 2) == Point(2, 1)));
	EXPECT(!(Point(1, 2) == Point(1, 3)));

	// Arithmetic
	EXPECT(Point(1, 2) + Point(3, 4) == Point(4, 6));
	EXPECT(Point(5, 5) - Point(2, 8) == Point(3, -3));
	EXPECT(Point(-1, -2) + Point(1, 2) == Point(0, 0));

	// Compound assignment
	Point p(10, 20);
	p += Point(1, -5);
	EXPECT(p == Point(11, 15));
	p -= Point(11, 5);
	EXPECT(p == Point(0, 10));

	// Chaining compound assignment
	Point q(0, 0);
	(q += Point(1, 2)) += Point(3, 4);
	EXPECT(q == Point(4, 6));
}

static void TestUPoint()
{
	using KTech::UPoint;

	EXPECT(UPoint{} == UPoint(0, 0));
	EXPECT(UPoint(2, 3).x == 2);
	EXPECT(UPoint(2, 3).y == 3);

	EXPECT(UPoint(1, 2) == UPoint(1, 2));
	EXPECT(!(UPoint(1, 2) == UPoint(1, 3)));

	EXPECT(UPoint(1, 2) + UPoint(3, 4) == UPoint(4, 6));
	EXPECT(UPoint(10, 10) - UPoint(3, 4) == UPoint(7, 6));

	UPoint u(5, 6);
	u += UPoint(1, 1);
	EXPECT(u == UPoint(6, 7));
	u -= UPoint(2, 3);
	EXPECT(u == UPoint(4, 4));
}

static void TestRGB()
{
	using KTech::RGB;

	EXPECT(RGB{} == RGB(0, 0, 0));
	const RGB c(10, 20, 30);
	EXPECT(c.r == 10);
	EXPECT(c.g == 20);
	EXPECT(c.b == 30);

	EXPECT(RGB(1, 2, 3) == RGB(1, 2, 3));
	EXPECT(!(RGB(1, 2, 3) == RGB(3, 2, 1)));
	EXPECT(!(RGB(1, 2, 3) == RGB(1, 9, 3)));

	// Predefined palette samples
	EXPECT(KTech::RGBColors::black == RGB(0, 0, 0));
	EXPECT(KTech::RGBColors::white == RGB(255, 255, 255));
	EXPECT(KTech::RGBColors::red == RGB(255, 0, 0));
	EXPECT(KTech::RGBColors::green == RGB(0, 255, 0));
	EXPECT(KTech::RGBColors::primaryBlue == RGB(0, 0, 255));
	EXPECT(!(KTech::RGBColors::red == KTech::RGBColors::green));
}

static void TestRGBA()
{
	using KTech::RGB;
	using KTech::RGBA;

	EXPECT(RGBA{} == RGBA(0, 0, 0, 0));
	const RGBA c(1, 2, 3, 4);
	EXPECT(c.r == 1);
	EXPECT(c.g == 2);
	EXPECT(c.b == 3);
	EXPECT(c.a == 4);

	// From RGB + alpha
	const RGBA fromRgb(RGB(9, 8, 7), 255);
	EXPECT(fromRgb == RGBA(9, 8, 7, 255));

	// Equality includes alpha
	EXPECT(RGBA(1, 2, 3, 4) == RGBA(1, 2, 3, 4));
	EXPECT(!(RGBA(1, 2, 3, 4) == RGBA(1, 2, 3, 5)));
	EXPECT(!(RGBA(1, 2, 3, 4) == RGBA(0, 2, 3, 4)));

	// RGBA is-a RGB for channel access via base
	const RGBA opaqueRed = KTech::RGBAColors::red;
	EXPECT(opaqueRed.r == 255);
	EXPECT(opaqueRed.a == 255);
	EXPECT(KTech::RGBAColors::transparent == RGBA(0, 0, 0, 0));
	EXPECT(static_cast<const RGB&>(opaqueRed) == RGB(255, 0, 0));
}

static void TestCell()
{
	using KTech::Cell;
	using KTech::RGB;

	// Defaults: space on black/black
	const Cell def;
	EXPECT(def.c == ' ');
	EXPECT(def.f == KTech::RGBColors::black);
	EXPECT(def.b == KTech::RGBColors::black);

	const Cell x('X', KTech::RGBColors::red, KTech::RGBColors::blue);
	EXPECT(x.c == 'X');
	EXPECT(x.f == KTech::RGBColors::red);
	EXPECT(x.b == KTech::RGBColors::blue);

	EXPECT(Cell('A', RGB(1, 2, 3), RGB(4, 5, 6)) == Cell('A', RGB(1, 2, 3), RGB(4, 5, 6)));
	EXPECT(!(Cell('A', KTech::RGBColors::red, KTech::RGBColors::black) == Cell('B', KTech::RGBColors::red, KTech::RGBColors::black)));
	EXPECT(!(Cell('A', KTech::RGBColors::red, KTech::RGBColors::black) == Cell('A', KTech::RGBColors::green, KTech::RGBColors::black)));
	EXPECT(!(Cell('A', KTech::RGBColors::red, KTech::RGBColors::black) == Cell('A', KTech::RGBColors::red, KTech::RGBColors::white)));
}

static void TestCellA()
{
	using KTech::Cell;
	using KTech::CellA;
	using KTech::RGBA;

	// Defaults: space with transparent colors
	const CellA def;
	EXPECT(def.c == ' ');
	EXPECT(def.f == KTech::RGBAColors::transparent);
	EXPECT(def.b == KTech::RGBAColors::transparent);

	const CellA z('Z', KTech::RGBAColors::green, RGBA(0, 0, 0, 128));
	EXPECT(z.c == 'Z');
	EXPECT(z.f == KTech::RGBAColors::green);
	EXPECT(z.b == RGBA(0, 0, 0, 128));

	// From Cell + alphas
	const Cell solid('S', KTech::RGBColors::white, KTech::RGBColors::black);
	const CellA withAlpha(solid, 200, 100);
	EXPECT(withAlpha.c == 'S');
	EXPECT(withAlpha.f == RGBA(255, 255, 255, 200));
	EXPECT(withAlpha.b == RGBA(0, 0, 0, 100));

	EXPECT(CellA('Q', RGBA(1, 2, 3, 4), RGBA(5, 6, 7, 8)) ==
		   CellA('Q', RGBA(1, 2, 3, 4), RGBA(5, 6, 7, 8)));
	EXPECT(!(CellA('Q', KTech::RGBAColors::red, KTech::RGBAColors::black) ==
			 CellA('R', KTech::RGBAColors::red, KTech::RGBAColors::black)));
}

int main()
{
	TestPoint();
	TestUPoint();
	TestRGB();
	TestRGBA();
	TestCell();
	TestCellA();

	std::cout << "basic types: " << g_passed << " passed, " << g_failures << " failed\n";
	return g_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
