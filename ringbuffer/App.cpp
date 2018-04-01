#include <opencv2\opencv.hpp>
#include "ringbuffer.hpp"

using namespace std;
using namespace cv;

#define devView(i) imshow(#i,i)

enum state {
	neg = -1,
	unknow = 0,
	pos = 1,
};
struct state3{
	int xst, yst, zst;
	//double confidence[3];
};
ringbuffer<Point> trail(39);
vector<state3> mstseq;
Point center(ringbuffer<Point> seq) {
	Point ct = seq[0];
	for (size_t i = 1; i < seq.size(); i++)
		ct += seq[i];
	ct.x /= seq.size();
	ct.y /= seq.size();
	return ct;
}
vector<state3> shrinkState1D(vector<state3> seqid) {
	vector<state3> sseqid;
	for (size_t i = 1; i < seqid.size(); i++)
	{
		if (seqid[i - 1].xst != seqid[i].xst /*||
			seqid[i - 1].yst != seqid[i].yst ||
			seqid[i - 1].zst != seqid[i].zst*/
			) {
			sseqid.push_back(seqid[i]);
			cout << "[" << seqid[i].xst /*<< "," << seqid[i].yst << "," << seqid[i].zst*/ << "]" ;
		}	
	}
	cout << endl;
	return sseqid;
}
state3 cvt(ringbuffer<Point> &seq,int thresh) {
	state3 st;
	Point ct = center(seq);
	st.xst = unknow;
	if (ct.x - seq.end().x > thresh)st.xst = neg;
	if (ct.x - seq.end().x < -thresh)st.xst = pos;
	st.yst = unknow;
	if (ct.y - seq.end().y > thresh)st.yst = neg;
	if (ct.y - seq.end().y < -thresh)st.yst = pos;
	//st.zst = unknow;
	//if (ct.z - seq.end().z > thresh)st.zst = neg;
	//if (ct.z - seq.end().z < -thresh)st.zst = pos;
	return st;
}


void onMouse(int event, int x, int y, int flags, void* param) {
	//auto trail = (ringbuffer<Point>*)param;
	cout << event << " " << flags << endl;
	if (flags == EVENT_LBUTTONDOWN) {
		trail.push_back(Point(x, y));
	}
	else
	{
		trail.setTo(Point (-1,-1));
		mstseq.clear();
	}
}

int main()
{
	state3 st3;
	Mat canvas = Mat::zeros(Size(640, 480), CV_8UC3); devView(canvas);
	setMouseCallback("canvas", onMouse, &trail);
	for (; waitKey(1) != 27;) {
		canvas -= Scalar(64, 64, 64);
		auto c = cvt(trail,50);
		mstseq.push_back(c);
		shrinkState1D(mstseq);
		//line(canvas, trail.begin(), trail.end(), Scalar(127, 127, 127 * (1 + state)));
		for (size_t i = 1; i < trail.size(); i++){
			auto color = Scalar(127, 127, 255 * ((double)i / trail.size()));
			line(canvas, trail[i - 1], trail[i], color);
		}
		circle(canvas, trail.begin(), 10, Scalar(255, 255, 0));
		circle(canvas, trail.end(), 10, Scalar(255, 0, 255));
		circle(canvas, center(trail), 10, Scalar(0, 255, 255));

		line(canvas, trail.end() + Point(127 * c.xst, 123*c.yst), trail.end(), Scalar(255, 255, 255), 1);
		devView(canvas);
	}


}
