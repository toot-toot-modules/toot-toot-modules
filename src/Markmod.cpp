// MarkMod: squarewave generator
// Author: Mark Williams
// See LICENSE.txt for licensing information
// Modified from included template in module, using the BSD license 
 
#include "toot-toot-modules.hpp"

struct MarkMod : Module {
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;
	float sweepMod = 0.0;

	MarkMod() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

};


void MarkMod::step() {
	// Implement a simple square wave
	float deltaTime = engineGetSampleTime();
	

	// Compute the frequency from the pitch parameter and add input frequency
	// Add a sine for some nice sweeping
	float pitch = params[PITCH_PARAM].value;
	//pitch += inputs[PITCH_INPUT].value *  sinf(2.0f * M_PI * phase);
	pitch += inputs[PITCH_INPUT].value;
	pitch = clamp(pitch, -4.0f, 4.0f);

	//Setting base frequency to middle c
	float freq = 261.626f * powf(2.0f, pitch);

	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0f)
		phase -= 1.0f;

	// Compute the sine output
	//float sine = sinf(2.0f * M_PI * phase);
	//float saw = 4*sinf(7.0f * M_PI ) / (7 * M_PI) ;
	float square = sinf(2.0f * M_PI * phase) >=0.0 ? 1.0: -1.0;
	
	// Modulate to sweep the frequency
	sweepMod += deltaTime;
	if (sweepMod >= 1.0f)
	{	
		//random +=  % 2.0f 	
		sweepMod -= 1.0f;
	}
	outputs[SINE_OUTPUT].value =  5.0f * square + 3.0f * phase ;

	// Blink light at 1Hz
	blinkPhase += deltaTime;
	if (blinkPhase >= 1.0f)
		blinkPhase -= 1.0f;
	lights[BLINK_LIGHT].value = (blinkPhase <  pitch) || (blinkPhase < .5) ? 1.0f : 0.0f;
}


struct MarkModWidget : ModuleWidget {
	MarkModWidget(MarkMod *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/MarkMod.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, MarkMod::PITCH_PARAM, -3.0, 3.0, 0.0));

		addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, MarkMod::PITCH_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(33, 275), Port::OUTPUT, module, MarkMod::SINE_OUTPUT));

		addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, MarkMod::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelMarkMod = Model::create<MarkMod, MarkModWidget>("toot-toot-modules", "Mark-Mod", "Mark module", OSCILLATOR_TAG);
