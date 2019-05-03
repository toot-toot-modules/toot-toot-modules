#include "toot-toot-modules.hpp"


struct NGHarmonizer : Module {
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		WAVE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_LOW,
		OUTPUT_HIGH,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;

	NGHarmonizer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void NGHarmonizer::step() {
	float pitch = params[PITCH_PARAM].value;
	float input_wave = inputs[WAVE_INPUT].value;

	outputs[OUTPUT_LOW].value = input_wave - pitch;
	outputs[OUTPUT_HIGH].value = input_wave + pitch;
}


struct NGHarmonizerWidget : ModuleWidget {
	NGHarmonizerWidget(NGHarmonizer *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/NGHarmonizer.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, NGHarmonizer::PITCH_PARAM, -3.0, 3.0, 0.0));

		addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, NGHarmonizer::WAVE_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(18, 275), Port::OUTPUT, module, NGHarmonizer::OUTPUT_LOW));
		addOutput(Port::create<PJ301MPort>(Vec(48, 275), Port::OUTPUT, module, NGHarmonizer::OUTPUT_HIGH));

		addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, NGHarmonizer::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelNGHarmonizer = Model::create<NGHarmonizer, NGHarmonizerWidget>("toot-toot-modules", "NGHarmonizer", "NG Harmonizer", OSCILLATOR_TAG);
