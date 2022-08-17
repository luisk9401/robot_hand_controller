
#ifndef _BIT_SRC_H_
#define _BIT_SRC_H_

#include <systemc-ams>

SCA_TDF_MODULE(ADC)
{
	sca_tdf::sca_de::sca_out<sc_uint<8>> out_ADC;//16 o 32?
	sca_tdf::sca_in<double> in_Analog;
	sca_tdf::sca_in<bool> CS;
 
	
  	SCA_CTOR(ADC);

  	void processing();
};

#endif // _BIT_SRC_H_
