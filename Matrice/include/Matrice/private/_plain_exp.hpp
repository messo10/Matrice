/**********************************************************************
This file is part of Matrice, an effcient and elegant C++ library.
Copyright(C) 2018-2022, Zhilong(Dgelom) Su, all rights reserved.

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
**********************************************************************/
#pragma once
#include <functional>
#include <cassert>
#include "_shape.hpp"
#include "_type_traits.h"
#include "_size_traits.h"
#include "_tag_defs.h"
#include "forward.hpp"
#include "math/_primitive_funcs.hpp"
#if defined(MATRICE_SIMD_ARCH)
#include "arch/simd.h"
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4715 4661 4224 4267 4244 4819 4199 26495)
#endif

DGE_MATRICE_BEGIN
#pragma region <!-- Forward declarations and matrix traits supplements -->
// \forward declarations 
_DETAIL_BEGIN
//template<typename _Ty> class Matrix;
//template<typename _Ty, int _Rows, int _cols> class Matrix_;
template<typename _Derived, typename _Traits, typename _Ty> class Base_;

template<typename _Ty, size_t _Depth> class _Tensor;

template<typename _Ty, typename> class _Blas_kernel_impl;

#if MATRICE_MATH_KERNEL==MATRICE_USE_MKL
class _Blas_kernel_wrapper;
#endif
_DETAIL_END

template<typename _Ty, MATRICE_ENABLE_IF(is_scalar_v<_Ty>)>
constexpr static _Ty zero = static_cast<_Ty>(0);
template<typename _Ty, MATRICE_ENABLE_IF(is_scalar_v<_Ty>)>
constexpr static _Ty one = static_cast<_Ty>(1);
template<typename _Ty, MATRICE_ENABLE_IF(is_scalar_v<_Ty>)>
constexpr static _Ty two = static_cast<_Ty>(2);

template<typename _Ty, int _M, int _N> 
struct is_matrix<Matrix_<_Ty, _M, _N>> : std::true_type {};

template<typename _Ty, int _M>
struct is_fxdvector<Matrix_<_Ty, _M, 1>> : std::true_type {};

template<class _Derived, class _Traits, typename _Ty>
struct is_matrix<detail::Base_<_Derived, _Traits, _Ty>> : std::true_type {};

template<typename _Ty, size_t _Depth>
struct is_tensor<detail::_Tensor<_Ty, _Depth>> : std::true_type {};

template<class _Derived, class _Traits, typename _Ty>
struct matrix_traits<detail::Base_<_Derived, _Traits, _Ty>> {
	using type = remove_all_t<_Ty>;
	using value_type = remove_all_t<_Ty>;
	enum { _M = _Traits::_M, _N = _Traits::_N };
	static constexpr auto rows = _M;
	static constexpr auto cols = _N;
	static constexpr bool Is_base = std::true_type::value;
};
template<typename _Ty, int _Rows, int _Cols> 
struct matrix_traits<Matrix_<_Ty, _Rows, _Cols>> {
	using type = remove_all_t<_Ty>;
	using value_type = remove_all_t<_Ty>;
	using category = tag::_Matrix_tag;
	enum { _M = _Rows, _N = _Cols };
	static constexpr auto rows = _Rows;
	static constexpr auto cols = _Cols;
	static constexpr bool Is_base = std::false_type::value;
};
template<typename _Ty>
struct matrix_traits<detail::_Tensor<_Ty, 0>> {
	using type = remove_all_t<_Ty>;
	using value_type = remove_all_t<_Ty>;
	using category = tag::_Tensor_tag;
	static constexpr auto _M = 0, _N = 0;
	static constexpr auto rows = _M;
	static constexpr auto cols = _N;
	static constexpr auto Is_base = std::false_type::value;
};

#pragma endregion
DGE_MATRICE_END

MATRICE_EXPR_BEGIN
template<typename _T, typename _U, typename _Op> class EwiseBinaryExp;
template<typename _T, typename _U, typename _Op> class MatBinaryExp;
template<typename _T, typename _Op> class MatUnaryExp;

struct Exp {
#define MATRICE_MAKE_EWISE_UNOP(NAME) \
template<typename _Ty> struct _Ewise_##NAME { \
	enum {flag = ewise}; \
	using category = tag::_Ewise_##NAME##_tag; \
	MATRICE_GLOBAL_FINL constexpr auto operator() (const _Ty& _Val) const {\
		return (NAME(_Val)); \
	}\
};
#define MATRICE_MAKE_EWISE_BIOP(NAME) \
template<typename _Ty> struct _Ewise_##NAME { \
	enum {flag = ewise}; \
	using category = tag::_Ewise_##NAME##_tag; \
	template<typename _Uy = _Ty> \
	MATRICE_GLOBAL_FINL constexpr auto operator() (const _Ty& _Left, const _Uy& _Right) const {\
		return (NAME(_Left, _Right)); \
	}\
};
#define MATRICE_MAKE_ARITH_OP(OP, NAME) \
template<typename _Rhs, MATRICE_ENABLE_IF(std::true_type::value)> \
MATRICE_GLOBAL_FINL auto operator OP(const _Rhs& _Right) { \
	return EwiseBinaryExp<derived_t, _Rhs, Op::_Ewise_##NAME<value_t>>(*_CDTHIS, _Right); \
} \
template<typename _Lhs, MATRICE_ENABLE_IF(std::true_type::value)> friend \
MATRICE_GLOBAL_FINL auto operator OP(const _Lhs& _Left, const_derived& _Right) { \
	return EwiseBinaryExp<_Lhs, derived_t, Op::_Ewise_##NAME<value_t>>(_Left, _Right); \
} 
	using default_type = double;
	/**
	 * \expression tags
	 */
	enum OpFlag {
		ewise = 0, mmul = 1, inv = 2, trp = 3, sum = 4, undef = -1
	};
	/**
	 * \expression option
	 */
	template<int _Option> struct option {
		enum { value = _Option };
	};
	/**
	 * \factorial_t<N> = N!
	 */
	template<int N> struct factorial_t {
		enum { value = factorial_t<N - 1>::value*N };
	};
	template<> struct factorial_t<0> {
		enum { value = 1 };
	};

	/**
	 * \expression operators
	 */
	struct Op {
		MATRICE_MAKE_EWISE_BIOP(add);
		MATRICE_MAKE_EWISE_BIOP(sub);
		MATRICE_MAKE_EWISE_BIOP(mul);
		MATRICE_MAKE_EWISE_BIOP(div);
		MATRICE_MAKE_EWISE_BIOP(max);
		MATRICE_MAKE_EWISE_BIOP(min);
		MATRICE_MAKE_EWISE_UNOP(sqrt);
		MATRICE_MAKE_EWISE_UNOP(exp);
		MATRICE_MAKE_EWISE_UNOP(abs);
		MATRICE_MAKE_EWISE_UNOP(log);
		MATRICE_MAKE_EWISE_UNOP(log2);
		MATRICE_MAKE_EWISE_UNOP(log10);
		MATRICE_MAKE_EWISE_UNOP(floor);

		/**
		 * \accumulates all elements of input expression
		 */
		template<typename _Ty> struct _Accum_exp {
			enum { flag = sum };
			using value_type = _Ty;
			MATRICE_GLOBAL_INL auto operator()(const value_type& _Exp) {
				return _Exp.sum();
			}
		};

		/**
		 * \matrix inverse operator
		 */
		template<typename _Ty> struct _Mat_inv { 
			enum { flag = inv };
			using value_type = _Ty;
			using category = tag::_Matrix_inv_tag;
			MATRICE_GLOBAL _Ty* operator()(int M, value_type* Out, value_type* In = nullptr) const;
		};
		
		/**
		 * \matrix transpose operator
		 */
		template<typename _Ty> struct _Mat_trp { 
			enum { flag = trp };
			using value_type = _Ty;
			using category = tag::_Matrix_trp_tag;
			MATRICE_HOST_FINL value_type operator()(int M, value_type* Out, value_type* i) const noexcept { 
				return (Out[int(i[1])*M + int(i[0])]); 
			}
		};
		
		/**
		 * \matrix spread multiplication
		 */
		template<typename _Ty> struct _Mat_sprmul {
			enum { flag = undef };
			using value_type = _Ty;
			using category = tag::_Expression_tag;
			template<typename _Lhs, typename _Rhs> MATRICE_GLOBAL_FINL
			value_type operator() (const _Lhs& lhs, const _Rhs& rhs, int r, int c) const noexcept { 
				return (lhs(r) * rhs(c)); 
			}
		};

		/**
		 * \matrix multiplication
		 */
		template<typename _Ty> struct _Mat_mul {
			enum { flag = mmul };
			using category = tag::_Matrix_mul_tag;
			using value_type = _Ty;
#ifdef MATRICE_SIMD_ARCH
			using packet_type = simd::Packet_<value_type, packet_size_v>;
#endif

			template<typename _Rhs> MATRICE_GLOBAL_FINL
			value_type operator() (const _Ty* lhs, const _Rhs& rhs, int c, int _plh = 0) const noexcept {
				value_type val = value_type(0);
				const int K = rhs.rows(), N = rhs.cols();
#ifdef MATRICE_SIMD_ARCH
				for (int k = 0; k < K; ++k) val += lhs[k] * rhs(k*N + c);
#else
				for (int k = 0; k < K; ++k) val += lhs[k] * rhs(k*N + c);
#endif
				return (val);
			}
			template<typename _Lhs, typename _Rhs> MATRICE_GLOBAL_FINL
			value_type operator() (const _Lhs& lhs, const _Rhs& rhs, int r, int c) const noexcept {
				value_type _Ret = value_type(0);

				//const int K = rhs.rows(), N = rhs.cols(), _Idx = r * lhs.cols();
				const auto K = rhs.shape().h, N = rhs.shape().w;
				const auto _Idx = r * lhs.shape().w;
				for (auto k = 0; k < K; ++k) 
					_Ret += lhs(_Idx + k) * rhs(k*N + c);

				return (_Ret);
			}
			template<typename _Lhs, typename _Rhs> MATRICE_GLOBAL_FINL
			value_type operator() (const _Lhs& _L, const _Rhs& _R, int r, int c, tag::_Matrix_tag, tag::_Matrix_tag) {
				const int K = _R.rows(), N = _R.cols(), _Idx = r * _L.cols();
#if MATRICE_MATH_KERNEL==MATRICE_USE_MKL
				return detail::template _Blas_kernel_impl<value_type>::dot(_L(_Idx), _R(c), K, 1, N);
#else
				value_type _Ret = value_type(0);
				for (auto k = 0; k < K; ++k) _Ret += _L(_Idx + k) * _R(k * N + c);
				return (_Ret);
#endif
			}
			template<typename _Lhs, typename _Rhs> MATRICE_GLOBAL_FINL
			value_type operator() (const _Lhs& _L, const _Rhs& _R, int r, int c, tag::_Matrix_tag, tag::_Matrix_view_tag) {
				const int K = _R.rows(), N = _R.cols(), _Idx = r * _L.cols();
				value_type _Ret = value_type(0);
				for (auto k = 0; k < K; ++k) _Ret += _L(_Idx + k) * _R(k*N + c);
				return (_Ret);
			}
		};
	};

	/**
	 * \expression base class
	 */
	template<typename _Op> struct Base_
	{
#define _CDTHIS static_cast<const_derived_pointer>(this)
		using traits_t = expression_traits<_Op>;
		using value_type = typename traits_t::value_type;
		using value_t = value_type;
		using eval_type = typename traits_t::auto_matrix_type;
		using derived_t = typename traits_t::type;
		using derived_pointer = derived_t*;
		using const_derived = const derived_t;
		using const_derived_pointer = const derived_pointer;
		enum {options = traits_t::options,
			rows_at_compiletime = traits_t::rows, 
			cols_at_compiletime = traits_t::cols};

		MATRICE_GLOBAL_FINL Base_() {}
		MATRICE_GLOBAL_FINL Base_(const shape_t<3>& _Shape) 
			: Shape(_Shape) {
			M = (Shape.rows()), N = (Shape.cols());
#ifdef MATRICE_DEBUG
			DGELOM_CHECK(M != 0, "Input rows in Base_<_Op> is zero!");
			DGELOM_CHECK(N != 0, "Input cols in Base_<_Op> is zero!");
#endif
		}

		/**
		 * \method Base_<_Op>::eval()
		 * \brief Evaluate this expression to the matrix type which is deduced according to the optimal type recorded by the expression.
		 * Specially, this method returns Scalar<value_type> iff the shape of matrix_type is deduced as 1-by-1 in compile time.
	     */
		MATRICE_GLOBAL_FINL auto eval() const {
			eval_type _Ret(shape());
			derived_pointer(this)->assign_to(_Ret);
			return forward<eval_type>(_Ret);
		}

		/** 
		 * \method Base_<_Op>::eval<_Rety>()
		 * \brief Evaluate this expression to the given type _Rety.
		 * \param <_Rety> any Matrix_<> compatible type.
		 */
		template<typename _Rety> 
		MATRICE_GLOBAL_FINL _Rety eval() const {
			_Rety ret(derived().shape());
			return forward<_Rety>(ret = *derived_pointer(this));
		}

		// \retrieve the evaluated result
		template<typename _Ret>
		MATRICE_GLOBAL_FINL _Ret& assign(_Ret& res) const noexcept {
			if (res.size() == 0) res.create(rows(), cols());
			derived_pointer(this)->assign_to(res);
			return (res);
		}

		MATRICE_GLOBAL_FINL derived_t& derived() noexcept {
			return *derived_pointer(this);
		}
		MATRICE_GLOBAL_FINL const derived_t& derived() const noexcept {
			return *derived_pointer(this);
		}

		// \formulate matmul expression
		template<typename _Rhs>
		MATRICE_GLOBAL_FINL auto mul(const _Rhs& _rhs) const noexcept {
			return MatBinaryExp<derived_t, _Rhs, Op::_Mat_mul<value_t>>(*_CDTHIS, _rhs);
		}

		template<typename _Rhs>
		MATRICE_HOST_INL auto mul_inplace(const _Rhs& _rhs) const {
#if MATRICE_MATH_KERNEL == MATRICE_USE_MKL
			typename expression_traits<
				MatBinaryExp<Base_,_Rhs, Op::_Mat_mul<value_type>>
			>::auto_matrix_type _Ret(rows(), _rhs.cols());
			return detail::_Blas_kernel_wrapper::gemm(derived(), _rhs, _Ret);
#else
			return mul(_rhs).eval();
#endif
		}

		/**
		 *\brief spread this to multiply with _rhs element-wisely; 
		 if _rhs can be evaluated to a square matrix, this should be unrolled to a 1D array if it is not.
		 *\param [_rhs] must be a matrix with a type of Matrix_<>, or an expression can be evaluated to a matrix.
		 */
		template<typename _Rhs>
		MATRICE_GLOBAL_INL auto spreadmul(const _Rhs& _rhs) const {
			conditional_t<is_matrix_v<_Rhs>, 
				_Rhs, typename _Rhs::matrix_type> _Res(_rhs.shape());
			//spread this along each row to mul. with _rhs
			if (cols() == 1 || size() == _rhs.rows()) {
#pragma omp parallel for if(_Res.rows() > 100)
				{
					for (index_t r = 0; r < _Res.rows(); ++r) {
						const auto _Val = _CDTHIS->operator()(r);
						auto _Ptr = _Res[r];
						for (index_t c = 0; c < _Res.cols(); ++c) {
							_Ptr[c] = _Val * this->operator()(c, r);
						}
					}
				}
			}
			// spread each entry along column to mul. with _rhs
			else if (rows() == 1 || size() == _Res.cols()) {
#pragma omp parallel for if(_Res.cols() > 100)
				{
					for (index_t r = 0; r < _Res.rows(); ++r) {
						auto _Ptr = _Res[r];
						for (index_t c = 0; c < _Res.cols(); ++c) {
							_Ptr[c] = _CDTHIS->operator()(c)*this->operator()(c,r);
						}
					}
				}
			}
			else {
				DGELOM_ERROR("Only one-dimension array spread is supported.");
			}
			return forward<decltype(_Res)>(_Res);
		}

		/**
		 * \sum over all expression entries
		 */
		MATRICE_GLOBAL_INL auto sum() const noexcept {
			auto _Ret = value_t(0); 
			int _Size = derived_pointer(this)->size();
#pragma omp parallel if(_Size > 1000)
		{
#pragma omp for reduction(+:_Ret)
			for (int i = 0; i < _Size; ++i) 
				_Ret += derived_pointer(this)->operator()(i);
		}
			return (_Ret);
		}
		/**
		 * \average of all entries
		 */
		MATRICE_GLOBAL_INL auto avg() const noexcept {
			return (_CDTHIS->sum() / _CDTHIS->size());
		}
		/**
		 * \variation of all entries
		 */
		MATRICE_GLOBAL_INL auto var() const noexcept {
			const auto _Avg = this->avg();
			const auto _Diff = Exp::EwiseBinaryExp< derived_t, decltype(_Avg),
				Exp::Op::_Ewise_sub<value_t>>( *static_cast<const derived_t*>(this), _Avg);
			return (_Diff * _Diff).avg();
		}

		/**
		 * \operator for expression evaluation
		 */
		MATRICE_GLOBAL_INL auto operator()(tag::_Expression_eval_tag) const {
			eval_type _Ret(rows(), cols());
			_CDTHIS->assign_to(_Ret);
			return forward<eval_type>(_Ret);
		}
		/**
		 * \2-d ewise evaluation operator
		 */
		MATRICE_GLOBAL_FINL auto operator()(size_t x, size_t y) noexcept {
			return _CDTHIS->operator()(x + y * cols());
		}
		MATRICE_GLOBAL_FINL const auto operator()(size_t x, size_t y)const noexcept {
			return _CDTHIS->operator()(x + y * cols());
		}

		MATRICE_GLOBAL_FINL constexpr size_t rows() const noexcept { 
			return derived().shape().h;;
		}
		MATRICE_GLOBAL_FINL constexpr size_t cols() const noexcept { 
			return derived().shape().w;
		}
		MATRICE_GLOBAL_FINL constexpr size_t size() const noexcept {
			return rows() * cols();
		}

		/**
		 *\brief Get full shape of dst. {Height, Width, Depth}
		 */
		MATRICE_GLOBAL_FINL const auto& shape() const noexcept {
			return (Shape); 
		}
		MATRICE_GLOBAL_FINL auto& shape() noexcept {
			return (Shape);
		}

		/**
		 *\brief Get full dims {Height, Width, Depth}
		 */
		MATRICE_GLOBAL_FINL constexpr auto& dims() const noexcept {
			return (Shape);
		}

		MATRICE_MAKE_ARITH_OP(+, add);
		MATRICE_MAKE_ARITH_OP(-, sub);
		MATRICE_MAKE_ARITH_OP(*, mul);
		MATRICE_MAKE_ARITH_OP(/, div);

	protected:
		size_t M, K, N;
		shape_t<3> Shape;
#undef _CDTHIS
	};

	// \matrix element-wise binary operation expression: +, -, *, /, ...
	template<typename T, typename U, typename _BinaryOp, 
		bool _T = is_scalar_v<T>, bool _U = is_scalar_v<U>> 
	class EwiseBinaryExp {};

	template<typename T, typename U, typename _BinaryOp>
	class EwiseBinaryExp<T, U, _BinaryOp, false, false> 
		: public Base_<EwiseBinaryExp<T, U, _BinaryOp, false, false>>
	{
		using _Mybase = Base_<EwiseBinaryExp<T,U,_BinaryOp,false,false>>;
	public:
		using _Mybase::rows_at_compiletime;
		using _Mybase::cols_at_compiletime;
		using typename _Mybase::value_t;
		using typename _Mybase::eval_type;
		using _Mybase::operator();
		using category = category_type_t<_BinaryOp>;
		enum { options = option<ewise>::value };

		MATRICE_GLOBAL_INL EwiseBinaryExp(const T& _lhs, const U& _rhs)
		 :_Mybase(detail::_Union(_lhs.shape(),_rhs.shape())),
			_LHS(_lhs), _RHS(_rhs) {}

		MATRICE_GLOBAL_FINL auto eval() const noexcept {
			return _Mybase::eval();
		}
		MATRICE_GLOBAL_FINL auto operator()(size_t _idx) const noexcept{
			return _Op(_LHS(_idx), _RHS(_idx));
		}

		template<typename _Mty> 
		MATRICE_GLOBAL_FINL void assign_to(_Mty& _Res) const noexcept{
			if (_LHS.size() == _RHS.size()) { //element-wise operation
//#pragma omp parallel for if(_Res.size() > 100)
				for (index_t i = 0; i < _Res.size(); ++i)
					_Res(i) = this->operator()(i);
			}
			else { //spreaded element-wise operation
//#pragma omp parallel for if(_Res.size() > 100)
				for (index_t i = 0; i < _Res.size(); ++i)
					_Res(i) = _Op(_LHS(i/_Mybase::cols()), _RHS(i));
			}
		}

		// Added at May/6/2022
		MATRICE_GLOBAL_FINL auto shape() const noexcept {
			return min(_LHS.shape(), _RHS.shape());
		}

	private:
		const T& _LHS; const U& _RHS;
		_BinaryOp _Op;
		using _Mybase::M;
		using _Mybase::N;
	};
	template<typename T, typename U, typename _BinaryOp>
	class EwiseBinaryExp<T, U, _BinaryOp, true, false> 
		: public Base_<EwiseBinaryExp<T, U, _BinaryOp, true, false>>
	{
		using _Mybase = Base_<EwiseBinaryExp<T, U, _BinaryOp, true, false>>;
	public:
		using _Mybase::rows_at_compiletime;
		using _Mybase::cols_at_compiletime;
		using typename _Mybase::value_t;
		using typename _Mybase::eval_type;
		using _Mybase::operator();
		using category = category_type_t<_BinaryOp>;
		enum { options = option<ewise>::value };

		MATRICE_GLOBAL_INL EwiseBinaryExp(const T _scalar, const U& _rhs)
			noexcept :_Mybase(_rhs.shape()), _Scalar(_scalar), _RHS(_rhs) {}

		MATRICE_GLOBAL_FINL auto eval() const noexcept {
			return _Mybase::eval();
		}
		MATRICE_GLOBAL_FINL value_t operator()(size_t _idx) const noexcept {
			return _Op(_Scalar, _RHS(_idx));
		}

		template<typename _Mty> 
		MATRICE_GLOBAL_INL void assign_to(_Mty& res) const noexcept {
//#pragma omp parallel for
			for (index_t i = 0; i < res.size(); ++i) 
				res(i) = this->operator()(i);
		}

		// Added at May/6/2022
		MATRICE_GLOBAL_FINL auto shape() const noexcept {
			return _RHS.shape();
		}

	private:
		const value_t _Scalar = std::numeric_limits<value_t>::infinity();
		const U& _RHS;
		_BinaryOp _Op;
		using _Mybase::M;
		using _Mybase::N;
	};
	template<typename T, typename U, typename _BinaryOp>
	class EwiseBinaryExp<T, U, _BinaryOp, false, true> 
		: public Base_<EwiseBinaryExp<T, U, _BinaryOp, false, true>>
	{
		using _Mybase = Base_<EwiseBinaryExp<T, U, _BinaryOp, false, true>>;
	public:
		using _Mybase::rows_at_compiletime;
		using _Mybase::cols_at_compiletime;
		using typename _Mybase::value_t;
		using typename _Mybase::eval_type;
		using _Mybase::operator();
		using category = category_type_t<_BinaryOp>;
		enum { options = option<ewise>::value };

		MATRICE_GLOBAL_INL EwiseBinaryExp(const T& _lhs, const U _scalar)
			noexcept :_Mybase(_lhs.shape()), _Scalar(_scalar), _LHS(_lhs) {
		}

		MATRICE_GLOBAL_FINL auto eval() const noexcept {
			return _Mybase::eval();
		}
		MATRICE_GLOBAL_FINL auto operator()(size_t _idx) const noexcept {
			return _Op(_LHS(_idx), _Scalar);
		}

		template<typename _Mty> 
		MATRICE_GLOBAL_INL void assign_to(_Mty& res) const noexcept {
//#pragma omp parallel for
			for (index_t i = 0; i < res.size(); ++i) 
				res(i) = this->operator()(i);
		}

		// \returns Shape of the evaluated type. Added at May/6/2022
		MATRICE_GLOBAL_FINL auto shape() const noexcept {
			return _LHS.shape();
		}
	private:
		const value_t _Scalar = infinity_v<value_t>;
		const T& _LHS;
		_BinaryOp _Op;
		using _Mybase::M;
		using _Mybase::N;
	};

	// \matrix element-wise unary operation expression: abs, log, sqrt...
	template<typename T, typename _UnaryOp> 
	class EwiseUnaryExp : public Base_<EwiseUnaryExp<T, _UnaryOp>>
	{
		using const_reference_t = add_const_reference_t<T>;
		using _Mybase = Base_<EwiseUnaryExp<T, _UnaryOp>>;
	public:
		using _Mybase::rows_at_compiletime;
		using _Mybase::cols_at_compiletime;
		using typename _Mybase::value_t;
		using typename _Mybase::eval_type;
		using _Mybase::operator();
		using category = category_type_t<_UnaryOp>;
		enum { options = expression_options<_UnaryOp>::value };

		EwiseUnaryExp(const_reference_t _rhs) noexcept
			:_Mybase(_rhs.shape()), _RHS(_rhs) {}

		MATRICE_GLOBAL_FINL auto eval() const noexcept {
			return _Mybase::eval();
		}
		MATRICE_GLOBAL_FINL auto operator()(size_t _idx) const noexcept {
			return _Op(_RHS(_idx));
		}

		template<typename _Mty> 
		MATRICE_GLOBAL_FINL void assign_to(_Mty& res) const noexcept {
			for (index_t i = 0; i < res.size(); ++i) 
				res(i) = this->operator()(i);
		}

		/**
		 * \brief returns the pointer of the source object
		 */
		MATRICE_GLOBAL_FINL decltype(auto) data() const noexcept {
			return _RHS.data();
		}

		// Added at May/6/2022
		MATRICE_GLOBAL_FINL auto shape() const noexcept {
			return _RHS.shape();
		}
	private:
		_UnaryOp _Op;
		const_reference_t _RHS;
		using _Mybase::M;
		using _Mybase::N;
	};

	// \matrix binary operation expression: matmul(), ...
	template<class T, class U, typename _BinaryOp> 
	class MatBinaryExp : public Base_<MatBinaryExp<T, U, _BinaryOp>>
	{
		using _Myt = MatBinaryExp;
		using _Mybase = Base_<MatBinaryExp<T, U, _BinaryOp>>;
	public:
		using _Mybase::rows_at_compiletime;
		using _Mybase::cols_at_compiletime;
		using typename _Mybase::value_t;
		using typename _Mybase::eval_type;
		using category = category_type_t<_BinaryOp>;
		enum{options = option<_BinaryOp::flag>::value};

		MATRICE_GLOBAL_INL MatBinaryExp(const T& _lhs, const U& _rhs) 
			noexcept :_Mybase(_lhs.shape()), _LHS(_lhs), _RHS(_rhs) {
			M = _LHS.rows(), K = _LHS.cols(), N = _RHS.cols();
			// If K is not equal to the rows of _RHS but equal to its cols,
			// we set N to the rows of _RHS. This case happens for transpose.
			if (K != _RHS.rows() && K == N) {
				N = _RHS.rows();
			}
		}

		MATRICE_GLOBAL_FINL auto eval() const noexcept {
			return _Mybase::eval();
		}
		MATRICE_HOST_FINL value_t* operator()(value_t* res) const {
			return _Op(_LHS.data(), _RHS.data(), res, M, K, N);
		}
		MATRICE_GLOBAL_FINL value_t operator()(int r, int c) const {
			return _Op(_LHS, _RHS, r, c);
		}
		MATRICE_GLOBAL_FINL value_t operator()(int _idx) const noexcept {
			const int r = _idx / _Mybase::cols();
			const auto c = _idx - r * _Mybase::cols();
			return _Op(_LHS, _RHS, r, c);
		}

		template<typename _Mty>
		MATRICE_GLOBAL_INL void assign_to(_Mty& res) const noexcept {
#if MATRICE_MATH_KERNEL == MATRICE_USE_MKL
			for (int i = 0; i < res.size(); ++i)
				res(i) = this->operator()(i);
#else
			for (int i = 0; i < res.size(); ++i) 
				res(i) = this->operator()(i);
#endif
		}

		// \return Shape of resulted type.
		// \note Added at May/6/2022
		MATRICE_GLOBAL_FINL auto shape() const noexcept {
			return shape_t<3>(_LHS.shape().h, _RHS.shape().w, 
				min(_LHS.shape().d, _RHS.shape().d));
		}
	private:
		const T& _LHS; 
		const U& _RHS;
		_BinaryOp _Op;
		using _Mybase::M;
		using _Mybase::K;
		using _Mybase::N;
	};

	// \matrix unary operation expression: inverse(), transpose(), ...
	template<class T, typename _UnaryOp> 
	class MatUnaryExp : public Base_<MatUnaryExp<T, _UnaryOp>>
	{
		using _Mybase = Base_<MatUnaryExp<T, _UnaryOp>>;
	public:
		using _Mybase::rows_at_compiletime;
		using _Mybase::cols_at_compiletime;
		using typename _Mybase::value_t;
		using typename _Mybase::eval_type;
		using category = category_type_t<_UnaryOp>;
		enum {options = option<_UnaryOp::flag>::value};

		MATRICE_GLOBAL_INL MatUnaryExp(const T& inout) noexcept
			:_Mybase(inout.shape()), _RHS(inout), _ANS(inout) {
			if constexpr (options == trp) { 
				std::swap(M, N); 
				_Mybase::Shape = _Mybase::Shape.t();
			}
		}
		MATRICE_GLOBAL_INL MatUnaryExp(const T& _rhs, T& _ans)
			: _Mybase(_rhs), _ANS(_ans) {
		}
		MATRICE_GLOBAL_INL MatUnaryExp(MatUnaryExp&& _other)
			: _Mybase(_other.shape()), options(_other.options) {
			*this = move(_other);
		}

		MATRICE_GLOBAL_FINL auto eval() const noexcept {
			return _Mybase::eval();
		}
		MATRICE_GLOBAL_FINL auto operator()() const noexcept { 
			return _Op(_Mybase::rows(), _ANS.data(), _RHS.data());
		}
		MATRICE_GLOBAL_FINL value_t operator()(int r, int c) const { 
			if constexpr (options == inv) return _ANS(c + r * N);
			if constexpr (options == trp) return _ANS(r + c * N);
		}
		MATRICE_GLOBAL_FINL value_t operator()(int _idx) const { 
			if constexpr (options == trp)
				//_idx = _idx * M + _idx / N * (1 - N * M); //Comment on May/8/2022
				_idx = _idx *this->rows() + _idx / this->cols() * (1 - this->size());
			return _ANS(_idx);
		}

		MATRICE_GLOBAL_FINL T& ans() noexcept { 
			return _ANS; 
		}
		MATRICE_GLOBAL_FINL const T& ans() const noexcept { 
			return _ANS; 
		}

		template<typename _Rhs, 
			typename _Ret = MatBinaryExp<MatUnaryExp, _Rhs, Op::_Mat_mul<value_t>>> 
		MATRICE_GLOBAL_INL _Ret mul(const _Rhs& _rhs) {
			if constexpr (options == inv) this->operator()();
			if constexpr (options == trp)                   ;
			return _Ret(*this, _rhs);
		}

		template<typename _Mty>
		MATRICE_GLOBAL_INL void assign_to(_Mty& res) noexcept {
			if constexpr (options == inv) {
				_Op(M, res.data(), _RHS.data());
			}
			if constexpr (options == trp) {
				const int _Size = this->size();
				if (res.data() == _RHS.data()) {
					_Mty _Res(res.shape());
					for (int i = 0; i < _Size; ++i)
							_Res(i) = (*this)(i);
					res = forward<_Mty>(_Res);
				}
				else {
					for (int i = 0; i < _Size; ++i)
						res(i) = (*this)(i);
				}
			}
		} /*i*N+(1-N*M)*(i/M) is replaced by i at left hand*/

		/**
		 * \brief returns the pointer of the source object
		 */
		MATRICE_GLOBAL_INL decltype(auto) data() const noexcept {
			return _RHS.data();
		}

		// Added at May/6/2022
		MATRICE_GLOBAL_FINL auto shape() const noexcept {
			if constexpr (options == trp) {
				return _RHS.shape().t();
			}
			else {
				return _RHS.shape();
			}
		}

	private:
		const T& _RHS;
		const T& _ANS;
		_UnaryOp _Op;
		using _Mybase::M;
		using _Mybase::N;
		using _Mybase::size;
	};

#undef MATRICE_MAKE_ARITH_OP
#undef MATRICE_MAKE_EWISE_UNOP
#undef MATRICE_MAKE_EWISE_BIOP
};

template<typename _Exp, MATRICE_ENABLE_IF(is_expression_v<_Exp>)>
using _Matrix_exp = typename Exp::Base_<_Exp>;

MATRICE_EXPR_END

///<-------------- I am the lovely seperate line -------------->

DGE_MATRICE_BEGIN
using _Exp = exprs::Exp;
using _Exp_op = _Exp::Op;
using _Exp_tag = _Exp::OpFlag;

template<typename _Derived>
struct is_expression<_Exp::Base_<_Derived>>:std::true_type {};
template<typename T, typename U, typename _Op, bool _T, bool _U>
struct is_expression<_Exp::EwiseBinaryExp<T,U,_Op,_T,_U>>:std::true_type {};
template<typename T, typename _Op>
struct is_expression<_Exp::EwiseUnaryExp<T,_Op>>:std::true_type {};
template<typename T, typename U, typename _Op>
struct is_expression<_Exp::MatBinaryExp<T,U,_Op>>:std::true_type {};
template<typename T, typename _Op>
struct is_expression<_Exp::MatUnaryExp<T,_Op>>:std::true_type {};

template<typename T, typename U, typename _Op>
struct expression_traits<_Exp::EwiseBinaryExp<T, U, _Op, false, false>> {
	using type = _Exp::EwiseBinaryExp<T, U, _Op, false, false>;
	using value_type = common_type_t<typename T::value_t, typename U::value_t>;
	enum { options = _Exp::option<_Exp_tag::ewise>::value };
	static constexpr auto rows = max_integer_v<T::rows_at_compiletime, U::rows_at_compiletime>;
	static constexpr auto cols = max_integer_v<T::cols_at_compiletime, U::cols_at_compiletime>;
	using auto_matrix_type = detail::Matrix_<value_type, rows, cols>;
};
template<typename T, typename U, typename _Op>
struct expression_traits<_Exp::EwiseBinaryExp<T, U, _Op, true, false>> {
	using type = _Exp::EwiseBinaryExp<T, U, _Op, true, false>;
	using value_type = typename U::value_t;
	enum { options = _Exp::option<_Exp_tag::ewise>::value };
	static constexpr auto rows = U::rows_at_compiletime;
	static constexpr auto cols = U::cols_at_compiletime;
	using auto_matrix_type = detail::Matrix_<value_type, rows, cols>;
};
template<typename T, typename U, typename _Op>
struct expression_traits<_Exp::EwiseBinaryExp<T, U, _Op, false, true>> {
	using type = _Exp::EwiseBinaryExp<T, U, _Op, false, true>;
	using value_type = typename T::value_t;
	enum { options = _Exp::option<_Exp_tag::ewise>::value };
	static constexpr auto rows = T::rows_at_compiletime;
	static constexpr auto cols = T::cols_at_compiletime;
	using auto_matrix_type = detail::Matrix_<value_type, rows, cols>;
};

template<typename T, typename _Op>
struct expression_traits<_Exp::EwiseUnaryExp<T, _Op>> {
	using type = _Exp::EwiseUnaryExp<T, _Op>;
	using value_type = typename T::value_t;
	enum { options = expression_options<_Op>::value };
	static constexpr auto rows = T::rows_at_compiletime;
	static constexpr auto cols = T::cols_at_compiletime;
	using auto_matrix_type = detail::Matrix_<value_type, rows, cols>;
};
template<typename T, typename U, typename _Op>
struct expression_traits<_Exp::MatBinaryExp<T, U, _Op>> {
	using type = _Exp::MatBinaryExp<T, U, _Op>;
	using value_type = common_type_t<typename T::value_t, typename U::value_t>;
	enum { options = expression_options<_Op>::value };
	static constexpr auto rows = T::rows_at_compiletime;
	static constexpr auto cols = U::cols_at_compiletime;
	using auto_matrix_type = detail::Matrix_<value_type, rows, cols>;
};
template<typename T, typename _Op>
struct expression_traits<_Exp::MatUnaryExp<T, _Op>> {
	using type = _Exp::MatUnaryExp<T, _Op>;
	using value_type = typename T::value_t;
	enum { options = expression_options<_Op>::value };
	static constexpr auto rows = conditional_size_v<(options & _Exp_tag::trp) == 
		_Exp_tag::trp, T::cols_at_compiletime, T::rows_at_compiletime>;
	static constexpr auto cols = conditional_size_v<(options & _Exp_tag::trp) == 
		_Exp_tag::trp, T::rows_at_compiletime, T::cols_at_compiletime>;
	using auto_matrix_type = detail::Matrix_<value_type, rows, cols>;
};

using exprs::Exp;
// *\element-wise addition
template<
	typename _Lhs, typename _Rhs,
	typename value_t = conditional_t<is_scalar_v<_Rhs>, typename _Lhs::value_t, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseBinaryExp<_Lhs, _Rhs, _Exp_op::_Ewise_add<value_t>>>
MATRICE_GLOBAL_FINL auto operator+(const _Lhs& _left, const _Rhs& _right) { 
	return _Op(_left, _right); 
}
// *\element-wise subtraction
template<
	typename _Lhs, class _Rhs,
	typename value_t = conditional_t<is_scalar_v<_Rhs>, typename _Lhs::value_t, typename _Rhs::value_t>,
	typename     _Op = _Exp::EwiseBinaryExp<_Lhs, _Rhs, _Exp_op::_Ewise_sub<value_t>>>
MATRICE_GLOBAL_FINL auto operator-(const _Lhs& _left, const _Rhs& _right) { 
	return _Op(_left, _right); 
}
// *\element-wise multiplication
template<
	typename _Lhs, typename _Rhs,
	typename value_t = conditional_t<is_scalar_v<_Rhs>, typename _Lhs::value_t, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseBinaryExp<_Lhs, _Rhs, _Exp_op::_Ewise_mul<value_t>>>
MATRICE_GLOBAL_FINL auto operator*(const _Lhs& _left, const _Rhs& _right) { 
	return _Op(_left, _right); 
}
// *\element-wise division
template<
	typename _Lhs, typename _Rhs,
	typename value_t = conditional_t<is_scalar_v<_Rhs>, _Rhs, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseBinaryExp<_Lhs, _Rhs, _Exp_op::_Ewise_div<value_t>>>
MATRICE_GLOBAL_FINL auto operator/(const _Lhs& _left, const _Rhs& _right) { 
	return _Op(_left, _right); 
}
// *\element-wise maximum
template<
	typename _Lhs, typename _Rhs,
	typename value_t = conditional_t<is_scalar_v<_Rhs>, _Rhs, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseBinaryExp<_Lhs, _Rhs, _Exp_op::_Ewise_max<value_t>>>
MATRICE_GLOBAL_FINL auto max(const _Lhs& _left, const _Rhs& _right) { 
	return _Op(_left, _right); 
}
// *\element-wise minimum
template<
	typename _Lhs, typename _Rhs,
	typename value_t = conditional_t<is_scalar_v<_Rhs>, typename _Lhs::value_t, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseBinaryExp<_Lhs, _Rhs, _Exp_op::_Ewise_min<value_t>>>
MATRICE_GLOBAL_FINL auto min(const _Lhs& _left, const _Rhs& _right) { 
	return _Op(_left, _right); 
}

// *\element-wise square operation
template<typename _Rhs>
MATRICE_GLOBAL_FINL constexpr auto sq(const _Rhs& _right) { 
	return (_right*_right); 
}

// *\element-wise squar root operation
template<
	typename _Rhs,
	typename value_t = enable_if_t<is_scalar_v<typename _Rhs::value_t>, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseUnaryExp<_Rhs, _Exp_op::_Ewise_sqrt<value_t>>>
MATRICE_GLOBAL_FINL auto sqrt(const _Rhs& _right) { 
	return _Op(_right); 
}

// *\element-wise exp()
template<
	typename _Rhs,
	typename value_t = enable_if_t<is_scalar_v<typename _Rhs::value_t>, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseUnaryExp<_Rhs, _Exp_op::_Ewise_exp<value_t>>>
MATRICE_GLOBAL_FINL auto exp(const _Rhs& _right) { 
	return _Op(_right); 
}

// *\element-wise log()
template<
	typename _Rhs,
	typename value_t = enable_if_t<is_scalar_v<typename _Rhs::value_t>, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseUnaryExp<_Rhs, _Exp_op::_Ewise_log<value_t>>>
MATRICE_GLOBAL_FINL auto log(const _Rhs& _right) { 
	return _Op(_right); 
}

// *\element-wise abs()
template<
	typename _Rhs,
	typename value_t = enable_if_t<is_scalar_v<typename _Rhs::value_t>, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseUnaryExp<_Rhs, _Exp_op::_Ewise_abs<value_t>>>
MATRICE_GLOBAL_FINL auto abs(const _Rhs& _right) { 
	return _Op(_right); 
}

// *\element-wise floor()
template<
	typename _Rhs,
	typename value_t = enable_if_t<is_scalar_v<typename _Rhs::value_t>, typename _Rhs::value_t>,
	typename _Op = _Exp::EwiseUnaryExp<_Rhs, _Exp_op::_Ewise_floor<value_t>>>
MATRICE_GLOBAL_FINL auto floor(const _Rhs& _right) { 
	return _Op(_right); 
}

// *\transpose expression
template<
	typename _Rhs,
	typename value_t = enable_if_t<is_scalar_v<typename _Rhs::value_t>, typename _Rhs::value_t>,
	typename _Op = _Exp::MatUnaryExp<_Rhs, _Exp_op::_Mat_trp<value_t>>>
MATRICE_GLOBAL_FINL auto transpose(const _Rhs& _right) { 
	return _Op(_right); 
}

// *\matmul expression
template<
	typename _Lhs, typename _Rhs,
	typename value_t = common_type_t<typename _Lhs::value_t, typename _Rhs::value_t>,
	typename _Op = _Exp::MatBinaryExp<_Lhs, _Rhs, _Exp_op::_Mat_mul<value_t>>>
MATRICE_GLOBAL_FINL auto matmul(const _Lhs& _left, const _Rhs& _right) {
	return _Op(_left, _right);
}

// *\outer product expression : xy^T
template<typename _Lhs, typename _Rhs>
MATRICE_GLOBAL_FINL auto outer_product(const _Lhs& _left, const _Rhs& _right) {
	return _left.mul(_right.t()).eval();
}

// *\summation of expression
template<
	typename _Rhs, 
	typename _Op = _Exp_op::_Accum_exp<_Rhs>>
	MATRICE_GLOBAL_FINL auto accum(const _Rhs& _right, _Op&& _op = _Op()) {
	return _op(_right);
}

// *\expression of the symetric part: sym(x)
template<typename _Ty>
MATRICE_GLOBAL_FINL auto sym(const _Ty& _x) noexcept {
	return (_x + transpose(_x)) * _Ty::value_t(0.5);
}

// *\expression of the skew part: skew(x)
template<typename _Ty>
MATRICE_GLOBAL_FINL auto skew(const _Ty& _x) noexcept {
	return (_x - transpose(_x)) * _Ty::value_t(0.5);
}

// *\expression of the covariance of the given data matrix '_x'.
template<typename _Ty>
MATRICE_GLOBAL_FINL auto covar(const _Ty& _x) noexcept {
	auto temp = _x - accum(_x) / _x.size();
	const auto fact = 2. / (_x.rows() + _x.cols());
	return (transpose(temp).mul(temp)*fact).eval();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

DGE_MATRICE_END