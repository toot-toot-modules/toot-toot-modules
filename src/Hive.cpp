#include "toot-toot-modules.hpp"
#include "dsp/resampler.hpp"
#include "dsp/filter.hpp"


struct Oscillator {
    float phase = 0.0f;
    float freq;
    float pw = 0.5f;
    float pitch;
    float v_oct = 0.0f;

    void step_update(float deltaTime, float pitch_param, float pitch_input,
                float fm_param, float fm_input, float pwm){
        set_pitch(pitch_param, pitch_input, fm_param, fm_input);
        set_freq();
        set_phase(deltaTime);
        set_pw(pwm);
    }

    void set_freq () {
        freq = 261.626f * powf(2.0f, pitch);
    }

    void set_phase(float t) {
        phase += freq * t;
	    if (phase >= 1.0f)
		    phase -= 1.0f;
    }

    void set_pitch(float p, float v_oct, float fm_scalar, float fm) {
        pitch = p + v_oct;
        pitch += quadraticBipolar(fm_scalar) * 12.0f * fm;
        pitch = clamp(pitch, -4.0f, 4.0f);
    }

    void set_pw(float pwm) {
        pw = clamp(pwm, 0.01f, 1.0f-0.01f);
    }

    // Compute the sine output
    float sine() {
	    return sinf(2.0f * M_PI * phase);
    }

    // all from wolfram alpha
    // Compute the triangle output
    float tri() {
        return 2 / M_PI * asinf(sinf(M_PI * phase * 2.0f));
    } 


    // Compute the sawtooth output
    float saw() {
        return -2 / M_PI * atanf(1 / tanf(phase * M_PI));
    }

    // Compute the square wave output
    float square() {
        return (phase < pw) ? 1.0f : -1.0f;
    }

    // all wave
    float all_wave(float wave) {
        wave = clamp(wave, 0.0f, 3.0f);
	    if (wave < 1.0f)
		    return crossfade(sine(), tri(), wave);
	    else if (wave < 2.0f)
		    return crossfade(tri(), saw(), wave - 1.0f);
	    else
		    return crossfade(saw(), square(), wave - 2.0f);
    }

    // square/saw
    float sqr_saw(float wave) {
        wave = clamp(wave, 0.0f, 1.0f);
        return crossfade(saw(), square(), wave);
    }

    // saw/sin/triangle
    float saw_tri_sin(float wave) {
        wave = clamp(wave, 0.0f, 2.0f);
        if (wave < 1.0f)
            return crossfade(sine(), saw(), wave);
        else
            return crossfade(saw(), tri(), wave - 1.0f);
    }

};



struct Hive : Module {
	enum ParamIds {
        QPITCH_PARAM,
        DPITCH_PARAM,
        QFM_PARAM,
        DFM_PARAM,
		QWAVE_PARAM,
        DWAVE_PARAM,
        QPWM_PARAM,
        DPWM_PARAM,
        QSELF_PARAM,
        FM_PARAM,
        PW_PARAM,
        WAVE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
        FM_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        OUT_OUTPUT,
        ALL_OUTPUT,
        SQRSAW_OUTPUT,
        SST_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;

	Hive() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

    Oscillator queen; // modulator
    Oscillator drone; // carrier
};



void Hive::step() {
	// Implement a simple sine oscillator
	float deltaTime = engineGetSampleTime();

    queen.step_update(deltaTime, params[QPITCH_PARAM].value, inputs[PITCH_INPUT].value,
                    params[QFM_PARAM].value, 0.0f, params[QPWM_PARAM].value);
    queen.step_update(deltaTime, params[QPITCH_PARAM].value, inputs[PITCH_INPUT].value,
                    params[QFM_PARAM].value, (queen.saw_tri_sin(params[QSELF_PARAM].value * 5.0f)), params[QPWM_PARAM].value);
    float fm = queen.all_wave(params[QWAVE_PARAM].value) * 5.0f;
    drone.step_update(deltaTime, params[DPITCH_PARAM].value, inputs[PITCH_INPUT].value,
                    params[DFM_PARAM].value, fm, params[DPWM_PARAM].value);

    outputs[OUT_OUTPUT].value = drone.sqr_saw(params[DWAVE_PARAM].value) * 5.0f;

	// Blink light at 1Hz
	blinkPhase += deltaTime;
	if (blinkPhase >= 1.0f)
		blinkPhase -= 1.0f;
	lights[BLINK_LIGHT].value = (blinkPhase < 0.5f) ? 1.0f : 0.0f;
}


struct HiveWidget : ModuleWidget {
	HiveWidget(Hive *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Hive.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(40, 50), module, Hive::QPITCH_PARAM, -3.0, 3.0, 0.0));
        addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(320, 265), module, Hive::DPITCH_PARAM, -3.0, 3.0, 0.0));


        addParam(ParamWidget::create<Rogan1PSGreen>(Vec(125, 380-230), module, Hive::QFM_PARAM, 0.0, 1.0, 0.0));
        addParam(ParamWidget::create<Rogan1PSGreen>(Vec(275, 380-155), module, Hive::DFM_PARAM, 0.0, 1.0, 0.0));

        addParam(ParamWidget::create<Rogan1PSRed>(Vec(195, 380-272), module, Hive::QWAVE_PARAM, 0.0, 3.0, 0.0));
        addParam(ParamWidget::create<Rogan1PSRed>(Vec(185, 380-130), module, Hive::DWAVE_PARAM, 0.0, 1.0, 0.0));

        addParam(ParamWidget::create<Rogan1PSBlue>(Vec(275, 380-267), module, Hive::QPWM_PARAM, 0.0, 1.0, 0.5));
        addParam(ParamWidget::create<Rogan1PSBlue>(Vec(100, 380-140), module, Hive::DPWM_PARAM, 0.0, 1.0, 0.5));

        addParam(ParamWidget::create<Rogan1PSWhite>(Vec(323, 380-340), module, Hive::QSELF_PARAM, 0.0, 1.0, 0.0));


        addInput(Port::create<PJ301MPort>(Vec(50, 120), Port::INPUT, module, Hive::PITCH_INPUT));
        addOutput(Port::create<PJ301MPort>(Vec(50, 150), Port::OUTPUT, module, Hive::OUT_OUTPUT));
	}
};

Model *modelHive = Model::create<Hive, HiveWidget>("toot-toot-modules", "Hive", "Hive", OSCILLATOR_TAG);
