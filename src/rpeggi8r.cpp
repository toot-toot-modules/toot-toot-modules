#include <random>
#include "toot-toot-modules.hpp"


struct Rpeggi8r : Module {
	enum ParamIds {
		FREQ_PARAM,
		ARP_PARAM,
		A_PARAM,
		B_PARAM,
		C_PARAM,
		D_PARAM,
		E_PARAM,
		F_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		ARP_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	std::ranlux24 random;
	std::uniform_int_distribution<> toneDistribution;

	float phase = 0.0;
	float blinkPhase = 0.0;
	float arpPhase = 0.0;
	int currentTone;
	int previousTone;
	int previousTripleTone;
	int direction;

	Rpeggi8r();
	void step() override;

	void arpeggiate();
	void linearArpeggiate();
	void linearBidirectionalArpeggiate();
	void linearTriplesArpeggiate();
	void randomArpeggiate();
	void randomNoRepeatArpeggiate();
};


Rpeggi8r::Rpeggi8r()
  : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
  , random()
  , toneDistribution(A_PARAM, NUM_PARAMS-1)
{
	currentTone = A_PARAM;
	previousTone = NUM_PARAMS-1;
	previousTripleTone = previousTone - 1;
	direction = +1;
}

void Rpeggi8r::step() {
	// Get the sample time for time related calculations
	float deltaTime = engineGetSampleTime();

	// Get the arpeggiation frequency
	float freq = params[FREQ_PARAM].value;

	// Arpeggiate when enough time has passed
	arpPhase += freq * deltaTime;
	if (arpPhase >= 1.0f) {
		arpPhase -= 1.0f;
		arpeggiate();
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

void Rpeggi8r::arpeggiate() {
	int method = params[ARP_PARAM].value;
	switch (method) {
		case 1:
			linearArpeggiate();
			break;
		case 2:
			linearBidirectionalArpeggiate();
			break;
		case 3:
			linearTriplesArpeggiate();
			break;
		case 4:
			randomArpeggiate();
			break;
		case 5:
			randomNoRepeatArpeggiate();
			break;
		default:
			linearTriplesArpeggiate();
	}
}

void Rpeggi8r::linearArpeggiate() {
	// Move to next tone
	previousTone = currentTone;
	currentTone++;

	// Reset back to first tone after moving past the last tone
	if (currentTone == NUM_PARAMS)
		currentTone = A_PARAM;
}

void Rpeggi8r::linearBidirectionalArpeggiate() {
	// Determine next tone in order
	previousTone = currentTone;
	if (direction > 0) {
		if (currentTone < NUM_PARAMS-1) {
			// Linearly increase
			currentTone += direction;
		}
		else {
			// Flip direction and linearly decrease
			direction = -direction;
			currentTone += direction;
		}
	}
	else if (direction < 0){
		if (currentTone > A_PARAM) {
			// Linearly decrease
			currentTone += direction;
		}
		else {
			// Flip direction and linearly increase
			direction = -direction;
			currentTone += direction;
		}
	}

}

void Rpeggi8r::linearTriplesArpeggiate() {
	if (previousTone == A_PARAM) {
		// Move from middle triple tone to end
		previousTone = currentTone;
		previousTripleTone = currentTone;
		currentTone = NUM_PARAMS-1;
	}
	else if (previousTone == (NUM_PARAMS-1)) {
		// Move from starting triple tone to middle
		previousTone = currentTone;
		currentTone = previousTripleTone + 1;

		// Reset back to first triple tone after moving past the last triple
		if (currentTone == NUM_PARAMS-1)
			currentTone = A_PARAM + 1;
	}
	else {
		// Move from ending triple tone to start
		previousTone = currentTone;
		currentTone = A_PARAM;
	}
}

void Rpeggi8r::randomArpeggiate() {
	// Move to a random tone
	previousTone = currentTone;
	currentTone = toneDistribution(random);
}

void Rpeggi8r::randomNoRepeatArpeggiate() {
	// Move to a random tone other than the current one
	previousTone = currentTone;
	while (previousTone == currentTone) {
		currentTone = toneDistribution(random);
	}
}

struct Rpeggi8rWidget : ModuleWidget {
	Rpeggi8rWidget(Rpeggi8r *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/rpeggi8r.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<RoundLargeBlackKnob>(Vec(15, 65), module, Rpeggi8r::FREQ_PARAM, 1.0f, 16.0f, 6.0f));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(55, 78), module, Rpeggi8r::ARP_PARAM, 1.0f, 5.99f, 3.5f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(15, 145), module, Rpeggi8r::A_PARAM, 432.0f, 1728.0f, 432.0f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(50, 145), module, Rpeggi8r::B_PARAM, 432.0f, 1728.0f, 544.29f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(15, 180), module, Rpeggi8r::C_PARAM, 432.0f, 1728.0f, 647.27f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(50, 180), module, Rpeggi8r::D_PARAM, 432.0f, 1728.0f, 864.0f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(15, 215), module, Rpeggi8r::E_PARAM, 432.0f, 1728.0f, 1088.57f));
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(50, 215), module, Rpeggi8r::F_PARAM, 432.0f, 1728.0f, 1294.54f));

		addOutput(Port::create<PJ301MPort>(Vec(32, 275), Port::OUTPUT, module, Rpeggi8r::ARP_OUTPUT));
	}
};


Model *modelRpeggi8r = Model::create<Rpeggi8r, Rpeggi8rWidget>("toot-toot-modules", "Rpeggi8r", "Hex input arpeggiator", OSCILLATOR_TAG);
