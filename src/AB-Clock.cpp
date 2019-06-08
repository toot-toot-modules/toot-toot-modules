#include "toot-toot-modules.hpp"


struct ABClock : Module {
	enum ParamIds {
		FREQ_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
        SAW_OUTPUT,
        SQUARE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float phase = 0.0;

	ABClock() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void ABClock::step() {
	float deltaTime = engineGetSampleTime();

    // scale bpm based on an input knob, 120bpm in the middle
    float freq = 2.0f + params[FREQ_PARAM].value;

	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0f)
		phase -= 1.0f;

	// Compute the output
    // Doubling the saw to match the square wave, since things trigger on the voltage change
    if (phase < 0.5f) {
	    outputs[SAW_OUTPUT].value = 5.0f * (1 - (2 * phase));
    } else {
        outputs[SAW_OUTPUT].value = 5.0f * (2 * phase);
    }
    outputs[SQUARE_OUTPUT].value = (phase < 0.5f) ? 5.0f : 0.0f;
}


struct ABClockWidget : ModuleWidget {
	ABClockWidget(ABClock *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/ABClock.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, ABClock::FREQ_PARAM, -2.0, 2.0, 0.0));

		addOutput(Port::create<PJ301MPort>(Vec(33, 225), Port::OUTPUT, module, ABClock::SAW_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(33, 275), Port::OUTPUT, module, ABClock::SQUARE_OUTPUT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelABClock = Model::create<ABClock, ABClockWidget>("toot-toot-modules", "AB-Clock", "AB Clock", OSCILLATOR_TAG);
