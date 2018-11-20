/**************************************************************************
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
**************************************************************************/
#pragma once

#include "_base.h"

MATRICE_ALGS_BEGIN

template<typename _Ty, std::size_t _Opt> class _Spline_interpolation {};

template<typename _Ty>
class _Spline_interpolation<_Ty, _BICBSPL>
	: public _Interpolation_base<_Spline_interpolation<_Ty, _BICBSPL>>
{
	using _Myt = _Spline_interpolation;
	using _Mybase = _Interpolation_base<_Myt>;
public:
	using typename _Mybase::matrix_type;
	using typename _Mybase::value_type;
	using typename _Mybase::point_type;
	static constexpr auto Ldv = 4; // \leading dimension of value kernel
	static constexpr auto Ldg = 3; // \leading dimension of gradient kernel

	_Spline_interpolation(const matrix_type& _Data) : _Mybase(_Data) {}

	MATRICE_HOST_INL void _Coeff_impl();
	MATRICE_HOST_INL auto _Val_dx_n(const value_type& _) const {
		return Matrix_<value_type, Ldv, 1>{1, _, _*_, _*_*_};
	}
	MATRICE_HOST_INL auto _Val_dy_n(const value_type& _) const {
		return Matrix_<value_type, 1, Ldv>{1, _, _*_, _*_*_};
	}
	MATRICE_HOST_INL auto _Grad_dx_n(const value_type& _) const {
		return Matrix_<value_type, Ldg, 1>{1, _, _*_};
	}
	MATRICE_HOST_INL auto _Grad_dy_n(const value_type& _) const {
		return Matrix_<value_type, 1, Ldg>{1, _, _*_};
	}
	MATRICE_HOST_INL auto& _Kernel_of_value() const { return (_Kov); }
	MATRICE_HOST_INL auto& _Kernel_of_grad() const { return (_Kog); }

private:
	const Matrix_<value_type, 4, 4> _Kov { // \Kernel of value interpolation
		1., -3., 3., -1., 4., 0., -6., 3., 
		1., 3., 3., -3., 0., 0., 0., 1. };
	const Matrix_<value_type, 4, 3> _Kog { // \Kernel of gradient interpolation
		-3., 6., -3., 0., -12., 9., 
		3., 6., -9., 0., 0., 3. };
};

template<typename _Ty>
class _Spline_interpolation<_Ty, _BIQNSPL>
	: public _Interpolation_base<_Spline_interpolation<_Ty, _BIQNSPL>>
{
	using _Myt = _Spline_interpolation;
	using _Mybase = _Interpolation_base<_Myt>;
public:
	using typename _Mybase::matrix_type;
	using typename _Mybase::value_type;
	using typename _Mybase::point_type;
	static constexpr auto Ldv = 6; // \leading dimension of value kernel
	static constexpr auto Ldg = 5; // \leading dimension of gradient kernel

	_Spline_interpolation(const matrix_type& _Data) : _Mybase(_Data) {}

	MATRICE_HOST_INL void _Coeff_impl();
	MATRICE_HOST_INL auto _Val_dx_n(const value_type& _) const {
		return Matrix_<value_type, Ldv, 1>{1, _, _*_, _*_*_, _*_*_*_, _*_*_*_*_};
	}
	MATRICE_HOST_INL auto _Val_dy_n(const value_type& _) const {
		return Matrix_<value_type, 1, Ldv>{1, _, _*_, _*_*_, _*_*_*_, _*_*_*_*_};
	}
	MATRICE_HOST_INL auto _Grad_dx_n(const value_type& _) const {
		return Matrix_<value_type, Ldg, 1>{1, _, _*_, _*_*_, _*_*_*_};
	}
	MATRICE_HOST_INL auto _Grad_dy_n(const value_type& _) const {
		return Matrix_<value_type, 1, Ldg>{1, _, _*_, _*_*_, _*_*_*_};
	}
	MATRICE_HOST_INL auto& _Kernel_of_value() const { return (_Kov); }
	MATRICE_HOST_INL auto& _Kernel_of_grad() const { return (_Kog); }
private:
	const Matrix_<value_type, Ldv, Ldv> _Kov { // \Kernel of value interpolation
		1., -5., 10., -10., 5., -1.,  26., -50., 20., 20., -20., 5.,
		66., 0., -60., 0., 30., -10., 26.,50.,20.,-20.,-20.,10.,
		1.,  5., 10., 10., 5.,  -5.,  0., 0., 0., 0., 0., 1. };
	const Matrix_<value_type, Ldv, Ldg> _Kog { // \Kernel of gradient interpolation
		-5., 20., 30., 20., -5.,  -50., 40., 60., -80., 25.,
		0., -120., 0., 120., -50., 50., 40., -60., -80., 10.,
		5.,  20., 30., 20., -25.,  0.,  0.,   0.,  0.,  5. };
};

template<typename _Ty>
class _Spline_interpolation<_Ty, _BISPSPL>
	: public _Interpolation_base<_Spline_interpolation<_Ty, _BISPSPL>>
{
	using _Myt = _Spline_interpolation;
	using _Mybase = _Interpolation_base<_Myt>;
public:
	using typename _Mybase::matrix_type;
	using typename _Mybase::value_type;
	using typename _Mybase::point_type;
	static constexpr auto Ldv = 8; // \leading dimension of value kernel
	static constexpr auto Ldg = 7; // \leading dimension of gradient kernel

	_Spline_interpolation(const matrix_type& _Data) : _Mybase(_Data) {}

	MATRICE_HOST_INL void _Coeff_impl();
	MATRICE_HOST_INL auto _Val_dx_n(const value_type& _) const {
		return Matrix_<value_type, Ldv, 1>{1, _, _*_, _*_*_, _*_*_*_, _*_*_*_*_, _*_*_*_*_*_, _*_*_*_*_*_*_};
	}
	MATRICE_HOST_INL auto _Val_dy_n(const value_type& _) const {
		return Matrix_<value_type, 1, Ldv>{1, _, _*_, _*_*_, _*_*_*_, _*_*_*_*_, _*_*_*_*_*_, _*_*_*_*_*_*_};
	}
	MATRICE_HOST_INL auto _Grad_dx_n(const value_type& _) const {
		return Matrix_<value_type, Ldg, 1>{1, _, _*_, _*_*_, _*_*_*_, _*_*_*_*_, _*_*_*_*_*_};
	}
	MATRICE_HOST_INL auto _Grad_dy_n(const value_type& _) const {
		return Matrix_<value_type, 1, Ldg>{1, _, _*_, _*_*_, _*_*_*_, _*_*_*_*_, _*_*_*_*_*_};
	}
	MATRICE_HOST_INL auto& _Kernel_of_value() const { return (_Kov);}
	MATRICE_HOST_INL auto& _Kernel_of_grad() const { return (_Kog); }

private:
	const Matrix_<value_type, Ldv, Ldv> _Kov { // \Kernel of value interpolation
		1., -7., 21., -35., 35., -21., 7., -1.,
		120., -392., 504., -280., 0., 84., -42., 7.,
		1191., -1715., 315., 665., -315., -105., 105., -21.,
		2416., 0., -1680., 0., 560., 0., -140., 35.,
		1191., 1715., 315., -665., -315., 105., 105., -35.,
		120., 392., 504., 280., 0., -84., -42., 21.,
		1., 7., 21., 35., 35., 21., 7., -7.,
		0., 0., 0., 0., 0., 0., 0., 1. };
	const Matrix_<value_type, Ldv, Ldg> _Kog { // \Kernel of gradient interpolation
		-7., 42., -105., 140., -105., 42., -7.,
		-392., 1008., -840., 0., 420., -252., 49.,
		-1715., 630., 1995., -1260., -525., 630., -147.,
		0., -3360., 0., 2240., 0., -840., 245.,
		1715., 630., -1995., -1260., 525., 630., -245.,
		392., 1008., 840., 0., -420., -252., 147.,
		7., 42., 105., 140., 105., 42., -49.,
		0., 0., 0., 0., 0., 0., 7. };
};

///<brief> Class BicubicSplineInterp will be deprecated </brief>
template<typename _Ty, std::size_t _Opt = _BICBSPL>
class BicubicSplineInterp : public InterpBase_<_Ty, BicubicSplineInterp<_Ty, _Opt>>
{
	using base_t = InterpBase_<_Ty, BicubicSplineInterp<_Ty, _Opt>>;
public:
	enum { option = _Opt };
	using typename base_t::value_t;
	using typename base_t::matrix_t;

	MATRICE_GLOBAL_FINL BicubicSplineInterp(const matrix_t& _Data)
		:base_t(_Data) {}

	MATRICE_HOST_INL void _Bspline_coeff(const matrix_t& _Data);

private:
	using base_t::m_coeff;
	const types::Matrix_<value_t, 4, 4> m_icoef{1, -3, 3, -1, 4, 0, -6, 3, 1, 3, 3, -3, 0, 0, 0, 1};
	const types::Matrix_<value_t, 4, 3> m_gcoef{-3, 6, -3, 0, -12, 9, 3, 6, -9, 0, 0, 3};
};
MATRICE_ALGS_END