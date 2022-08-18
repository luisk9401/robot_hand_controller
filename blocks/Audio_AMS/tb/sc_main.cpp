#include "systemc.h"
#include "systemc-ams.h"
#include "sin_src.cpp"
#include "wav_src.cpp"

//#include "LPF.cpp"
//#include "ADC.cpp"
#include "../src/Audio_receiver.cpp"

int sc_main(int argc, char* argv[])
{

	using namespace sc_core;
 	using namespace sca_util;
 	
 	const double v_supply = 5.0; // V
 	// Simulation stop time
 	
 	const sc_time t_stop(1.50, SC_MS);
 	// ADC sampling period (equal to TDF time step)
 	const sc_time dt_adc(5.0, SC_NS);	

  	// Resolution of the ADC
 	const int NBitsADC = 16;
  
  	sca_tdf::sca_signal<double> wave_in("wave_in");
  
  	sca_tdf::sca_signal<sc_dt::sc_int<NBitsADC> > adc_sig("adc_sig"); 	

  	//sin_src sine("sine", 1.0 , 1.0e3, sca_core::sca_time( 5.0, sc_core::SC_NS ) );
  	//sine.out(wave_in);
  	
  	wav_src wav("wav", 1.0 , sca_core::sca_time( 5.0, sc_core::SC_NS ) );
  	wav.out(wave_in);

  	Audio_receiver<NBitsADC> receiver("receiver",20000.,dt_adc, v_supply);
  	receiver.wave_in(wave_in);
  	receiver.adcout8(adc_sig);
  	// tracing
  	sca_util::sca_trace_file* atf = sca_util::sca_create_vcd_trace_file( "ar.vcd" );
  	//sca_util::sca_trace( atf, in_bits, "in_bits" );
  	sca_util::sca_trace( atf, wave_in, "wave_in" );
  	sca_util::sca_trace( atf, adc_sig, "adc_sig" );
  
  //Simulation

  	std::cout << "Simulation started..." << std::endl;

  	sc_core::sc_start(t_stop);

  	std::cout << "Simulation finished." << std::endl;

  	sca_util::sca_close_vcd_trace_file( atf );
  	sc_stop();

  return 0;
}
