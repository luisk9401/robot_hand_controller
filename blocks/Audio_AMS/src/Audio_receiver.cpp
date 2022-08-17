#include <systemc-ams>

#include "ADC.cpp"
#include "LPF.cpp"
template<int NBitsADC>
//sc_module(Audio_receiver){
class Audio_receiver : public sc_core::sc_module
{
   public:
	sca_tdf::sca_in<double> wave_in; 
	sca_tdf::sca_out<sc_dt::sc_int<NBitsADC>> adcout8;

	//LPF* lp;
	//ad_converter* ad_c;
	

	explicit Audio_receiver(sc_core::sc_module_name nm, double freq, sca_core::sca_time dt_adc_, double v_max): wave_in("wave_in"), adcout8("adcout8"),adc_1("adc_1",v_max), lp("lp",freq)
	{
		lp.in(wave_in);
		lp.out(w_out);
		
		adc_1.set_timestep(dt_adc_);
		adc_1.in_Analog(wave_in);
		adc_1.out_ADC(adcout8);
	
	
	/*	ar new Audio_receiver("ar");
		ar->wave_in(w_in);
		ar->adcout8(adcout8);
		
		lp = new LPF("lp", freq);
		lp->in(w_in);
		lp->out(w_out);
		
		ad_c = new ad_converter("adc");
		ad_c->in_Analog(w_out);
		ad_c->CS(CS)
		ad_c->out_ADC(adcout8);
	*/
	}
	
   private:
   	sca_tdf::sca_signal<double>  w_out;
   	ad_converter<NBitsADC> adc_1;
   	LPF lp;	

};

