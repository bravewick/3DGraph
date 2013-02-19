/*
 * Graph.h
 *
 *  Created on: Jan 10, 2013
 *      Author: CodeArt
 */

#ifndef GRAPH_H_
#define GRAPH_H_
#include "GraphApi.h"
#include "GFont.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shaders.h"
#include "RenderText.h"
// during development I lake to keep all data public. cleanup will be done later.


/*
 * Helper class that split a string and parse its values to an array of floats.
 */
class ParseInput
{
public:
	// static data
	// dynamic data
	ParseInput(){}
	ParseInput(char *data,int len = -1) : m_data(data) {}
	~ParseInput() {}

	void set(char *data,int len = -1)
	{
		m_data = data;
		m_len = len;
		if (m_len < 0)
			m_len = strlen(data);
	}

	int parse2d(std::vector<float>&table, char delim=',')
	{
		// Need a join function... check where...

		// simple join func
		if (m_len < 0)
			m_len = strlen(m_data);

		if (m_len <= 0)
			return -1;

		for(int i=0; i < m_len;)
		{
			char *pos = strchr(&m_data[i], (int)delim);
			if (pos == 0)
			{
				// whole string is one data entry or error
				float f = atof(&m_data[i]);
				table.push_back(f);
				break;
			}
			int len = pos - &m_data[i];
			i += len+1;
		}
		return 0;
	}

protected:

	char *m_data;		// input data stream or static.
	int  m_len;
};




/*
 * Graph namespace , that contains all graph and input of data classes into the same namespace..
 */
namespace MoGraph
{

class Scene;



/*
 * Input class for Graphs handling Static or Dynamic or Stream of data.
 */
class Input
{
public:
	ParseInput m_input;

	// static

	// dynamic
protected:
};

/*
 * Screen class manageing widget size in absolute resolutions, info
 */
class Screen
{
public:
	// gui obj
	int 				m_width;
	int 				m_height;
	bool 				m_fullscreen;
	int 				m_type;
	// table should be an own class handling the range of data to be displayed, average out dependent on scale etc.
	std::vector<float> 	m_table;
	// input obj

protected:

};

class Render
{
public:
	Render(Scene &scene) : mScene(scene) {}
	virtual~Render() 		{};
	virtual void init() 	{};
	virtual void draw() 	{};
	Scene &getScene() 		{return mScene;}

protected:
	Scene &mScene;
};


/*
 * Bar class handles one single bar for rendering
 */

// TODO split up obje part for object handling.
class Bar
{
protected:
	float 		mValue;
	glm::vec4 	mColor;

public:
	Bar() : mValue(1.0f), mColor(1.0f,1.0f,1.0f,1.0f)	{}
	void setColor(float r,float g,float b, float a)		{ mColor.x = r; mColor.y = g; mColor.z = b; mColor.w = a; }
	void setColor(glm::vec4 &col)						{ mColor = col;}
	void setValue(float val) 							{mValue = val;}
	float getValue() 									{return mValue;}
	glm::vec4 & getColor() 								{return mColor;}
};

/*
 * BarMgr handles all the bars that are being displayed
 *
 */
class BarMgr : public Render
{
protected:
	std::vector<Bar> 			mBars;		// question absolute amount of bars or display fromc..to
	std::vector<glm::vec3> 		mVertices;
	std::vector<unsigned short> mFaces;
	BarShader					mShader;

	void create3D();			// create 3D obj. one instance of a cube with origo at bottom with indices
public:


	BarMgr(Scene &scene) : Render(scene) 	{create3D();}
	virtual ~BarMgr()						{}

	void addBars(int n) 					{mBars.resize(n);}
	void addBar(Bar &b) 					{mBars.push_back(b);}
	std::vector<Bar> &getBars() 			{return mBars;}
	int size()								{return mBars.size();}
	Bar &getBar(int i) 						{return mBars[i];}
	std::vector<glm::vec3>& vertices() 		{return mVertices;}
	std::vector<unsigned short>& faces() 	{return mFaces;}
	BarShader &getShader() 					{return mShader;}

	// virtuals from render class
	virtual void init();
	virtual void draw();
};

/*
 * Handles the AXIS lines includeing grid marks to show the steps for a value.
 */
class Axis
{
protected:

	std::vector<glm::vec3> mVertices;
public:
	std::vector<glm::vec3>& vertices()	{return mVertices;}
	int size() 							{return mVertices.size();}
};

/*
 * AxisMgr class handles several Axis lines for the graph display
 */
class AxisMgr : public Render
{
protected:
	std::vector<Axis> 	mAxis;		// value and time  can support up to 3D X,Y,Z
	LineShader			mShader; 	// Shader to be used for rendering the lines.
	float				mGridStep;	// Step of each grid bars in height. (note total height is amount*step)
	int 				mGridLines;	// Grid lines for graph bar in Y... stepped with mStep, to get height grid,
	void create3D();
public:
	AxisMgr(Scene &scene) : Render(scene), mGridStep(1.0f), mGridLines(2) {}
	virtual ~AxisMgr() {}

	enum {AXIS_X,AXIS_Y,AXIS_Z};
	Axis &getAxis(int i) 				{return mAxis[i];}
	std::vector<Axis>& getAxisArray() 	{return mAxis;}
	int size() 							{return mAxis.size();}
	LineShader &getShader() 			{return mShader;}
	void addAxis(int axis);
	void setGridStep(float step)		{mGridStep  = step;}
	void setGridLines(int nLines)		{mGridLines = nLines;}
	float getGridStep()					{return mGridStep;}
	int	getGridLines()					{return mGridLines;}
	// virtuals from render class
	virtual void init();
	virtual void draw();
};

/*
 * Text class text display in graph
 */
class Text
{
public:
	std::string 	mText;
	glm::vec3 		mPos;
	glm::vec3 		mColor;
	int 			mFontType;		// reference for the font to use
	// need to add text data to render here.
};

/*
 * TextMgr class handle all texts that is needed for the graphs.
 */
class TextMgr : public Render
{
protected:
	std::vector<Text> mText;
	// add some common properties
	// Text Shader using point sprites ?
public:
	TextMgr(Scene &scene) : Render(scene) 	{}
	Text &getText(int i) 					{return mText[i];}
	int size() 								{return mText.size();}

	virtual ~TextMgr()						{}
	virtual void init()						{}
	virtual void draw()						{}

};

/*
 * Scene handling all object in scene.
 */
class Scene
{
protected:
	bool 		mFitToScreen;		// fit to screen or use a std fixed view matrix.
	int 		mGridX;				// Bar grid X
	int 		mGridZ;				// Bar grid Z note mGridX*mGridZ = total amount of bars being used.
	int			mWidth;
	int			mHeight;
	float 		mTick;
	float 		mDistToCam;			// internally calculated for fit to screen.

	glm::mat4 	mProjection;		// Projection Matrix
	glm::mat4 	mView;				// View Matrix
	glm::mat4 	mWorld;				// World Matrix
	glm::mat4 	mPVW;				// Projection * View * World Matrix

	AxisMgr 	mAxisMgr;			// Axis mangaer contains all Axis and Grid lineups dependent on graph type
	BarMgr  	mBarMgr;			// Bar manager contains all bars and vertices common for them
	TextMgr 	mTextMgr;			// Text manager handles fonts for text printouts.


	float	*mValues;
	int		mValuesSz;

	glm::vec4 *mColors;
	int		mColorsSz;


	std::vector<unsigned short> mIndices;	// Index list of faces

public:
	Scene() : mFitToScreen(true), mGridX(1), mGridZ(1), mWidth(1), mHeight(1), mTick(0.0f), mDistToCam(1.0f), mAxisMgr(*this),mBarMgr(*this),mTextMgr(*this){}
	void 	create(int gridX,int gridZ, int lines = 5, float stepY = 1.0f, bool bFitToScreen = true);
	BarMgr 	&getBarMgr() 			{return mBarMgr;}
	AxisMgr &getAxisMgr()			{return mAxisMgr;}
	TextMgr &getTextMgr()			{return mTextMgr;}
	int 	getGridX()				{return mGridX;}
	int		getGridZ() 				{return mGridZ;}
	float	getCx()					{return mGridX*0.5f;}
	float	getCz()					{return mGridZ*0.5f;}
	glm::mat4 & getProjectionMat() 	{return mProjection;}
	glm::mat4 & getViewMat()		{return mView;}
	glm::mat4 & getWorldMat()		{return mWorld;}
	glm::mat4 & getPVWMat()			{return mPVW;}
	void setWorldMat(glm::mat4 &m) 	{mWorld = m;}
	std::vector<unsigned short> &getIndices() {return mIndices;}
	void	setTick(float tick)		{mTick = tick;}
	float	getTick()				{return mTick;}
	void	setWidth(int w)			{mWidth = w;}
	int		getWidth()				{return mWidth;}
	void	setHeight(int h)		{mHeight = h;}
	int		getHeight()				{return mHeight;}
	void updateMatrix();

	void setValues(float *values,int sz) { mValues = values; mValuesSz = sz;}
	void setColors(glm::vec4 *colors, int sz) {mColors = colors; mColorsSz = sz;}
	float getValue(int i) {return mValues[i];}
	glm::vec4 & getColor(int i) {return mColors[i];}

};

/*
 * Layout
 */
class Graph : public IGraph /* should include a interface here. */
{
protected:
	Scene 	mScene;
	IFont 	*mFont;
	int 	mWidth;
	int 	mHeight;
	int		mGridX;
	int 	mGridZ;
	int 	mStartTime;
	glm::vec4 mBKColor;

	RenderText		mRenderText;
	void drawBars(float tick)	{	mScene.getBarMgr().draw();	}
	void drawAxis(float tick)	{	mScene.getAxisMgr().draw();	}
	void drawText(float tick)
	{
		glm::vec4 rgba(1.0f,1.0f,1.0f,1.0f);
		std::string str = "MoSync 3D Graph Library 0.7 Beta";
		glm::vec3 pos 	= glm::vec3(-(float)mGridX*0.5f, 0.0f,(float)mGridZ*0.5f);
		float px 		= mRenderText.DrawText(str.c_str(), pos, rgba, mScene.getGridX(), mScene.getGridZ(), mScene.getPVWMat(), mScene.getTick(), true);
		str 			= "Welcome to Wonderland";
		pos.y 			+= -2.0f;
		pos.x 			+= px;
		rgba.r 			= 0.0f;
		rgba.g 			= 0.0f;
		mRenderText.DrawText(str.c_str(), pos, rgba, mScene.getGridX(), mScene.getGridZ(), mScene.getPVWMat(), mScene.getTick(), true);
	}

	void initShaderLines()		{	mScene.getAxisMgr().init();	}
	void initShaderBars()		{	mScene.getBarMgr().init();	}
	void initShaderText()		{}
	int  initGL();

public:
	Graph() : mFont(0), mWidth(1), mHeight(1), mGridX(1), mGridZ(1), mStartTime(0), mBKColor(0.0f,0.0f,0.0f,1.0f) {}
	virtual ~Graph() {}
	virtual int init(int x,int z, int gridLines, float step, bool bFitScreen, IFont* font,int width,int height);
	virtual void setValues(float *values,int sz) 			{ mScene.setValues(values,sz);}
	virtual void setColors(glm::vec4 *colors, int sz) 		{mScene.setColors(colors,sz);}
	virtual void setBKColor(glm::vec4 &color) 				{ mBKColor = color;}
	virtual void draw();
	virtual Scene &getScene() 								{return mScene;}
};

}
#endif /* GRAPH_H_ */
