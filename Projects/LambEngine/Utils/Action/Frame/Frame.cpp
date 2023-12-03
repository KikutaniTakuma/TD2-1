#include "Frame.h"
#include <cassert>

Frame::Frame() :
	func(),
	isStart(false),
	startTime(),
	timeFormStart(0),
	frameCount(0)
{}

void Frame::Start() {
	isStart = true;

	startTime = std::chrono::steady_clock::now();
}
void Frame::Puase() {
	isStart = false;
}
void Frame::Stop() {
	isStart = false;
	startTime = std::chrono::steady_clock::time_point::min();
	timeFormStart = std::chrono::milliseconds(0);
	frameCount = 0;
}

void Frame::Update() {
	if(isStart){
		assert(func);
		func();

		auto end = std::chrono::steady_clock::now();
		timeFormStart = std::chrono::duration_cast<decltype(timeFormStart)>(end - startTime);
		frameCount++;
	}
}