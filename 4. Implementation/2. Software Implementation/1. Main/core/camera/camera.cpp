#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "camera.hpp"
#include "log.hpp"

#define BUF_TYPE V4L2_BUF_TYPE_VIDEO_CAPTURE
#define MEM_TYPE V4L2_MEMORY_MMAP

using namespace std;

#define LOG_DETAILS()	log::details << errno << " - " << strerror(errno) << std::endl
#define CLEAR(x) memset(&(x), 0, sizeof(x))


static int xioctl(int fh, int request, void *arg)
{
    int r;

	do {
		r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

Camera::Camera(uint32_t dev_index, const Camera::Format& format) {

	streaming = false;
	openDevice(dev_index, format, true);

}

Camera::~Camera() {

	if (isOpen())
		closeDevice();
}

Camera::Error Camera::openDevice(uint32_t dev_index, const Camera::Format& format, bool silent) {

	struct stat st;
	v4l2_fmtdesc desc;
	int descriptor;
	std::string device = string("/dev/video") + std::to_string(dev_index);

	if (isOpen())
		closeDevice();

	/* Verify the existence of the device */
	if (-1 == stat(device.c_str(), &st)) {
		
		if (!silent) {
			log::error << "Cannot identify device '" << device << "'" << std::endl;
			LOG_DETAILS();
		}

		return NO_DEVICE;
	}

	log::ok << "Device '" << device << "' found" << std::endl;

	/* Affer the device type */
	if (!S_ISCHR(st.st_mode)) {
		log::error << "Device '" << device << "' is of the wrong type" << std::endl;
		LOG_DETAILS();
		
		return WRONG_DEVICE;
	}

	// log::ok << "Device parameters are as expected" << std::endl;

	/* Attempt to open de device */
	descriptor = open(device.c_str(), O_RDWR /*| O_NONBLOCK*/, 0);

	if (-1 == descriptor) {
		log::error << "Cannot open device ' " << device << "'" << std::endl;
		LOG_DETAILS();		
	}

	// log::ok << "Device opened successfully" << std::endl;

	/* Keep relevant device data */
	this->dev.name = device;
	this->dev.index = dev_index;
	this->dev.descriptor = descriptor;
	this->format = format;

	if (this->format.encoding == AUTO) {
		memset(&desc,0,sizeof(desc));
		desc.index = dev.index;
		desc.type = BUF_TYPE;
		if (xioctl(descriptor, VIDIOC_ENUM_FMT, &desc) == -1) {
			if (errno == EINVAL)
				int a = 3; 
			log::error << "Could not infer encoding automatically. Defaulting to MJPEG" << std::endl;
			this->format.encoding = MJPEG;
			LOG_DETAILS();	
		}
		this->format.encoding = (Encoding)(desc.pixelformat);
	}

	log::ok << "Device is open" << std::endl;

	initialize();

	return OK;
}

bool Camera::isOpen()  {
	return dev.descriptor != -1;
}

void Camera::closeDevice()  {
	
	if (isOpen()) {
		close(this->dev.descriptor);
		this->dev.name = "";
		this->dev.descriptor = -1;
		uninitialize();
	}

	log::ok << "Device closed" << std::endl;
}

Camera::Error Camera::initialize() {
	
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	/* Check if a device is open and ready to be initialized */
	if (!isOpen()) {
		log::error << "Device needs to be opened before initializing" << std::endl;
		return NO_DEVICE;
	}

	/* Query device capabilities */
	if (-1 == xioctl(this->dev.descriptor, VIDIOC_QUERYCAP, &cap)) {
		log::error << "Device capability query failed";

		if (EINVAL == errno)
			log::generic << " (it is not a V4L2 device)" << std::endl;

		LOG_DETAILS();

		return DEV_NOT_CAP;
	}

	/* Check if the selected device is one of videio capture */
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		log::error << "Not a video capture device" << std::endl;
		LOG_DETAILS();
		return DEV_NOT_CAP;
	}

	log::ok << "Device is capable of video capture" << std::endl;

	/* Select video capture buffer type and check cropping capabilities */
	CLEAR(cropcap);
	cropcap.type = BUF_TYPE;

	if (0 == xioctl(dev.descriptor, VIDIOC_CROPCAP, &cropcap)) {
		
		crop.type = BUF_TYPE;
		crop.c = cropcap.defrect;

		if (-1 == xioctl(dev.descriptor, VIDIOC_S_CROP, &crop)) {
			
			log::warning << "Device not capable of video crop" << std::endl;
			LOG_DETAILS();
		}

	}

	/* Select video format */
	CLEAR(fmt);
	fmt.type = BUF_TYPE;

	fmt.fmt.pix.width = this->format.width;
	fmt.fmt.pix.height = this->format.height;
	fmt.fmt.pix.pixelformat = this->format.encoding;
	fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	

	if (-1 == xioctl(dev.descriptor, VIDIOC_S_FMT, &fmt)) {
		
		if (errno == EINVAL)
			log::error << "Unsupported encoding" << std::endl;
		else // errno = EBUSY
			log::error << "Device unavailable" << std::endl;

		LOG_DETAILS();
		return DEV_NOT_CAP;
	}

	//log::ok << "Video capturing capabilities assured" << std::endl;

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
			fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
			fmt.fmt.pix.sizeimage = min;
	
	/* Reserve memory space */
	initializeMemoryMap();

	log::ok << "Device is initialized" << std::endl;

	return OK;
}

Camera::Error Camera::uninitialize() {

	for (uint32_t i = 0; i < BUFFER_COUNT; i++) {
		if (-1 == munmap(buffers[i].start, buffers[i].length)) {
			log::error << "Failed to unmap buffer " << i << \
					"; possibly not mapped" << std::endl;
			LOG_DETAILS();
			return BAD_UNMAP;
		}
	}

	log::ok << "Device uninitialized" << std::endl;

	return OK;
}

Camera::Error Camera::initializeMemoryMap() {

	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;

	/* Attempt to initiate memory mapped I/O */
	CLEAR(req);
	req.count = BUFFER_COUNT;
	req.type = BUF_TYPE;
	req.memory = MEM_TYPE;

	/* Check for memory mapping support */
	if (-1 == xioctl(dev.descriptor, VIDIOC_REQBUFS, &req)) {
		
		if (errno == EINVAL) {
			
			log::error << "Memory mapping could not be initiated: " \
					"no such capability on this device" << std::endl;
			LOG_DETAILS();
			return DEV_NOT_CAP;

		} else {
			log::error << "Memory mapping could not be initiated" << std::endl;
			LOG_DETAILS();
			return DEV_NOT_CAP;
		}
	}

	/* Create mappings for each buffer in memory */
	for (int n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		
		/* Query the status of the buffer */
		CLEAR(buf);
		buf.type = BUF_TYPE;
		buf.memory = MEM_TYPE;
		buf.index = n_buffers;

		if (-1 == xioctl(dev.descriptor, VIDIOC_QUERYBUF, &buf)) {
			log::error << "Buffer unavailable" << std::endl;
			LOG_DETAILS();
			return RESOURCE_UNAVAIL;
		}

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					dev.descriptor, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start) {
			log::error << "Failed to map buffer" << n_buffers << \
					" to a space in memory" << std::endl;
			LOG_DETAILS();
			return RESOURCE_UNAVAIL;
		}
	}

	log::ok << "Created and mapped " << BUFFER_COUNT << " buffers" << std::endl;

	return OK;
}

Camera::Error Camera::start() {
	
	unsigned int i;
	enum v4l2_buf_type type;
	struct v4l2_buffer buf;

	/* Enqueue buffers for capture */
	for (i = 0; i < BUFFER_COUNT; i++) {

		CLEAR(buf);
		buf.type = BUF_TYPE;
		buf.memory = MEM_TYPE;
		buf.index = i;

		if (-1 == xioctl(dev.descriptor, VIDIOC_QBUF, &buf)) {
			log::error << "Failed to enqueue buffer " << i << \
					" for capture" << std::endl;
			LOG_DETAILS();
			return UNDEF;
		}	
	}

	/* Attempt to start streaming */
	type = BUF_TYPE;
	if (-1 == xioctl(dev.descriptor, VIDIOC_STREAMON, &type)) {
		log::error << "Could not start stream" << std::endl;
		LOG_DETAILS();
		
		/* At this point, none of the well-defined errors may apply */
		return UNDEF;
	}

	log::ok << "Started streaming" << std::endl;

	streaming = true;
}

bool Camera::isStreaming() {

	return streaming;
}

Camera::Error Camera::stop() {

	struct v4l2_buffer buf;

	/* Signal if no stream was running */
	if (!streaming)
		log::info << "No capture is currently active" << std::endl;

	/* Stop video stream */		
	CLEAR(buf);
	buf.type = BUF_TYPE;
 	xioctl(dev.descriptor, VIDIOC_STREAMOFF, &buf);
	
	streaming = false;
	
	log::ok << "Stopped streaming" << std::endl;

	return OK;	
}

Camera::Error Camera::capture(std::string& filename) {

	fd_set fds;
	struct timeval tv;
	int32_t result;
	struct v4l2_buffer buf;
	//ofstream out_file;
	//cFILE* out_file;

	if(!isOpen()) {
		log::error << "Device must be opened before capturing" << std::endl;
		return DEV_NOT_OPEN;
	}

	FD_ZERO(&fds);
	FD_SET(dev.descriptor, &fds);

	/* Wait for the descriptor to be ready for reading until timeout */
	tv.tv_sec = 0;
	tv.tv_usec = FETCH_TIMEOUT*1000;
	result = select(dev.descriptor + 1, &fds, NULL, NULL, &tv);

	/* Error */
	if (result == -1) {
		if (errno == EINTR) {
			log::error << "Failed to capture frame" << std::endl;
			log::details << "Descriptor unavailable" << std::endl;
			return RESOURCE_UNAVAIL;
		}
	
	/* Timeout */
	} else if (result == 0) {
		log::error << "Timed out while trying to capture frame" << std::endl;
		return CAPTURE_TIMEOUT;
	}

	/* Read the frame and return if failed */
	result = (int32_t)readFrame(buf, filename);

	if (result != OK)
		return static_cast<Camera::Error>(result);

	return OK;
}

Camera::Error Camera::readFrame (v4l2_buffer& buf, std::string filename) {

	ofstream out_file;

	Error return_val = OK;

	/* Read frame */
	CLEAR(buf);
	buf.type = BUF_TYPE;
	buf.memory = MEM_TYPE;

	/* Attempt to dequeue buffer */
	if (-1 == xioctl(dev.descriptor, VIDIOC_DQBUF, &buf)) {

		log::error << "Failed to capture frame" << std::endl;

		switch (errno) {
		case EAGAIN:
			log::details << "No buffer in the outgoing queue" << std::endl;
			return NO_BUF_OUT;

		case EIO:
			log::details << "Internal error" << std::endl;
			/* TODO: Implement buffer recovery procedure (see spec)*/
			return INTERNAL;

		default:
			log::details << "Undefined error" << std::endl;
			return UNDEF;
		}
	}

	log::ok << "Read frame" << std::endl;

	out_file.open(filename, ios::out | ios::trunc | ios::binary);
	
	if (!out_file.is_open()) {
		log::error << "Could not open file as requested " << std::endl;
		return_val = RESOURCE_UNAVAIL;
		goto cleanup;
	}
	
	log::ok << "Opened " << filename << " for writing" << std::endl;

    out_file.write(static_cast<char*>(buffers[buf.index].start), buf.bytesused);
    
	if (!out_file.good()) {
		out_file.close();
		log::error << "An error occured while writing to file " << std::endl;
		goto cleanup;
	}

	log::ok << "Saved " << filename << std::endl;

	cleanup:
	
	if (-1 == xioctl(dev.descriptor, VIDIOC_QBUF, &buf)) {
		log::error << "Failed to enqueue buffer for capture" << std::endl;
		LOG_DETAILS();
		return UNDEF;
	}

	return return_val;
}

Camera::Error Camera::drawRectangle (const Point2D& start, uint32_t width, uint32_t height) {

}

