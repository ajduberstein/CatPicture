#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
  private:
	  Surface* mySurface_; //Surface with pixel array

	  //Track frames
    int frame_number_;
	boost::posix_time::ptime app_start_time_;

	struct triangle_info{
		int height;
		int base;
		int corner;
	};

	//Width and height of the screen
	static const int kAppWidth=800;
	static const int kAppHeight=600;
	static const int kTextureSize=1024; //Must be the next power of 2 bigger or equal to app dimensions

	/**
	 * Fill a rectangle with an argyle pattern
	 *
	 * Every space between (x1,y1) and (x2,y2) will be affected. The dimensions of the squares comprising the 
	 * pieces of the argyle pattern are given by rect_with and rect_height
	 *
	 */
	void tileWithRectangles(uint8_t* pixels, int x1, int y1, int x2, int y2, int rect_width, int rect_height, Color8u fill1, Color8u border1, Color8u fill2, Color8u border2);

	void drawLine(uint8_t* pixels, int x1, int y1, int x2, int y2);

	void placeTriangle(uint8_t* pixels, int x1, int y1, int x2, int y2, int rect_width, int rect_height, Color8u fill1, Color8u border1, Color8u fill2, Color8u border2);

	void placeCircle(uint8_t* pixels, int x1, int y1, int x2, int y2, int rect_width, int rect_height, Color8u fill1, Color8u border1, Color8u fill2, Color8u border2);

	void blurEdges(uint8_t* image_to_blur, uint8_t* blur_pattern);

	void animate(uint8_t* image_to_animate, uint8_t* animation);
};

void tileWithRectangles(uint8_t* pixels, int x1, int y1, int x2, int y2, int rect_width, int rect_height, Color8u fill1, Color8u border1, Color8u fill2, Color8u border2){
	int startx = (x1 < x2) ? x1 : x2;
	int endx = (x1 < x2) ? x2 : x1;
	int starty = (y1 < y2) ? y1 : y2;
	int endy = (y1 < y2) ? y2 : y1;
	
	//Do some bounds checking
	if(endx < 0) return; //Visible part of rectangle is off screen
	if(endy < 0) return; //Visible part of rectangle is off screen
	if(startx >= kAppWidth) return; //Visible part of rectangle is off screen
	if(starty >= kAppHeight) return; //Visible part of rectangle is off screen
	if(endx >= kAppWidth) endx = kAppWidth-1;
	if(endy >= kAppHeight) endy = kAppHeight-1;
	
	
	//Variable creation can be slow, so moved it outside the loop
	Color8u c = Color8u(255,0,0);
	int y_distance_from_start;
	int rects_tall;
	int rect_row;
	int y;
	bool in_horiz_border;
	
	int x_distance_from_start;
	int rects_along;
	int rect_col;
	int x;
	bool in_vert_border;
	
	//I do the loops with x on the inside because this incurs less cache misses
	for(y=((starty >= 0) ? starty : 0); y<=endy; y++){
		y_distance_from_start = y - starty;
		rects_tall = y_distance_from_start/rect_height; //How many squares down from the top of the board?
		
		rect_row = y_distance_from_start%rect_height;
		in_horiz_border = (rect_row == 0 || rect_row == rect_height-1);
		
		for(x=((startx >= 0) ? startx : 0); x<=endx; x++){
			x_distance_from_start = x - startx;
			rects_along = x_distance_from_start/rect_width; //How many squares along from the left of the board?
			
			rect_col = x_distance_from_start%rect_width;
			in_vert_border = (rect_col == 0 || rect_col == rect_width-1);
			
			//This is what makes the checkerboard pattern.
			if((rects_tall + rects_along)%2 == 0){
				//I originally had c = fill1, but it turns out that is REALLY slow. Probably causes a copy
				// constructor to get called!
				c.r = fill1.r;
				c.b = fill1.b;
				c.g = fill1.g;
				if(in_horiz_border || in_vert_border){
					c.r = border1.r;
					c.b = border1.b;
					c.g = border1.g;
				}
			} else {
				c.r = fill2.r;
				c.b = fill2.b;
				c.g = fill2.g;
				if(in_horiz_border || in_vert_border){
					c.r = border2.r;
					c.b = border2.b;
					c.g = border2.g;
				}
			}
			pixels[3*(x + y*kTextureSize)] = c.r;
			pixels[3*(x + y*kTextureSize)+1] = c.g;
			pixels[3*(x + y*kTextureSize)+2] = c.b;
		}
	}
}


void HW01App::setup()
{
}

void HW01App::mouseDown( MouseEvent event )
{
}

void HW01App::update()
{
	uint8_t* dataArray = (*mySurface_).getData();
	Color8u fill1 = Color8u(128,128,192);
	Color8u border1 = Color8u(192,192,255);
	Color8u fill2 = Color8u(192,192,192);
	Color8u border2 = Color8u(255,255,255);
}

void HW01App::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( HW01App, RendererGl )