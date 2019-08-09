
#include "../../include/Matrice/core/solver.h"
#include "../../include/Matrice/core/matrix.h"
#include "../../include/Matrice/private/nonfree/_lnalge.h"

DGE_MATRICE_BEGIN _DETAIL_BEGIN

/**
 *\Perform decomposition for coeff. matrix A.
 */
template<typename _T> LinearOp::info_t LinearOp::OpBase<_T>::_Impl(view_t& A)
{
	typename view_t::pointer pCoef = A.data();
	size_t _M = A.rows(), _N = A.cols();
	if (_M != _N) throw std::runtime_error("Support only for square matrix.");

	int layout = layout_traits<view_t>::is_rmajor(A.format) ? rmaj : cmaj;
	info_t info;

	if (layout_traits<view_t>::is_symmetric(A.format)) {
		info.alg = solver_type::CHD;
		if constexpr (type_bytes<value_t>::value == 4)
#if MATRICE_MATH_KERNEL==MATRICE_USE_MKL
			info.status = LAPACKE_spotrf(layout, 'L', _N, (float*)pCoef, _N);
#elif MATRICE_MATH_KERNEL==MATRICE_USE_FKL
			info.status = flapk::_scholy(fkl::sptr(pCoef), _N);
#endif
		if constexpr (type_bytes<value_t>::value == 8)
#if MATRICE_MATH_KERNEL==MATRICE_USE_MKL
			info.status = LAPACKE_dpotrf(layout, 'L', _N, (double*)pCoef, _N);
#elif MATRICE_MATH_KERNEL == MATRICE_USE_FKL
			info.status = flapk::_dcholy(fkl::dptr(pCoef), _N);
#endif
		return info;
	}

	{ //general dense matrix
		info.alg = solver_type::LUF;
		Matrix_<int, view_t::CompileTimeCols, min(view_t::CompileTimeCols, 1)> iwp(_N, 1);
		if constexpr (type_bytes<value_t>::value == 4)
#if MATRICE_MATH_KERNEL==MATRICE_USE_MKL
			info.status = LAPACKE_sgetrf(layout, _M, _N, (float*)pCoef, _N, iwp.data());
#elif MATRICE_MATH_KERNEL==MATRICE_USE_FKL
			info.status = flapk::_sLU(fkl::sptr(pCoef), iwp.data(), _N);
#endif
		if constexpr (type_bytes<value_t>::value == 8)
#if MATRICE_MATH_KERNEL==MATRICE_USE_MKL
			info.status = LAPACKE_dgetrf(layout, _M, _N, (double*)pCoef, _N, iwp.data());
#elif MATRICE_MATH_KERNEL==MATRICE_USE_FKL
			info.status = flapk::_dLU(fkl::dptr(pCoef), iwp.data(), _N);
#endif
		for (int i = 1; i <= _N; ++i) if (i != iwp(i)) info.sign *= -1;
		return info;
	}
}
template LinearOp::info_t LinearOp::OpBase<float>::_Impl(Matrix_<value_t, ::dynamic>&);
template LinearOp::info_t LinearOp::OpBase<double>::_Impl(Matrix_<value_t, ::dynamic>&);

/**
 *\Solve the solution X.
 */
template<typename _T> void LinearOp::OpBase<_T>::_Impl(view_t& A, view_t& X)
{
	const auto n = A.cols() - 1;
	const auto inc = X.cols();

	if (this->_Info.alg == solver_type::GLS) return;

	auto _fn = [&](typename view_t::iterator b)->auto {
		for (auto i = 1; i <= n; ++i) {
			value_t sum = value_t(0);
			for (auto j = 0; j < i; ++j) 
				sum += A[i][j] * b[j*inc];
			b[i*inc] -= sum;
		}
		b[n*inc] = b[n*inc] / A[n][n];
		return b;
	};
	
	if (this->_Info.alg == solver_type::CHD) {
		auto L = A.t();
		for (auto c = 0; c < inc; ++c) {
			auto b = _fn(X.begin() + c);
			for (auto i = n - 1; i >= 0; --i) {
				auto k = i * (n + 1);
				value_t sum = value_t(0);
				for (auto j = i + 1; j <= n; ++j)
					sum += L(j + k)*b[j*inc];
				b[i*inc] = (b[i*inc] - sum) / L(i + k);
			}
		}
		return;
	}
	if (this->_Info.alg == solver_type::LUF) {
		for (auto c = 0; c < inc; ++c) {
			auto b = _fn(X.begin() + c);
			for (auto i = n - 1; i >= 0; --i) {
				value_t sum = value_t(0);
				for (auto j = i + 1; j <= n; ++j)
					sum += A[i][j] * b[j*inc];
				b[i*inc] = (b[i*inc] - sum) / A[i][i];
			}
		}
		return;
	}
}
template void LinearOp::OpBase<float>::_Impl(Matrix_<value_t, ::dynamic>&, Matrix_<value_t, ::dynamic>&);
template void LinearOp::OpBase<double>::_Impl(Matrix_<value_t, ::dynamic>&, Matrix_<value_t, ::dynamic>&);

/**
 *\Perform SVD decomposition for U.
 */
template<typename _T> LinearOp::info_t LinearOp::OpBase<_T>::_Impl(view_t& U, view_t& S, view_t& Vt)
{
	using value_t = typename view_t::value_t;
	if (S.empty) S.create(U.cols(), 1);
	if (Vt.empty) Vt.create(U.cols(), U.cols());

	info_t info;
	info.status = detail::_Lapack_kernel_impl<value_t>::svd(
		U.data(), S.data(), Vt.data(), U.shape().tiled());
	info.alg = solver_type::SVD;

	return info;
}
template LinearOp::info_t LinearOp::OpBase<float>::_Impl(Matrix_<value_t, ::dynamic>&, Matrix_<value_t, ::dynamic>&, Matrix_<value_t, ::dynamic>&);
template LinearOp::info_t LinearOp::OpBase<double>::_Impl(Matrix_<value_t, ::dynamic>&, Matrix_<value_t, ::dynamic>&, Matrix_<value_t, ::dynamic>&);

_DETAIL_END DGE_MATRICE_END
