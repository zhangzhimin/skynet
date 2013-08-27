/******************************************************************************
Created By : Zhang Zhimin
Created On : 2013/3/5
Purpose    : 
********************************************************************************/

#pragma once

#include <skynet\core\logic.hpp>

#include <skynet\utility\cycle_num.hpp>
#include <skynet\utility\math.hpp>


namespace skynet{namespace geometry{

	//define a ray for the ray lazy_cast. it's useful but not effective.
	template <typename size_t D>
	class ray{
	public:
		static const size_t			dim = D;
		typedef point<float, dim>	point_type;
		typedef point<float, dim>	speed_type;

		ray(const point_type &pos, const speed_type &dir) no_throw
			: m_start_point(pos), m_current_point(pos), m_speed(dir), m_time(0), 
			m_end_time(std::numeric_limits<size_t>::max()){ }

		point_type start_point() const						{ return m_start_point; }
		speed_type speed() const							{ return m_speed; }
		point_type current_point() const					{ return m_current_point; }

		size_t time()	const								{ return m_time; }
		void time(const size_t &v){ 
			m_time = v; 
			m_current_point = m_start_point + m_time * m_speed;
		}
		size_t end_time() const								{ return m_end_time; }
		void end_time(const size_t &v)						{ m_end_time = v; }

		void march(){
			++m_time;
			m_current_point += m_speed;
		}

		bool termination() const{
			return m_time >= m_end_time;
		}

	private:
		point_type	m_start_point;
		point_type	m_current_point;
		speed_type	m_speed;
		size_t		m_time;
		size_t		m_end_time;
	};

	enum ray_stop_type{
		terminate = 0,
		break_off = 1
	};

	//the Func should be the functor : return bool, input ray.
	template <typename size_t D, typename Func>
	ray_stop_type ray_cast(ray<D> &ray, Func fun){
		while (true){
			if (ray.termination())
				return terminate;

			if (!fun(ray))
				return break_off;

			ray.march();
		}
	}

	template <typename size_t DN>
	class circle_directions : public std::array<point<float, 2>, DN>{
	public:
		static const size_t direction_num = DN;

		circle_directions(){
			float theta = 2.0f*PI / DN;

			for (int i = 0; i < direction_num; ++i){
				auto temp = theta * i;
				(*this)[i] = point<float, 2>(std::cos(temp), std::sin(temp));
			}
		}
	};

	template <typename M>
	index2 center2D(const M &mat, const index2 &init_index, int max_counter = 40){
		static const size_t DN = 16;
		static const circle_directions<DN>  speeds;

		mask2d mat_valid(mat.extent());
		mat_valid.set_all_bit_true();
		set_boundary(mat_valid, false);
		auto mat_safe = and(mat, mat_valid);

		cycle_num<0, DN> c_num1(0);
		cycle_num<0, DN> c_num2(DN/2);
		ray<2>::point_type cur_index = static_cast<ray<2>::point_type>(init_index);
		for (int i = 0; i < max_counter; ++i){
			ray<2> ray1(cur_index, speeds[c_num1]);
			ray<2> ray2(cur_index, speeds[c_num2]);
			++c_num1;
			++c_num2;

			auto fun = [&mat_safe](const ray<2> &ray)->bool{
				auto index = nearest_neighbor(ray.current_point());
				return mat_safe.get_value(index);
			};
			ray_cast(ray1, fun);
			ray_cast(ray2, fun);
			auto temp_index = 0.5f*(ray1.current_point()+ray2.current_point());

			cur_index = 0.8f * temp_index + 0.2f * cur_index;
		}

		return nearest_neighbor(cur_index);
	}

}}