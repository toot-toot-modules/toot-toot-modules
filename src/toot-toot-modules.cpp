#include "toot-toot-modules.hpp"


Plugin *plugin;


void init(Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);

	// Add all Models defined throughout the plugin
	p->addModel(modelMyModule);
	p->addModel(modelNGHarmonizer);
	p->addModel(modelABClock);
	p->addModel(modelMarkMod);
    p->addModel(modelHive);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
