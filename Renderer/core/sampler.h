#pragma once
#include "math/math.h"

enum eFilter
{
	Filter_Min_Mag_Linear_Mip_Point
};

enum eAddressMode
{
	Address_Mode_Warp,
	Address_Mode_Mirror,
	Address_Mode_Clamp,
	Address_Mode_Border,
	Address_Mode_Mirror_once
};

struct SamplerState
{
	eFilter Filter;
	eAddressMode AddressU;
	eAddressMode AddressV;
	Color BorderColor;
};