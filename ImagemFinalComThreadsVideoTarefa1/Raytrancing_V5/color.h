#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

class image {
	int width;
	int height;
	unsigned char* img;
	int line_size;
	int cel_size;
	int line_count;
public:
	image(int w, int h) :
		width(w), height(h) {
		img = new unsigned char[width * height * 3 * sizeof(int)];
		cel_size = sizeof(unsigned char) * 3;
		line_size = cel_size * width;
		line_count = 0;
	}
	~image() {
		delete[] img;
	}
	void set(int x, int y, color pixel_color, int samples_per_pixel) {

		auto r = pixel_color.x();
		auto g = pixel_color.y();
		auto b = pixel_color.z();
		// Divide the color by the number of samples and gamma-correct for gamma=2.0.
		auto scale = 1.0 / samples_per_pixel;
		r = sqrt(scale * r);
		g = sqrt(scale * g);
		b = sqrt(scale * b);

		unsigned char* pos = &img[y * line_size + x * cel_size];

		pos[0] = (unsigned char)(255 * clamp(r, 0.0, 0.999));
		pos[1] = (unsigned char)(255 * clamp(g, 0.0, 0.999));
		pos[2] = (unsigned char)(255 * clamp(b, 0.0, 0.999));
	}

	void write_colors(std::ostream& out) {
		unsigned char* pos;

		out << setfill('0');
		out << "P3\n" << setw(5) << width << " " << setw(5) << height << "\n255\n";

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				pos = &img[y * line_size + x * cel_size];
				out << setw(3) << int{ pos[0] } << " " << setw(3) << int{ pos[1] } << " " << setw(3) << int{ pos[2] } << " ";
			}
			out << "\n";
		}
	}

	void writeBitmapFile(ofstream& out) {
		char padding[3] = { 0,0,0 };
		char color[3] = { 0, 0, 0 };
		int paddingSize = (4 - (width * bytesPerPixel) % 4) % 4;

		char* fileHeader = createBitMapFileHeader(height, width, paddingSize);
		char* infoHeader = createBitMapInfoHeader(height, width);

		out.write((char*)fileHeader, fileHeaderSize);
		out.write((char*)infoHeader, infoHeaderSize);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int p = (height - y) * line_size + x * cel_size;
				color[2] = img[p];
				color[1] = img[p + 1];
				color[0] = img[p + 2];
				out.write(color, sizeof(color));
			}
			out.write(padding, paddingSize);
		}
	}

private:
	const int bytesPerPixel = 3; //red, green, blue
	const int fileHeaderSize = 14;
	const int infoHeaderSize = 40;

	char* createBitMapFileHeader(int height, int width, int paddingSize) {
		int fileSize = fileHeaderSize + infoHeaderSize + (bytesPerPixel * width + paddingSize) * height;
		static unsigned char fileHeader[] = { 0,0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
		};
		fileHeader[0] = (unsigned char)('B');
		fileHeader[1] = (unsigned char)('M');
		fileHeader[2] = (unsigned char)(fileSize);
		fileHeader[3] = (unsigned char)(fileSize >> 8);
		fileHeader[4] = (unsigned char)(fileSize >> 16);
		fileHeader[5] = (unsigned char)(fileSize >> 24);
		fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);
		return (char*)fileHeader;
	}

	char* createBitMapInfoHeader(int height, int width) {
		static unsigned char infoHeader[] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0,
			0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
		};

		infoHeader[0] = (unsigned char)(infoHeaderSize);
		infoHeader[4] = (unsigned char)(width);
		infoHeader[5] = (unsigned char)(width >> 8);
		infoHeader[6] = (unsigned char)(width >> 16);
		infoHeader[7] = (unsigned char)(width >> 24);
		infoHeader[8] = (unsigned char)(height);
		infoHeader[9] = (unsigned char)(height >> 8);
		infoHeader[10] = (unsigned char)(height >> 16);
		infoHeader[11] = (unsigned char)(height >> 24);
		infoHeader[12] = (unsigned char)(1);
		infoHeader[14] = (unsigned char)(bytesPerPixel * 8);

		return (char*)infoHeader;
	}
};

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();
	// Divide the color by the number of samples and gamma-correct for gamma=2.0.
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);
	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
#endif
