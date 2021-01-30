#pragma once

#include <string>
#include <vector>

#include <linux/videodev2.h>

#define BUFFER_COUNT	4

class Camera {

public:

	enum Encoding {
		MJPEG = V4L2_PIX_FMT_MJPEG,
		JPEG = V4L2_PIX_FMT_JPEG,
		MPEG = V4L2_PIX_FMT_MPEG
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

	private:
		uint32_t x;
		uint32_t y;

	public:
		Point2D(uint32_t x, uint32_t y) {
			this->x = x;
			this->y = y;
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
		BAD_UNMAP=-9
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

	bool capturing;

public:

	Camera(const std::string& device = std::string(""), const Format& format = Format());
	~Camera();

	Error openDevice(const std::string& device, const Format& format);
	bool isOpen();
	void closeDevice();
	
	Error start();
	bool isCapturing();
	Error stop();

	Error drawRectangle(const Point2D& start, uint32_t width, uint32_t height);
};