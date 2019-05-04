#include "toot-toot-modules.hpp"
#include "dsp/digital.hpp"
#include "dsp/ringbuffer.hpp"
#include "dsp/fft.hpp"

struct NGHarmonizer : Module {
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		WAVE_INPUT,
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_LOW,
		OUTPUT_THRU,
		OUTPUT_HIGH,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float upper = 10.0f;
	float lower = -10.0f;
	

	unsigned int currStep = 0;
	unsigned int prevStep = 0;

	float inFreq = 0.0f;
	float high_phase = 0.0f;
	float low_phase = 0.0f;

	SchmittTrigger input_trigger;

	NGHarmonizer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void NGHarmonizer::step() {
	float deltat = engineGetSampleTime();
	float in_v = inputs[WAVE_INPUT].value;
	float pitch = params[PITCH_PARAM].value;
	if (inputs[PITCH_INPUT].active == true) {
		pitch = inputs[PITCH_INPUT].value;
	}


	currStep++;
	if (input_trigger.process(rescale(in_v, 0.1f, 1.7f, 0.0f, 1.0f))) {
		if (prevStep == 0) {
			inFreq = 0;
		}
		else {
			inFreq = floor(engineGetSampleRate() / (float)(currStep - prevStep));
			//printf("%f\n", inFreq);
		}
		prevStep = currStep;
	}



	low_phase += (inFreq * 0.749159091f) * deltat;
	if (low_phase >= 1.0f)
		low_phase -= 1.0f;

	float low_sine = sinf(2.0f * M_PI * low_phase);
	outputs[OUTPUT_LOW].value = 5.0f * low_sine;

	high_phase += (inFreq * 1.334840909f) * deltat;
	if (high_phase >= 1.0f)
		high_phase -= 1.0f;
	
	float high_sine = sinf(2.0f * M_PI * high_phase);
	outputs[OUTPUT_HIGH].value = 5.0f * high_sine;




	outputs[OUTPUT_THRU].value = in_v;
	//outputs[OUTPUT_HIGH].value = output_high;
}

struct NGHarmonizerWidget : ModuleWidget {
	NGHarmonizerWidget(NGHarmonizer *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/NGHarmonizer.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, NGHarmonizer::PITCH_PARAM, -10.0, 10.0, 0.0));
		addInput(Port::create<PJ301MPort>(Vec(33, 150), Port::INPUT, module, NGHarmonizer::PITCH_INPUT));

		addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, NGHarmonizer::WAVE_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(5, 255), Port::OUTPUT, module, NGHarmonizer::OUTPUT_LOW));
		addOutput(Port::create<PJ301MPort>(Vec(33, 255), Port::OUTPUT, module, NGHarmonizer::OUTPUT_THRU));
		addOutput(Port::create<PJ301MPort>(Vec(61, 255), Port::OUTPUT, module, NGHarmonizer::OUTPUT_HIGH));

		addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, NGHarmonizer::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelNGHarmonizer = Model::create<NGHarmonizer, NGHarmonizerWidget>("toot-toot-modules", "NGHarmonizer", "NG Harmonizer", OSCILLATOR_TAG);
