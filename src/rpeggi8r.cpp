#include "toot-toot-modules.hpp"


struct Rpeggi8r : Module {
	enum ParamIds {
		FREQ_PARAM,
		A_PARAM,
		B_PARAM,
		C_PARAM,
		D_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		//FREQ_INPUT,
		//A_INPUT,
		//B_INPUT,
		//C_INPUT,
		//D_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ARP_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		//BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;
	float arpPhase = 0.0;
	int currentTone;

	Rpeggi8r();
	void step() override;

	void linearAppegiate();
};


Rpeggi8r::Rpeggi8r() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
	currentTone = A_PARAM;
}

void Rpeggi8r::step() {
	// Get the sample time for time related calculations
	float deltaTime = engineGetSampleTime();

	// Get the appregiation frequency
	float freq = params[FREQ_PARAM].value;

	// Appegiate when enough time has passed
	arpPhase += freq * deltaTime;
	if (arpPhase >= 1.0f) {
		arpPhase -= 1.0f;
		linearAppegiate();
	}

	// Get the output frequency for the current tone
	float tone = params[currentTone].value;

	// Accumulate phase for the output sine wave
	phase += tone * deltaTime;
	if (phase >= 1.0f)
		phase -= 1.0f;

	// Calculate the value of the current tone
	float sine = sinf(2.0f * M_PI * phase);

	// Send tone to arpeggiation output
	outputs[ARP_OUTPUT].value = 5.0f * sine;
}

void Rpeggi8r::linearAppegiate() {
	// Move to next tone
	currentTone++;

	// Reset back to first tone after moving past the last tone
	if (currentTone == NUM_PARAMS)
		currentTone = A_PARAM;
}

struct Rpeggi8rWidget : ModuleWidget {
	Rpeggi8rWidget(Rpeggi8r *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/rpeggi8r.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<RoundLargeBlackKnob>(Vec(30, 60), module, Rpeggi8r::FREQ_PARAM, 1.0f, 16.0f, 4.0f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(15, 115), module, Rpeggi8r::A_PARAM, 432.0f, 864.0f, 432.0f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(50, 115), module, Rpeggi8r::B_PARAM, 432.0f, 864.0f, 544.29f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(15, 150), module, Rpeggi8r::C_PARAM, 432.0f, 864.0f, 647.27f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(50, 150), module, Rpeggi8r::D_PARAM, 432.0f, 864.0f, 864.0f));

		//addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, Rpeggi8r::FREQ_INPUT));
		//addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, Rpeggi8r::A_INPUT));
		//addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, Rpeggi8r::B_INPUT));
		//addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, Rpeggi8r::C_INPUT));
		//addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, Rpeggi8r::D_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(50, 300), Port::OUTPUT, module, Rpeggi8r::ARP_OUTPUT));

		//addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, Rpeggi8r::BLINK_LIGHT));
	}
};


Model *modelRpeggi8r = Model::create<Rpeggi8r, Rpeggi8rWidget>("toot-toot-modules", "Rpeggi8r", "Quad input arpeggiator", OSCILLATOR_TAG);
