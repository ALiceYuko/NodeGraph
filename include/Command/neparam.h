#ifndef NEPARAMETERS_H
#define NEPARAMETERS_H

#include <string>

namespace node_edit
{
	class NEParameters
	{
	public:
		NEParameters() = default;
	};

	template<class T>
	class PODParameters :public NEParameters {
	private:
		T value;
	public:
		PODParameters(T v) :value(v) {}
		void setValue(T v) { value = v; }
		T getValue() { return value; }
	};


	class StringParameters :public NEParameters {
	private:
		std::string value;
	public:
		StringParameters(std::string v) :value(v) {}
		void setValue(std::string v) { value = v; }
		std::string getValue() { return value; }
	};

	class MouseMoveParameters : public NEParameters {
	private:
		double x_s;
		double y_s;
		double x_e;
		double y_e;
	public:
		MouseMoveParameters() :x_s(0), y_s(0), x_e(0), y_e(0) {}
		void set_start_pnt(double vx_s, double vy_s) { x_s = vx_s; y_s = vy_s; }
		void set_end_pnt(double vx_e, double vy_e) { x_e = vx_e; y_e = vy_e; }
		double getx_s() { return x_s; }
		double gety_s() { return y_s; }
		double getx_e() { return x_e; }
		double gety_e() { return y_e; }

	};
	#endif // PARAMETERS_H	
}

