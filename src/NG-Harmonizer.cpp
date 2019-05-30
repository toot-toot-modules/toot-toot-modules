#include "toot-toot-modules.hpp"
#include "dsp/digital.hpp"
#include "dsp/ringbuffer.hpp"
#include "dsp/fft.hpp"

#define NUM_HARMONIES 4
#define HARMONY_KNOB_Y 130
#define HARMONY_IN_Y HARMONY_KNOB_Y + 50
#define HARMONY_OUT_Y HARMONY_IN_Y + 50
#define HARMONY_OUT_CV_Y HARMONY_OUT_Y + 50
#define WAVE_IN_X 62
#define WAVE_IN_Y 58
#define TOTAL_X 62
#define TOTAL_Y HARMONY_OUT_CV_Y + 50
#define THRU_X TOTAL_X - 47
#define THRU_Y TOTAL_Y
#define TOTAL_CV_X TOTAL_X + 47
#define TOTAL_CV_Y TOTAL_Y

#define OUT_FONT_SIZE 10.5f
#define IN_FONT_SIZE 12.0f


struct NGHarmonizer : Module {
	enum ParamIds {
		HARMONY_PARAM,
		NUM_PARAMS = HARMONY_PARAM + NUM_HARMONIES
	};
	enum InputIds {
		WAVE_INPUT,
		HARMONY_INPUT,
		NUM_INPUTS = HARMONY_INPUT + NUM_HARMONIES
	};
	enum OutputIds {
		THRU_OUTPUT,
		TOTAL_OUTPUT,
		TOTAL_OUTPUT_CV,
		HARMONY_OUTPUT,
		HARMONY_OUTPUT_CV = HARMONY_OUTPUT + NUM_HARMONIES,
		NUM_OUTPUTS = HARMONY_OUTPUT_CV + NUM_HARMONIES
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
	float phases[NUM_HARMONIES];

	SchmittTrigger input_trigger;

	enum StepRatios {
		DOWN_FIVE_HALF, DOWN_FIVE,
		DOWN_FOUR_HALF, DOWN_FOUR,
		DOWN_THREE_HALF, DOWN_THREE,
		DOWN_TWO_HALF, DOWN_TWO,
		DOWN_ONE_HALF, DOWN_ONE,
		DOWN_HALF,
		ONE,
		UP_HALF,
		UP_ONE, UP_ONE_HALF,
		UP_TWO, UP_TWO_HALF,
		UP_THREE, UP_THREE_HALF,
		UP_FOUR, UP_FOUR_HALF,
		UP_FIVE, UP_FIVE_HALF,
		NUM_STEP_RATIOS
	};

	float RatioTable[NUM_STEP_RATIOS] = { // from A as a baseline
		0.529727273f, // down five half (A#)
		0.561227273f, // down five (B)
		0.594613636f, // down four half (C)
		0.629954545f, // down four (C#)
		0.667409091f, // down three half (D)
		0.707113636f, // down three (D#)
		0.749159091f, // down two half (E)
		0.793704545f, // down two (F)
		0.840886364f, // down one half (F#)
		0.890909091f, // down one (G)
		0.943863636f, // down half (G#)
		1.0f,		  // one (A)
		1.059454545f, // up half (A#)
		1.122454545f, // up one (B)
		1.189204545f, // up one half (C)
		1.259931818f, // up two (C#)
		1.334840909f, // up two half (D)
		1.414204545f, // up three (D#)
		1.498295455f, // up three half (E)
		1.587409091f, // up four (F)
		1.681795455f, // up four half (F#)
		1.781795455f, // up five (G)
		1.88775f     // up five half (G#)
	};


	NGHarmonizer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		for (int i = 0; i < NUM_HARMONIES; i++) {
			phases[i] = 0.0f;
		}
	}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void NGHarmonizer::step() {
	float deltat = engineGetSampleTime();
	float in_v = inputs[WAVE_INPUT].value;
	float total_out = 0.0f;
	float total_out_cv = 0.0f;
	int harmony_params[NUM_HARMONIES];

	for (int i = 0; i < NUM_HARMONIES; ++i) {
		harmony_params[i] = params[HARMONY_PARAM + i].value;
		if (inputs[HARMONY_INPUT + i].active) {
			harmony_params[i] = abs(int((inputs[HARMONY_INPUT + i].value) * 2.2));
		}
	}

	// We only want to step if it's active
	if (inputs[WAVE_INPUT].active) {
		currStep++;
	}
	else { // If it's not active, reset
		currStep = 0;
		prevStep = 0;
	}

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


	for (int i = 0; i < NUM_HARMONIES; i++) {
		phases[i] += (inFreq * RatioTable[harmony_params[i]]) * deltat;
		if (phases[i] >= 1.0f)
			phases[i] -= 1.0f;
		float sine = sinf(2.0f * M_PI * phases[i]);
		total_out += (5.0f / (NUM_HARMONIES + 1) ) * sine;
		outputs[HARMONY_OUTPUT + i].value = 5.0f * sine;
		outputs[HARMONY_OUTPUT_CV + i].value = (5.0f * sine) + 5.0f;
	}

	outputs[TOTAL_OUTPUT].value = total_out + (in_v / (NUM_HARMONIES + 1));
	outputs[TOTAL_OUTPUT_CV].value = total_out + 5.0f;
	outputs[THRU_OUTPUT].value = in_v;
}

struct NGHarmonizerWidget : ModuleWidget {
	NGHarmonizerWidget(NGHarmonizer *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/NGHarmonizer.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		Label* harmonyLabel = new Label;
		harmonyLabel->color = nvgRGB(0, 0, 0);
		harmonyLabel->box.pos = Vec(box.size.x / 2 - 37, HARMONY_KNOB_Y - 38);
		harmonyLabel->text = "Harmonies";
		addChild(harmonyLabel);

		Label* cvLabel = new Label;
		cvLabel->color = nvgRGB(0, 0, 0);
		cvLabel->box.pos = Vec(box.size.x / 2 - 26, HARMONY_IN_Y - 20);
		cvLabel->text = "CV / IN";
		cvLabel->fontSize = IN_FONT_SIZE;
		addChild(cvLabel);

		Label* outputLabel = new Label;
		outputLabel->color = nvgRGB(255, 255, 255);
		outputLabel->box.pos = Vec(box.size.x / 2 - 18, HARMONY_OUT_Y - 18);
		outputLabel->text = "OUT";
		outputLabel->fontSize = OUT_FONT_SIZE;
		addChild(outputLabel);

		Label* outputCVLabel = new Label;
		outputCVLabel->color = nvgRGB(255, 255, 255);
		outputCVLabel->box.pos = Vec(box.size.x / 2 - 26, HARMONY_OUT_CV_Y - 18);
		outputCVLabel->text = "CV / OUT";
		outputCVLabel->fontSize = OUT_FONT_SIZE;
		addChild(outputCVLabel);

		for (int i = 0, j = 8; i < NUM_HARMONIES; i++, j += 35) {
			Label* harmonyNumber = new Label;
			harmonyNumber->color = nvgRGB(0, 0, 0);
			harmonyNumber->box.pos = Vec(j + 2.5, HARMONY_KNOB_Y - 20);
			harmonyNumber->text = std::to_string(i + 1);
			addChild(harmonyNumber);

			addParam(ParamWidget::create<RoundBlackSnapKnob>(Vec(j, HARMONY_KNOB_Y), module, NGHarmonizer::HARMONY_PARAM + i, 0.0f, NGHarmonizer::NUM_STEP_RATIOS - 1, NGHarmonizer::NUM_STEP_RATIOS / 2));
			addInput(Port::create<PJ301MPort>(Vec(j + 2, HARMONY_IN_Y), Port::INPUT, module, NGHarmonizer::HARMONY_INPUT + i));
			addOutput(Port::create<PJ301MPort>(Vec(j + 2, HARMONY_OUT_Y), Port::OUTPUT, module, NGHarmonizer::HARMONY_OUTPUT + i));
			addOutput(Port::create<PJ301MPort>(Vec(j + 2, HARMONY_OUT_CV_Y), Port::OUTPUT, module, NGHarmonizer::HARMONY_OUTPUT_CV + i));
		}

		addInput(Port::create<PJ301MPort>(Vec(WAVE_IN_X, WAVE_IN_Y), Port::INPUT, module, NGHarmonizer::WAVE_INPUT));

		Label* waveLabel = new Label;
		waveLabel->color = nvgRGB(0, 0, 0);
		waveLabel->box.pos = Vec(WAVE_IN_X - 12, WAVE_IN_Y - 18);
		waveLabel->text = "V/OCT";
		waveLabel->fontSize = IN_FONT_SIZE;
		addChild(waveLabel);

		addOutput(Port::create<PJ301MPort>(Vec(THRU_X, THRU_Y), Port::OUTPUT, module, NGHarmonizer::THRU_OUTPUT));

		Label* thruLabel = new Label;
		thruLabel->color = nvgRGB(255, 255, 255);
		thruLabel->box.pos = Vec(THRU_X - 9, THRU_Y - 18);
		thruLabel->text = "THRU";
		thruLabel->fontSize = OUT_FONT_SIZE;
		addChild(thruLabel);


		addOutput(Port::create<PJ301MPort>(Vec(TOTAL_X, TOTAL_Y), Port::OUTPUT, module, NGHarmonizer::TOTAL_OUTPUT));

		Label* totalLabel = new Label;
		totalLabel->color = nvgRGB(255, 255, 255);
		totalLabel->box.pos = Vec(TOTAL_X - 5, TOTAL_Y - 18);
		totalLabel->text = "TOT";
		totalLabel->fontSize = OUT_FONT_SIZE;
		addChild(totalLabel);

		addOutput(Port::create<PJ301MPort>(Vec(TOTAL_CV_X, TOTAL_CV_Y), Port::OUTPUT, module, NGHarmonizer::TOTAL_OUTPUT_CV));

		Label* totalCVLabel = new Label;
		totalCVLabel->color = nvgRGB(255, 255, 255);
		totalCVLabel->box.pos = Vec(TOTAL_CV_X - 13, TOTAL_CV_Y - 18);
		totalCVLabel->text = "TOT/CV";
		totalCVLabel->fontSize = OUT_FONT_SIZE;
		addChild(totalCVLabel);
		//addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, NGHarmonizer::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelNGHarmonizer = Model::create<NGHarmonizer, NGHarmonizerWidget>("toot-toot-modules", "NGHarmonizer", "NG Harmonizer", OSCILLATOR_TAG);
