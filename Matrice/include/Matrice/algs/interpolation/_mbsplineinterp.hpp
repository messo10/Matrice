/*********************************************************************
This file is part of Matrice, an effcient and elegant C++ library.
Copyright(C) 2018-2019, Zhilong(Dgelom) Su, all rights reserved.

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/
#pragma once

#include "_base.h"

MATRICE_ALGS_BEGIN
template<typename _Ty> 
class _Spline_interpolation<_Ty, _TAG mbspl_tag> : public
	_Interpolation_base<_Spline_interpolation<_Ty, _TAG mbspl_tag>>
{
	static_assert(is_scalar_v<_Ty>, "template type _Ty must be a scalar.");
	using _Myt = _Spline_interpolation<_Ty, _TAG mbspl_tag>;
	using _Mybase = _Interpolation_base<_Myt>;
public:
	using typename _Mybase::category;
	using typename _Mybase::value_type;
	using typename _Mybase::point_type;
	using typename _Mybase::matrix_type;
	using _Mybase::_Interpolation_base;

	MATRICE_HOST_FINL void _Coeff_impl();
};

MATRICE_ALGS_END