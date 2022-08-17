#include "systemc.h"
#include "systemc-ams.h"
template<int NBitsADC>

class ad_converter : public sca_tdf::sca_module
{
  public:	
	//TDF input ports
	sca_tdf::sca_in<double> in_Analog;
	sca_tdf::sca_out<sc_dt::sc_int<NBitsADC>> out_ADC;//16 o 32?
	//variable declaration
	//sc_uint<8> bin_value;
	//double an_value;
	//int analog_2_int, i, r, dev;
	//module constructor
	ad_converter(sc_core::sc_module_name,double v_max): in_Analog("in_Analog"), out_ADC("out_ADC"), v_max(v_max)
	{
		sc_assert((2 <= NBitsADC) && (NBitsADC <= 64)); 
		sc_assert(v_max > 0.0);		
	}
	void processing()
	{
		using namespace std;
		//Process done when the CS is high
		double v_in = in_Analog.read();
		if (v_in < -v_max) {
			out_ADC.write(-((1 << (NBitsADC - 1)) - 1));
		} else if (v_in > v_max) {
			out_ADC.write((1 << (NBitsADC - 1)) - 1);
		} else {
			sc_dt::sc_int<NBitsADC>
			q_v_in = lround((v_in / v_max) * ((1 << (NBitsADC - 1)) - 1));
			out_ADC.write(q_v_in);
		}
			
	
	/*	//Analog sampled value to integer
		an_value = in_Analog.read();
		analog_2_int = (int) (an_value);
		//conversion to binary
		for(i=0; i<8; i++)
		{
			r = analog_2_int % 2;
			dev = analog_2_int / 2;
			bin_value[i] = r;
			analog_2_int = dev;
		}
		//Output result
		out_ADC.write(bin_value);
	*/	
	}
	private: 
		 const double v_max;
};

