/**************************************************************************
This file is part of Matrice, an effcient and elegant C++ library.
Copyright(C) 2018, Zhilong(Dgelom) Su, all rights reserved.

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#pragma once

#include <tuple>
#include <valarray>
#include "../_matrix_base.hpp"
#include "../_matrix_exp.hpp"
#include "../_range.h"

DGE_MATRICE_BEGIN namespace detail {

template<typename _Ty, int _M = 0, int _N = 0, std::size_t _K = 0,
	typename matrix_type = types::Matrix_<_Ty, _M, _N>>
class _Tensor_impl MATRICE_NONHERITABLE : public std::valarray<matrix_type> {
	using _Mybase = std::valarray<matrix_type>;
	using _Myt_traits = matrix_traits<matrix_type>;
public:
	using type = matrix_type;
	using value_type = typename _Myt_traits::type;
	using value_t = value_type;
	using _Mybase::operator*;

	_Tensor_impl(std::size_t _Rows)
		: _Mybase(m_size = _Rows), m_rows(_Rows), m_cols(1) {}
	_Tensor_impl(std::size_t _Rows, const matrix_type& _Mat)
		: _Mybase(_Mat, m_size = _Rows), m_rows(_Rows), m_cols(1) {}
	_Tensor_impl(std::size_t _Rows, std::size_t _Cols) 
		: _Mybase(m_size = _Rows*_Cols), m_rows(_Rows), m_cols(_Cols) {}
	_Tensor_impl(std::size_t _Rows, std::size_t _Cols, const matrix_type& _Mat) : _Mybase(_Mat, m_size = _Rows * _Cols), m_rows(_Rows), m_cols(_Cols) {}

	MATRICE_HOST_INL auto rows() const { return m_rows; }
	MATRICE_HOST_INL auto cols() const { return m_cols; }

private:
	std::size_t m_rows, m_cols, m_size;
};
template<typename _Ty, int _M, int _N, int _K>
struct is_tensor<_Tensor_impl<_Ty, _M, _N, _K>> : std::true_type {};

template<typename _Ty, int _M, int _N, int _K>
struct tensor_traits< _Tensor_impl<_Ty, _M, _N, _K>> {
	using value_type = _Ty;
	using element_type = Matrix_<value_type, _M, _N>;
};

template<typename _Ty, int _M = 0, int _N = _M>
class _Multi_matrix MATRICE_NONHERITABLE
	: public std::vector<types::Matrix_<_Ty, _M, _N>>
{
	using _Mybase = std::vector<types::Matrix_<_Ty, _M, _N>>;
	using _Myt = _Multi_matrix;
public:
	using matrix_type = typename _Mybase::value_type;
	using value_type = typename matrix_type::value_type;
	using value_t = value_type;

	MATRICE_HOST_FINL explicit _Multi_matrix(std::size_t _Count) 
		: _Mybase(_Count) {
	}
	MATRICE_HOST_FINL explicit _Multi_matrix(const matrix_type& _Mat, std::size_t _Count)
		: _Mybase(_Count, _Mat) {
	}
	MATRICE_HOST_FINL _Multi_matrix(const std::initializer_list<matrix_type>& _L)
		: _Mybase(_L.size()) {
		for (auto _Idx = 0; _Idx < this->size(); ++_Idx) {
			this->operator[](_Idx) = *(_L.begin() + _Idx);
		}
	}
	MATRICE_HOST_FINL _Myt& operator= (const std::initializer_list<matrix_type>& _L) {
		if (this->size() < _L.size()) this->resize(_L.size());
		for (auto _Idx = 0; _Idx < this->size(); ++_Idx) {
			this->operator[](_Idx) = *(_L.begin() + _Idx);
		}
	}
	/**
	 * \gets the _N block views of the multi-matrix after pos of _Off-set.
	 */
	template<std::size_t _N, std::size_t _Off = 0, typename _Ity = std::size_t> 
	MATRICE_HOST_FINL auto view_n (const std::tuple<_Ity, _Ity, _Ity, _Ity>& _R) const {
		return tuple_n<_N-1>::_(this->data()+ _Off, [&](const matrix_type& _Mat) {
			return _Mat.block(_R); 
		});
	}
	template<std::size_t _N, std::size_t _Off = 0, typename _Ity = std::size_t>
	MATRICE_HOST_FINL auto view_n(_Ity _L, _Ity _R, _Ity _U, _Ity _D) const {
		return tuple_n<_N - 1>::_(this->data()+ _Off, [&](const matrix_type& _Mat) {
			return _Mat.block(_L, _R, _U, _D);
		});
	}
	template<std::size_t _N, std::size_t _Off = 0, typename _Ity = std::size_t>
	MATRICE_HOST_FINL auto view_n(const range<_Ity>& _Rx, const range<_Ity>& _Ry) const {
		return tuple_n<_N - 1>::_(this->data() + _Off, [&](const matrix_type& _Mat) {
			return _Mat.block(_Rx.begin(), _Rx.end(), _Ry.begin(), _Ry.end());
		});
	}
};

} DGE_MATRICE_END