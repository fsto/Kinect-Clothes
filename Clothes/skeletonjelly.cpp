#include "skeletonjelly.hpp"
#include "GestureTracker.h"

static const char FORMAT_BPP[2] = {4, 3};

#define CHECK_RC(x) {\
	if (x != XN_STATUS_OK)\
		return x;\
}

void XN_CALLBACK_TYPE cb_newUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	((Kinect *)pCookie)->onNewUser(nId);
}

void XN_CALLBACK_TYPE cb_lostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	((Kinect *)pCookie)->onLostUser(nId);
}

void XN_CALLBACK_TYPE cb_poseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	((Kinect *)pCookie)->onPoseDetected(strPose, nId);
}

void XN_CALLBACK_TYPE cb_calibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	((Kinect *)pCookie)->onCalibrationStart(nId);
}

void XN_CALLBACK_TYPE cb_calibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	((Kinect *)pCookie)->onCalibrationEnd(nId, bSuccess);
}

Kinect::Kinect()
	: trackerCallback(0)
{
	_needPose = false;
	_error = XN_STATUS_OK;
	_init = false;
	_paused = false;
	_autoTrack = true;

	_gotImage = false;
	_renderFormat = RENDER_RGBA;

	_tickTime = 1000 / 30;

	_eventCallback = 0;
	_callbackData = 0;

	_elapsed = 0;

	for (unsigned int i = 0; i < MAX_USERS; ++i)
		_userData[i] = 0;
}

Kinect::~Kinect()
{
	for (unsigned int i = 0; i < MAX_USERS; ++i)
		delete _userData[i];

	_context.Shutdown();
}

XnStatus Kinect::init(SensorMode depthMode, SensorMode imageMode)
{
	XnMapOutputMode sensorModes[] =
	{
		{ 0, 0, 0 }, /* SENSOR_DISABLED */
		{ 320, 240, 60 }, /* SENSOR_QVGA */
		{ 640, 480, 30 }, /* SENSOR_VGA */
		{ 1280, 1024, 10 } /* SENSOR_SXGA */
	};

	xn::Query qDepth, qImage, qGesture, qHands;
	XnCallbackHandle cb_user, cb_calibration, cb_pose;
	
	if (depthMode == SENSOR_DISABLED)
		return XN_STATUS_NOT_IMPLEMENTED;

	qDepth.AddSupportedCapability(XN_CAPABILITY_MIRROR);
	qDepth.AddSupportedMapOutputMode(sensorModes[depthMode]);

	qImage.AddSupportedCapability(XN_CAPABILITY_MIRROR);
	qImage.AddSupportedMapOutputMode(sensorModes[imageMode]);

	_error = _context.Init();
	CHECK_RC(_error);

	if (imageMode != SENSOR_DISABLED) /* init image generator */
	{
    	_error = _image.Create(_context, &qImage);
    	CHECK_RC(_error);
    
    	_error = _image.SetMapOutputMode(sensorModes[imageMode]);
    	CHECK_RC(_error);
    
    	_error = _image.SetPixelFormat(XN_PIXEL_FORMAT_RGB24);
    	CHECK_RC(_error);

		_image.GetMirrorCap().SetMirror(true);
		_gotImage = true;
	}

	/* init depth generator */
	{
    	_error = _depth.Create(_context, &qDepth);
    	CHECK_RC(_error);
    
    	_error = _depth.SetMapOutputMode(sensorModes[depthMode]);
    	CHECK_RC(_error);

		_depth.GetMirrorCap().SetMirror(true);

		_depth.GetAlternativeViewPointCap().SetViewPoint(_image);
	}

	/* init user generator */
	{
    	_error = _userGen.Create(_context);
    	CHECK_RC(_error);
    
    	if (!_userGen.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    		return XN_STATUS_ERROR;
    
    	_userGen.RegisterUserCallbacks(cb_newUser, cb_lostUser, this, cb_user);
    	_userGen.GetSkeletonCap().RegisterCalibrationCallbacks(cb_calibrationStart, cb_calibrationEnd, this, cb_calibration);
    
    	if (_userGen.GetSkeletonCap().NeedPoseForCalibration())
    	{
    		_needPose = true;
    
    		if (!_userGen.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    			return XN_STATUS_ERROR;
    		_userGen.GetPoseDetectionCap().RegisterToPoseCallbacks(cb_poseDetected, NULL, this, cb_pose);
    		_userGen.GetSkeletonCap().GetCalibrationPose(_calibrationPose);
    	}  
		_userGen.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	}

	_error = _context.StartGeneratingAll();
	CHECK_RC(_error);

	_init = true;
	return XN_STATUS_OK;
}


void Kinect::tick(int elapsed)
{
	_elapsed += elapsed;
	_context.WaitAndUpdateAll();
	//_userGen.WaitAndUpdateData();
	//_depth.WaitAndUpdateData();

	if (_elapsed >= _tickTime)
	{
		for (unsigned int i = 1; i < MAX_USERS; ++i)
			if (userActive(i))
				updateUserData(i, _userData[i]);

		_elapsed -= _tickTime;
	}
}

XnStatus Kinect::resetUser(XnUserID id /*= DEFAULT_USER*/)
{
	if (userActive(id))
	{
    	_userGen.GetPoseDetectionCap().StopPoseDetection(id);
    	_userGen.GetSkeletonCap().Reset(id);
		_userData[id]->status = USER_ACTIVE;
		return XN_STATUS_OK;
	}

	return XN_STATUS_NO_MATCH;
}

XnStatus Kinect::trackUser(XnUserID id /*= DEFAULT_USER*/)
{
	if (userActive(id))
	{
    	if (_userData[id]->status != USER_ACTIVE)
    		resetUser(id);
    
    	if (_needPose)
    	{
    		_userGen.GetPoseDetectionCap().StartPoseDetection(_calibrationPose, id);
    		_userData[id]->status |= USER_LOOKING_FOR_POSE;
    	}
    	else
    	{
    		_userGen.GetSkeletonCap().RequestCalibration(id, true);
    	}

		return XN_STATUS_OK;
	}
	return XN_STATUS_NO_MATCH;
}

void Kinect::onNewUser(XnUserID id)
{
#ifdef _DEBUG
	printf("Got user %d\n", id);
#endif
	if (id < MAX_USERS)
	{
		if (_userData[id] == 0)
    		_userData[id] = new KinectUser;

		_userData[id]->tracker = new Tracker(_userData[id], trackerCallback);

		_userData[id]->status = USER_ACTIVE;

		if (_autoTrack)
			trackUser(id);

		if (_eventCallback != 0)
			this->_eventCallback(this, CB_NEW_USER, id, _callbackData);
	}
}

void Kinect::onLostUser(XnUserID id)
{
	#ifdef _DEBUG
	printf("Lost user %d\n", id);
	#endif
	if (userActive(id))
	{
		_userData[id]->status = USER_INACTIVE;

		if (_eventCallback != 0)
			this->_eventCallback(this, CB_LOST_USER, id, _callbackData);
	}
}


void Kinect::onPoseDetected(const XnChar* strPose, XnUserID id)
{
	if (userActive(id))
	{
		_userData[id]->status &= ~USER_LOOKING_FOR_POSE;
		_userData[id]->status |= USER_GOT_POSE;

    	_userGen.GetPoseDetectionCap().StopPoseDetection(id);
    	_userGen.GetSkeletonCap().RequestCalibration(id, true);

		if (_eventCallback != 0)
			this->_eventCallback(this, CB_POSE_DETECTED, id, _callbackData);
	}
}

void Kinect::onCalibrationStart(XnUserID id)
{
	if (userActive(id))
	{
		_userData[id]->status |= USER_CALIBRATING;

		if (_eventCallback != 0)
			this->_eventCallback(this, CB_CALIBRATION_START, id, _callbackData);
	}
}

void Kinect::onCalibrationEnd(XnUserID id, XnBool success)
{
	if (userActive(id))
	{
    	if (success)
    	{
    		_userGen.GetSkeletonCap().StartTracking(id);
    		_userData[id]->status &= ~USER_CALIBRATING;
    		_userData[id]->status |= USER_TRACKING;

			_userData[id]->scale = 0;
			_userData[id]->tracker->reset();

    		if (_eventCallback != 0)
    			this->_eventCallback(this, CB_CALIBRATION_SUCCESS, id, _callbackData);
    	}
    	else
    	{
    		_userData[id]->status = USER_ACTIVE;

    		if (_eventCallback != 0)
    			this->_eventCallback(this, CB_CALIBRATION_FAIL, id, _callbackData);

			if (_autoTrack)
				trackUser(id);
    	}
	}
}

void Kinect::calculateHistogram(int resolutionX, int resolutionY, const XnDepthPixel *depth_pixels)
{
	int numPoints = 0;

	unsigned int *histogram = _histogram;

	memset((void *)histogram, 0x0, MAX_DEPTH * sizeof(unsigned int));

	for (int y = 0; y < resolutionY; y++)
	{
		for (int x = 0; x < resolutionX; x++)
		{
			unsigned int val = *depth_pixels++ & DEPTH_MASK;

			if (val != 0)
			{
				histogram[val]++;
				numPoints++;
			}
		}
	}

	for (int idx = 1; idx < MAX_DEPTH; idx++)
	{
		histogram[idx] += histogram[idx - 1];
	}

	if (numPoints > 0)
	{
		for (int idx = 1; idx < MAX_DEPTH; idx++)
		{
			int color = (unsigned char)(255 * (1.0f - ((float)histogram[idx] / (float)numPoints)));
			histogram[idx] = color;
		}
	}
}

void Kinect::renderImage(unsigned char *buffer, int pitch)
{
	xn::ImageMetaData imageMD;
	const XnUInt8 *image  = NULL;

	if (buffer == 0 || !_gotImage)
		return;

	_image.GetMetaData(imageMD);
	image = imageMD.Data();

	const int resX = imageMD.XRes();
	const int resY = imageMD.YRes();

	const int bpp = FORMAT_BPP[_renderFormat];

	pitch = pitch ? (pitch - resX) * bpp : 0;

	if (pitch == 0 && _renderFormat == RENDER_RGB)
	{
		memcpy(buffer, image, resX * resY * bpp);
		return;
	}

	unsigned char *dst = (unsigned char *)buffer;

	for (int y = 0; y < resX; ++y, dst += pitch)
		for (int x = 0; x < resY; ++x, dst += bpp, image += 3)
		{
			dst[0] = image[0];
			dst[1] = image[1];
			dst[2] = image[2];
			dst[3] = 0xFF;
		}
}

void Kinect::renderDepth(unsigned char *buffer, bool background, int pitch)
{
	static const int COLOR_COUNT = 7;
	static const unsigned char COLORS[COLOR_COUNT][3] = 
	{
		{0xFF, 0xFF, 0xFF},
		{0xFF, 0x00, 0x00},
		{0x00, 0xFF, 0x00},
		{0x00, 0x00, 0xFF},
		{0xFF, 0xFF, 0x00},
		{0x00, 0xFF, 0xFF},
		{0xFF, 0x00, 0xFF},
	};

	assert(_init);

	if (buffer == 0)
		return;

	unsigned int *histogram = _histogram;
	unsigned char *dst = (unsigned char *)buffer;

	xn::SceneMetaData sceneMD;
	_userGen.GetUserPixels(0, sceneMD);

	xn::DepthMetaData depthMD;
	_depth.GetMetaData(depthMD);

    const XnDepthPixel *depthPixels = depthMD.Data();
	const XnLabel *labelPixels = sceneMD.Data();

	const int resolutionX = depthMD.XRes();
	const int resolutionY = depthMD.YRes();

	const int bpp = FORMAT_BPP[_renderFormat];

	const unsigned char bg_masks[2] = {0x0, 0xFF};
	const unsigned char force_mask = background ? 0xFF : 0x0;

	pitch = pitch ? (pitch - resolutionX) * bpp : 0;

	calculateHistogram(resolutionX, resolutionY, depthPixels);

	for (int y = 0; y < resolutionY; y++)
	{
		for (int x = 0; x < resolutionX; x++)
		{
            unsigned int val = *depthPixels++ & DEPTH_MASK;
            int c = *labelPixels++ % COLOR_COUNT;

			unsigned char mask = bg_masks[(int)(c > 0)] | force_mask;
			unsigned char pixel = histogram[val] & mask;
            
            dst[0] = pixel & COLORS[c][0];
            dst[1] = pixel & COLORS[c][1];
            dst[2] = pixel & COLORS[c][2];
            dst[3] = mask;

			dst += bpp;
		}

		dst += pitch;
	}
}

void Kinect::updateUserData(XnUserID id, KinectUser *data)
{
	static XnSkeletonJoint jointTranslation[] =
	{
		XN_SKEL_HEAD, 
        XN_SKEL_NECK, 
        XN_SKEL_TORSO, 
        XN_SKEL_WAIST, 
        XN_SKEL_LEFT_COLLAR, 
        XN_SKEL_LEFT_SHOULDER, 
        XN_SKEL_LEFT_ELBOW, 
        XN_SKEL_LEFT_WRIST, 
        XN_SKEL_LEFT_HAND, 
        XN_SKEL_LEFT_FINGERTIP, 
        XN_SKEL_RIGHT_COLLAR, 
        XN_SKEL_RIGHT_SHOULDER, 
        XN_SKEL_RIGHT_ELBOW, 
        XN_SKEL_RIGHT_WRIST, 
        XN_SKEL_RIGHT_HAND, 
        XN_SKEL_RIGHT_FINGERTIP, 
        XN_SKEL_LEFT_HIP, 
        XN_SKEL_LEFT_KNEE, 
        XN_SKEL_LEFT_ANKLE, 
        XN_SKEL_LEFT_FOOT, 
        XN_SKEL_RIGHT_HIP, 
        XN_SKEL_RIGHT_KNEE, 
        XN_SKEL_RIGHT_ANKLE, 
        XN_SKEL_RIGHT_FOOT,
	};

	static const float planeDepth = 500.0f;

	XnUInt32XYPair resolution = getDepthResolution();

	if (data->status & USER_TRACKING)
	{
		for (int i = 0; i < KINECT_JOINT_MAX; ++i) 
			_userGen.GetSkeletonCap().GetSkeletonJointPosition(id, jointTranslation[i], data->joints[i]);
		
		//uppdatera gesture-tracker
		data->tracker->Tick();
	}

	_userGen.GetCoM(id, data->centerOfMass);
	_depth.ConvertRealWorldToProjective(1, &data->centerOfMass, &data->centerOfMass);

//	data->centerOfMass.X /= (float)resolution.X;
//	data->centerOfMass.Y /= (float)resolution.Y;
}

char const* Kinect::errorMessage()
{
	return (_error != XN_STATUS_OK) ? xnGetStatusString(_error) : 0;
}

void Kinect::setEventCallback(Callback callback, void *userData)
{
	_eventCallback = callback;
	_callbackData = userData;
}

void Kinect::setTicksPerSecond(int ticksPerSecond)
{
	_tickTime = ticksPerSecond ? 1000 / ticksPerSecond : 0;
}

unsigned int Kinect::getImageTexSize(int pitch /*= 0*/)
{
	XnUInt32XYPair res = getImageResolution();
	int bpp = FORMAT_BPP[_renderFormat];
	return pitch ? (pitch * res.Y * bpp) : (res.X * res.Y * bpp);
}

unsigned int Kinect::getDepthTexSize(int pitch /*= 0*/)
{
	XnUInt32XYPair res = getDepthResolution();
	int bpp = FORMAT_BPP[_renderFormat];
	return pitch ? (pitch * res.Y * bpp) : (res.X * res.Y * bpp);
}

XnUInt32XYPair Kinect::getDepthResolution()
{
	XnMapOutputMode output;
	XnUInt32XYPair resolution;

	_depth.GetMapOutputMode(output);
	resolution.X = output.nXRes;
	resolution.Y = output.nYRes;

	return resolution;
}

XnUInt32XYPair Kinect::getImageResolution()
{
	XnMapOutputMode output;
	XnUInt32XYPair resolution;

	_image.GetMapOutputMode(output);
	resolution.X = output.nXRes;
	resolution.Y = output.nYRes;

	return resolution;
}

void Kinect::setRenderFormat(RenderFormat format)
{
	_renderFormat = format;
}



