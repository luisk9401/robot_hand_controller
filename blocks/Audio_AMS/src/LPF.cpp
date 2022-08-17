#include "systemc.h"
#include "systemc-ams.h"

SCA_TDF_MODULE(LPF) 
{// a lowpass filter using an ltf module
	sca_tdf::sca_in<double> in;// input double (wave) 
	sca_tdf::sca_out<double> out;// output is the filtered wave	
	
	 
	
	LPF(sc_core::sc_module_name n, double freq_cut):in("in"), out("out"), freq_cut(freq_cut) {}
	
	void initialize() 
	{
		Nom(0)= 1.0; 
		Denom(0)=1.0; // values for the LTF    
		Denom(1)= 1.0/(2.0*M_PI*freq_cut);// to describe a lowpass-filter   
	} 
	void processing()
	{     
		out.write(ltf_1(Nom,Denom, in.read()));   
	} 
	private:
		sca_tdf::sca_ltf_nd ltf_1;// The Laplace-Transform module  
		sca_util::sca_vector<double> Nom, Denom;// Vectors for the Laplace-Transform module 
		double freq_cut;// the cutoff-frequency of the lowpass
};
