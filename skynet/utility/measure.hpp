/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/10/24
Purpose    : define some physical measurement unit to measure the real image size. 
********************************************************************************/

#pragma once

#include <skynet/config.hpp>

namespace skynet{namespace measure{

	template <typename unit_type_>
	struct length_express{
		typedef unit_type_	unit_type;

		unit_type &operator()(){
			return *static_cast<unit_type *>(this);
		}

		const unit_type &operator()() const{
			return *static_cast<const unit_type *>(this);
		}
	};

	class mm : public length_express<mm>{
	public:
		typedef mm	 unit_type;
		double value;

		mm(const double &v) : value(v) { }
		mm &operator=(const double &v){
			value = v;
			return *this;
		}

		template <typename unit_type>
		mm(const length_express<unit_type> &l_e){
			value = l_e().value * length_express<unit_type>::unit_type::m_per_unit;
		}
		
		const static int m_per_unit = 1;
	};

	class cm : public length_express<cm>{
	public:
		typedef cm	unit_type;
		double value;

		cm(const double &v) : value(v) {}
		cm &operator=(const double &v){
			value = v;
			return *this;
		}

		template <typename unit_type_>
		cm(const length_express<unit_type_> &l_e){
			value = l_e().value * length_express<unit_type_>::unit_type::m_per_unit / unit_type::m_per_unit;
		}

		const static int m_per_unit  = 10;
	};

	//-------------------------------------operations------------------------------------------------------------

	template <typename L1, typename L2>
	L1 operator+(const length_express<L1> &lhs, const length_express<L2> &rhs){
		 return (lhs().value * L1::m_per_unit + rhs().value * L2::m_per_unit) / L1::m_per_unit;
	}
	
	template <typename L1, typename L2>
	L1 operator-(const length_express<L1> &lhs, const length_express<L2> &rhs){
		 return (lhs().value * L1::m_per_unit - rhs().value * L2::m_per_unit) / L1::m_per_unit;
	}

	template <typename L>
	L operator*(const double &scale, const length_express<L> &l_e){
		return L(l_e().value * scale);
	}

	template <typename L>
	void operator*=(length_express<L> &l_e, const double &scale){
		l_e().value *= scale;
	}

}}
