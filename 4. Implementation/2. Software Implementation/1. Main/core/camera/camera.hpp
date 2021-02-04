#ifndef _CAMERA_HPP_H
#define _CAMERA_HPP_H

#include <string>
#include <vector>

#include <linux/videodev2.h>

#define BUFFER_COUNT	4
#define FETCH_TIMEOUT	50

class Camera {

public:

	enum Encoding {
		MJPEG = V4L2_PIX_FMT_MJPEG,
		JPEG = V4L2_PIX_FMT_JPEG,
		MPEG = V4L2_PIX_FMT_MPEG,
		MPEG1 = V4L2_PIX_FMT_MPEG1,	
		MPEG2 = V4L2_PIX_FMT_MPEG2,
		MPEG4 = V4L2_PIX_FMT_MPEG4,
		YUV420 = V4L2_PIX_FMT_YUV420,
		YUV422P = V4L2_PIX_FMT_YUV422P,
	};

	struct Format {

		uint32_t width;
		uint32_t height;
		enum Encoding encoding;

		Format(uint32_t width = 640, uint32_t height = 480, 
				enum Encoding encoding = Encoding::MJPEG) {
			this->width = width;
			this->height = height;
			this->encoding = encoding;
		}

		Format(const Format& fmt) {
			this->encoding = fmt.encoding;
			this->height = fmt.height;
			this->width = fmt.width;
		}
	};

	struct Point2D {

		uint32_t x;
		uint32_t y;

		Point2D(uint32_t x, uint32_t y) {
			this->x = x;
			this->y = y;
		}
	};

	struct Color {

		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color(uint32_t a, uint32_t r = 0, uint32_t g = 0, uint32_t b = 0) {
			this->a = a;
			this->r = r;
			this->g = g;
			this->b = b;
		}
	};

	enum Error {
		OK=0,
		NO_DEVICE=-1,
		WRONG_DEVICE=-2,
		OP_NOT_PERM=-3,
		NOT_OPEN=-4,
		DEV_NOT_CAP=-5,
		INCOMP_ENCODING=-6,
		NO_MEM=-7,
		RESOURCE_UNAVAIL=-8,
		BAD_UNMAP=-9,
		CAPTURE_TIMEOUT=-10,
		NO_BUF_OUT=-11,
		DEV_NOT_OPEN=-12,
		INTERNAL=-13,
		UNDEF=-14,
	};

	struct Device {
		
		std::string name;
		int32_t descriptor;

		Device(std::string name = "", int32_t descriptor = -1) : \
				name(name), descriptor(descriptor) {

		}
	};

private:

	struct Buffer {
		void *start;
		size_t length;
	};

private:
	
	Buffer buffers[BUFFER_COUNT];

	Device dev;
	Format format;

	Error initialize();
	Error uninitialize();
	Error initializeMemoryMap();

	Error readFrame(v4l2_buffer& buf, std::string filename);

	bool streaming;

public:

	Camera(const std::string& device = std::string(""), const Format& format = Format());
	~Camera();

	Error openDevice(const std::string& device, const Format& format, bool silent = false);
	bool isOpen();
	void closeDevice();
	
	Error start();
	bool isStreaming();
	Error stop();

	Error capture(std::string& filename);

	Error drawRectangle(std::string input_file, std::string output_file, 
			const Point2D& start, const Point2D& end, const Color& fill, 
			const Color& stroke);
	
	Error drawText(std::string input_file, std::string output_file, 
			std::string text, const Point2D& start, uint32_t thickness, 
			uint32_t size, const Color& stroke);
};

#endif