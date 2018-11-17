/***********************************************************************
This file is part of Matrice, an effcient and elegant C++ library.
Copyright(C) 2018, Zhilong(Dgelom) Su, all rights reserved.

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

#include "../../core/matrix.h"
#include "../../core/vector.h"
#include "../../core/solver.h"
#include "../../private/_range.h"

MATRICE_ALGS_BEGIN
enum {
	INTERP     = 18,

	BSPLINE    = 2307,
	BILINEAR   = 2152,
	BICUBIC    = 2156,
	BIQUINTIC  = 2160,
	BISEPTIC   = 2164
};

template<typename _Ty, size_t _Options> class BilinearInterp;
template<typename _Ty, size_t _Options> class BicubicSplineInterp;
template<typename _Ty, size_t _Options> class BiquinticSplineInterp;
template<typename _Ty, size_t _Options> class BisepticSplineInterp;

template<typename _Ty, size_t _Options = INTERP|BILINEAR> 
struct interpolation_traits
{ using type = BilinearInterp<_Ty, _Options>; };
template<typename _Ty> 
struct interpolation_traits<_Ty, INTERP|BICUBIC|BSPLINE>
{ using type = BicubicSplineInterp<_Ty, INTERP|BICUBIC|BSPLINE>; };
template<typename _Ty>
struct interpolation_traits<_Ty, INTERP|BIQUINTIC|BSPLINE>
{ using type = BiquinticSplineInterp<_Ty, INTERP|BIQUINTIC|BSPLINE>; };
template<typename _Ty>
struct interpolation_traits<_Ty, INTERP|BISEPTIC|BSPLINE>
{ using type = BisepticSplineInterp<_Ty, INTERP|BISEPTIC|BSPLINE>; };

template<typename _Ty, size_t _Options>
using interpolation_traits_t = typename interpolation_traits<_Ty, _Options>::type;

template<typename _Ty, typename _Derived> class InterpBase_
{
	using derived_t = _Derived;
public:
	using value_t = _Ty;
	using value_type = value_t;
	using matrix_t = Matrix<value_type>;

	template<typename... _Args>
	MATRICE_GLOBAL_FINL InterpBase_(const _Args&... args);

	MATRICE_GLOBAL_FINL auto& operator() () const { return m_coeff; }

protected:
	template<typename... _Args>
	MATRICE_GLOBAL_FINL void _Bspline_coeff(const _Args& ...args);

	const value_type m_eps = value_type(1.0e-7);
	matrix_t m_coeff;
};

template<typename _Ty, size_t _Options> class _Spline_interpolation;
template<typename _Ty, size_t _Options>
struct interpolation_traits<_Spline_interpolation<_Ty, _Options>> {
	using value_type = _Ty;
	using matrix_type = Matrix<value_type>;
	using type = _Spline_interpolation<value_type, _Options>;
};

template<typename _Derived> class _Interpolation_base {
	using _Myt = _Interpolation_base;
	using _Mydt = _Derived;
	using _Mytraits = interpolation_traits<_Mydt>;
public:
	using value_type = typename _Mytraits::value_type;
	using matrix_type = typename _Mytraits::matrix_type;
	using point_type = Vec2_<value_type>;

	_Interpolation_base(const matrix_type& _Data) 
		: _Mydata(_Data) {
		static_cast<_Mydt*>(this)->_Coeff_impl();
	}

	MATRICE_HOST_INL auto operator()(const point_type& _Pos, rect<int> _R = rect<int>(0,0,1,1)) const {
		return static_cast<const _Mydt*>(this)->_Value_impl(_Pos, _R);
	}

private:
	const matrix_type& _Mydata;
	const value_type _Myeps = value_type(1.0e-7);
	matrix_type _Mycoeff;
};

MATRICE_ALGS_END
#include "_base.inl"