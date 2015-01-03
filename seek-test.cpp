#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>

#include "seek.hpp"

using namespace std;
using namespace LibSeek;

inline void sleep(float secs) {
	chrono::milliseconds dura(int(1000*secs));
	this_thread::sleep_for(dura);
}

int main() {
	setbuf(stdout, NULL);
	Imager iface;
	iface.init();

	Frame frame;
	
	iface.frame_init(frame);

	while (true) {
		iface.frame_acquire(frame);

		{
			int h = frame.height();
			int w = frame.width();
			vector<uint16_t> img(w*h);
			int _max = 0;
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					uint16_t v = frame.data()[y*w+x];
					if (v > _max) _max = v;
				}
			}
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					uint16_t v = frame.data()[y*w+x];
					v = int(v) * 0xffff / _max;
					img[y*w+x] = v;
				}
			}
			fwrite((uint8_t*)img.data(), sizeof(uint16_t), w*h, stdout);
		}
	}

	for (int i = 0; i < 30; i++) {
		iface.frame_acquire(frame);

		{
			char filename[30];
			sprintf(filename, "frame-%03d.pgm", i);
			FILE * f = fopen(filename, "wb");
			int w = frame.width();
			int h = frame.height();
			int res = fprintf(f, "P5 %d %d 65535\n", w, h);
			int _max = 0;
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					uint16_t v = frame.data()[y*w+x];
					if (v > _max) _max = v;
				}
			}
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					uint16_t v = frame.data()[y*w+x];
					v = int(v) * 0xffff / _max;
					v = htobe16(v);
					res = fwrite((uint8_t*)&v, sizeof(uint16_t), 1, f);
				}
			}
			fclose(f);
		}
	}

	iface.frame_exit(frame);

	iface.exit();
}
