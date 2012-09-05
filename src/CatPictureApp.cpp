#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include <math.h>
#include <algorithm>
#include "Resources.h"
#define PI 3.14159265

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);
  private:
	  Surface* mySurface_; //Surface with pixel array
	//Smooth animation is provided by these constants
	int param;
	float wave_index;

	//Width and height of the screen
	static const int kAppWidth=800;
	static const int kAppHeight=600;
	static const int kTextureSize=1024; //Must be the next power of 2 bigger or equal to app dimensions
		
	/**
	 * Draws rectangle on the canvas 
	 *
	 * TODO
	 *
	 * Satisfies requirement A.1
	 */
	void placeRectangle(uint8_t* pixels, int x1, int y1, int x2, int y2, int rect_width, int rect_height, Color8u border);

	void drawSolidRectangle(uint8_t* pixels, int x1, int y1, int x2, int y2);

	/**
	 * Places a bunch of lines forming an asterisk where the cursor clicks 
	 *
	 * TODO Describe the logic
	 *
	 * Satisfies requirement A.3
	 * TODO Implement
	 */
	void drawLine(uint8_t* pixels, int x1, int y1, int x2, int y2, Color8u fill);
	void drawBackground(uint8_t* pixels, Color8u init_fill);
	void drawRightTriangle(uint8_t* pixels, int x1, int y1, int x2, int y2);
	void drawFullTriangle(uint8_t* pixels, int x1, int y1, int x2, int y2, int x3, int y3, Color8u fill);

	/**
	 * Places a circle where the click occurs
	 *
	 * TODO Describe the logic
	 *
	 */
	void placeCircle(uint8_t* pixels, int x, int y, int radius);

	/**
	 * Blurs edges of image
	 *
	 * TODO Describe the logic
	 *
	 */
	void blurEdges(uint8_t* image_to_blur);
};

void CatPictureApp::prepareSettings(Settings* settings)
{
	(*settings).setWindowSize(kAppWidth,kAppHeight);
	(*settings).setResizable(false);
}
//Borrowed from Dr. Brinkman's code--this produces a very '80's texture on the back of the surface--fulfills no requirement, but it could be interesting.
void CatPictureApp::placeRectangle(uint8_t* pixels, int startx, int starty, int endx, int endy, int rect_width, int rect_height, Color8u border)
{
	int currentY;
	int currentX;
	bool in_horiz_border;
	bool in_vert_border; 
	int x_distance_from_start;
	int y_distance_from_start;
	int rectHeight = endy - starty;
	int rectLength = endx - startx;
	int rects_along;
	int rect_col;
	int rects_tall;
	int rect_row;
	for (currentY = starty; currentY<=endy; currentY++)
	{
		y_distance_from_start = currentY - starty;
		rects_tall = y_distance_from_start%rectHeight;
		rect_row = y_distance_from_start%rectLength;
		in_horiz_border = (rect_row == 0 || rect_row == rect_height-1);			
		for(currentX = startx; currentX <= endx; currentX++)
		{
			x_distance_from_start = currentX - starty;
			rects_along = x_distance_from_start/rect_width;
			rect_col = x_distance_from_start/rect_width;
			in_vert_border = (rect_col == 0 || rect_col == rect_width-1);
			if(in_horiz_border || in_vert_border)
			{
				pixels[3*(currentX + currentY*kTextureSize)] = border.r;
				pixels[3*(currentX + currentY*kTextureSize)+1] = border.g;
				pixels[3*(currentX + currentY*kTextureSize)+2] = border.b;
			}
		}
	}
}

void CatPictureApp::drawSolidRectangle(uint8_t* pixels, int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < kTextureSize && j < y2; j++)
	{
		for (int i = x1; i < kTextureSize && i < x2 ; i++)
		{
			int current_point = 3*(i + j*kTextureSize);
			pixels[current_point] = 0;
			pixels[current_point+1] = 0;
			pixels[current_point+2] = 0;
		}
	}
}
void CatPictureApp::drawLine(uint8_t* pixels, int x1, int y1, int x2, int y2, Color8u fill)
{
	int current_point;
	//CASE 1 -- Vertical line
	if (x1 == x2){
		int y = min(y1,y2);
		for (int i = min(y1,y2); i < kTextureSize && i < max(y1,y2); i++){
			int current_point = 3*(x1 + i*kTextureSize);
			pixels[current_point] = fill.r;
			pixels[current_point+1] = fill.g;
			pixels[current_point+2] = fill.b;
		}
	}
	//CASE 2 -- Horizontal line
	else if (y2 == y1){
		for (int i = min(x1,x2); i < kTextureSize && i < max(x1,x2); i++){
			int current_point = 3*(i + y1*kTextureSize);
			pixels[current_point] = fill.r;
			pixels[current_point+1] = fill.g;
			pixels[current_point+2] = fill.b;
		}
	}
	//CASE 3 -- Diagonal line
	//Bresenham's line algorithm, credit to Wikipedia. 
	//Pseudocode for algorithm viewable at http://en.wikipedia.org/wiki/Bresenham's_line_algorithm#Simplification
	else{
		double dx = abs(x2-x1);
		double dy = abs(y2-y1);
		int sx = (x1 < x2) ? 1 : -1;
		int sy = (y1 < y2) ? 1 : -1;
		double error = dx-dy;
		double twice_error;
		while (x1 != x2 && y1 != y2 && y1 < kTextureSize && x1 < kTextureSize){
			current_point = 3*(x1 + y1*kTextureSize);
			pixels[current_point] = fill.r;
			pixels[current_point+1] = fill.g;
			pixels[current_point+2] = fill.b; 
			twice_error = 2*error; 
			if (twice_error > -1*dy){
				error -= dy;
				x1 += sx;
			}
			if (twice_error < dx){
				error += dx;
				y1 += sy;
			}
		}
	}
}

void CatPictureApp::blurEdges(uint8_t* image_to_blur)
{
	int total = 0;
	int radius = 9;
		for (int j = 0; j < kAppHeight; ++j)
		{
			for (int i = 0; i < kAppWidth; ++i)
			{
				total = 0;
				for (int kj = -radius; kj <= radius; ++kj)
					for (int ki = -radius; ki <= radius; ++ki)
						total += image_to_blur[3*((i + ki) + (j + kj)*kTextureSize)];
				image_to_blur[3*(i + j*kTextureSize)] = (1/9)*total;
				image_to_blur[3*(i + j*kTextureSize)+1] = (1/9)*total;
				image_to_blur[3*(i + j*kTextureSize)+2] = (1/9)*total;
			}
		}
}

void CatPictureApp::drawRightTriangle(uint8_t* pixels, int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < kTextureSize && j < y2; j++)
	{
		for (int i = x2; i < kTextureSize && i >= j ; i--)
		{
				int current_point = 3*(i + j*kTextureSize);
				pixels[current_point] = 0;
				pixels[current_point+1] = 0;
				pixels[current_point+2] = 0;
		}
	}
}

void CatPictureApp::drawFullTriangle(uint8_t* pixels, int x1, int y1, int x2, int y2, int x3, int y3, Color8u fill)
{
	drawLine(pixels,x1,y1,x3,y3,fill);
	drawLine(pixels,x2,y2,x3,y3,fill);
	drawLine(pixels,x1,y1,x2,y2,fill);
}

void CatPictureApp::drawBackground(uint8_t* pixels, Color8u init_fill){
	int current_point;
	for (int i = 0; i < kTextureSize; i++)
		for (int j = 0; j < kTextureSize; j++){
			current_point = 3*(j + i*kTextureSize);
			pixels[current_point] = i;
			pixels[current_point+1] = i;
			pixels[current_point+2] = i;
		}
}

void CatPictureApp::setup()
{
	uint8_t* dataArray = (*mySurface_).getData();
	/*for (int i = 0; i < 1000; i+=10)
	{
		for (int j = 0; j < 500; j += 50)
		{
			placeRectangle(dataArray,5+i,10+j,100+i,100+j*2,2,90,Color8u(200,200,200));
		}
	}*/
	//blurEdges(dataArray);
	//drawBackground(dataArray, Color8u(0,0,0));
	//drawSolidRectangle(dataArray,10,10,100,100);
	//drawLine(dataArray,100,100,100,250, Color8u(60,60,60));
	//drawRightTriangle(dataArray,250,250,500,500);
	//blurEdges(dataArray);
}

void CatPictureApp::mouseDown( MouseEvent event )
{
}

void CatPictureApp::update()
{
	/*param+=(rand()%100);
	int random = rand()%5;
	uint8_t* dataArray = (*mySurface_).getData();
	wave_index = cos(param*PI/180)*100;
	drawFullTriangle(dataArray, ceil(400+wave_index),400+random, ceil(475+wave_index)+random,500,500,ceil(400+wave_index), Color8u(wave_index,param,20));
	wave_index = sin(param*PI/180)*100;
	drawFullTriangle(dataArray, ceil(200+wave_index),200+random, ceil(275+wave_index)+random,300,300,ceil(200+wave_index), Color8u(120-wave_index,param,20));
	if (param > 100){
		param = 0;
	}*/
}

void CatPictureApp::draw()
{
	// Draw the surface 
	gl::draw(*mySurface_);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
