/*********************************************************************
This file is part of Matrice, an effcient and elegant C++ library for 
3D Vision and Photo-Mechanics.
Copyright(C) 2018-2021, Zhilong(Dgelom) Su, all rights reserved.
*********************************************************************/
#pragma once
#include "../core.hpp"

MATRICE_ALG_BEGIN(vision)
inline _DETAIL_BEGIN

// Tag dispatcher, for rays traveling from air to glass.
struct air2glass_tag {
	static constexpr auto n1 = 1.f;
	static constexpr auto n2 = 1.52f;
};

// Tag dispatcher, for rays traveling from glass to air.
struct glass2water_tag {
	static constexpr auto n1 = 1.52f;
	static constexpr auto n2 = 1.33f;
};

// Traits
template<typename _Ty> 
struct is_refractive_tag : std::false_type {};
struct is_refractive_tag<air2glass_tag> : std::true_type {};
struct is_refractive_tag<glass2water_tag> : std::true_type {};
template<typename _Ty>
constexpr auto is_refractive_tag_v = is_refractive_tag<_Ty>::value;

/// <summary>
/// \brief CLASS TEMPLATE, refractive 3d reconstruction.
/// This module is thread-safe.
/// </summary>
/// <typeparam name="_Ty">data type</typeparam>
template<typename _Ty> 
requires is_floating_point_v<_Ty>
class _Refractive_reconstruction {
	using _Myt = _Refractive_reconstruction;
public:
	using value_type = _Ty;
	template<size_t _Dim> using vector_t = auto_vector_t<value_type, _Dim>;

	/// <summary>
	/// \brief Refracting interface type with normalized normal.
	/// </summary>
	struct interface_type
	{
		MATRICE_HOST_FINL interface_type() = default;
		MATRICE_HOST_FINL
		interface_type(value_type _Nx, value_type _Ny, value_type _Nz) noexcept
			: _Mynormal{_Nx, _Ny, _Nz} {
			const auto _Norm = _Mynormal.norm<2>();
			_Norm == 1 ? _Mynormal.normalize(_Norm) : ;
		}

		MATRICE_HOST_FINL decltype(auto) normal() const noexcept {
			return _Mynormal;
		}
		MATRICE_HOST_FINL decltype(auto) normal() noexcept {
			return _Mynormal;
		}

		// normal vector
		vector_t<3> _Mynormal;

		// distance $D$ from the origin, for defining interface $\Pi_1$
		value_type  _Mydist{ 0 };

		// interface shift $d$, for defining interface $\Pi_2$
		value_type  _Myshift{ 0 };
	};

	/// <summary>
	/// \brief Light ray type with origin and direction.
	/// </summary>
	struct ray_type
	{
		vector_t<3> _Myorigin;
		vector_t<3> _Mydirection;
	};

	/**
	 * \brief CTOR, create an object with a given interface.
	 */
	explicit _Refractive_reconstruction(interface_type&& _Interf) noexcept
		:_Myinterface(_Interf) {
	}

	/**
	 * \brief METHOD, set the origin of the reference frame $O-XYZ$.
	 */
	_Myt& set_origin(const vector_t<3>& _O) noexcept {
		_Myorigin = _O;
		return (*this);
	}
	
	/**
	 * \brief METHOD, set the translation of the right camera relative to  the frame $O-XYZ$.
	 */
	_Myt& set_t(const vector_t<3>& _t) noexcept {
		_Mytrans = _t;
		return (*this);
	}

	/**
	 * \brief METHOD, compute the true object point $\mathbf{Q}$.
	 * \param '_P' the distorted object point $\mathbf{P}$, estimated by regular triangulation.
	 * \return 'Q' the true object point.
	 */
	auto compute(const vector_t<3>& _P) const noexcept {
		const auto [left_direc, right_direc] = _Eval_incident_directions(_P);

	}

private:
	auto _Eval_incident_directions(const vector_t<3>& _P) const noexcept;

	template<typename _Tag, MATRICE_ENABLE_IF(is_refractive_tag_v<_Tag>)>
	auto _Eval_intersections(_Tag, const ray_type& l) const noexcept;

	interface_type _Myinterface;

	// \sa Point $\mathbf{O}$
	vector_t<3> _Myorigin;

	// \sa Point $\mathbf{t} = (t_x, t_y, t_z)$
	vector_t<3> _Mytrans;

};
_DETAIL_END

/// <summary>
/// ALIAS TEMPLAE
/// \sa detail::_Refractive_reconstruction
/// </summary>
/// <typeparam name="_Ty"></typeparam>
template<typename _Ty>
using refractive_reconstruction = detail::_Refractive_reconstruction;

MATRICE_ALG_END(vision)

#include "_refractive3d.inl"