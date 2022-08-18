#include <iostream>
#include <cmath>
#include "AudioFile.h"

class wav_src : public sca_tdf::sca_module
{
public:

 	sca_tdf::sca_out<double> out; // output port
	wav_src( sc_core::sc_module_name nm, double ampl_,
		          sca_core::sca_time Tm_ )
	: out("out"), ampl(ampl_), Tm(Tm_)
	{}

	void initialize() 
	{
		inputFilePath = "./test-audio.wav";
		loadedOK = a.load (inputFilePath);
		i = 0;
		cout << "Ini" << i++ ; 
	}

	void set_attributes()
	{
	  set_timestep(Tm);
	}

	void processing()
	{
	    	double data = a.samples[1][i]*ampl;
		out.write((double)data);
		i++;

	}
	private:
		int i;
		double ampl; // gain		
  		sca_core::sca_time Tm; // module time step
  		std::string inputFilePath;
  		AudioFile<double> a;
  		bool loadedOK;
};
