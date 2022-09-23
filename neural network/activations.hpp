#pragma once
#include <utility.h>
#include <algorithms.h>
#include <cmath>
#include "utils.hpp"

namespace network {
	
	struct base_activation {
		template <range _Range>
		inline auto forward_apply(const _Range& _Rng) const {
			_Range _Res = _Rng;
			for (auto&& e : _Res) e = forward(e);
			return _Res;
		}
		template <range _Range>
		inline auto backward_apply(const _Range& _Rng) const {
			_Range _Res = _Rng;
			for (auto&& e : _Res) e = backward(e);
			return _Res;
		}
		virtual inline double forward(double x) const = 0;
		virtual inline double backward(double x) const = 0;
	};

	struct sigmoid : public base_activation {
		inline double forward(double x) const override {
			return 1.0 / (1 + std::exp(-x));
		}

		constexpr double backward(double x) const override {
			return x * (1 - x);
		}
	};

	struct tanh : public base_activation {
		inline double forward(double x) const override {
			return std::tanh(x);
		}

		constexpr double backward(double x) const override {
			return 1 - x * x;
		}
	};

	struct relu : public base_activation {
		constexpr double forward(double x) const override {
			return ::max(0.0, x);
		}

		constexpr double backward(double x) const override {
			return x > 0 ? 1 : 0;
		}
	};

	struct softmax : public base_activation {
		template <range _Range>
		inline auto forward_apply(const _Range& _Rng) const {
			_Range _Res = _Rng;
			for (int i = 0; i < _Res.rows(); ++i) {
				for (auto&& e : _Res[i]) e = std::exp(e);
				double _Sum = ::utils::sum(_Res[i], _Res[i] + _Res.columns());
				for (auto&& e : _Res[i]) e /= _Sum;
			}
			return _Res;
			
		}
		constexpr double forward(double x) const override {
			return x;
		}
		constexpr double backward(double x) const override {
			return x * (1 - x);
		}
	};

	template <class fn>
	concept activation_fn = is_same<fn, softmax> || 
		is_base_of_v<base_activation, fn> && requires(fn _Fn, double x) {
		_Fn.forward(x);
	};
}